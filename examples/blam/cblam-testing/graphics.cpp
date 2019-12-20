#include <blam/cblam.h>
#include <coffee/comp_app/AppWrap>
#include <coffee/core/CApplication>
#include <coffee/core/CArgParser>
#include <coffee/core/CFiles>
#include <coffee/core/EventHandlers>
#include <coffee/core/Scene>
#include <coffee/graphics/apis/CGLeamRHI>

#include <coffee/interfaces/graphics_subsystem.h>

#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace StandardInput;
using GFX       = RHI::GLEAM::GLEAM_API;
using GFX_ALLOC = RHI::Components::GraphicsAllocator<GFX>;
using PIP_PARAM = GFX_ALLOC::AllocData::PipelineParams;

using camera_t     = typing::vectors::scene::camera<scalar>;
using std_camera_t = StandardCamera<camera_t*, StandardCameraOpts*>;

struct BlamData
{
    BlamData() :
        map_file(""), std_camera(MkShared<std_camera_t>(&camera, &camera_opts))
    {
        map_file =
            Resource(MkUrl(GetInitArgs().arguments().at(0), RSCA::SystemFile));
    }

    /* Blam! data */
    Resource                   map_file;
    blam::scn::scenario const* scenario;

    Vector<GFX::D_DATA> bsps;
    ShPtr<GFX::BUF_A>   bsp_buf;
    ShPtr<GFX::BUF_E>   bsp_index;
    ShPtr<GFX::V_DESC>  bsp_attr;
    PIP_PARAM*          bsp_pipeline;

    camera_t camera;
    Matf4    camera_matrix;

    StandardCameraOpts  camera_opts;
    ShPtr<std_camera_t> std_camera;
};

i32 blam_main(i32, cstring_w*)
{
    using comp_app::detail::duration;
    using comp_app::detail::EntityContainer;
    using comp_app::detail::time_point;

    auto blam_queue = RuntimeQueue::CreateNewQueue("Blam Graphics!");

    comp_app::app_error app_ec;
    auto&               e = comp_app::createContainer();
    comp_app::configureDefaults(e);
    comp_app::addDefaults(e, *e.service<comp_app::AppLoader>(), app_ec);
    comp_app::AppContainer<BlamData>::addTo(
        e,
        [](EntityContainer& e, BlamData& data, time_point const&) {
            if(!FileExists(data.map_file))
                Throw(undefined_behavior("map file not found"));

            blam::map_container map(data.map_file, blam::pc_version);
            auto&               magic = map.magic;

            {
                auto map_name      = map.map->full_mapname();
                auto window_config = e.service<comp_app::WindowInfo>();
                window_config->setName(
                    window_config->name() + " : " + map_name);
            }

            data.scenario = map.tags->scenario(map.map)
                                .to_reflexive<blam::scn::scenario>()
                                .data(magic)
                                .data;

            {
                using namespace Display::EventHandlers;
                auto eventhandler =
                    e.service<comp_app::BasicEventBus<Input::CIEvent>>();

                eventhandler->addEventHandler(
                    0,
                    ExitOn<OnKey<Input::CK_Escape>>(
                        e.service_ref<comp_app::Windowing>()));

                eventhandler->addEventHandler(
                    0, std_camera_t::KeyboardInput(data.std_camera));
                eventhandler->addEventHandler(
                    0, std_camera_t::MouseInput(&data.camera));
            }

            /* Set up graphics data */
            GFX::OPTS options;
            options.crash_on_error        = true;
            options.old_shader_processing = false;
            auto& gfx =
                e.register_subsystem_inplace<GFX_ALLOC::tag_type, GFX_ALLOC>(
                    options, true);

            data.bsp_buf = gfx.alloc_buffer<GFX::BUF_A>(
                RSCA::WriteOnly | RSCA::Persistent, 0);
            data.bsp_index = gfx.alloc_buffer<GFX::BUF_E>(
                RSCA::WriteOnly | RSCA::Persistent, 0);

            using vertex_type = blam::vert::vertex<blam::vert::uncompressed>;

            GFX::V_ATTR pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr;
            pos_attr.m_idx  = 0;
            tex_attr.m_idx  = 1;
            nrm_attr.m_idx  = 2;
            bin_attr.m_idx  = 3;
            tan_attr.m_idx  = 4;
            pos_attr.m_off  = offsetof(vertex_type, position);
            tex_attr.m_off  = offsetof(vertex_type, texcoord);
            nrm_attr.m_off  = offsetof(vertex_type, normal);
            bin_attr.m_off  = offsetof(vertex_type, binorm);
            tan_attr.m_off  = offsetof(vertex_type, tangent);
            tex_attr.m_size = 2;

            pos_attr.m_size = nrm_attr.m_size = bin_attr.m_size =
                tan_attr.m_size               = 3;
            pos_attr.m_type = tex_attr.m_type = nrm_attr.m_type =
                bin_attr.m_type = tan_attr.m_type = semantic::TypeEnum::Scalar;
            pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
                bin_attr.m_stride = tan_attr.m_stride = sizeof(vertex_type);
            pos_attr.m_bassoc = tex_attr.m_bassoc = nrm_attr.m_bassoc =
                bin_attr.m_bassoc = tan_attr.m_bassoc = 0;

            data.bsp_attr = gfx.alloc_desc<5>(
                {{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr}});

            data.bsp_attr->setIndexBuffer(data.bsp_index.get());
            data.bsp_attr->bindBuffer(0, *data.bsp_buf);

            u32 num_vertices = 0;
            u32 num_elements = 0;

            /* Start loading up vertex data */
            auto scenario = data.scenario;
            for(auto const& bsp : scenario->bsp_info.data(magic))
            {
                auto        bsp_magic = bsp.bsp_magic(magic);
                auto const& section =
                    bsp.to_bsp(bsp_magic).to_header().data(bsp_magic)[0];

                for(auto const& part : section.submesh_groups.data(bsp_magic))
                    for(auto const& mesh : part.material.data(bsp_magic))
                    {
                        auto indices = mesh.pc_indices(section).data(bsp_magic);
                        auto vertices = mesh.pc_vertices().data(bsp_magic);

                        GFX::D_DATA mesh_draw;
                        mesh_draw.m_voff   = mesh.base_vertex();
                        mesh_draw.m_eoff   = mesh.index_offset();
                        mesh_draw.m_elems  = indices.elements * 3;
                        mesh_draw.m_eltype = semantic::TypeEnum::UShort;
                        data.bsps.push_back(mesh_draw);

                        num_vertices += vertices.size;
                        num_elements += indices.size;

                        break;
                    }
            }

            data.bsp_buf->commit(num_vertices);
            data.bsp_index->commit(num_elements);

            auto vert_buffer =
                data.bsp_buf->map(0, num_vertices).as<vertex_type>();
            auto idx_buffer =
                data.bsp_index->map(0, num_elements).as<blam::vert::face>();

            num_vertices = 0;
            num_elements = 0;

            for(auto const& bsp : scenario->bsp_info.data(magic))
            {
                auto        bsp_magic = bsp.bsp_magic(magic);
                auto const& section =
                    bsp.to_bsp(bsp_magic).to_header().data(bsp_magic)[0];

                for(auto const& part : section.submesh_groups.data(bsp_magic))
                    for(auto const& mesh : part.material.data(bsp_magic))
                    {
                        auto vertices = mesh.pc_vertices().data(bsp_magic);
                        auto indices = mesh.pc_indices(section).data(bsp_magic);

                        MemCpy(vertices, vert_buffer.at(num_vertices));
                        MemCpy(indices, idx_buffer.at(num_elements));

                        num_vertices += vertices.elements;
                        num_elements += indices.elements;

                        break;
                    }
            }

            data.bsp_buf->unmap();
            data.bsp_index->unmap();

            auto& pipeline = gfx.alloc_standard_pipeline<2>(
                {{"map.vert"_rsc, "map.frag"_rsc}});

            data.bsp_pipeline = &pipeline;

            auto const& view_u = *std::find_if(
                pipeline.constants_begin(),
                pipeline.constants_end(),
                pipeline.constant_by_name("view"));

            pipeline.set_constant(view_u, Bytes::From(data.camera_matrix));

            pipeline.build_state();
        },
        [](EntityContainer& e,
           BlamData&        data,
           time_point const&,
           duration const&) {
            GFX::DefaultFramebuffer()->clear(0, {0.5f, 0.5f, 0.9f, 1.f}, 1.0);

            data.std_camera->tick();

            {
                using namespace typing::vectors::scene;
                data.camera_matrix =
                    GenPerspective(data.camera) * GenTransform(data.camera) *
                    typing::vectors::scale<scalar>(Matf4(), {0.1f});
            }

            GFX::ERROR ec;
            for(auto const& draw : data.bsps)
                GFX::Draw(
                    data.bsp_pipeline->pipeline(),
                    data.bsp_pipeline->get_state(),
                    *data.bsp_attr,
                    GFX::D_CALL(true, false),
                    draw,
                    ec);
        },
        [](EntityContainer& e, BlamData& data, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(blam_main)
