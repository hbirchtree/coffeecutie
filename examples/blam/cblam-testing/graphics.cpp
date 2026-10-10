#include "blam_files.h"
#include "camera_control.h"
#include "components.h"
#include "data.h"
#include "forge_controller.h"
#include "input/touch_overlay.h"
#include "journal.h"
#include "loading.h"
#include "map_loading.h"
#include "map_marker.h"
#include "network/networking.h"
#include "offline_maps.h"
#include "animation_controller.h"
#include "gameplay.h"
#include "impact_sounds.h"
#include "physics.h"
#if defined(POC_COMBAT)
#include "poc/app_combat.h"
#endif
#include "render/occluder.h"
#include "render/rendering.h"
#include "resource_creation.h"
#include "script_component.h"
#include "selected_version.h"
#include "sounds.h"
#include "ui.h"
#include "ui_game_setup.h"
#include "ui_profile.h"
#include "ui_caching.h"

#include <peripherals/stl/magic_enum.hpp>

#include <coffee/core/CApplication>
#include <coffee/core/Scene>

#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/app_wrap.h>
#include <coffee/comp_app/fps_counter.h>
#include <coffee/comp_app/gl_config.h>
#include <coffee/core/coffee_args.h>
#include <coffee/graphics/apis/gleam/rhi_emulation.h>
#include <platforms/sysinfo.h>

#if defined(BLAM_CURSED_ENABLED)
#include "cursed.h"
#endif

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
        options.add_options("Audio")
            //
            ("no-sound", "Start volume set to 0 (audio still computed)")
            //
            ;
        options.add_options("Graphics")
            //
            ("gfx-level",
             "Override graphics API, eg. core:4:6, es:2:0",
             cxxopts::value<std::string>())
            //
            ("gfx-emulate",
             "Emulate certain device stereotype. Some limitations apply. "
             "Does not emulate surface render types. "
             "Will only include the compatible subset of extensions (foreign "
             "texture formats must be software-decoded). "
             "Available device types:\n"
             "PowerVR_SGX530\nMali400\nMaliG710\nAdreno540\nAdreno620\nWebGL_"
             "Mobile\nWebGL_Desktop",
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
             "Server to connect to on startup. Pass the join string the "
             "server prints, which carries its key so the connection is "
             "authenticated: ip:port#auth=ed25519:<key>, or "
             "ws://gateway#<id>;auth=ed25519:<key>",
             cxxopts::value<std::string>())
            //
            ("listen",
             "Interface to start a server on",
             cxxopts::value<std::string>())
            //
            ("gateway-register",
             "webrtc-gateway /server-signal URL to register this --listen "
             "server with, so browser clients can be routed to it",
             cxxopts::value<std::string>())
            //
            ("gateway-server-id",
             "Server ID to register under with --gateway-register",
             cxxopts::value<std::string>())
            //
            ("gateway-auth-key",
             "Path to the server's Ed25519 private key PEM, which signs its "
             "certificate and WebRTC metadata. Generated if the file does not "
             "exist. Without it, a server generates a key in memory each run",
             cxxopts::value<std::string>())
            //
            ("server-key",
             "Ed25519 public key (base64) of the server named by --server. "
             "The connection then requires a certificate signed by that key, "
             "so nothing on the path can present its own. Not needed with a "
             "join string, which carries the key",
             cxxopts::value<std::string>())
            //
            ("relay-only",
             "Always relay traffic, no peer-to-peer",
             cxxopts::value<bool>()->default_value("false"))
            //
            ("net-lag",
             "Simulated round-trip latency added to this process's packets, ms",
             cxxopts::value<libc_types::u32>())
            //
            ("net-jitter",
             "Simulated mean extra delay per packet, ms",
             cxxopts::value<libc_types::f32>())
            //
            ("net-loss",
             "Simulated packet loss in each direction, percent",
             cxxopts::value<libc_types::f32>());
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
    auto& controller   = loader.config<comp_app::ControllerConfig>();
    controller.options = comp_app::ControllerConfig::BackgroundInput;

#if defined(SELECT_API_OPENGL)
    auto& glConfig        = loader.config<comp_app::GLConfig>();
    glConfig.swapInterval = 0;
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
            e.register_component_inplace<AnimationPlayback>();
            e.register_component_inplace<BspReference>();
            e.register_component_inplace<CameraLerp>();
            e.register_component_inplace<DebugDraw>();
            e.register_component_inplace<DepthInfo>();
            e.register_component_inplace<DrawState>();
            e.register_component_inplace<Light>();
            e.register_component_inplace<MeshTrackingData>();
            e.register_component_inplace<Model>();
            e.register_component_inplace<MultiplayerSpawn>();
            e.register_component_inplace<NetworkInfo>();
            e.register_component_inplace<ObjectPhysics>();
            e.register_component_inplace<ObjectSpawn>();
            e.register_component_inplace<PlayerCamera>();
            e.register_component_inplace<PlayerInfo>();
            e.register_component_inplace<PlayerInput>();
            e.register_component_inplace<PhysicsData>();
            e.register_component_inplace<ShaderData>();
            e.register_component_inplace<SoundEffects>();
            e.register_component_inplace<SubModel>();
            e.register_component_inplace<TriggerVolume>();
            e.register_component_inplace<Visibility>();
            e.register_component_inplace<WorldInfo>();

            e.register_subsystem_inplace<comp_app::FrameCounter>();
            auto& game_bus = e.register_subsystem_inplace<GameEventBus>();
            auto& journal  = e.register_subsystem_inplace<Journal>();
            if(journal.enabled())
            {
                auto type_stack =
                    std::make_shared<std::vector<GameEvent::EventType>>();
                game_bus.addEventData(
                    {0, [type_stack](GameEvent& ev, const void*) {
                         type_stack->push_back(ev.type);
                     }});
                game_bus.addEventData(
                    {9999, [&journal, type_stack](GameEvent& ev, const void*) {
                         auto original = type_stack->back();
                         type_stack->pop_back();
                         nlohmann::json data{
                             {"event", magic_enum::enum_name(original)}};
                         if(ev.type != original)
                         {
                             if(ev.type == GameEvent::None)
                                 data["cancelled"] = true;
                             else
                                 data["became"] =
                                     magic_enum::enum_name(ev.type);
                         }
                         journal.record("game_event", std::move(data));
                     }});
            }
            e.register_subsystem_inplace<BlamFiles<halo_version>>();
            e.register_subsystem_inplace<LoadingStatus>();

            alloc_resource_loader(e);
            alloc_occluder(e);
            alloc_physics(e);
            alloc_gameplay(e);
            alloc_animation_controller(e);
            alloc_impact_sounds(e);
            alloc_scripting(e);
            setup_load_eventhandlers(e);
            alloc_camera_control(e);
#if defined(POC_COMBAT)
            alloc_poc_combat(e);
#endif

            auto& params = e.register_subsystem_inplace<RenderingParameters>();
            if(arguments.contains("gfx-tex-resolution"))
                params.mipmap_bias = arguments["gfx-tex-resolution"].as<int>();
            else
                params.mipmap_bias = 0;

            auto& gfx  = e.register_subsystem_inplace<gfx::system>();
            auto  opts = [&arguments]() -> gleam::api::load_options_t {
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
                        Throw(
                            std::out_of_range(
                                "option given to --gfx-emulate no valid"));
                }
                if(arguments.contains("gfx-level"))
                {
                    auto level     = arguments["gfx-level"].as<std::string>();
                    auto split     = level.find(":");
                    auto profile   = level.substr(0, split);
                    auto ver_full  = level.substr(split + 1);
                    auto ver_split = ver_full.find(":");
                    auto major     = std::stoi(ver_full.substr(0, ver_split));
                    auto minor     = std::stoi(ver_full.substr(ver_split + 1));
                    return gleam::api::load_options_t{
                        // Shift version into 0xXY0 format
                         .api_version = (major << 8) | (minor << 4),
                         .api_type    = profile == "es" ? gfx::api_type_t::es
                                                        : gfx::api_type_t::core,
                    };
                }
                return {};
            }();
            auto load_error = gfx.load(e, opts);

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
            if(!arguments.count("no-sound"))
            {
                auto& snd = e.register_subsystem_inplace<oaf::system>();
                if(auto error = snd.load(e))
                {
                    cWarning("Failed to load audio: {}", error.value());
                    if(auto error = snd.load(e, oaf::system::dummy()))
                    {
                        cWarning(
                            "Failed to load audio dummy: {}", error.value());
                        return;
                    }
                }
                snd.set_distance_model(oaf::api::exponential);
                snd.collect_info(*e.service<comp_app::AppInfo>());
            }
#endif
#if defined(FEATURE_ENABLE_DiscordLatte)
            using namespace net::url_literals;
            auto& discord = e.register_subsystem_inplace<discord::Subsystem>(
                rq::runtime_queue::CreateNewThreadQueue("Online").value(),
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
                    auto& gbus = e.subsystem_cast<GameEventBus>();
                    gbus.addEventFunction<ServerStateUpdate>(
                        0, std::move(handler));
                    gbus.addEventFunction<ServerJoinInfo>(
                        0, [&discord](GameEvent&, ServerJoinInfo* join) {
                            // set join info
                            platform::online::PartyDescUpdate data;
                            data.partyId     = join->server_id.str();
                            data.join.secret = join->secret.str();
                            discord.presence().update(std::move(data));
                        });
                    gbus.addEventFunction<MapLoadFinishedEvent<halo_version>>(
                        0,
                        [&discord](
                            GameEvent&,
                            MapLoadFinishedEvent<halo_version>* load) {
                            discord.presence().putState(load->map_title);
                        });
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
                    [arguments, &e] -> oaf::system* {
                        if(arguments.count("no-sound"))
                            return nullptr;
                        return &e.subsystem_cast<oaf::system>();
                    }()
#else
                    nullptr
#endif
                );
            alloc_sound_system(e, arguments.count("no-sound") ? false : true);

            {
                auto& sound_pref = e.subsystem_cast<SoundPreferences>();
                sound_pref.master_volume =
                    arguments.count("no-sound") > 0 ? 0.f : 1.f;
            }

            {
                auto& bitm_cache =
                    e.register_subsystem_inplace<BitmapCache<halo_version>>(
                        &gfx, &params);
                auto& shader_cache =
                    e.register_subsystem_inplace<ShaderCache<halo_version>>(
                        std::ref(bitm_cache));
                bool const snorm8_normals =
                    !gfx.feature_info().vertex.vertex_attrib_i_pointer;
                auto& model_cache =
                    e.register_subsystem_inplace<ModelCache<halo_version>>(
                        std::ref(bitm_cache), std::ref(shader_cache), &gfx);
                model_cache.snorm8_normals = snorm8_normals;
                e.register_subsystem_inplace<DebugMarkers>().enabled =
                    &e.subsystem_cast<RenderingParameters>().debug_markers;
                auto& bsp_cache =
                    e.register_subsystem_inplace<BSPCache<halo_version>>(
                        std::ref(bitm_cache),
                        std::ref(shader_cache),
                        std::ref(sound_cache),
                        e.service<comp_app::EventBus<SoundEvent>>());
                bsp_cache.snorm8_normals = snorm8_normals;
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
                const bool use_touch =
                    platform::info::device::variant() == platform::info::DevicePhone;
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
            alloc_profile_provider(e);
            alloc_game_setup_provider(e);
            alloc_networking(
                e,
                arguments.count("gateway-auth-key")
                    ? arguments["gateway-auth-key"].as<std::string>()
                    : std::string());
#if defined(BLAM_CURSED_ENABLED)
            cursed::setup_cursed_loaders(e);
#endif
            alloc_forge_controller(e);

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
            /* Maps uploaded with BlamMapUpload only exist in IndexedDB, so
             * loads under their prefixes never go to the network. Without
             * ?map=, boot this game version's uploaded ui.map if there is
             * one, instead of fetching the bundled one. */
            offline_maps::register_storage();
            if(!compile_info::supports_command_line && !arguments.count("map"))
                if(auto offline = offline_maps::default_map())
                    map_filename = MkUrl(*offline, RSCA::AssetFile);
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
            auto& app_bus = e.subsystem_cast<comp_app::BasicEventBus<comp_app::AppEvent>>();

            if constexpr(compile_info::platform::is_android)
            {
                // Map Android back action to game/menu switch
                // In the future maybe better support for back nav
                app_bus.addEventFunction<comp_app::NavigationEvent>(
                    0, [&](auto&, comp_app::NavigationEvent* nav) {
                        if(nav->navigation_type != comp_app::NavigationEvent::Back)
                            return;
                        UIEvent ev{.type = UIEvent::navigation};
                        for(auto player : e.select<PlayerCamera, PlayerInput>())
                        {
                            auto [cam, input] = player.components();
                            if(cam.keyboard.enabled)
                                input.start = true;
                        }
                    });
            }

            /* B or a resume button closed the menu without start */
            e.subsystem_cast<UIEventBus>().addEventFunction<UIMenuLeave>(
                0, [&e](UIEvent&, UIMenuLeave* leave) {
                    for(auto player : e.select<PlayerInfo, PlayerInput>())
                    {
                        auto [info, input] = player.components();
                        if(info.seat_idx == leave->seat_idx)
                            input.input_mode = PlayerInput::input_mode_t::game;
                    }
                    e.subsystem_cast<RenderingParameters>().render_ui = false;
                });

            e.subsystem_cast<BlamFiles<halo_version>>().map_directory = map_dir;

            {
                auto& sim = e.subsystem_cast<NetworkState>().simulation;
                if(arguments.count("net-lag"))
                    sim.lag_ms = arguments["net-lag"].as<libc_types::u32>();
                if(arguments.count("net-jitter"))
                    sim.jitter_ms = arguments["net-jitter"].as<libc_types::f32>();
                if(arguments.count("net-loss"))
                    sim.loss_pct = arguments["net-loss"].as<libc_types::f32>();
            }

            if(arguments.count("server"))
            {
                /* When we're a client, skip trying to load a map on startup */
                GameEvent              event{GameEvent::MapRequestListing};
                MapRequestListingEvent request{};
                gbus.inject(event, &request);
                event = {GameEvent::ServerConnect};
                ServerConnectEvent connect{
                    .type   = ServerConnectEvent::Server,
                    .remote = arguments["server"].as<std::string>(),
                };
                connect.relay_only = arguments["relay-only"].as<bool>();
                if(arguments.count("server-key"))
                    connect.server_public_key =
                        arguments["server-key"].as<std::string>();
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
                    connect.relay_only = arguments["relay-only"].as<bool>();
                    if(arguments.count("gateway-register"))
                    {
                        connect.gateway_register_url =
                            arguments["gateway-register"].as<std::string>();
                        connect.gateway_server_id =
                            arguments.count("gateway-server-id")
                                ? arguments["gateway-server-id"]
                                      .as<std::string>()
                                : std::string("default");
                        if(arguments.count("gateway-auth-key"))
                            connect.gateway_auth_key =
                                arguments["gateway-auth-key"].as<std::string>();
                    }
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
            auto& uibus = e.subsystem_cast<UIEventBus>();

            UIEvent uiev{.type = UIEvent::navigation};

            for(auto entity : e.select<
                              PlayerCamera,
                              PlayerInput,
                              PlayerInfo,
                              NetworkInfo,
                              Model>())
            {
                auto [cam, input, info, net, mod] = entity.components();
                if(info.permissions.camera)
                {
                    bool controller_connected = controllers &&
                        cam.controller.index.has_value();
                    if(controller_connected)
                        controller_sample_input(
                            input.look_delta,
                            input.movement,
                            input.accel,
                            cam.controller.opts,
                            controllers->state(*cam.controller.index),
                            t);

                    if(input.rotation)
                    {
                        cam.camera.rotation =
                            *std::exchange(input.rotation, std::nullopt);
                        net.changes.viewport = true;
                    }
                    if(input.position)
                    {
                        cam.camera.position =
                            *std::exchange(input.position, std::nullopt);
                        net.changes.transform = net.changes.viewport = true;
                    }

                    if(input.look_delta != Vecf2{})
                    {
                        cam.camera_.rotate(
                            cam.camera, input.look_delta.x, input.look_delta.y);
                        input.look_delta     = {};
                        net.changes.viewport = true;
                    }

                    /* In physics mode the body owns the position: let
                     * tick() rotates the view and
                     * refresh cached vectors, but drop their freecam
                     * fly-movement so it doesn't fight the physics follow. */
                    auto const freecam_pos = cam.camera.position;

                    cam.camera_.refresh_basis(cam.camera, cam.camera_opts);

                    if(cam.keyboard.enabled)
                        StandardCamera::sample_keys(
                            input.keys,
                            cam.camera_opts,
                            input.movement,
                            input.accel,
                            t);

                    auto controller_buttons = [&controllers, &cam] {
                        return controllers->state(*cam.controller.index).buttons.e;
                    };
                    auto key_pressed = [&input](u16 key) {
                        return StandardCamera::has_key(input.keys, key);
                    };

                    if(input.input_mode == PlayerInput::input_mode_t::menu)
                    {
                        if(controller_connected)
                        {
                            input.accept   |= controller_buttons().a;
                            input.back     |= controller_buttons().b || controller_buttons().back;
                            input.option   |= controller_buttons().y;
                            input.option_2 |= controller_buttons().x;
                            input.left     |= controller_buttons().p_left;
                            input.right    |= controller_buttons().p_right;
                            input.up       |= controller_buttons().p_up;
                            input.down     |= controller_buttons().p_down;
                        }
                        if(cam.keyboard.enabled)
                        {
                            input.accept |= key_pressed(Input::CK_EnterCR);
                            input.back   |= key_pressed(Input::CK_BackSpace);
                            input.left   |= key_pressed(Input::CK_Left);
                            input.right  |= key_pressed(Input::CK_Right);
                            input.up     |= key_pressed(Input::CK_Up);
                            input.down   |= key_pressed(Input::CK_Down);
                        }
                    } else if(info.riding.vehicle != 0)
                    {
                        /* A rider's camera and vehicle belong to Gameplay */
                    } else if(!info.mode.physics)
                    {
                        // Check that there's input
                        // Avoid needless movement
                        if(input.movement != Vecf3{})
                        {
                            cam.camera_.move(
                                cam.camera,
                                input.movement.x,
                                input.movement.y,
                                input.movement.z,
                                input.accel);
                            net.changes.transform = net.changes.viewport = true;
                        }
                        if(controller_connected)
                            input.back |= controller_buttons().back;
                    } else if(!info.is_remote())
                    {
                        cam.camera.position = freecam_pos;
                        // TODO: Check for changes in position on physics
                        // movement
                        net.changes.transform = net.changes.viewport = true;

                        /* Project onto the ground plane so looking down
                         * doesn't drive the capsule into the floor */
                        auto planar = [](Vecf3 v) {
                            v.z      = 0.f;
                            f32 len2 = glm::dot(v, v);
                            return len2 > 1e-8f ? v / std::sqrt(len2) : Vecf3{};
                        };
                        auto const& wrap = cam.camera_;

                        Vecf3 dir =
                            planar(wrap.cached.forward) * input.movement.x +
                            planar(wrap.cached.right) * input.movement.y;
                        bool jump = input.jump;

                        if(cam.keyboard.enabled)
                            jump |= key_pressed(Input::CK_Space);
                        if(controller_connected)
                            jump |= controller_buttons().a;
                        /* Clamp instead of normalize: keyboard diagonals
                         * cap at 1, partial stick deflection stays analog */
                        if(f32 len2 = glm::dot(dir, dir); len2 > 1.f)
                            dir /= std::sqrt(len2);
                        if(controller_connected)
                            input.back |= controller_buttons().back;

                        const f32 move_speed = 10.f * input.accel;
                        /* As high as 4 wu/s jumped under 9.81, in Halo's
                         * gravity */
                        const f32 jump_speed = 2.3f;

                        Physics::Event    ev{Physics::Event::Velocity};
                        Physics::Velocity velocity{
                            .entity_id  = entity.id(),
                            .velocity   = dir * move_speed,
                            .preserve_z = true,
                            .jump       = jump ? jump_speed : 0.f,
                        };
                        e.subsystem_cast<PhysicsBus>().process(ev, &velocity);
                    }

                    /* Gameplay acts on these */
                    constexpr i16 trigger_pressed = 8192;
                    /* Throttle is the stick or W/S as is; movement is
                     * scaled for flying the camera */
                    f32 throttle = 0.f;
                    if(controller_connected)
                        throttle -= convert_i16_f(
                            controllers->state(*cam.controller.index)
                                .axes.e.l_y);
                    if(cam.keyboard.enabled)
                        throttle += (key_pressed(Input::CK_w) ? 1.f : 0.f) -
                                    (key_pressed(Input::CK_s) ? 1.f : 0.f);
                    input.intent = {
                        .throttle = std::clamp(throttle, -1.f, 1.f),
                        .use      = (controller_connected &&
                                controller_buttons().x) ||
                               (cam.keyboard.enabled &&
                                key_pressed(Input::CK_f)),
                        .grab = (controller_connected &&
                                 controllers->state(*cam.controller.index)
                                         .axes.e.t_r > trigger_pressed) ||
                                (cam.keyboard.enabled &&
                                 (input.mouse_buttons &
                                  Input::CIMouseButtonEvent::RightButton)),
                    };

                    /* Sampled in every mode, freecam included */
                    if(controller_connected)
                        input.start |= controller_buttons().start;
                    if(cam.keyboard.enabled)
                        input.start |= key_pressed(Input::CK_F2);

                    auto emit_nav_event = [uiev = uiev, &uibus, &info](
                        UINavigation::action_t action) mutable
                    {
                        UINavigation nav{
                            .action = action,
                            .seat_idx = info.seat_idx,
                        };
                        uibus.inject(uiev, &nav);
                    };

                    if(input.start)
                    {
                        bool const to_menu =
                            input.input_mode == PlayerInput::input_mode_t::game;
                        input.input_mode =
                            to_menu ? PlayerInput::input_mode_t::menu
                                    : PlayerInput::input_mode_t::game;
                        e.subsystem_cast<RenderingParameters>().render_ui =
                            to_menu;
                        emit_nav_event(
                            to_menu ? UINavigation::open : UINavigation::close);
                    }
                    if(input.accept)
                        emit_nav_event(UINavigation::accept);
                    if(input.option)
                        emit_nav_event(UINavigation::option);
                    if(input.option_2)
                        emit_nav_event(UINavigation::option_2);
                    if(input.back)
                    {
                        emit_nav_event(UINavigation::back);
                        info.mode.physics = !info.mode.physics;
                    }
                    if(input.left)
                        emit_nav_event(UINavigation::left);
                    if(input.right)
                        emit_nav_event(UINavigation::right);
                    if(input.up)
                        emit_nav_event(UINavigation::up);
                    if(input.down)
                        emit_nav_event(UINavigation::down);

                    /* Consumed: sampled fresh every frame from held keys and
                     * stick state, never carried over */
                    input.movement = {};
                    input.accel    = 1.f;
                    input.jump     = false;

                    input.frame_end();
                }

                cam.camera.zVals = {100.f, 0.001f};

                /* Fold the vertex→BSP permutation into the rotation so that
                 * cam.camera.position can be stored in plain vertex space.
                 * Equivalent to: R * T(-bsp_pos) * bsp_basis. */
                static const Matf4 bsp_basis{
                    {0, 0, 1, 0},
                    {1, 0, 0, 0},
                    {0, 1, 0, 0},
                    {0, 0, 0, 1},
                };

                Matf4 view_matrix = glm::translate(
                    glm::mat4_cast(cam.camera.rotation) * bsp_basis,
                    -cam.camera.position);

                cam.matrix       = GenPerspective(cam.camera);
                cam.matrix[2][2] = 0.f;
                cam.matrix       = cam.matrix * view_matrix;
                cam.rotation = glm::mat4_cast(cam.camera.rotation) * bsp_basis;


                /* The biped stands under its camera, turned only by yaw,
                 * the way scenery is placed; Gameplay seats a rider's */
                if(info.riding.vehicle == 0)
                {
                    Vecf3 const forward = glm::transpose(Matf3(cam.rotation)) *
                                          Vecf3{0.f, 0.f, -1.f};
                    mod.position = cam.camera.position -
                                   Vecf3{0, 0, info.biped.eye_height};
                    mod.rotation =
                        Quatf(Vecf3(0, 0, std::atan2(forward.y, forward.x)));
                    mod.transform = glm::translate(Matf4(1), mod.position) *
                                    glm::mat4_cast(mod.rotation);
                }
            }

            /* Controllers no player owns still drive the menus, seated by
             * controller index, so another player can join a split screen
             * lobby without the game creating a seat (and viewport) for them */
            if(controllers && e.subsystem_cast<RenderingParameters>().render_ui)
            {
                struct menu_pad_t
                {
                    debounced_button_t accept, back, option, option_2;
                    debounced_button_t up, down, left, right;
                };
                static std::array<menu_pad_t, 4> pads;

                std::array<bool, 4> owned{};
                for(auto player : e.select<PlayerCamera>())
                    if(auto const* cam = e.get<PlayerCamera>(player.id());
                       cam && cam->controller.index &&
                       *cam->controller.index < owned.size())
                        owned[*cam->controller.index] = true;

                u32 const count =
                    std::min<u32>(controllers->count(), pads.size());
                for(u32 idx = 0; idx < count; ++idx)
                {
                    auto& pad = pads[idx];
                    if(owned[idx])
                    {
                        pad = {};
                        continue;
                    }
                    auto const buttons = controllers->state(idx).buttons.e;
                    pad.accept |= buttons.a;
                    pad.back |= buttons.b || buttons.back;
                    pad.option |= buttons.y;
                    pad.option_2 |= buttons.x;
                    pad.up |= buttons.p_up;
                    pad.down |= buttons.p_down;
                    pad.left |= buttons.p_left;
                    pad.right |= buttons.p_right;

                    std::pair<debounced_button_t*, UINavigation::action_t> const
                        actions[] = {
                            {&pad.accept, UINavigation::accept},
                            {&pad.back, UINavigation::back},
                            {&pad.option, UINavigation::option},
                            {&pad.option_2, UINavigation::option_2},
                            {&pad.up, UINavigation::up},
                            {&pad.down, UINavigation::down},
                            {&pad.left, UINavigation::left},
                            {&pad.right, UINavigation::right},
                        };
                    for(auto const& [button, action] : actions)
                    {
                        if(*button)
                        {
                            UINavigation nav{.action = action, .seat_idx = idx};
                            uibus.inject(uiev, &nav);
                        }
                        button->frame_end();
                    }
                }
            }
        },
        [](EntityContainer&, BlamData<halo_version>&, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN_CUSTOM(blam_main, 0x1 | 0x2)
