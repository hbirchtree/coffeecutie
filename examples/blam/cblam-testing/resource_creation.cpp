#include "resource_creation.h"

#include "components.h"
#include "touch_overlay.h"

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
            1024, std_camera_t::MouseInput(camera.std_camera));

        auto eventhandler_w = e.service<comp_app::BasicEventBus<Event>>();

        eventhandler_w->addEventFunction<ResizeEvent>(
            0, [&camera](Event&, ResizeEvent* resize) {
                camera.camera.aspect = static_cast<f32>(resize->w) / resize->h;
                cDebug("Window resize: {}x{}", resize->w, resize->h);
            });
    }

    gfx::api&      api       = e.subsystem_cast<gfx::system>();
    BlamResources& resources = e.register_subsystem_inplace<BlamResources>();

    resources.background_worker
        = rq::runtime_queue::CreateNewThreadQueue("Background Worker").value();

    e.register_subsystem_inplace<PostProcessParameters>();

    auto access = RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable;

    if constexpr(compile_info::platform::is_emscripten)
        access = RSCA::WriteOnly | RSCA::Immutable | RSCA::Discard;

    resources.bsp_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.bsp_buf->alloc();
    resources.bsp_buf->commit(memory_budget::bsp_buffer);
    resources.bsp_index = api.alloc_buffer(gfx::buffers::element, access);
    resources.bsp_index->alloc();
    resources.bsp_index->commit(memory_budget::bsp_elements);
    resources.bsp_light_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.bsp_light_buf->alloc();
    resources.bsp_light_buf->commit(memory_budget::bsp_buffer / 2);

    resources.model_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.model_buf->alloc();
    resources.model_buf->commit(memory_budget::mesh_buffer);
    resources.model_index = api.alloc_buffer(gfx::buffers::element, access);
    resources.model_index->alloc();
    resources.model_index->commit(memory_budget::mesh_elements);

    resources.model_matrix_store
        = api.alloc_buffer(gfx::buffers::constants, access);
    if(api.feature_info().buffer.ssbo)
    {
        //        resources.model_matrix_store
        //            = api.alloc_buffer(gfx::buffers::shader_writable, access);
        resources.material_store
            = api.alloc_buffer(gfx::buffers::shader_writable, access);
    } else if(api.feature_info().buffer.ubo)
    {
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

    resources.world_store = api.alloc_buffer(gfx::buffers::constants, access);
    resources.world_store->alloc();
    resources.world_store->commit(sizeof(materials::world_data));

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

    if constexpr(std::is_same_v<halo_version, blam::xbox_version_t>)
        common_attributes.at(1) = gfx::vertex_attribute::from_member(
            &model_vertex_type::texcoord, gfx::vertex_float_type);

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
        {"normal", 2},
        {"binormal", 3},
        {"tangent", 4},
    });

    resources.debug_lines = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.debug_lines->alloc();
    resources.debug_lines->commit(memory_budget::debug_buffer / 2);
    resources.debug_line_colors
        = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.debug_line_colors->alloc();
    resources.debug_line_colors->commit(memory_budget::debug_buffer / 2);

    {
        auto pos = resources.debug_lines->map<Vecf3>(0);
        auto col = resources.debug_line_colors->map<Vecf3>(0);
        pos[0]   = Vecf3(0);
        pos[1]   = Vecf3(10, 0, 0);
        pos[2]   = Vecf3(0);
        pos[3]   = Vecf3(0, 10, 0);
        pos[4]   = Vecf3(0);
        pos[5]   = Vecf3(0, 0, 10);

        col[0] = Vecf3(1, 0, 0);
        col[1] = Vecf3(0, 1, 0);
        col[2] = Vecf3(0, 0, 1);

        resources.debug_lines->unmap();
        resources.debug_line_colors->unmap();

        compo::EntityRecipe debug_draw;
        debug_draw.components = {
            compo::type_hash_v<DebugDraw>(),
        };
        auto  x_    = e.create_entity(debug_draw);
        auto  y_    = e.create_entity(debug_draw);
        auto  z_    = e.create_entity(debug_draw);
        auto& x     = x_.get<DebugDraw>();
        auto& y     = y_.get<DebugDraw>();
        auto& z     = z_.get<DebugDraw>();
        x.color_ptr = 0;
        x.data = gfx::draw_command::data_t{
            .arrays = {
                .count = 2,
                .offset = 0,
            },
        };
        y.color_ptr = 1;
        y.data = gfx::draw_command::data_t{
            .arrays = {
                .count = 2,
                .offset = 2,
            },
        };
        z.color_ptr = 2;
        z.data = gfx::draw_command::data_t{
            .arrays = {
                .count = 2,
                .offset = 4,
            },
        };

        auto& camera     = e.create_entity(debug_draw).get<DebugDraw>();
        camera.color_ptr = 3;
        camera.data = gfx::draw_command::data_t{
            .arrays = {
                .count = 6,
                .offset = 6,
            },
        };
        col[3] = Vecf3{1, 0, 1};
    }

    {
        struct debug_vertex
        {
            Vecf3 pos;
        };
        auto& debug_attr = resources.debug_attr;
        debug_attr       = api.alloc_vertex_array();
        debug_attr->alloc();
        debug_attr->add(gfx::vertex_attribute::from_member(&debug_vertex::pos));
        auto color_vtx = gfx::vertex_attribute::from_member(&debug_vertex::pos);
        color_vtx.index     = 1;
        color_vtx.buffer.id = 1;
        color_vtx.value.flags
            = gfx::vertex_attribute::attribute_flags::instanced;
        debug_attr->add(color_vtx);
        debug_attr->set_buffer(gfx::buffers::vertex, resources.debug_lines, 0);
        debug_attr->set_buffer(
            gfx::buffers::vertex, resources.debug_line_colors, 1);
        debug_attr->set_attribute_names({{"position", 0}, {"color", 1}});
    }

    //    if constexpr(compile_info::platform::is_android)
    {
        resources.offscreen = api.alloc_rendertarget();
        resources.color
            = api.alloc_texture(gfx::textures::d2, PixDesc(PixFmt::RGB16F), 1);
        resources.depth = api.alloc_texture(
            gfx::textures::d2, PixDesc(PixFmt::Depth32F), 1);
        resources.offscreen->alloc();
        auto const& size = resources.offscreen_size;
        resources.color->alloc(size_3d<i32>{size.x, size.y}.convert<u32>());
        resources.depth->alloc(size_3d<i32>{size.x, size.y}.convert<u32>());

        resources.offscreen->attach(
            gfx::render_targets::attachment::color, *resources.color, 0);
        resources.offscreen->attach(
            gfx::render_targets::attachment::depth, *resources.depth, 0);
        resources.offscreen->resize({0, 0, size.x, size.y});

        resources.color->set_swizzle(
            gfx::textures::swizzle_t::red,
            gfx::textures::swizzle_t::green,
            gfx::textures::swizzle_t::blue,
            gfx::textures::swizzle_t::one);

        if constexpr(compile_info::platform::is_emscripten)
            resources.offscreen = api.default_rendertarget();
    }
}

static void create_shader_program(
    gfx::api&                        api,
    Resource&&                       blob,
    std::string_view                 vertex_entrypoint,
    std::string_view                 fragment_entrypoint,
    std::shared_ptr<gfx::program_t>& result,
    std::string_view                 label)
{
    result = api.alloc_program();
    result->add(
        gleam::program_t::stage_t::Vertex,
        api.alloc_shader(
            gfx::shader_format_t::spv, blob.data(), {}, vertex_entrypoint));
    result->add(
        gleam::program_t::stage_t::Fragment,
        api.alloc_shader(
            gfx::shader_format_t::spv, blob.data(), {}, fragment_entrypoint));
    if(auto res = result->compile(); res.has_error())
    {
        auto [msg] = res.error();
        cFatal("Failed to compile {0} shader: {1}", label, msg);
        result = {};
    }
}

static void create_spv_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;

    auto _ = api.debug().scope();

    create_shader_program(
        api,
        "debug_lines.spv"_rsc,
        "debug_lines_main"sv,
        "debug_lines_main"sv,
        resources.debug_lines_pipeline,
        "debug_lines");
    create_shader_program(
        api,
        "scenery_uber.spv"_rsc,
        "scenery_main"sv,
        "scenery_uber_main"sv,
        resources.model_pipeline,
        "scenery");
    create_shader_program(
        api,
        "map_uber.spv"_rsc,
        "map_main"sv,
        "map_uber_main"sv,
        resources.bsp_pipeline,
        "bsp");
    create_shader_program(
        api,
        "map_uber.spv"_rsc,
        "map_main"sv,
        "map_uber_main"sv,
        resources.senv_micro_pipeline,
        "bsp_senv_micro");
    create_shader_program(
        api,
        "map_uber.spv"_rsc,
        "map_main"sv,
        "wireframe_main"sv,
        resources.wireframe_pipeline,
        "map_wireframe");
}

struct shader_pair_t
{
    std::string_view                 vertex_file;
    std::string_view                 fragment_file;
    std::shared_ptr<gfx::program_t>& shader;
};

template<size_t N>
static void create_shaders(
    gfx::api& api, std::array<shader_pair_t, N>&& shaders)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;

    std::string_view variant;

    if(api.api_type() == gfx::api_type_t::es)
    {
        if(api.api_version() == std::make_tuple(3u, 2u))
            variant = "es320"sv;
        else
            variant = "es300"sv;
    } else
    {
        if(api.api_version() == std::make_tuple(4u, 6u))
            variant = "core460"sv;
        else if(api.api_version() >= std::make_tuple(4u, 3u))
            variant = "core430"sv;
        else if(api.api_version() >= std::make_tuple(4u, 1u))
            variant = "core410"sv;
        else
            variant = "core330"sv;
    }

    if(variant.empty())
        Throw(std::runtime_error("no shader variant selected, good night"));

    for(shader_pair_t& shader : shaders)
    {
        if(shader.shader)
        {
            shader.shader->dealloc();
        }

        auto vertex_url = MkUrl(
            Strings::fmt("{0}.{1}.vert", shader.vertex_file, variant),
            RSCA::AssetFile);
        auto fragment_url = MkUrl(
            Strings::fmt("{0}.{1}.frag", shader.fragment_file, variant),
            RSCA::AssetFile);

        auto pipeline = shader.shader ? shader.shader : api.alloc_program();
        pipeline->add(
            gfx::program_t::stage_t::Vertex,
            api.alloc_shader(Resource(vertex_url).data()));
        pipeline->add(
            gfx::program_t::stage_t::Fragment,
            api.alloc_shader(Resource(fragment_url).data()));
        if(auto res = pipeline->compile(); res.has_error())
        {
            auto [msg] = res.error();
            cFatal("Failed to compile shader: {0}", msg);
        }
        shader.shader = pipeline;
    }
}

static void create_uber_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;

    std::array<shader_pair_t, 4> shaders = {{
        {
            .vertex_file   = "debug_lines"sv,
            .fragment_file = "debug_lines"sv,
            .shader        = resources.debug_lines_pipeline,
        },
        {
            .vertex_file   = "scenery"sv,
            .fragment_file = "scenery_uber"sv,
            .shader        = resources.model_pipeline,
        },
        {
            .vertex_file   = "map"sv,
            .fragment_file = "map_uber"sv,
            .shader        = resources.bsp_pipeline,
        },
        {
            .vertex_file   = "map"sv,
            .fragment_file = "wireframe"sv,
            .shader        = resources.wireframe_pipeline,
        },
    }};

    create_shaders(api, std::move(shaders));

    resources.senv_micro_pipeline = resources.bsp_pipeline;
}

static void create_uber_lite_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;

    std::array<shader_pair_t, 4> shaders = {{
        {
            .vertex_file   = "debug_lines"sv,
            .fragment_file = "debug_lines"sv,
            .shader        = resources.debug_lines_pipeline,
        },
        {
            .vertex_file   = "scenery"sv,
            .fragment_file = "scenery_uber_lite"sv,
            .shader        = resources.model_pipeline,
        },
        {
            .vertex_file   = "map"sv,
            .fragment_file = "map_uber_lite"sv,
            .shader        = resources.bsp_pipeline,
        },
        {
            .vertex_file   = "map"sv,
            .fragment_file = "wireframe"sv,
            .shader        = resources.wireframe_pipeline,
        },
    }};

    create_shaders(api, std::move(shaders));

    resources.senv_micro_pipeline = resources.bsp_pipeline;
}

static void create_standard_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;

    std::array<shader_pair_t, 5> shaders = {{
        {
            .vertex_file   = "debug_lines"sv,
            .fragment_file = "debug_lines"sv,
            .shader        = resources.debug_lines_pipeline,
        },
        {
            .vertex_file   = "scenery"sv,
            .fragment_file = "scenery"sv,
            .shader        = resources.model_pipeline,
        },
        {
            .vertex_file   = "map"sv,
            .fragment_file = "map"sv,
            .shader        = resources.bsp_pipeline,
        },
        {
            .vertex_file   = "map"sv,
            .fragment_file = "map_senv"sv,
            .shader        = resources.senv_micro_pipeline,
        },
        {
            .vertex_file   = "map"sv,
            .fragment_file = "wireframe"sv,
            .shader        = resources.wireframe_pipeline,
        },
    }};

    create_shaders(api, std::move(shaders));
}

void create_shaders(compo::EntityContainer& e)
{
    gfx::api&      gfx       = e.subsystem_cast<gfx::system>();
    BlamResources& resources = e.subsystem_cast<BlamResources>();

    auto _ = gfx.debug().scope();

    auto const& features = gfx.feature_info();
    auto const& bugs     = gfx.workarounds().bugs;

    const bool use_spv  = features.program.spirv;
    const bool use_uber = features.texture.cube_array && features.buffer.ssbo
                          && !lowspec_hardware && !bugs.adreno_3xx;
    const bool use_uber_lite = features.buffer.ubo && !bugs.adreno_3xx;

    if(use_spv && false)
    {
        create_spv_shaders(gfx, resources);
        return;
    }

    if(use_uber)
    {
        create_uber_shaders(gfx, resources);
        return;
    }

    if(use_uber_lite)
    {
        create_uber_lite_shaders(gfx, resources);
        return;
    }

    create_standard_shaders(gfx, resources);
}

void set_resource_labels(EntityContainer& e)
{
    if constexpr(!compile_info::debug_mode)
        return;
    gfx::api&        api       = e.subsystem_cast<gfx::system>();
    gfx::debug::api& debug     = api.debug();
    BlamResources&   resources = e.subsystem_cast<BlamResources>();

    debug.annotate(*resources.bsp_pipeline, "map_basic");
    debug.annotate(*resources.senv_micro_pipeline, "map_senv");
    debug.annotate(*resources.model_pipeline, "scenery");
    debug.annotate(*resources.debug_lines_pipeline, "debug_lines");
    debug.annotate(*resources.wireframe_pipeline, "wireframe");

    debug.annotate(*resources.bsp_attr, "bsp_vao");
    debug.annotate(*resources.bsp_buf, "bsp_vertex_buf");
    debug.annotate(*resources.bsp_light_buf, "bsp_light_buf");
    debug.annotate(*resources.bsp_index, "bsp_index_buf");

    debug.annotate(*resources.model_attr, "model_vao");
    debug.annotate(*resources.model_buf, "model_vertex_buf");
    debug.annotate(*resources.model_index, "model_index_buf");

    debug.annotate(*resources.material_store, "material_buffer");
    debug.annotate(*resources.model_matrix_store, "model_matrices");

    debug.annotate(*resources.debug_attr, "debug_vao");
    debug.annotate(*resources.debug_lines, "debug_vertices");

    if(api.default_rendertarget() != resources.offscreen)
        debug.annotate(*resources.offscreen, "offscreen");
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
            camera.camera.rotation = glm::normalize(glm::quat(Vecf3{
                0,
                location.rot,
                //                0,
                glm::pi<f32>() / 2.f,
            }));
        }
    }

    camera.controller_opts.sens.move = {.1f, .1f};
    camera.camera_opts.accel.alt     = 50.f;
}
