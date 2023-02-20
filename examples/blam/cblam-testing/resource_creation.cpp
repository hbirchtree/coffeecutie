#include "resource_creation.h"

#include "components.h"

#include <coffee/core/input/eventhandlers.h>

using Coffee::ProfContext;
using semantic::RSCA;

void create_resources(compo::EntityContainer& e)
{
    ProfContext _(__FUNCTION__);

    BlamCamera& camera = e.register_subsystem_inplace<BlamCamera>();

    {
        using namespace Coffee::Display;
        using namespace Coffee::Display::EventHandlers;
        using namespace Coffee;

        auto eventhandler
            = e.service<comp_app::BasicEventBus<Input::CIEvent>>();

        eventhandler->addEventHandler(
            1024,
            ExitOn<OnKey<Input::CK_Escape>>(
                [window = e.service_ref<comp_app::Windowing>()]() mutable {
                    window.lock()->close();
                }));

        eventhandler->addEventHandler(
            1024, std_camera_t::KeyboardInput(camera.std_camera));
        eventhandler->addEventHandler(
            1024, std_camera_t::MouseInput(&camera.camera));

        auto eventhandler_w = e.service<comp_app::BasicEventBus<Event>>();

        eventhandler_w->addEventFunction<ResizeEvent>(
            0, [&camera](Event&, ResizeEvent* resize) {
                camera.camera.aspect = static_cast<f32>(resize->w) / resize->h;
            });
    }

    gfx::api&      api       = e.subsystem_cast<gfx::system>();
    BlamResources& resources = e.register_subsystem_inplace<BlamResources>();

    auto access = RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable;

    resources.bsp_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.bsp_buf->alloc();
    resources.bsp_buf->commit(memory_budget::bsp_buffer);
    resources.bsp_index = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.bsp_index->alloc();
    resources.bsp_index->commit(memory_budget::bsp_elements);
    resources.bsp_light_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.bsp_light_buf->alloc();
    resources.bsp_light_buf->commit(memory_budget::bsp_buffer / 2);

    resources.model_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.model_buf->alloc();
    resources.model_buf->commit(memory_budget::mesh_buffer);
    resources.model_index = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.model_index->alloc();
    resources.model_index->commit(memory_budget::mesh_elements);

    if(api.feature_info().buffer.ssbo)
    {
        resources.model_matrix_store
            = api.alloc_buffer(gfx::buffers::shader_writable, access);
        resources.material_store
            = api.alloc_buffer(gfx::buffers::shader_writable, access);
    } else if(api.feature_info().buffer.ubo)
    {
        resources.model_matrix_store
            = api.alloc_buffer(gfx::buffers::constants, access);
        resources.material_store
            = api.alloc_buffer(gfx::buffers::constants, access);
    } else
    {
        resources.model_matrix_store
            = api.alloc_buffer(gfx::buffers::vertex, access);
        resources.material_store
            = api.alloc_buffer(gfx::buffers::vertex, access);
    }
    resources.model_matrix_store->alloc();
    resources.model_matrix_store->commit(memory_budget::matrix_buffer);
    resources.material_store->alloc();
    resources.material_store->commit(memory_budget::material_buffer);

    resources.bsp_attr             = api.alloc_vertex_array();
    gfx::vertex_array_t& bsp_array = *resources.bsp_attr;
    bsp_array.alloc();

    using vertex_type = std::conditional_t<
        std::is_same_v<halo_version, blam::xbox_version_t>,
        ::xbox_vertex_type,
        ::vertex_type>;
    using light_vertex_type = std::conditional_t<
        std::is_same_v<halo_version, blam::xbox_version_t>,
        ::light_xbox_vertex_type,
        ::light_vertex_type>;

    std::array<gfx::vertex_attribute, 6> common_attributes = {{
        gfx::vertex_attribute::from_member(&vertex_type::position),
        gfx::vertex_attribute::from_member(&vertex_type::texcoord),
        gfx::vertex_attribute::from_member(&vertex_type::normal),
        gfx::vertex_attribute::from_member(&vertex_type::binorm),
        gfx::vertex_attribute::from_member(&vertex_type::tangent),
        gfx::vertex_attribute::from_member(
            &light_vertex_type::texcoord, gfx::vertex_float_type),
    }};

    {
        auto& light     = common_attributes.back();
        light.buffer.id = 1;
        if constexpr(std::is_same_v<halo_version, blam::xbox_version_t>)
            light.value.flags
                = gfx::vertex_attribute::attribute_flags::normalized
                  | gfx::vertex_attribute::attribute_flags::packed;
        else
            light.value.flags = gfx::vertex_attribute::attribute_flags::none;
    }
    for(auto i : Range<u32>(6))
    {
        common_attributes.at(i).index = i;
        bsp_array.add(common_attributes.at(i));
    }
    bsp_array.set_buffer(gfx::buffers::vertex, resources.bsp_buf, 0);
    bsp_array.set_buffer(gfx::buffers::vertex, resources.bsp_light_buf, 1);
    bsp_array.set_buffer(gfx::buffers::element, resources.bsp_index);
    bsp_array.set_attribute_names({
        {"position", 0},
        {"tex", 1},
        {"normal", 2},
        {"binormal", 3},
        {"tangent", 4},
        {"light_tex", 5},
    });

    using model_vertex_type = std::conditional_t<
        std::is_same_v<halo_version, blam::xbox_version_t>,
        blam::vert::mod2_vertex<blam::vert::compressed>,
        blam::vert::mod2_vertex<blam::vert::uncompressed>>;

    resources.model_attr            = api.alloc_vertex_array();
    gfx::vertex_array_t& mod2_array = *resources.model_attr;
    mod2_array.alloc();

    common_attributes = {{
        gfx::vertex_attribute::from_member(&model_vertex_type::position),
        gfx::vertex_attribute::from_member(&model_vertex_type::texcoord),
        gfx::vertex_attribute::from_member(&model_vertex_type::normal),
        gfx::vertex_attribute::from_member(&model_vertex_type::binorm),
        gfx::vertex_attribute::from_member(&model_vertex_type::tangent),
    }};

    for(auto i : Range<u32>(5))
    {
        common_attributes.at(i).index = i;
        mod2_array.add(common_attributes.at(i));
    }
    mod2_array.set_buffer(gfx::buffers::vertex, resources.model_buf, 0);
    mod2_array.set_buffer(gfx::buffers::element, resources.model_index);
    mod2_array.set_attribute_names({
        {"position", 0},
        {"tex", 1},
    });

    resources.debug_lines = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.debug_lines->alloc();
    resources.debug_lines->commit(memory_budget::debug_buffer / 2);
    {
        struct debug_vertex
        {
            Vecf3 pos;
        };
        auto& debug_attr = resources.debug_attr;
        debug_attr       = api.alloc_vertex_array();
        debug_attr->alloc();
        debug_attr->add(gfx::vertex_attribute::from_member(&debug_vertex::pos));
        debug_attr->set_buffer(gfx::buffers::vertex, resources.debug_lines, 0);
        debug_attr->set_attribute_names({{"position", 0}});
    }

    //    if constexpr(compile_info::platform::is_android)
    {
        resources.offscreen = api.alloc_rendertarget();
        resources.color
            = api.alloc_texture(gfx::textures::d2, PixDesc(PixFmt::RGB16F), 1);
        resources.depth = api.alloc_texture(
            gfx::textures::d2, PixDesc(PixFmt::Depth32F), 1);
        resources.offscreen->alloc();
        auto size_u = resources.offscreen_size.convert<u32>();
        resources.color->alloc({size_u.w, size_u.h});
        resources.depth->alloc({size_u.w, size_u.h});

        resources.offscreen->attach(
            gfx::render_targets::attachment::color, *resources.color, 0);
        resources.offscreen->attach(
            gfx::render_targets::attachment::depth, *resources.depth, 0);
        auto& size_i = resources.offscreen_size;
        resources.offscreen->resize({0, 0, size_i.w, size_i.h});
    }
}

void create_shaders(compo::EntityContainer& e)
{
    gfx::api&      gfx       = e.subsystem_cast<gfx::system>();
    BlamResources& resources = e.subsystem_cast<BlamResources>();

    const bool use_old_shaders
        = gfx.api_version() != std::make_tuple<u32>(4, 6);
    const bool use_es_shaders = gfx.api_type() == gleam::api_type_t::es;
    auto       map_shader     = use_es_shaders    ? "map.es320.vert"_rsc
                                : use_old_shaders ? "map.410.vert"_rsc
                                                  : "map.vert"_rsc;
    {
        auto pipeline = gfx.alloc_program();
        pipeline->add(
            gfx::program_t::stage_t::Vertex,
            gfx.alloc_shader(map_shader.data()));
        pipeline->add(
            gfx::program_t::stage_t::Fragment,
            gfx.alloc_shader((use_es_shaders    ? "map.frag"_rsc
                              : use_old_shaders ? "map.410.frag"_rsc
                                                : "map.frag"_rsc)
                                 .data())
            //
        );
        if(auto res = pipeline->compile(); res.has_error())
        {
            auto [msg] = res.error();
            cFatal("Failed to compile bsp shader: {0}", msg);
        }
        resources.bsp_pipeline = pipeline;
    }
    {
        auto pipeline = gfx.alloc_program();
        pipeline->add(
            gfx::program_t::stage_t::Vertex,
            gfx.alloc_shader((use_es_shaders    ? "scenery.es320.vert"_rsc
                              : use_old_shaders ? "scenery.410.vert"_rsc
                                                : "scenery.vert"_rsc)
                                 .data()));
        pipeline->add(
            gfx::program_t::stage_t::Fragment,
            //                    gfx.alloc_shader("white.frag"_rsc.data())
            gfx.alloc_shader((use_es_shaders    ? "white.es.frag"_rsc
                              : use_old_shaders ? "scenery.410.frag"_rsc
                                                : "scenery.frag"_rsc)
                                 .data())
            //
        );
        if(auto res = pipeline->compile(); res.has_error())
        {
            auto [msg] = res.error();
            cFatal("Failed to compile scenery shader: {0}", msg);
        }
        resources.model_pipeline = pipeline;
    }
    {
        auto pipeline = gfx.alloc_program();
        pipeline->add(
            gfx::program_t::stage_t::Vertex,
            gfx.alloc_shader(map_shader.data()));
        pipeline->add(
            gfx::program_t::stage_t::Fragment,
            gfx.alloc_shader(
                //                        "white.frag"_rsc.data(),
                //                                            gfx.alloc_shader(
                (use_es_shaders    ? "map_senv.frag"_rsc
                 : use_old_shaders ? "map_senv.410.frag"_rsc
                                   : "map_senv.frag"_rsc)
                    .data(),
                {{"MICRO_BLEND", "1"}, {"PRIMARY_BLEND", "1"}}));
        if(auto res = pipeline->compile(); res.has_error())
        {
            auto [msg] = res.error();
            cFatal("Failed to compile senv shader: {0}", msg);
        }
        resources.senv_micro_pipeline = pipeline;
    }
    {
        auto pipeline = gfx.alloc_program();
        pipeline->add(
            gfx::program_t::stage_t::Vertex,
            gfx.alloc_shader(map_shader.data()));
        pipeline->add(
            gfx::program_t::stage_t::Fragment,
            //                    gfx.alloc_shader("white.frag"_rsc.data())
            gfx.alloc_shader((use_es_shaders    ? "white.es.frag"_rsc
                              : use_old_shaders ? "wireframe.410.frag"_rsc
                                                : "wireframe.frag"_rsc)
                                 .data())
            //
        );
        if(auto res = pipeline->compile(); res.has_error())
        {
            auto [msg] = res.error();
            cFatal("Failed to compile wireframe shader: {0}", msg);
        }
        resources.debug_lines_pipeline = pipeline;
    }

    {
        auto pipeline = gfx.alloc_program();
        pipeline->add(
            gfx::program_t::stage_t::Vertex,
            gfx.alloc_shader("debug_lines.vert"_rsc.data()));
        pipeline->add(
            gfx::program_t::stage_t::Fragment,
            gfx.alloc_shader("debug_lines.frag"_rsc.data()));
        if(auto res = pipeline->compile(); res.has_error())
        {
            auto [msg] = res.error();
            cFatal("Failed to compile debug lines shader: {0}", msg);
        }
        resources.debug_lines_pipeline = pipeline;
    }
}

void create_camera(
    compo::EntityContainer&                              e,
    semantic::Span<const blam::scn::player_spawn> const& spawns)
{
    BlamCamera& camera = e.subsystem_cast<BlamCamera>();

    /* Move the camera to a player spawn location */
    {
        //        auto transform
        //            =
        //            typing::vectors::matrixify(typing::vectors::normalize_quat(
        //                  Quatf(1, -math::pi_f / 4, 0, 0)))
        //              * typing::vectors::scale(Matf4(), {10})
        //              ;

        if(!spawns.empty())
        {
            auto& location         = spawns[0];
            camera.camera.position = location.pos * Vecf3(-1);
            //                = (Vecf4(location.pos + Vecf3(0, 0, 1), 1) /**
            //                -1.f*/) * transform;
            // TODO: Fix facing of camera here
            cDebug("Facing of player: {0}", location.rot);
            camera.camera.rotation = typing::vectors::euler(Vecf3{
                0,
                location.rot,
                -math::pi / 2.f,
            });
        }
    }

    camera.std_camera->up_direction = {0, 0, 1};
}
