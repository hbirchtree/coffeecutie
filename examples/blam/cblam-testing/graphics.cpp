#include "data.h"
#include "map_loading.h"
#include "occluder.h"
#include "rendering.h"
#include "resource_creation.h"
#include "script_component.h"
#include "selected_version.h"
#include "touch_overlay.h"
#include "ui.h"
#include "ui_caching.h"

#include <coffee/comp_app/fps_counter.h>
#include <coffee/core/coffee_args.h>
#include <coffee/graphics/apis/gleam/rhi_emulation.h>

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/net_profiling.h>
#endif

using namespace Coffee;

void install_imgui_widgets(
    compo::EntityContainer& e, std::function<void(Url const&)>&& map_select);

i32 blam_main()
{
    cxxopts::ParseResult arguments;
    if constexpr(
        !compile_info::platform::is_android
        && !compile_info::platform::is_emscripten)
    {
        cxxopts::Options options(
            "Blam! Graphics", "A prototype for a Blam! engine");
        Coffee::BaseArgParser::GetBase(options);
        options.positional_help("map name or directory");
        auto& args = GetInitArgs();
        arguments  = options.parse(args.size(), args.data());
        if(BaseArgParser::PerformDefaults(options, args) >= 0)
            return 0;
    }

    rq::runtime_queue::CreateNewQueue("Blam Graphics!").assume_value();
#if defined(FEATURE_ENABLE_ASIO)
    C_UNUSED(auto _ = Net::RegisterProfiling());
#endif

    comp_app::app_error app_ec;

    auto& e      = comp_app::createContainer();
    auto& loader = comp_app::configureDefaults(e);

    auto& window = loader.config<comp_app::WindowConfig>();
    window.flags = comp_app::window_flags_t::windowed
                   | comp_app::window_flags_t::resizable;
    if constexpr(compile_info::platform::is_emscripten)
        window.flags = comp_app::window_flags_t::resizable;
    auto& touch = loader.config<comp_app::TouchConfig>();
    touch.options |= comp_app::TouchConfig::TouchToMouse;

#if defined(SELECT_API_OPENGL)
    auto& glConfig        = loader.config<comp_app::GLConfig>();
    glConfig.swapInterval = 1;
    if constexpr(
        compile_info::debug_mode && !compile_info::platform::is_emscripten)
        glConfig.profile |= comp_app::GLConfig::Debug;
#endif

    cDebug(
        "Buffer budget: {0} / {1} MB",
        memory_budget::grand_total,
        memory_budget::grand_total / Unit_MB);

    comp_app::addDefaults(e, *e.service<comp_app::AppLoader>(), app_ec);
    comp_app::AppContainer<BlamData<halo_version>>::addTo(
        e,
        [arguments](
            EntityContainer&        e,
            BlamData<halo_version>& data,
            time_point const&) {
            ProfContext _(__FUNCTION__);

            auto& gfx        = e.register_subsystem_inplace<gfx::system>();
            auto  load_error = gfx.load(
                // gfx::emulation::webgl::desktop()
                // gfx::emulation::qcom::adreno_320()
                // gfx::emulation::arm::mali_g710()
                // gfx::emulation::amd::rx560_pro()
            );

            if(load_error)
            {
                cWarning(
                    "Failed to initialize gfx::api: {0}",
                    magic_enum::enum_name(load_error.value()));
            }

            gfx.collect_info(*e.service<comp_app::AppInfo>());

            gfx.debug().enable();
            gfx.debug().add_callback([](gfx::group::debug_severity sev,
                                        std::string_view const&    msg) {
                if(sev == gfx::group::debug_severity::notification)
                    return;
                cDebug("GL: {0}", msg);
            });
            cDebug("GL version: {0} {1}", gfx.api_name(), gfx.api_version());
            {
                auto size = e.service<comp_app::Windowing>()->size();
                cDebug("Window size: {0}x{1}", size.w, size.h);
            }
            cDebug("GL extensions: {0}", gfx.extensions());

            e.register_component_inplace<Model>();
            e.register_component_inplace<SubModel>();
            e.register_component_inplace<BspReference>();
            e.register_component_inplace<ObjectSpawn>();
            e.register_component_inplace<MultiplayerSpawn>();
            e.register_component_inplace<ShaderData>();
            e.register_component_inplace<MeshTrackingData>();
            e.register_component_inplace<DebugDraw>();
            e.register_component_inplace<TriggerVolume>();
            e.register_component_inplace<Light>();
            e.register_component_inplace<DepthInfo>();

            e.register_subsystem_inplace<comp_app::FrameTag>();
            e.register_subsystem_inplace<GameEventBus>();
            e.register_subsystem_inplace<BlamFiles<halo_version>>();

            install_imgui_widgets(e, [&e](Url const& map) {
                auto&        gbus = e.subsystem_cast<GameEventBus>();
                GameEvent    ev{GameEvent::MapLoadStart};
                MapLoadEvent load{.file = map};
                gbus.inject(ev, &load);
            });
            auto& params = e.register_subsystem_inplace<RenderingParameters>();
            params.mipmap_bias = 0;
            e.register_subsystem_inplace<LoadingStatus>();

            {
                auto& bitm_cache
                    = e.register_subsystem_inplace<BitmapCache<halo_version>>(
                        &gfx, &params);
                auto& shader_cache
                    = e.register_subsystem_inplace<ShaderCache<halo_version>>(
                        std::ref(bitm_cache));
                e.register_subsystem_inplace<ModelCache<halo_version>>(
                    std::ref(bitm_cache), std::ref(shader_cache), &gfx);
                e.register_subsystem_inplace<BSPCache<halo_version>>(
                    std::ref(bitm_cache), std::ref(shader_cache));
                auto& font_cache
                    = e.register_subsystem_inplace<FontCache<halo_version>>(
                        &gfx);
                e.register_subsystem_inplace<UIElementCache<halo_version>>(
                    std::ref(bitm_cache), std::ref(font_cache));
            }

            if(auto window = e.service<comp_app::WindowInfo>())
            {
                window->setName("Blam!");
            }

            {
#if defined(COFFEE_ANDROID)
                using android::app_info;
                const bool use_touch = app_info().device_type()
                                       == app_info::device_type_t::phone;
#else
                constexpr bool use_touch = false;
#endif
                if(use_touch)
                    create_touch_overlay(e);
            }
            create_resources(e);
            create_shaders(e);
            set_resource_labels(e);
            alloc_renderer(e);
            alloc_occluder(e);
            alloc_ui_system(e);
            alloc_scripting(e);
            setup_load_eventhandlers(e);

            using namespace ::platform::url::constructors;

            Url map_filename;
            Url map_dir;

            if constexpr(
                compile_info::platform::is_android
                || compile_info::platform::is_emscripten)
            {
#if defined(COFFEE_ANDROID)
                map_filename = MkUrl(
                    android::intent().extra("map").value_or("beavercreek.map"),
                    RSCA::AssetFile);
                map_dir = "."_asset;
#else
                map_filename             = "b30.map"_asset;
                map_dir                  = "."_asset;
#endif
            } else if(arguments.unmatched().size() == 2)
            {
                auto url = MkUrl(arguments.unmatched().at(1), RSCA::SystemFile);
                if(auto info = platform::file::file_info(url);
                   info.has_value()
                   && info.value().mode == platform::file::mode_t::file)
                {
                    map_filename = url;
                    map_dir
                        = map_filename.path().dirname().url(RSCA::SystemFile);
                } else
                    map_dir = url;
            } else
            {
                map_dir = "."_asset;
            }
            GameEvent    event{GameEvent::MapLoadStart};
            MapLoadEvent load{
                .directory = map_dir,
            };
            if(map_filename.valid())
                load.file = map_filename;
            e.subsystem_cast<GameEventBus>().inject(event, &load);
        },
        [](EntityContainer& e,
           BlamData<halo_version>&,
           time_point const&,
           duration const& t) {
            auto& camera = e.subsystem_cast<BlamCamera>();

            /* Update camera position */
            camera.std_camera->tick(t);
            if(e.service<comp_app::ControllerInput>())
                camera.controller_camera(
                    e.service<comp_app::ControllerInput>()->state(0), t);
            using namespace typing::vectors::scene;
            camera.camera.aspect
                = e.service<comp_app::Windowing>()->size().aspect();
            camera.camera.zVals = {1500.f, 0.001f};

            Matf4 view_matrix = glm::translate(
                glm::scale(glm::identity<glm::mat4>(), glm::vec3(1))
                    * glm::mat4_cast(camera.camera.rotation),
                camera.camera.position);

            camera.camera_matrix       = GenPerspective(camera.camera);
            camera.camera_matrix[2][2] = 0.f;
            camera.camera_matrix[2][3] = -1.f;
            camera.camera_matrix[3][2] = 0.001f;
            camera.camera_matrix       = camera.camera_matrix * view_matrix;
            camera.rotation_matrix = glm::mat3_cast(camera.camera.rotation);
        },
        [](EntityContainer&, BlamData<halo_version>&, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN_CUSTOM(blam_main, 0x1 | 0x2)
