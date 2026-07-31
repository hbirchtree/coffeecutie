#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "journal.h"
#include "map_marker.h"
#include "networking.h"
#include "physics.h"
#include "pose_apply.h"
#include "pose_demo_spawn.h"
#include "rendering.h"
#include "resource_creation.h"
#include "selected_version.h"
#include "sounds.h"
#include "ui_caching.h"

#include <cxxopts.hpp>
#include <peripherals/stl/magic_enum.hpp>

#include <coffee/comp_app/app_wrap.h>
#include <coffee/comp_app/dummy_plug.h>
#include <coffee/comp_app/file_mapper.h>
#include <coffee/comp_app/fps_counter.h>
#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>
#include <coffee/core/coffee_args.h>
#include <coffee/core/task_queue/task.h>

using namespace Coffee;

i32 pose_demo_main()
{
    cxxopts::ParseResult arguments;
    {
        cxxopts::Options options(
            "Blam Pose Demo", "wasm pose-retarget proof of concept");
        Coffee::BaseArgParser::GetBase(options);
        if constexpr(compile_info::implicit_resource_dir)
            options.custom_help("[resource dir] [map file/dir] [OPTION...]");
        else
            options.custom_help("[map file/dir] [OPTION...]");
        options.positional_help("map name or directory");
        options.add_options("Graphics")(
            "gfx-level",
            "Override graphics API, eg. core:4:6, es:2:0",
            cxxopts::value<std::string>());
        if constexpr(!compile_info::supports_command_line)
        {
            options.add_options("Game")(
                "map", "Which map file to load", cxxopts::value<std::string>());
            options.add_options("Demo")
                //
                ("biped", "Biped to spawn", cxxopts::value<std::string>())
                //
                ("weapon", "Weapon to spawn", cxxopts::value<std::string>())
                //
                ("marker",
                 "Marker to attach weapon to",
                 cxxopts::value<std::string>())
                //
                ("start_anim",
                 "Animation to start with",
                 cxxopts::value<std::string>())
                //
                ("height", "Starting height", cxxopts::value<float>())
                //
                ;
        }
        auto& args = GetInitArgs();
        arguments  = options.parse(args.size(), args.data());
        if(BaseArgParser::PerformDefaults(options, args) >= 0)
            return 0;
    }

    rq::runtime_queue::CreateNewQueue("Blam Pose Demo!").assume_value();

    auto& e      = comp_app::createContainer();
    auto& loader = comp_app::configureDefaults(e);

    auto& window = loader.config<comp_app::WindowConfig>();
    window.flags = comp_app::window_flags_t::windowed |
                   comp_app::window_flags_t::resizable;
    if constexpr(compile_info::platform::is_emscripten)
        window.flags = comp_app::window_flags_t::resizable;

    comp_app::app_error app_ec;
    comp_app::addDefaults(e, loader, app_ec);
    C_ERROR_CHECK(app_ec)

    comp_app::AppContainer<BlamData<halo_version>>::addTo(
        e,
        [arguments](
            EntityContainer& e, BlamData<halo_version>&, time_point const&) {
            using namespace compo;

            /* Same component set graphics.cpp registers (graphics.cpp:143-158)
             * — the reused caching.cpp/resource_creation.cpp/rendering.cpp
             * select<>/get<> across all of these. */
            e.register_component_inplace<Model>();
            e.register_component_inplace<SubModel>();
            e.register_component_inplace<BspReference>();
            e.register_component_inplace<ObjectSpawn>();
            e.register_component_inplace<NetworkInfo>();
            e.register_component_inplace<PhysicsData>();
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
            e.register_component_inplace<PlayerInput>();
            e.register_component_inplace<Visibility>();

            e.register_subsystem_inplace<comp_app::FrameTag>();
            e.register_subsystem_inplace<GameEventBus>();
            e.register_subsystem_inplace<Journal>();
            e.register_subsystem_inplace<BlamFiles<halo_version>>();
            e.register_subsystem_inplace<LoadingStatus>();

            e.register_subsystem_inplace<PhysicsBus>();
            auto& net_state = e.register_subsystem_inplace<NetworkState>();
            net_state.client_state = NetworkState::ClientState::None;
            net_state.server_state = NetworkState::ServerState::None;

            auto& params = e.register_subsystem_inplace<RenderingParameters>();
            params.clear_color   = Vecf3(0, 1, 0);
            params.debug_clear   = true;
            params.debug_markers = false;
            params.mipmap_bias   = 0;

            auto& gfx  = e.register_subsystem_inplace<gfx::system>();
            auto  opts = [&arguments]() -> gleam::api::load_options_t {
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

            auto& sound_cache =
                e.register_subsystem_inplace<SoundCache<halo_version>>(nullptr);

            {
                auto& bitm_cache =
                    e.register_subsystem_inplace<BitmapCache<halo_version>>(
                        &gfx, &params);
                auto& shader_cache =
                    e.register_subsystem_inplace<ShaderCache<halo_version>>(
                        std::ref(bitm_cache));
                e.register_subsystem_inplace<ModelCache<halo_version>>(
                    std::ref(bitm_cache), std::ref(shader_cache), &gfx);
                e.register_subsystem_inplace<DebugMarkers>().enabled =
                    &e.subsystem_cast<RenderingParameters>().debug_markers;
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
                window->setName("Blam Pose Demo");

            create_resources(e);
            create_shaders(e);
            set_resource_labels(e);
            alloc_renderer(e);

#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
            auto& dummyConfig = e.subsystem_cast<comp_app::AppLoader>()
                                    .config<comp_app::dummy_plug::Config>();
            if(dummyConfig.enabled)
            {
                auto& dummy =
                    e.subsystem_cast<comp_app::dummy_plug::DummyEventBus>();
                dummy.addEventData({
                    .prio = 0,
                    .handler =
                        [&e](
                            comp_app::dummy_plug::DummyEvent& ev, const void*) {
                            if(ev.event != "pose_apply")
                                return;
                            if(!ev.data.contains("bones"))
                                return;
                            if(!g_pose_demo_biped_model.valid())
                            {
                                cWarning(
                                    "pose_demo: pose_apply received before "
                                    "biped spawned");
                                return;
                            }
                            apply_pose(
                                e.subsystem_cast<ModelCache<halo_version>>(),
                                g_pose_demo_biped_model,
                                ev.data["bones"]);
                        },
                });
                dummy.addEventData({
                    .prio = 0,
                    .handler =
                        [](comp_app::dummy_plug::DummyEvent& ev, const void*) {
                            if(ev.event != "mic_volume")
                                return;
                            g_pose_demo_mic_volume =
                                ev.data.value("volume", 0.f);
                        },
                });
                /* {type:'play_animation', name} plays once then falls back
                 * to the loop animation; {type:'loop_animation', name}
                 * replaces the loop outright. Both look up name in the
                 * biped's already-cached antr_hdr (set by
                 * spawn_static_biped) — no map access needed here. */
                dummy.addEventData({
                    .prio = 0,
                    .handler =
                        [&e](
                            comp_app::dummy_plug::DummyEvent& ev, const void*) {
                            if(ev.event != "play_animation" &&
                               ev.event != "loop_animation")
                                return;
                            if(!g_pose_demo_biped_model.valid())
                                return;
                            std::string name =
                                ev.data.value("name", std::string{});
                            auto& item =
                                e.subsystem_cast<ModelCache<halo_version>>()
                                    .get(g_pose_demo_biped_model);
                            auto idx = find_animation_by_name(
                                item.antr_hdr,
                                e.subsystem_cast<ModelCache<halo_version>>()
                                    .magic,
                                name);
                            if(!idx)
                            {
                                cWarning(
                                    "pose_demo: animation '{}' not found",
                                    name);
                                return;
                            }
                            if(ev.event == "play_animation")
                            {
                                g_pose_demo_oneshot_anim_idx = idx;
                                g_pose_demo_oneshot_start =
                                    std::chrono::steady_clock::now();
                            } else
                            {
                                g_pose_demo_loop_anim_idx    = idx;
                                g_pose_demo_oneshot_anim_idx = std::nullopt;
                            }
                        },
                });
            }
#endif

            using namespace ::platform::url::constructors;

            Url map_filename = MkUrl(
                compile_info::supports_command_line
                    ? (arguments.unmatched().size() >=
                               (compile_info::implicit_resource_dir ? 1 : 2)
                           ? arguments.unmatched().at(
                                 compile_info::implicit_resource_dir ? 0 : 1)
                           : "bloodgulch.map")
                : arguments.count("map") ? arguments["map"].as<std::string>()
                                         : "bloodgulch.map",
                compile_info::supports_command_line ? RSCA::SystemFile
                                                    : RSCA::AssetFile);
            Url bitmap_filename =
                (map_filename.path().dirname() / "bitmaps.map")
                    .url(map_filename.flags);

            using result_type = blam::map_container<halo_version>::result_type;
            using AsyncResource = comp_app::FileMapper::Resource;

            struct MapReadResult
            {
                result_type                    map;
                std::shared_ptr<AsyncResource> resource;
            };

            auto& file_mapper = e.subsystem_cast<comp_app::FileMapper>();
            auto& resources   = e.subsystem_cast<BlamResources>();

            auto map_read_task = rq::
                dependent_task<std::shared_ptr<AsyncResource>, MapReadResult>::
                    CreateProcessor(
                        file_mapper.fetch(map_filename),
                        [](std::shared_ptr<AsyncResource>* data) mutable
                            -> MapReadResult {
                            if(!data || !(*data))
                                return MapReadResult{
                                    blam::map_load_error::map_file_too_small,
                                    nullptr};
                            auto map =
                                blam::map_container<halo_version>::from_bytes(
                                    **data, halo_version_v);
                            return MapReadResult{std::move(map), *data};
                        });

            auto bitmap_fetch = file_mapper.fetch(bitmap_filename).share();

            auto map_load_task = rq::CreateMultiTask<int>(
                [&e](
                    MapReadResult                  map_result,
                    std::shared_ptr<AsyncResource> bitmap_data) -> int {
                    result_type& map = map_result.map;
                    if(map.has_error())
                    {
                        cWarning(
                            "pose_demo: failed to parse map: {}",
                            magic_enum::enum_name(map.error()));
                        return -1;
                    }

                    auto& files = e.subsystem_cast<BlamFiles<halo_version>>();
                    files.container   = std::move(map.value());
                    files.map_file    = map_result.resource;
                    files.bitmap_file = bitmap_data;

                    e.subsystem_cast<BitmapCache<halo_version>>().load_from(
                        files.container);
                    e.subsystem_cast<BSPCache<halo_version>>().load_from(
                        files.container);
                    e.subsystem_cast<ModelCache<halo_version>>().load_from(
                        files.container);
                    e.subsystem_cast<ShaderCache<halo_version>>().load_from(
                        files.container);
                    e.subsystem_cast<SoundCache<halo_version>>().load_from(
                        files.container);

                    auto& bitmaps =
                        e.subsystem_cast<BitmapCache<halo_version>>();
                    if(bitmap_data)
                    {
                        auto bitm_magic = blam::map_ptr(
                            static_cast<BytesConst>(*bitmap_data));
                        bitmaps.load_bitmaps_from(bitm_magic);
                    } else
                        cWarning(
                            "pose_demo: could not fetch bitmaps.map alongside "
                            "the map file");

                    {
                        auto& gpu = e.subsystem_cast<BlamResources>();
                        auto& model_cache =
                            e.subsystem_cast<ModelCache<halo_version>>();
                        auto vert                  = gpu.model_buf->map(0);
                        auto index                 = gpu.model_index->map(0);
                        model_cache.vert_buffer    = Bytes::ofContainer(vert);
                        model_cache.element_buffer = Bytes::ofContainer(index);
                    }

                    setup_fixed_camera(e);
                    spawn_static_biped(e, files.container);

                    {
                        auto& gpu = e.subsystem_cast<BlamResources>();
                        gpu.model_buf->unmap();
                        gpu.model_index->unmap();
                    }

                    bitmaps.allocate_storage();

                    auto& loading_status    = e.subsystem_cast<LoadingStatus>();
                    loading_status.app_info = e.service<comp_app::AppInfo>();
                    loading_status.loaded_map     = LoadingStatus::loaded;
                    loading_status.loaded_bitmaps = LoadingStatus::loaded;
                    loading_status.loaded_sounds  = LoadingStatus::loaded;
                    loading_status.loaded_shaders = LoadingStatus::loaded;
                    loading_status.check_all_loaded();
                    return 0;
                },
                map_read_task->get_future(),
                bitmap_fetch);

            rq::runtime_queue::Queue(std::move(map_load_task)).assume_value();
            rq::runtime_queue::Queue(
                resources.background_worker, std::move(map_read_task))
                .assume_value();
        },
        [](EntityContainer& e,
           BlamData<halo_version>&,
           time_point const&,
           duration const&) {
            for(auto entity : e.select<PlayerCamera>())
            {
                auto& cam = entity.get<PlayerCamera>();
                cam.camera.zVals = {100.f, 0.001f};

                static const Matf4 bsp_basis{
                    {0, 0, 1, 0}, {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 1}};

                Matf4 view_matrix = glm::translate(
                    glm::mat4_cast(cam.camera.rotation) * bsp_basis,
                    -cam.camera.position);

                cam.matrix       = GenPerspective(cam.camera);
                cam.matrix[2][2] = 0.f;
                cam.matrix       = cam.matrix * view_matrix;
                cam.rotation     = glm::mat3_cast(cam.camera.rotation);
            }

            if(g_pose_demo_pistol_attached)
            {
                auto* biped_model  = e.get<Model>(g_pose_demo_biped_entity);
                auto* pistol_model = e.get<Model>(g_pose_demo_pistol_entity);
                if(biped_model && pistol_model)
                {
                    auto& model_cache =
                        e.subsystem_cast<ModelCache<halo_version>>();
                    auto& biped_item = model_cache.get(g_pose_demo_biped_model);
                    if(g_pose_demo_hand_node_idx <
                           biped_item.bone_matrices.size() &&
                       g_pose_demo_hand_node_idx < biped_item.inv_bind.size())
                    {
                        Matf4 bind_world_hand = glm::inverse(
                            biped_item.inv_bind[g_pose_demo_hand_node_idx]);
                        Matf4 world_hand =
                            biped_item
                                .bone_matrices[g_pose_demo_hand_node_idx] *
                            bind_world_hand;
                        pistol_model->transform = biped_model->transform *
                                                  world_hand *
                                                  g_pose_demo_hand_marker_local;
                    }
                }
            }
        },
        [](EntityContainer&, BlamData<halo_version>&, time_point const&) {});

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN_CUSTOM(pose_demo_main, 0x1 | 0x2)
