#include <blam/cblam.h>
#include <coffee/comp_app/AppWrap>
#include <coffee/comp_app/gl_config.h>
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

using comp_app::detail::duration;
using comp_app::detail::time_point;
using vertex_type = blam::vert::vertex<blam::vert::uncompressed>;
using Components::ComponentRef;
using Components::EntityContainer;

struct world_object_t
{
    Vecf3  position;
    scalar rotation;
};

struct draw_info_t
{
    Vector<GFX::D_DATA> data;
};

using draw_info_list =
    Components::Allocators::VectorContainer<Components::TagType<draw_info_t>>;

struct scenery_meta_t
{
    ComponentRef<EntityContainer, draw_info_list::tag_type> draw_info;

    CString                         name;
    blam::scn::scenery_spawn const* spawn;
};

struct bsp_meta_t
{
    ComponentRef<EntityContainer, draw_info_list::tag_type> draw_info;

    CString                               name;
    blam::scn::bsp::submesh_header const* submesh;
};

using world_object_list = Components::Allocators::VectorContainer<
    Components::TagType<world_object_t>>;
using scenery_list = Components::Allocators::VectorContainer<
    Components::TagType<scenery_meta_t>>;
using bsp_list =
    Components::Allocators::VectorContainer<Components::TagType<bsp_meta_t>>;

struct BlamData
{
    BlamData() :
        map_file(MkUrl(GetInitArgs().arguments().at(0), RSCA::SystemFile)),
        map_container(map_file, blam::pc_version),
        std_camera(MkShared<std_camera_t>(&camera, &camera_opts))
    {
    }

    /* Blam! data */
    Resource                   map_file;
    blam::map_container        map_container;
    blam::scn::scenario const* scenario;

    Vector<GFX::D_DATA> bsps;
    ShPtr<GFX::BUF_A>   bsp_buf;
    ShPtr<GFX::BUF_E>   bsp_index;
    ShPtr<GFX::V_DESC>  bsp_attr;
    PIP_PARAM*          bsp_pipeline;
    GFX::RenderPass     bsp_pass;
    GFX::OPT_DRAW       bsp_draw;

    ShPtr<GFX::BUF_A>  scenery_buf;
    ShPtr<GFX::BUF_E>  scenery_index;
    ShPtr<GFX::V_DESC> scenery_attr;
    PIP_PARAM*         scenery_pipeline;
    Vector<Matf4>      scenery_mats;

    camera_t            camera;
    Matf4               camera_matrix;
    StandardCameraOpts  camera_opts;
    ShPtr<std_camera_t> std_camera;
};

void create_resources(EntityContainer& e, BlamData& data)
{
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
    auto& gfx = e.register_subsystem_inplace<GFX_ALLOC::tag_type, GFX_ALLOC>(
        options, true);

    data.bsp_buf = gfx.alloc_buffer<GFX::BUF_A>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    data.bsp_index = gfx.alloc_buffer<GFX::BUF_E>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);

    data.scenery_buf = gfx.alloc_buffer<GFX::BUF_A>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    data.scenery_index = gfx.alloc_buffer<GFX::BUF_E>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);

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

    pos_attr.m_size = nrm_attr.m_size = bin_attr.m_size = tan_attr.m_size = 3;
    pos_attr.m_type = tex_attr.m_type = nrm_attr.m_type = bin_attr.m_type =
        tan_attr.m_type = semantic::TypeEnum::Scalar;
    pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
        bin_attr.m_stride = tan_attr.m_stride = sizeof(vertex_type);
    pos_attr.m_bassoc = tex_attr.m_bassoc = nrm_attr.m_bassoc =
        bin_attr.m_bassoc = tan_attr.m_bassoc = 0;

    data.bsp_attr =
        gfx.alloc_desc<5>({{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr}});
    data.bsp_attr->setIndexBuffer(data.bsp_index.get());
    data.bsp_attr->bindBuffer(0, *data.bsp_buf);

    using model_vertex_type = blam::vert::mod2_vertex<blam::vert::uncompressed>;

    pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
        bin_attr.m_stride = tan_attr.m_stride = sizeof(model_vertex_type);

    data.scenery_attr =
        gfx.alloc_desc<5>({{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr}});
    data.scenery_attr->setIndexBuffer(data.scenery_index.get());
    data.scenery_attr->bindBuffer(0, *data.scenery_buf);
}

void load_scenario_bsp(EntityContainer& e, BlamData& data)
{
    auto& magic = data.map_container.magic;

    u32 num_vertices = 0;
    u32 num_elements = 0;

    u32 base_vertex = 0;
    u32 base_index  = 0;

    /* Start loading up vertex data */
    auto scenario = data.scenario;
    for(auto const& bsp : scenario->bsp_info.data(magic))
    {
        auto        bsp_magic = bsp.bsp_magic(magic);
        auto const& section =
            bsp.to_bsp(bsp_magic).to_header().data(bsp_magic)[0];

        for(auto const& part : section.submesh_groups.data(bsp_magic))
        {
            for(auto const& mesh : part.material.data(bsp_magic))
            {
                auto indices  = mesh.pc_indices(section).data(bsp_magic);
                auto vertices = mesh.pc_vertices().data(bsp_magic);

                GFX::D_DATA mesh_draw;
                mesh_draw.m_voff   = base_vertex;
                mesh_draw.m_eoff   = base_index;
                mesh_draw.m_elems  = indices.elements * 3;
                mesh_draw.m_eltype = semantic::TypeEnum::UShort;
                mesh_draw.m_insts  = 1;
                data.bsps.push_back(mesh_draw);

                cDebug(
                    "Mesh: voff={0}, verts={3}, eoff={1}, elems={2}",
                    mesh_draw.vertexOffset(),
                    mesh_draw.indexOffset(),
                    mesh_draw.elements(),
                    vertices.elements);

                num_vertices += vertices.size;
                num_elements += indices.size;

                base_vertex += vertices.elements;
                base_index += indices.elements * 3;
            }
        }
    }

    data.bsp_buf->commit(num_vertices);
    data.bsp_index->commit(num_elements);

    auto vert_buffer = data.bsp_buf->map(0, num_vertices).as<vertex_type>();
    auto idx_buffer =
        data.bsp_index->map(0, num_elements).as<blam::vert::face>();

    num_vertices = num_elements = 0;
    num_vertices = num_elements = 0;

    for(auto const& bsp : scenario->bsp_info.data(magic))
    {
        auto        bsp_magic = bsp.bsp_magic(magic);
        auto const& section =
            bsp.to_bsp(bsp_magic).to_header().data(bsp_magic)[0];

        for(auto const& part : section.submesh_groups.data(bsp_magic))
        {
            for(auto const& mesh : part.material.data(bsp_magic))
            {
                auto vertices = mesh.pc_vertices().data(bsp_magic);
                auto indices  = mesh.pc_indices(section).data(bsp_magic);

                auto vert_dest = vert_buffer.at(num_vertices);
                auto idx_dest  = idx_buffer.at(num_elements);

                MemCpy(vertices, vert_dest);
                MemCpy(indices, idx_dest);

                num_vertices += vertices.size;
                num_elements += indices.size;
            }
        }
    }

    data.bsp_buf->unmap();
    data.bsp_index->unmap();

    auto& bsp_pass       = data.bsp_pass;
    bsp_pass.pipeline    = data.bsp_pipeline->pipeline_ptr();
    bsp_pass.framebuffer = GFX::DefaultFramebuffer();
    for(auto const& draw : data.bsps)
        bsp_pass.draws.push_back({data.bsp_attr,
                                  &data.bsp_pipeline->get_state(),
                                  GFX::D_CALL().withIndexing().withInstancing(),
                                  draw});
    GFX::OptimizeRenderPass(bsp_pass, data.bsp_draw);
}

void load_scenario_scenery(EntityContainer& e, BlamData& data)
{
    auto  scenario = data.scenario;
    auto& map      = data.map_container;
    auto  index    = blam::tag_index_view(data.map_container);

    auto& magic        = data.map_container.magic;
    auto  vertex_magic = data.map_container.tags->vertex_magic(magic);

    using Components::Convenience::type_hash_v;

    Components::EntityRecipe scenery_meta;
    scenery_meta.components = {type_hash_v<draw_info_list::tag_type>()};
    Components::EntityRecipe scenery_instance;
    scenery_instance.components = {type_hash_v<scenery_list::tag_type>(),
                                   type_hash_v<world_object_list::tag_type>()};

    u32 num_vertices = 0, num_elements = 0, base_vertex = 0, base_index = 0;

    Vector<decltype(scenery_meta_t::draw_info)> scenery_refs;

    for(auto const& scenery : scenario->scenery.ref.data(magic))
    {
        auto const& scenery_tag = scenery[0];
        cDebug("Scenery item {0}", scenery_tag.name.to_string(magic));
        auto item_it = index.find(scenery_tag);

        if(item_it == index.end())
            continue;

        auto const& item =
            (*item_it)->to_reflexive<blam::scn::scenery>().data(magic)[0];

        auto model_it = index.find(item.model);

        if(model_it == index.end())
            continue;

        auto const& model =
            (*model_it)->to_reflexive<blam::mod2::header>().data(magic);

        auto scenery_object = e.ref(e.create_entity(scenery_meta));
        scenery_refs.push_back(
            e.ref_comp<draw_info_list::tag_type>(scenery_object.id()));

        auto& draw_data = scenery_object.get<draw_info_list::tag_type>();

        for(auto const& geom : model[0].geometries.data(magic))
            for(auto const& model : geom.mesh_headers.data(magic))
            {
                auto indices =
                    model.index_segment(*map.tags).data(vertex_magic);
                auto vertices =
                    model.vertex_segment(*map.tags).data(vertex_magic);

                if(!indices || !vertices)
                    continue;

                auto& d_data = draw_data.data.emplace_back();

                d_data.m_voff   = base_vertex;
                d_data.m_eoff   = base_index;
                d_data.m_elems  = indices.elements;
                d_data.m_eltype = semantic::TypeEnum::UShort;

                num_vertices += vertices.size;
                num_elements += indices.size;

                base_vertex += vertices.elements;
                base_index += indices.elements;
            }
    }

    data.scenery_buf->commit(num_vertices);
    data.scenery_index->commit(num_elements);

    auto vert_buffer =
        data.scenery_buf->map(0, num_vertices)
            .as<blam::vert::mod2_vertex<blam::vert::uncompressed>>();
    auto idx_buffer =
        data.scenery_index->map(0, num_elements).as<blam::vert::idx_t>();

    for(auto const& scenery : scenario->scenery.base.data(magic))
    {
        if(scenery.scenery_id >= scenery_refs.size())
            continue;

        auto instance = e.ref(e.create_entity(scenery_instance));

        {
            auto& world_pos    = instance.get<world_object_list::tag_type>();
            world_pos.position = scenery.pos;
            world_pos.rotation = scenery.rot;
        }

        {
            auto& scenery_info     = instance.get<scenery_list::tag_type>();
            scenery_info.spawn     = &scenery;
            scenery_info.draw_info = scenery_refs.at(scenery.scenery_id);
        }
    }

    num_elements = num_vertices = 0;

    for(auto const& scenery : scenario->scenery.ref.data(magic))
    {
        auto scenery_it = index.find(scenery[0]);

        if(scenery_it == index.end())
            continue;

        auto model_it = index.find((*scenery_it)
                                       ->to_reflexive<blam::scn::scenery>()
                                       .data(magic)[0]
                                       .model);

        if(model_it == index.end())
            continue;

        auto const& model =
            (*model_it)->to_reflexive<blam::mod2::header>().data(magic);

        for(auto const& geom : model[0].geometries.data(magic))
            for(auto const& model : geom.mesh_headers.data(magic))
            {
                auto indices =
                    model.index_segment(*map.tags).data(vertex_magic);
                auto vertices =
                    model.vertex_segment(*map.tags).data(vertex_magic);

                if(!indices || !vertices)
                    continue;

                MemCpy(indices, idx_buffer.at(num_elements));
                MemCpy(vertices, vert_buffer.at(num_vertices));

                num_vertices += vertices.size;
                num_elements += indices.size;
            }
    }

    data.scenery_buf->unmap();
    data.scenery_index->unmap();
}

i32 blam_main(i32, cstring_w*)
{
    auto blam_queue = RuntimeQueue::CreateNewQueue("Blam Graphics!");

    comp_app::app_error app_ec;
    auto&               e = comp_app::createContainer();
    comp_app::configureDefaults(e);

    auto loader = e.service<comp_app::AppLoader>();

    comp_app::addDefaults(e, *e.service<comp_app::AppLoader>(), app_ec);
    comp_app::AppContainer<BlamData>::addTo(
        e,
        [](EntityContainer& e, BlamData& data, time_point const&) {
            if(!FileExists(data.map_file))
                Throw(undefined_behavior("map file not found"));

            blam::map_container  map(data.map_file, blam::pc_version);
            blam::tag_index_view index(map);

            auto& magic = map.magic;

            data.scenario = map.tags->scenario(map.map)
                                .to_reflexive<blam::scn::scenario>()
                                .data(magic)
                                .data;
            {
                auto map_name      = map.map->full_mapname();
                auto window_config = e.service<comp_app::WindowInfo>();
                window_config->setName(
                    window_config->name() + " : " + map_name);
            }

            e.register_component<draw_info_list::tag_type>(
                MkUq<draw_info_list>());
            e.register_component<scenery_list::tag_type>(MkUq<scenery_list>());
            e.register_component<world_object_list::tag_type>(
                MkUq<world_object_list>());
            e.register_component<bsp_list::tag_type>(MkUq<bsp_list>());

            create_resources(e, data);

            auto& gfx      = e.subsystem_cast<GFX_ALLOC>();
            auto& pipeline = gfx.alloc_standard_pipeline<2>(
                {{"map.vert"_rsc, "map.frag"_rsc}});

            auto& scenery_pipeline = gfx.alloc_standard_pipeline<2>(
                {{"scenery.vert"_rsc, "scenery.frag"_rsc}});

            data.bsp_pipeline     = &pipeline;
            data.scenery_pipeline = &scenery_pipeline;

            auto const& view_u = *std::find_if(
                pipeline.constants_begin(),
                pipeline.constants_end(),
                pipeline.constant_by_name("view"));

            pipeline.set_constant(view_u, Bytes::From(data.camera_matrix));

            pipeline.build_state();

            load_scenario_bsp(e, data);
            load_scenario_scenery(e, data);

            GFX::RASTSTATE cull_disable;
            cull_disable.m_doCull = false;
            GFX::SetRasterizerState(cull_disable);

            GFX::DEPTSTATE depth_enable;
            depth_enable.m_test = true;
            GFX::SetDepthState(depth_enable);
        },
        [](EntityContainer& e,
           BlamData&        data,
           time_point const&,
           duration const&) {
            GFX::DefaultFramebuffer()->clear(0, {0.5f, 0.5f, 0.9f, 1.f}, 1.0);

            data.std_camera->tick();

            using namespace typing::vectors::scene;
            {
                data.camera.aspect =
                    e.service<comp_app::Windowing>()->size().aspect();
                data.camera.zVals = {0.01f, 1000.f};

                data.camera_matrix =
                    GenPerspective(data.camera) * GenTransform(data.camera) *
                    typing::vectors::matrixify(typing::vectors::normalize_quat(
                        Quatf(1, -pi / 4, 0, 0)));
            }

            GFX::ERROR ec;
            GFX::MultiDraw(data.bsp_pipeline->pipeline(), data.bsp_draw);

            for(auto const& scenery : e.select<scenery_list::tag_type>())
            {
                auto item = e.get<scenery_list::tag_type>(scenery.id);

                Matf4 mat =
                    data.camera_matrix * typing::vectors::translation<scalar>(
                                             Matf4(), item->spawn->pos);

                auto& draw_info = *item->draw_info;

                for(auto const& draw_ : draw_info.data)
                {
                    auto draw    = draw_;
                    draw.m_insts = 1;

                    data.scenery_pipeline->set_constant(
                        *std::find_if(
                            data.scenery_pipeline->constants_begin(),
                            data.scenery_pipeline->constants_end(),
                            data.scenery_pipeline->constant_by_name(
                                "transform")),
                        Bytes::From(mat));
                    data.scenery_pipeline->build_state();

                    GFX::Draw(
                        data.scenery_pipeline->pipeline(),
                        data.scenery_pipeline->get_state(),
                        *data.scenery_attr,
                        GFX::D_CALL().withIndexing().withTriStrip(),
                        draw);
                }
            }
        },
        [](EntityContainer& e, BlamData& data, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(blam_main)
