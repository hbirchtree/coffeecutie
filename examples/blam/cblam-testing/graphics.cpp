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

using cache_id_t = u64;

struct generation_idx_t
{
    generation_idx_t() : i(0), gen(0)
    {
    }
    generation_idx_t(cache_id_t i, u32 gen) : i(i), gen(gen)
    {
    }

    cache_id_t i;
    u32        gen;
    u32        _pad;

    inline bool valid() const
    {
        return i != 0 && gen != 0;
    }
};

template<typename T, typename... IType>
struct DataCache
{
    static constexpr cache_id_t invalid_id = 0;

    DataCache() : counter(0), generation(1)
    {
    }

    inline generation_idx_t predict(IType... param)
    {
        auto out  = ++counter;
        auto item = predict_impl(param...);

        if(!item.valid())
            return {invalid_id, 0};

        m_cache.insert({out, item});
        return {out, generation};
    }

    auto find(generation_idx_t id) const
    {
        if(id.gen < generation)
            Throw(undefined_behavior("stale reference"));

        return m_cache.find(id.i);
    }

    void evict_all()
    {
        evict_impl();

        m_cache.clear();
        counter = 0;
        generation++;
    }

    cache_id_t         counter;
    Map<cache_id_t, T> m_cache;
    u32                generation;

    virtual T    predict_impl(IType... param) = 0;
    virtual void evict_impl()
    {
    }
};

struct BSPItem
{
    blam::scn::bsp::submesh_header const* mesh;
    GFX::D_DATA                           draw;

    inline bool valid() const
    {
        return mesh;
    }
};

struct ModelItem
{
    struct SubModel
    {
        blam::mod2::submesh_header const* header;
        GFX::D_DATA                       draw;
    };
    struct LOD
    {
        blam::mod2::geometry_header const* header;
        Vector<SubModel>                   sub;
    };

    blam::mod2::header const* header;
    Array<LOD, 5>             lod_meshes;

    inline bool valid() const
    {
        return header;
    }
};

struct SceneryItem
{
    blam::scn::scenery const* scenery;
    generation_idx_t          model; /* Reference to cached model */

    inline bool valid() const
    {
        return scenery;
    }
};

struct BitmapItem
{
    blam::bitm::header_t const* header;
};

struct BitmapCache : DataCache<BitmapItem, blam::tagref_t const&>
{
};

struct BSPCache : DataCache<BSPItem, blam::scn::bsp::info const&>
{
    Bytes vert_buffer, element_buffer;
    szptr vert_ptr, element_ptr;

    virtual BSPItem predict_impl(blam::scn::bsp::info const& param) override
    {
        return BSPItem();
    }
};

struct ModelCache : DataCache<ModelItem, blam::tagref_t const&, u16>
{
    ModelCache(blam::map_container const& map) :
        tags(map.tags), index(map), magic(map.magic),
        vertex_magic(tags->vertex_magic(magic)), vert_ptr(0), element_ptr(0)
    {
    }

    blam::tag_index_t const* tags;
    blam::tag_index_view     index;
    blam::magic_data_t       magic;
    blam::magic_data_t       vertex_magic;

    Bytes vert_buffer, element_buffer;
    u32   vert_ptr, element_ptr;

    virtual ModelItem predict_impl(
        blam::tagref_t const& param, u16 lod) override
    {
        using namespace blam::mod2;

        auto header_it = index.find(param);

        if(header_it == index.end())
            return ModelItem();

        auto header =
            (*header_it)->to_reflexive<blam::mod2::header>().data(magic);

        ModelItem out;
        out.lod_meshes = {};
        out.header     = &header[0];

        u32  lod_i      = 0;
        auto lod_levels = header[0].geometries.data(magic);

        for(auto const& geom : lod_levels)
        {
            out.lod_meshes[lod_i].header = &geom;

            for(auto const& model : geom.mesh_headers.data(magic))
            {
                auto elements = model.index_segment(*tags).data(vertex_magic);
                auto vertices = model.vertex_segment(*tags).data(vertex_magic);

                using element_type =
                    std::remove_const<decltype(elements)::value_type>::type;
                using vertex_type =
                    std::remove_const<decltype(vertices)::value_type>::type;

                if(!elements || !vertices)
                    Throw(undefined_behavior(
                        "failed to locate element or vertex data"));

                auto& draw_data  = out.lod_meshes[lod_i].sub.emplace_back();
                draw_data.header = &model;
                draw_data.draw.m_eltype = semantic::TypeEnum::UShort;
                draw_data.draw.m_eoff = element_ptr / sizeof(blam::vert::idx_t);
                draw_data.draw.m_elems = C_FCAST<u32>(elements.elements);
                draw_data.draw.m_voff =
                    vert_ptr / sizeof(decltype(vertices)::value_type);
                draw_data.draw.m_insts = 1;

                MemCpy(vertices, vert_buffer.at(vert_ptr).as<vertex_type>());
                MemCpy(
                    elements,
                    element_buffer.at(element_ptr).as<element_type>());

                vert_ptr += vertices.size;
                element_ptr += vertices.size;
            }
            lod_i++;
        }

        return out;
    }
    virtual void evict_impl() override
    {
        vert_ptr    = 0;
        element_ptr = 0;
    }
};

struct SceneryCache : DataCache<SceneryItem, blam::tagref_t const&>
{
    SceneryCache(blam::map_container const& map, ModelCache& model_cache) :
        model_cache(model_cache), magic(map.magic), index(map)
    {
    }

    ModelCache&          model_cache;
    blam::magic_data_t   magic;
    blam::tag_index_view index;

    Map<i16, generation_idx_t> scenery_to_cache;

    virtual SceneryItem predict_impl(blam::tagref_t const& tag) override
    {
        auto it = index.find(tag);

        if(it == index.end())
            return {};

        auto scenery = (*it)->to_reflexive<blam::scn::scenery>().data(magic);

        if(scenery.elements != 1)
            Throw(undefined_behavior("no scenery found"));

        SceneryItem out;
        out.scenery = &scenery[0];
        out.model   = model_cache.predict(
            scenery[0].model.to_plain(), blam::mod2::lod_low_ext);

        if(!out.model.valid())
            return {};

        return out;
    }

    SceneryItem const* get_scenery(i16 id) const
    {
        auto map_it = scenery_to_cache.find(id);

        if(map_it == scenery_to_cache.end())
            return nullptr;

        auto it = find(map_it->second);

        if(it == m_cache.end())
            Throw(undefined_behavior(
                "item found in scenery mapping, but not in cache, stale ID?"));

        return &it->second;
    }
};

struct BlamData
{
    BlamData() :
        map_file(MkUrl(GetInitArgs().arguments().at(0), RSCA::SystemFile)),
        map_container(map_file, blam::pc_version), bsp_cache(),
        model_cache(map_container), scenery_cache(map_container, model_cache),
        std_camera(MkShared<std_camera_t>(&camera, &camera_opts))
    {
    }

    /* Blam! data */
    Resource                   map_file;
    blam::map_container        map_container;
    blam::scn::scenario const* scenario;

    BSPCache            bsp_cache;
    Vector<GFX::D_DATA> bsps;
    ShPtr<GFX::BUF_A>   bsp_buf;
    ShPtr<GFX::BUF_E>   bsp_index;
    ShPtr<GFX::V_DESC>  bsp_attr;
    PIP_PARAM*          bsp_pipeline;
    GFX::RenderPass     bsp_pass;
    GFX::OPT_DRAW       bsp_draw;

    ModelCache         model_cache;
    SceneryCache       scenery_cache;
    ShPtr<GFX::BUF_A>  scenery_buf;
    ShPtr<GFX::BUF_E>  scenery_index;
    ShPtr<GFX::V_DESC> scenery_attr;
    PIP_PARAM*         scenery_pipeline;
    Vector<Matf4>      scenery_mats;
    GFX::RenderPass    scenery_pass;
    GFX::OPT_DRAW      scenery_draw;
    ShPtr<GFX::BUF_S>  scenery_matrix_store;

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

    data.scenery_matrix_store = gfx.alloc_buffer<GFX::BUF_S>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, sizeof(Matf4), 0);

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

    data.scenery_buf->commit(50_MB);
    data.scenery_index->commit(25_MB);

    data.scenery_matrix_store->commit(10_MB);
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
    data.model_cache.vert_buffer    = data.scenery_buf->map();
    data.model_cache.element_buffer = data.scenery_index->map();

    auto scenario = data.scenario;
    auto magic    = data.map_container.magic;

    auto pipeline = data.scenery_pipeline;

    pipeline->build_state();

    i16 scenery_id = 0;
    for(auto const& scenery : scenario->scenery.ref.data(magic))
    {
        auto id = data.scenery_cache.predict(scenery[0]);

        if(!id.valid())
            continue;

        data.scenery_cache.scenery_to_cache.insert({scenery_id++, id});
    }

    GFX::RenderPass& scenery_pass = data.scenery_pass;
    scenery_pass.pipeline         = data.scenery_pipeline->pipeline_ptr();

    for(auto const& instance : scenario->scenery.base.data(magic))
    {
        auto const& scenery =
            data.scenery_cache.get_scenery(instance.scenery_id);

        if(!scenery)
            continue;

        auto const& model = (*data.model_cache.find(scenery->model)).second;

        for(auto const& mesh : model.lod_meshes[0].sub)
        {
            data.scenery_mats.push_back(
                typing::vectors::translation(Matf4(), instance.pos));
            scenery_pass.draws.push_back(
                {data.scenery_attr,
                 &data.scenery_pipeline->get_state(),
                 GFX::D_CALL().withIndexing().withTriStrip().withInstancing(),
                 mesh.draw});
        }
    }

    data.scenery_buf->unmap();
    data.scenery_index->unmap();

    GFX::OptimizeRenderPass(scenery_pass, data.scenery_draw);

    Vector<Matf4> matrices_realloc = std::move(data.scenery_mats);
    data.scenery_mats.resize(matrices_realloc.size());

    auto matrix_store = data.scenery_matrix_store->map().as<Matf4>();

    u32 mat_idx = 0;
    for(auto const& draw : scenery_pass.draws)
    {
        matrix_store[draw.d_data.m_ioff] = matrices_realloc.at(mat_idx);
        mat_idx++;
    }

    data.scenery_matrix_store->unmap();

    GFX::ERROR ec;
    data.scenery_matrix_store->bindrange(0, 0, 10_MB, ec);

    pipeline->set_constant(
        *std::find_if(
            pipeline->constants_begin(),
            pipeline->constants_end(),
            pipeline->constant_by_name("camera")),
        Bytes::From(data.camera_matrix));
    //    pipeline->set_constant(
    //        *std::find_if(
    //            pipeline->constants_begin(),
    //            pipeline->constants_end(),
    //            pipeline->constant_by_name("transform")),
    //        Bytes::CreateFrom(data.scenery_mats));
    pipeline->build_state();
    pipeline->get_state();
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
            GFX::MultiDraw(data.bsp_pipeline->pipeline(), data.bsp_draw, ec);
            C_ERROR_CHECK(ec)

            GFX::MultiDraw(
                data.scenery_pipeline->pipeline(), data.scenery_draw, ec);
            C_ERROR_CHECK(ec)

            //            for(auto const& scenery :
            //            e.select<scenery_list::tag_type>())
            //            {
            //                auto item =
            //                e.get<scenery_list::tag_type>(scenery.id);

            //                Matf4 mat =
            //                    data.camera_matrix *
            //                    typing::vectors::translation<scalar>(
            //                                             Matf4(),
            //                                             item->spawn->pos);

            //                auto& draw_info = *item->draw_info;

            //                for(auto const& draw_ : draw_info.data)
            //                {
            //                    auto draw    = draw_;
            //                    draw.m_insts = 1;

            //                    data.scenery_pipeline->set_constant(
            //                        *std::find_if(
            //                            data.scenery_pipeline->constants_begin(),
            //                            data.scenery_pipeline->constants_end(),
            //                            data.scenery_pipeline->constant_by_name(
            //                                "transform")),
            //                        Bytes::From(mat));
            //                    data.scenery_pipeline->build_state();

            //                    GFX::Draw(
            //                        data.scenery_pipeline->pipeline(),
            //                        data.scenery_pipeline->get_state(),
            //                        *data.scenery_attr,
            //                        GFX::D_CALL().withIndexing().withTriStrip(),
            //                        draw);
            //                }
            //            }
        },
        [](EntityContainer& e, BlamData& data, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(blam_main)
