#include "data.h"
#include "loading.h"
#include "rendering.h"
#include "resource_creation.h"
#include "selected_version.h"
#include <coffee/graphics/apis/gleam/rhi_emulation.h>

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/net_profiling.h>
#endif

#include <coffee/comp_app/fps_counter.h>

void install_imgui_widgets(
    compo::EntityContainer&           e,
    BlamData<halo_version>&           data,
    std::function<void(Url const&)>&& map_select);

static void filter_maps(std::vector<platform::file::file_entry_t>& files)
{
    auto remove_it
        = std::remove_if(files.begin(), files.end(), [](auto const& file) {
              Path filepath(file.name.data());
              if(filepath.extension() != "map")
                  return true;
              return file.name.find("bitmaps") != std::string::npos;
          });
    files.erase(remove_it, files.end());
}

static bool load_maps(
    BlamFiles& data, std::optional<Url> map_file, std::optional<Url> map_dir)
{
    if(map_dir)
    {
        data.map_directory = *map_dir;
        if(auto maps_ = platform::file::list(*map_dir); maps_.has_error())
        {
            cDebug("Failed to list maps: {0}", maps_.error());
        } else
        {
            using platform::url::constructors::MkSysUrl;

            auto maps = maps_.value();
            filter_maps(maps);
            data.maps.clear();
            auto base_dir     = data.map_directory.path();
            auto base_storage = data.map_directory.flags;
            for(auto const& map : maps)
                data.maps.push_back((base_dir / map.name).url(base_storage));
            std::sort(data.maps.begin(), data.maps.end());
            cDebug("Maps:");
            for(auto const& map : data.maps)
                cDebug(" - {0}", map.internUrl);
        }
    }
    if(map_file)
    {
        auto map_dir     = (*map_file).path().dirname();
        data.bitmap_file = std::make_unique<Resource>(
            (map_dir / "bitmaps.map").url(map_file->flags));
        data.map_file      = std::make_unique<Resource>(*map_file);
        data.map_directory = map_dir.url(RSCA::SystemFile);
        return true;
    }
    return false;
}

static void reinit_map(
    compo::EntityContainer& e, BlamData<halo_version>& data, BlamFiles* files)
{
    ProfContext _;

    auto& bitmaps = e.subsystem_cast<BitmapCache<halo_version>>();
    auto& bsps    = e.subsystem_cast<BSPCache<halo_version>>();
    auto& models  = e.subsystem_cast<ModelCache<halo_version>>();
    auto& shaders = e.subsystem_cast<ShaderCache<halo_version>>();

    /* clear entities, evict cache entries */
    e.remove_entity_if(
        [](compo::Entity const& e) { return any_flag_of(e.tags, ObjectGC); });

    data.map_container = blam::map_container<halo_version>::from_bytes(
                             *files->map_file, halo_version_v)
                             .value();
    files->bitm_magic
        = blam::magic_data_t(C_OCAST<BytesConst>(*files->bitmap_file).view);
    files->map_magic = data.map_container.magic;
    data.tags_view   = std::make_unique<blam::tag_index_view<halo_version>>(
        std::ref(data.map_container));
    data.scenario = data.map_container.tags->scenario(data.map_container.map)
                        .value()
                        ->data<blam::scn::scenario<halo_version>>(
                            data.map_container.magic)
                        .value();

    bitmaps.load_from(data.map_container, files->bitm_magic);
    bsps.load_from(data.map_container);
    models.load_from(data.map_container);
    shaders.load_from(data.map_container);

    load_scenario_bsp(e, data);
    load_scenario_scenery(e, data);

    create_camera(
        e, data.scenario->mp.player_spawns.data(files->map_magic).value());

    if(auto window_config = e.service<comp_app::WindowInfo>())
    {
        auto map_name = data.map_container.map->full_mapname();
        window_config->setName(Strings::fmt("Blam! : {0}", map_name));
    }
    {
        ProfContext _("Texture allocation");
        bitmaps.allocate_storage();
    }

    files->last_updated = compo::clock::now();
}

i32 blam_main(i32, cstring_w*)
{
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
        [](EntityContainer&        e,
           BlamData<halo_version>& data,
           time_point const&) {
            ProfContext _(__FUNCTION__);

            auto& gfx = e.register_subsystem_inplace<gfx::system>();
            auto  load_error
                = gfx.load(/*gfx::emulation::qcom::adreno_320()*/
//                                       {
//                                           .api_version = 0x410,
//                                           .api_type    =
//                                           gleam::api_type_t::core,
//                                       }
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

            e.register_subsystem_inplace<comp_app::FrameTag>();

            install_imgui_widgets(e, data, [&e, &data](Url const& map) {
                auto& files = e.subsystem_cast<BlamFiles>();
                load_maps(files, map, std::nullopt);
                reinit_map(e, data, &files);
            });

            auto& blam_files = e.register_subsystem_inplace<BlamFiles>();

            {
                auto& bitm_cache
                    = e.register_subsystem_inplace<BitmapCache<halo_version>>(
                        &blam_files, &gfx);
                auto& shader_cache
                    = e.register_subsystem_inplace<ShaderCache<halo_version>>(
                        std::ref(bitm_cache));
                auto& model_cache
                    = e.register_subsystem_inplace<ModelCache<halo_version>>(
                        std::ref(bitm_cache), std::ref(shader_cache), &gfx);
                e.register_subsystem_inplace<BSPCache<halo_version>>(
                    std::ref(bitm_cache),
                    std::ref(shader_cache),
                    std::ref(model_cache));
                //                auto& model_cache =
            }

            //            auto& magic = data.map_container.magic;

            //            e.register_subsystem_inplace<BlamScript<halo_version>>(
            //                std::ref(data.map_container), data.scenario,
            //                magic);

            if(auto window = e.service<comp_app::WindowInfo>())
            {
                window->setName("Blam!");
            }

            create_resources(e);
            create_shaders(e);
            set_resource_labels(e);
            alloc_renderer(e);

            using namespace ::platform::url::constructors;

            Url map_filename;
            Url map_dir;

            if constexpr(
                compile_info::platform::is_android
                || compile_info::platform::is_emscripten)
            {
#if defined(COFFEE_ANDROID)
                map_filename = MkUrl("", RSCA::AssetFile);
                map_dir      = "."_asset;
#else
                map_filename = "beavercreek.map"_asset;
                map_dir      = "."_asset;
#endif
            } else if(!GetInitArgs().arguments().empty())
            {
                map_filename
                    = MkUrl(GetInitArgs().arguments().at(0), RSCA::SystemFile);
                map_dir = map_filename.path().dirname().url(RSCA::SystemFile);
            } else
            {
                map_dir = "."_asset;
            }
            load_maps(
                blam_files,
                map_filename.valid() ? std::make_optional(map_filename)
                                     : std::nullopt,
                map_dir);
            if(map_filename.valid())
                reinit_map(e, data, &blam_files);
        },
        [](EntityContainer& e,
           BlamData<halo_version>&,
           time_point const&,
           duration const& t) {
            auto& camera = e.subsystem_cast<BlamCamera>();

            /* Update camera position */
            camera.std_camera->tick();
            if(e.service<comp_app::ControllerInput>())
                camera.controller_camera(
                    e.service<comp_app::ControllerInput>()->state(0), t);
            using namespace typing::vectors::scene;
            camera.camera.aspect = 16.f / 9.f;
            //                =
            //                e.service<comp_app::Windowing>()->size().aspect();
            //            camera.camera.aspect = 1.6f;
            camera.camera.zVals = {0.00000001f, 500.f};

            camera.camera_matrix
                = GenPerspective(camera.camera)
                  /* * GenTransform(camera.camera)*/
                  //
                  * typing::vectors::matrixify(typing::vectors::normalize_quat(
                        camera.camera.rotation))
                  * typing::vectors::scale(Matf4(), Vecf3(10))
                  * typing::vectors::translation(Matf4(), camera.camera.position)
                //
                ;
        },
        [](EntityContainer&, BlamData<halo_version>&, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(blam_main)
