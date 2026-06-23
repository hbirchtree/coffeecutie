#include "blam_files.h"
#include "data.h"
#include "map_loading.h"
#include "map_marker.h"
#include "networking.h"
#include "occluder.h"
#include "physics.h"
#include "rendering.h"
#include "resource_creation.h"
#include "script_component.h"
#include "selected_version.h"
#include "sounds.h"
#include "touch_overlay.h"
#include "ui.h"
#include "ui_caching.h"

#include <peripherals/stl/magic_enum.hpp>

#include <coffee/comp_app/fps_counter.h>
#include <coffee/core/coffee_args.h>
#include <coffee/graphics/apis/gleam/rhi_emulation.h>
#include <platforms/sysinfo.h>

#if defined(FEATURE_ENABLE_OAF)
#include <oaf/api_system.h>
#endif

#if defined(FEATURE_ENABLE_Net)
#include <coffee/net/curl_network_stats.h>
#include <coffee/net/net_profiling.h>
#include <coffee/net/net_resource.h>
#endif
#if defined(FEATURE_ENABLE_DiscordLatte)
#include <discord/discord_system.h>
#endif

using namespace Coffee;

void install_imgui_widgets(
    compo::EntityContainer& e, std::function<void(Url const&)>&& map_select);

i32 blam_main()
{
    cxxopts::ParseResult arguments;
    {
        cxxopts::Options options(
            "Blam! Graphics", "A prototype for a Blam! engine");
        Coffee::BaseArgParser::GetBase(options);
        if constexpr(compile_info::implicit_resource_dir)
            options.custom_help("[resource dir] [map file/dir] [OPTION...]");
        else
            options.custom_help("[map file/dir] [OPTION...]");
        options.positional_help("map name or directory");
        options.add_options("Graphics")
            //
            ("gfx-level",
             "Override graphics API, eg. core:4:6, es:2:0",
             cxxopts::value<std::string>())
            //
            ("gfx-emulate",
             "Emulate certain device stereotype. Some limitations apply. "
             "Does not emulate surface render types. "
             "Will only include the compatible subset of extensions (foreign texture formats must be software-decoded). "
             "Available device types:\n"
             "PowerVR_SGX530\nMali400\nMaliG710\nAdreno540\nAdreno620\nWebGL_Mobile\nWebGL_Desktop",
             cxxopts::value<std::string>())
            //
            ("gfx-tex-resolution",
             "Range of 1-4 of texture quality, 4 is highest",
             cxxopts::value<int>())
            //
            ;
        options.add_options("Networking")
            //
            ("server",
             "Server to connect to on startup",
             cxxopts::value<std::string>())
            //
            ("listen",
             "Interface to start a server on",
             cxxopts::value<std::string>())
            //
            ("master-server",
             "Master server to query for servers",
             cxxopts::value<std::string>());
        if constexpr(!compile_info::supports_command_line)
            options.add_options("Game")(
                "map", "Which map file to load", cxxopts::value<std::string>());
        auto& args = GetInitArgs();
        arguments  = options.parse(args.size(), args.data());
        if(BaseArgParser::PerformDefaults(options, args) >= 0)
            return 0;
    }

    rq::runtime_queue::CreateNewQueue("Blam Graphics!").assume_value();
#if defined(FEATURE_ENABLE_Net)
    C_UNUSED(auto _ = Net::RegisterProfiling());
#endif

    comp_app::app_error app_ec;

    auto& e      = comp_app::createContainer();
    auto& loader = comp_app::configureDefaults(e);

    auto& window = loader.config<comp_app::WindowConfig>();
    window.flags = comp_app::window_flags_t::windowed |
                   comp_app::window_flags_t::resizable;
    if constexpr(compile_info::platform::is_emscripten)
        window.flags = comp_app::window_flags_t::resizable;
    // auto& touch = loader.config<comp_app::TouchConfig>();
    // touch.options |= comp_app::TouchConfig::TouchToMouse;
    auto& controller = loader.config<comp_app::ControllerConfig>();
    controller.options = comp_app::ControllerConfig::BackgroundInput;

#if defined(SELECT_API_OPENGL)
    auto& glConfig        = loader.config<comp_app::GLConfig>();
    glConfig.swapInterval = 1;
    if constexpr(compile_info::debug_mode || true)
    {
        glConfig.profile |= comp_app::GLConfig::Debug;
    }
#endif

    cDebug(
        "Buffer budget: {0} / {1} MB",
        memory_budget::grand_total,
        memory_budget::grand_total / Unit_MB);

    comp_app::addDefaults(e, *e.service<comp_app::AppLoader>(), app_ec);
    comp_app::AppContainer<BlamData<halo_version>>::addTo(
        e,
        [arguments](
            EntityContainer& e,
            BlamData<halo_version>& /*data*/,
            time_point const&) {
            ProfContext _(__FUNCTION__);

            e.register_subsystem_inplace<net::CurlNetStats>(
                net::create_curl_context());
            e.register_component_inplace<Model>();
            e.register_component_inplace<SubModel>();
            e.register_component_inplace<BspReference>();
            e.register_component_inplace<ObjectSpawn>();
            e.register_component_inplace<NetworkInfo>();
            e.register_component_inplace<PlayerInfo>();
            e.register_component_inplace<SoundEffects>();
            e.register_component_inplace<MultiplayerSpawn>();
            e.register_component_inplace<ShaderData>();
            e.register_component_inplace<MeshTrackingData>();
            e.register_component_inplace<DebugDraw>();
            e.register_component_inplace<TriggerVolume>();
            e.register_component_inplace<Light>();
            e.register_component_inplace<DepthInfo>();
            e.register_component_inplace<PlayerCamera>();

            e.register_subsystem_inplace<comp_app::FrameTag>();
            e.register_subsystem_inplace<GameEventBus>();
            e.register_subsystem_inplace<BlamFiles<halo_version>>();
            e.register_subsystem_inplace<LoadingStatus>();

            alloc_occluder(e);
            alloc_physics(e);
            alloc_scripting(e);
            setup_load_eventhandlers(e);

            auto& params = e.register_subsystem_inplace<RenderingParameters>();
            if(arguments.contains("gfx-tex-resolution"))
                params.mipmap_bias = arguments["gfx-tex-resolution"].as<int>();
            else
                params.mipmap_bias = 0;

            auto& gfx        = e.register_subsystem_inplace<gfx::system>();
            auto opts = [&arguments]() -> gleam::api::load_options_t
            {
                if(arguments.contains("gfx-emulate"))
                {
                    auto target = arguments["gfx-emulate"].as<std::string>();
                    if(target == "PowerVR_SGX530")
                        return gfx::emulation::img::powervr_sgx530_bbb();
                    else if(target == "Mali400")
                        return gfx::emulation::arm::mali_400mp();
                    else if(target == "MaliG710")
                        return gfx::emulation::arm::mali_g710();
                    else if(target == "Adreno320")
                        return gfx::emulation::qcom::adreno_320();
                    else if(target == "Adreno540")
                        return gfx::emulation::qcom::adreno_540();
                    else if(target == "Adreno620")
                        return gfx::emulation::qcom::adreno_620();
                    else if(target == "WebGL_Mobile")
                        return gfx::emulation::webgl::mobile();
                    else if(target == "WebGL_Desktop")
                        return gfx::emulation::webgl::desktop();
                    else
                        Throw(std::out_of_range("option given to --gfx-emulate no valid"));
                }
                if(arguments.contains("gfx-level"))
                {
                    auto level = arguments["gfx-level"].as<std::string>();
                    auto split = level.find(":");
                    auto profile = level.substr(0, split);
                    auto ver_full = level.substr(split + 1);
                    auto ver_split = ver_full.find(":");
                    auto major = std::stoi(ver_full.substr(0, ver_split));
                    auto minor = std::stoi(ver_full.substr(ver_split + 1));
                    return gleam::api::load_options_t{
                        // Shift version into 0xXY0 format
                        .api_version = (major << 8) | (minor << 4),
                        .api_type = profile == "es" ? gfx::api_type_t::es : gfx::api_type_t::core,
                    };
                }
                return {};
            }();
            auto  load_error = gfx.load(e, opts);

            if(load_error)
            {
                cWarning(
                    "Failed to initialize gfx::api: {0}",
                    magic_enum::enum_name(load_error.value()));
                return;
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

#if defined(FEATURE_ENABLE_OAF)
            auto& snd = e.register_subsystem_inplace<oaf::system>();
            if(auto error = snd.load(e))
            {
                cWarning("Failed to load audio: {}", error.value());
                if(auto error = snd.load(e, oaf::system::dummy()))
                {
                    cWarning("Failed to load audio dummy: {}", error.value());
                    return;
                }
            }
            snd.set_distance_model(oaf::api::exponential);
            snd.collect_info(*e.service<comp_app::AppInfo>());
#endif
#if defined(FEATURE_ENABLE_DiscordLatte)
            using namespace net::url_literals;
            auto& discord = e.register_subsystem_inplace<discord::Subsystem>(
                rq::runtime_queue::CreateNewThreadQueue("Online")
                    .assume_value(),
                discord::DiscordOptions("1194446879027646576"));
            discord.start();

            discord.on_started<bool>(
                [&e](discord::Subsystem& discord) {
                    discord.game().put(
                        discord::DiscordGameDelegate::Builder(
                            "Blam!",
                            "Gaming",
                            "https://assetsio.reedpopcdn.com/"
                            "digitalfoundry-2021-halo-combat-evolved-season-7-"
                            "master-chief-collection-1622735120728.jpg?width="
                            "1600&"
                            "height=900&fit=crop&quality=100&format=png&enable="
                            "upscale&auto=webp"_https));
                    discord.presence().put({
                        .partyId    = "16420",
                        .curPlayers = 1,
                        .maxPlayers = 16,
                        .spectate =
                            {
                                .secret = "",
                            },
                        .join =
                            {
                                .secret = "poopy",
                            },
                    });
                    discord.presence().putState("Campaign");
                    auto handler = [&discord](
                                       GameEvent&, ServerStateUpdate* update) {
                        platform::online::PartyDescUpdate data;
                        switch(update->type)
                        {
                        case ServerStateUpdate::PlayerCount:
                            cDebug(
                                "Player count update: {}", update->num_field);
                            data.curPlayers = update->num_field;
                            break;
                        case ServerStateUpdate::PlayerMaxCount:
                            data.maxPlayers = update->num_field;
                            break;
                        case ServerStateUpdate::ServerName:
                            discord.presence().putState(
                                std::string(update->string_field.str()));
                            return;
                        default:
                            return;
                        }
                        discord.presence().update(std::move(data));
                    };
                    e.subsystem_cast<GameEventBus>()
                        .addEventFunction<ServerStateUpdate>(
                            0, std::move(handler));
                    return false;
                },
                []() {
                    cDebug("No Discord today :(");
                    return false;
                });
#endif

            auto& sound_cache =
                e.register_subsystem_inplace<SoundCache<halo_version>>(
#if defined(FEATURE_ENABLE_OAF)
                    &snd
#else
                    nullptr
#endif
                );
            alloc_sound_system(e);

            {
                auto& bitm_cache =
                    e.register_subsystem_inplace<BitmapCache<halo_version>>(
                        &gfx, &params);
                auto& shader_cache =
                    e.register_subsystem_inplace<ShaderCache<halo_version>>(
                        std::ref(bitm_cache));
                e.register_subsystem_inplace<ModelCache<halo_version>>(
                    std::ref(bitm_cache), std::ref(shader_cache), &gfx);
                e.register_subsystem_inplace<DebugMarkers>();
                e.register_subsystem_inplace<BSPCache<halo_version>>(
                    std::ref(bitm_cache),
                    std::ref(shader_cache),
                    std::ref(sound_cache),
                    e.service<comp_app::EventBus<SoundEvent>>());
                auto& font_cache =
                    e.register_subsystem_inplace<FontCache<halo_version>>(&gfx);
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
                const bool use_touch =
                    app_info().device_type() == app_info::device_type_t::phone;
#else
                constexpr bool use_touch = false;
#endif
                if(use_touch)
                    create_touch_overlay(e);
            }
            install_imgui_widgets(e, [](Url const&) {});
            create_resources(e);
            create_shaders(e);
            set_resource_labels(e);
            alloc_renderer(e);
            alloc_ui_system(e);
            alloc_networking(e);

            using namespace ::platform::url::constructors;

            /* Figure out if have a map to load OR what's the map directory */
            Url map_filename;
            Url map_dir;

            map_filename = MkUrl(
                compile_info::supports_command_line
                    ? (arguments.unmatched().size() >=
                               (compile_info::implicit_resource_dir ? 1 : 2)
                           ? arguments.unmatched().at(
                                 compile_info::implicit_resource_dir ? 0 : 1)
                           : ".")
                : arguments.count("map") ? arguments["map"].as<std::string>()
                                         : ".",
                compile_info::supports_command_line ? RSCA::SystemFile
                                                    : RSCA::AssetFile);
            if(auto info = platform::file::file_info(map_filename);
               info.has_value())
            {
                if(info.value().mode == platform::file::mode_t::directory)
                {
                    map_dir      = map_filename;
                    map_filename = {};
                } else if(info.value().mode == platform::file::mode_t::file)
                {
                    map_dir =
                        map_filename.path().dirname().url(map_filename.flags);
                }
            } else
            {
                map_dir = map_filename.path().dirname().url(map_filename.flags);
            }

            auto& gbus = e.subsystem_cast<GameEventBus>();

            if(arguments.count("server"))
            {
                /* When we're a client, skip trying to load a map on startup */
                e.subsystem_cast<BlamFiles<halo_version>>().map_directory =
                    map_dir;
                GameEvent              event{GameEvent::MapRequestListing};
                MapRequestListingEvent request{};
                gbus.inject(event, &request);
                event = {GameEvent::ServerConnect};
                ServerConnectEvent connect{
                    .type   = ServerConnectEvent::Server,
                    .remote = arguments["server"].as<std::string>(),
                };
                gbus.inject(event, &connect);
            } else
            {
                GameEvent    event{GameEvent::MapLoadStart};
                MapLoadEvent load{
                    .directory = map_dir,
                };
                if(map_filename.valid())
                    load.file = map_filename;
                else
                    load.file = MkUrl("ui.map", RSCA::AssetFile);
                gbus.inject(event, &load);

                if(arguments.count("listen"))
                {
                    GameEvent          event{GameEvent::ServerConnect};
                    ServerConnectEvent connect{
                        .type   = ServerConnectEvent::Listen,
                        .remote = arguments["listen"].as<std::string>(),
                    };
                    gbus.inject(event, &connect);
                }
            }

#if defined(COFFEE_EMSCRIPTEN) && defined(FEATURE_ENABLE_Net)
            platform::env::set_var("COFFEE_REPORT_URL", EMBEDDED_REPORT_URL);
            Net::ProfilingExport();
#endif
        },
        [](EntityContainer& e,
           BlamData<halo_version>&,
           time_point const&,
           duration const& t) {
            using namespace typing::vectors::scene;

            auto controllers = e.service<comp_app::ControllerInput>();

            for(auto& entity : e.select<PlayerCamera>())
            {
                auto* cam  = e.get<PlayerCamera>(entity.id);
                auto* info = e.get<PlayerInfo>(entity.id);
                if(!cam || !info)
                    continue;

                if(info->permissions.camera)
                {
                    if(cam->keyboard.enabled)
                        cam->camera_->tick(t);
                    if(controllers && cam->controller.index.has_value())
                    {
                        cam->camera_->tick(t);
                        auto state = controllers->state(*cam->controller.index);
                        controller_camera_update(
                            cam->camera_,
                            cam->controller.opts,
                            controllers->state(*cam->controller.index),
                            t);
                    }
                }

                cam->camera->zVals = {100.f, 0.001f};

                /* Fold the vertex→BSP permutation into the rotation so that
                 * cam->camera->position can be stored in plain vertex space.
                 * Equivalent to: R * T(-bsp_pos) * bsp_basis. */
                static const Matf4 bsp_basis{
                    {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 1}};

                Matf4 view_matrix = glm::translate(
                    glm::mat4_cast(cam->camera->rotation) * bsp_basis,
                    -cam->camera->position);

                cam->matrix       = GenPerspective(*cam->camera);
                cam->matrix[2][2] = 0.f;
                cam->matrix       = cam->matrix * view_matrix;
                cam->rotation     = glm::mat3_cast(cam->camera->rotation);
            }

#if defined(FEATURE_ENABLE_OAF)
            for(auto& entity : e.select<PlayerCamera>())
            {
                auto* cam  = e.get<PlayerCamera>(entity.id);
                auto* info = e.get<PlayerInfo>(entity.id);
                if(cam && info && info->seat_idx == 0)
                {
                    auto& snd = e.subsystem_cast<oaf::system>();
                    snd.listener()
                        .set_property<oaf::listener_property::position>(
                            cam->camera->position);
                    snd.listener()
                        .set_property<oaf::listener_property::orientation>(
                            glm::mat3_cast(cam->camera->rotation));
                    break;
                }
            }
#endif
        },
        [](EntityContainer&, BlamData<halo_version>&, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN_CUSTOM(blam_main, 0x1 | 0x2)
