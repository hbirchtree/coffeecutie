#include "bitmap_cache.h"
#include "blam_files.h"
#include "caching.h"
#include "coffee/comp_app/services.h"
#include "components.h"
#include "data.h"
#include "loading.h"
#include "networking.h"
#include "physics.h"
#include "resource_creation.h"
#include "selected_version.h"
#include "shader_cache.h"
#include "sounds.h"
#include "task.h"
#include "ui.h"
#include "ui_caching.h"

#include <algorithm>
#include <coffee/comp_app/file_mapper.h>
#include <coffee/components/entity_container.h>
#include <coffee/core/debug/formatting.h>

#include <fmt_extensions/url_types.h>

using Coffee::cDebug;
using Coffee::Resource;
using platform::url::Path;
using platform::url::Url;
using semantic::RSCA;

static void load_resources(
    compo::EntityContainer&             e,
    MapLoadFinishedEvent<halo_version>& finished,
    MapChangedEvent<halo_version>&      changed);

static void filter_maps(std::vector<platform::file::file_entry_t>& files)
{
    auto remove_it =
        std::remove_if(files.begin(), files.end(), [](auto const& file) {
            Path filepath(file.name.data());
            if(filepath.extension() != "map")
                return true;
            return file.name.find("bitmaps") != std::string::npos;
        });
    files.erase(remove_it, files.end());
}

static void load_bitmaps(compo::EntityContainer& e, blam::map_ptr const& magic)
{
    auto& api            = e.subsystem_cast<gleam::system>();
    auto& loading_status = e.subsystem_cast<LoadingStatus>();
    auto& bitmaps        = e.subsystem_cast<BitmapCache<halo_version>>();
    auto& files          = e.subsystem_cast<BlamFiles<halo_version>>();
    if constexpr(compile_info::platform::is_32bit)
        return;
    bitmaps.allocate_storage();
    loading_status.loaded_bitmaps = LoadingStatus::in_progress;
    if(auto tex_queue = api.queue<gleam::system::queues::texture_decode>())
    {
        [[maybe_unused]] auto res = rq::runtime_queue::QueueImmediate(
            tex_queue, rq::detail::duration(), [load = &loading_status] {
                load->loaded_bitmaps = LoadingStatus::loaded;
                load->check_all_loaded();
            });
    } else
    {
        loading_status.loaded_bitmaps = LoadingStatus::loaded;
        loading_status.check_all_loaded();
    }
}

static void load_sounds(compo::EntityContainer& e, blam::map_ptr const& magic)
{
    auto& files          = e.subsystem_cast<BlamFiles<halo_version>>();
    auto& sounds         = e.subsystem_cast<SoundCache<halo_version>>();
    auto& loading_status = e.subsystem_cast<LoadingStatus>();
    sounds.load_sounds_from(magic);
    loading_status.loaded_sounds = LoadingStatus::loaded;
    loading_status.check_all_loaded();
}

static void init_map(
    compo::EntityContainer& e, MapLoadFinishedEvent<halo_version>& finished)
{
    auto& loading_status = e.subsystem_cast<LoadingStatus>();
    if(std::exchange(loading_status.init_started, true))
    {
        cWarning("init_map fired twice for same map load, ignoring");
        return;
    }
    auto& bitmaps     = e.subsystem_cast<BitmapCache<halo_version>>();
    auto& bsps        = e.subsystem_cast<BSPCache<halo_version>>();
    auto& models      = e.subsystem_cast<ModelCache<halo_version>>();
    auto& shaders     = e.subsystem_cast<ShaderCache<halo_version>>();
    auto& sounds      = e.subsystem_cast<SoundCache<halo_version>>();
    auto& ui_elements = e.subsystem_cast<UIElementCache<halo_version>>();

    loading_status.app_info = e.service<comp_app::AppInfo>();

    u32 num = 0;
    for([[maybe_unused]] auto const& i : e.select(ObjectGC))
        num++;
    cDebug("Number of GC entities: {}", num);

    auto& files = e.subsystem_cast<BlamFiles<halo_version>>();

    GameEvent                     event{GameEvent::MapChanged};
    MapChangedEvent<halo_version> changed{
        .container = files.container,
        // .bitmap_magic = blam::magic_data_t(finished.bitmap_file.view),
        .scenario = files.container.tags
                        ->scenario(files.container.map, files.container.magic)
                        .value(),
    };
    e.subsystem_cast<GameEventBus>().inject(event, &changed);

    comp_app::EventBus<SoundEvent>* sound_bus =
        e.service<comp_app::EventBus<SoundEvent>>();

    bitmaps.load_from(changed.container);
    bsps.load_from(changed.container);
    models.load_from(changed.container);
    shaders.load_from(changed.container);
    sounds.load_from(changed.container);

    if(finished.bitmaps.has_value())
        bitmaps.load_bitmaps_from(*finished.bitmaps);

    event = {GameEvent::MapDataReady};
    MapDataReadyEvent ready{};
    e.subsystem_cast<GameEventBus>().inject(event, &ready);

    load_resources(e, finished, changed);
}

static void load_resources(
    compo::EntityContainer&             e,
    MapLoadFinishedEvent<halo_version>& finished,
    MapChangedEvent<halo_version>&      changed)
{
    auto& loading_status = e.subsystem_cast<LoadingStatus>();
    auto& bitmaps        = e.subsystem_cast<BitmapCache<halo_version>>();
    auto& bsps           = e.subsystem_cast<BSPCache<halo_version>>();
    auto& models         = e.subsystem_cast<ModelCache<halo_version>>();
    auto& shaders        = e.subsystem_cast<ShaderCache<halo_version>>();
    auto& sounds         = e.subsystem_cast<SoundCache<halo_version>>();
    auto& ui_elements    = e.subsystem_cast<UIElementCache<halo_version>>();
    auto& files          = e.subsystem_cast<BlamFiles<halo_version>>();
    auto* sound_bus      = e.service<comp_app::EventBus<SoundEvent>>();

    load_scenario_bsp(e, changed);
    load_scenario_scenery(e, changed);
    load_collision_debug(e, changed);
    load_ui_items(e, changed);

    if(finished.bitmaps.has_value())
    {
        bitmaps.allocate_storage();
        loading_status.loaded_bitmaps = LoadingStatus::loaded;
        loading_status.check_all_loaded();
    }

    // generation_idx_t player_model;
    // auto             bipeds =
    //     changed.scenario->objects.bipeds.palette.data(files.container.magic)
    //         .value();
    // for(auto const& mod2_tag : bipeds)
    // {
    //     player_model = models.predict(
    //         mod2_tag.front(), blam::mod2::mod2_lod::lod_high_ext);
    // }
    auto recipe = shared_recipes::player_recipe;
    u32 num_pinfo = 0;
    for(auto const& pinfo : e.select<PlayerInfo>())
    {
        auto const* info = e.get<PlayerInfo>(pinfo.id());
        if(info && !info->is_remote())
            ++num_pinfo;
    }
    // if(player_model.valid())
    //     recipe.components.push_back(compo::type_hash_v<Model>());
    u64 main_biped_id{0};
    if(num_pinfo == 0)
    {
        cDebug("Creating player data");
        auto* controllers         = e.service<comp_app::ControllerInput>();
        auto* window              = e.service<comp_app::Windowing>();
        u32   num_controllers     = controllers ? controllers->count() : 0,
            allocated_controllers = 0;
        for(auto i : range<>(4))
        {
            auto  ref       = e.create_entity(recipe);
            auto& info      = ref.get<PlayerInfo>();
            info.player_idx = i;
            info.seat_idx   = i;
            auto& camera    = ref.get<PlayerCamera>();
            if(i == 0)
            {
                cDebug("Keyboard seat assigned");
                main_biped_id           = ref.id();
                camera.keyboard.enabled = true;
                if(auto assigned =
                       e.subsystem_cast<NetworkState>().remote_player_idx)
                    info.player_idx = *assigned;
                if(auto const& name =
                       e.subsystem_cast<NetworkState>().local_player_name)
                    info.name = *name;
            }
            if(num_controllers > allocated_controllers)
            {
                camera.controller.index = allocated_controllers;
                ++allocated_controllers;
            }
        }
    }
    rq::runtime_queue::Queue(
        rq::dependent_task<void, void>::CreateSink(
            loading_status.finished.get_future(),
            [&e](void*) {
                cDebug("Load finish signalled!");
                for(auto player : e.select<PlayerInfo>())
                {
                    auto* player_info = e.get<PlayerInfo>(player.id());
                    if(!player_info->is_remote())
                    {
                        player_info->loading_progress = 100;
                    }
                }
                auto& loading    = e.subsystem_cast<LoadingStatus>();
                loading.progress = -1;
                loading.status   = "Complete!";
                loading.check_all_loaded();

                GameEvent ev{.type = GameEvent::MapAllLoaded};
                e.subsystem_cast<GameEventBus>().process(ev, nullptr);
            }))
        .assume_value();

    // For debugging: go through all the bitmaps
    u32 num_snds{0}, num_bitms{0}, num_mod{0};
    cDebug("Tags:");
    blam::tag_index_view                    index(files.container);
    std::function<void(blam::tag_t const&)> explore_tagc;
    explore_tagc = [&](blam::tag_t const& tag) {
        // Go through tag collections and load them
        // I think this is the way? There are no other references to eg.
        // the title music *other than* the tagc tags
        auto const& magic    = files.container.magic;
        auto        tag_coll = tag.data<blam::tag_collection_t>(magic).value();
        auto        tags     = tag_coll->tags.data(magic).value();
        for(auto const& tag : tags)
        {
            cDebug(" - tagc: {}", tag.tag_class_name());
            switch(tag.tag_class)
            {
            case blam::tag_class_t::Soul:
                // Put in UI cache
                ui_elements.explore(tag);
                break;
            case blam::tag_class_t::lsnd: {
                // Put in sound cache
                LoopSoundEvent loop = {
                    .sound = &tag,
                };
                SoundEvent ev = {
                    .type      = SoundEvent::loop_sound,
                    .entity_id = main_biped_id,
                };
                if(sound_bus)
                    sound_bus->inject(ev, &loop);
                break;
            }
            case blam::tag_class_t::tagc: {
                explore_tagc(*index.find(tag));
                break;
            }
            default:
                break;
            }
        }
    };

    for(blam::tag_t const& tag : index)
    {
        if(tag.matches(blam::tag_class_t::bitm))
        {
            num_bitms++;
        }
        if(tag.matches(blam::tag_class_t::snd))
        {
            num_snds++;
        }
        if(tag.matches(blam::tag_class_t::mod2))
        {
            num_mod++;
        }
        if(tag.matches(blam::tag_class_t::tag_collection))
        {
            // explore_tagc(tag);
        }
    }

    cDebug(
        "Map tag summary: {} bitmaps, {} sounds, {} models",
        num_bitms,
        num_snds,
        num_mod);

    /* TODO: This needs a network round-trip */
    create_camera(
        e,
        changed.scenario->player_start.locations.data(changed.container.magic)
            .value());

    if(auto window_config = e.service<comp_app::WindowInfo>())
    {
        auto map_name = files.container.map->full_mapname();
        window_config->setName(fmt::format("Blam! : {0}", map_name));
    }

    /* With Xbox, we're done loading here */
    if(files.container.map->version == blam::version_t::xbox)
    {
        loading_status.loaded_bitmaps = LoadingStatus::loaded;
        loading_status.loaded_sounds  = LoadingStatus::loaded;
    }
    /* MCC doesn't have a sounds file? Or maybe it's elsewhere */
    if(files.container.map->version == blam::version_t::mcc)
    {
        loading_status.loaded_sounds = LoadingStatus::loaded;
    }
    loading_status.loaded_map = LoadingStatus::loaded;
    loading_status.check_all_loaded();

    // TODO: One day load loc.map on PC/Custom Edition

    if(files.container.map->version == blam::version_t::xbox)
    {
        load_bitmaps(e, files.container.magic.ptr_only());
        load_sounds(e, files.container.magic.ptr_only());
    }
}

static MapListingEvent list_maps(
    compo::EntityContainer& e, MapLoadEvent const& load)
{
    MapListingEvent listing;
    if(load.directory)
    {
        listing.directory = load.directory.value();
    } else if(load.file)
    {
        using namespace platform::url::constructors;
        auto map_dir      = (*load.file).path().dirname();
        listing.directory = map_dir.url(load.file->flags);
        // data.bitmap_file = std::make_unique<Resource>(
        //     (map_dir / "bitmaps.map").url(map_file->flags));
        // data.map_file      = std::make_unique<Resource>(*map_file);
        // data.map_directory = map_dir.url(RSCA::SystemFile);
    }

    listing.bitmap_file =
        (listing.directory.path() / "bitmaps.map").url(listing.directory.flags);
    listing.sound_file =
        (listing.directory.path() / "sounds.map").url(listing.directory.flags);

    if(auto maps_ = platform::file::list(listing.directory); maps_.has_error())
    {
        cDebug("Failed to list maps: {0}", maps_.error());
    } else
    {
        using platform::url::constructors::MkSysUrl;

        auto maps = maps_.value();
        filter_maps(maps);
        auto base_dir     = listing.directory.path();
        auto base_storage = listing.directory.flags;
        for(auto const& map : maps)
            listing.maps.push_back((base_dir / map.name).url(base_storage));
        std::sort(listing.maps.begin(), listing.maps.end());
        cDebug("Maps:");
        for(auto const& map : listing.maps)
            cDebug(" - {0}", map.internUrl);
    }

    // TODO: Maybe cache this? Maybe map<name, url> for convenience

    GameEvent list{GameEvent::MapListing};
    e.subsystem_cast<GameEventBus>().inject(list, &listing);

    cDebug("map={}, bitmap={}", load.file, listing.bitmap_file);

    return listing;
}

static void open_map(compo::EntityContainer& e, MapLoadEvent const& load)
{
    LoadingStatus& loading = e.subsystem_cast<LoadingStatus>();

    /* clear entities, evict cache entries */
    e.remove_entity_if([&e](compo::Entity const& en) {
        return stl_types::any_flag_of(en.tags, ObjectGC);
    });

    if(auto* sound_bus = e.service<comp_app::EventBus<SoundEvent>>())
    {
        SoundEvent ev = {.type = SoundEvent::clear_all};
        sound_bus->process(ev, nullptr);
    }

    // TODO: Decouple this from open_map so we can do MapLoadByName
    // We also need a way to store the listed maps so we can implement that
    auto listing = list_maps(e, load);

    if(!load.file)
    {
        cWarning(
            "open_map: no valid file for this load (origin={}), aborting "
            "silently otherwise",
            magic_enum::enum_name(load.origin));
        return;
    }

    ProfContext _;

    loading.loading = true;

    using result_type = blam::map_container<halo_version>::result_type;

    {
        Physics::Event reset_ev{Physics::Event::Reset};
        Physics::Reset reset{};
        e.subsystem_cast<PhysicsBus>().process(reset_ev, &reset);
    }

    auto& files = e.subsystem_cast<BlamFiles<halo_version>>();

    files.bitmap_file.reset();
    files.sound_file.reset();

    loading.progress       = 0;
    loading.loaded_map     = LoadingStatus::none;
    loading.loaded_bitmaps = LoadingStatus::none;
    loading.loaded_sounds  = LoadingStatus::none;
    loading.init_started   = false;
    loading.finished       = std::promise<void>();

    auto& file_mapper   = e.subsystem_cast<comp_app::FileMapper>();
    using AsyncResource = comp_app::FileMapper::Resource;

    u32 const this_generation =
        ++e.subsystem_cast<BlamFiles<halo_version>>().load_generation;
    auto load_is_stale = [&e, this_generation]() {
        return e.subsystem_cast<BlamFiles<halo_version>>().load_generation !=
               this_generation;
    };

    std::function<void(std::string_view, i16)> progress_cb =
        rq::runtime_queue::BindToQueue(
            std::function<void(std::string_view, i16)>(
                [&loading](std::string_view status, i16 progress) {
                    loading.status = std::string(status.begin(), status.end());
                    loading.progress   = progress;
                    loading.loaded_map = progress == 100
                                             ? LoadingStatus::loaded
                                             : LoadingStatus::in_progress;
                    loading.check_all_loaded();
                }));

    auto store_map_file = rq::runtime_queue::BindToQueue(
        std::function<void(std::shared_ptr<AsyncResource>)>(
            [&e, load_is_stale](std::shared_ptr<AsyncResource> resource) {
                if(load_is_stale())
                    return;
                e.subsystem_cast<BlamFiles<halo_version>>().map_file =
                    std::move(resource);
            }));

    auto map_read_task =
        rq::dependent_task<std::shared_ptr<AsyncResource>, result_type>::
            CreateProcessor(
                file_mapper.fetch(*load.file),
                [progress_cb,
                 store_map_file](std::shared_ptr<AsyncResource>* data) mutable
                    -> result_type {
                    if(!data || !(*data))
                    {
                        rq::runtime_queue::CancelTask(
                            rq::runtime_queue::GetSelfId().assume_value());
                        return blam::map_load_error::map_file_too_small;
                    }
                    auto map = blam::map_container<halo_version>::from_bytes(
                        **data, halo_version_v, std::move(progress_cb));
                    if(map.has_error())
                        return stl_types::failure(map.error());
                    store_map_file(std::move(*data));
                    return stl_types::success(std::move(map.value()));
                });

    auto map_load_fun = [&e, load_is_stale](
                            result_type                    map,
                            std::shared_ptr<AsyncResource> bitmap_data) -> int {
        ProfContext _("Notifying systems of new map");

        if(load_is_stale())
        {
            cWarning("Discarding superseded map load");
            return -1;
        }
        if(map.has_error())
        {
            cWarning(
                "Failed to load map: {}", magic_enum::enum_name(map.error()));
            return -1;
        }
        auto& files = e.subsystem_cast<BlamFiles<halo_version>>();

        files.container   = std::move(map.value());
        files.bitmap_file = std::move(bitmap_data);

        auto&     gbus = e.subsystem_cast<GameEventBus>();
        GameEvent event{GameEvent::MapLoadFinished};
        MapLoadFinishedEvent<halo_version> finished = {
            .container = &files.container,
        };
        if(files.bitmap_file)
            finished.bitmaps =
                blam::map_ptr(static_cast<BytesConst>(*files.bitmap_file));
        finished.map_name  = files.container.internal_name();
        finished.map_title = files.container.name();
        gbus.inject(event, &finished);
        return 0;
    };

    if(!std::is_same_v<halo_version, blam::xbox_version_t>)
    {
        auto bitmaps_file = file_mapper.fetch(listing.bitmap_file).share();
        auto sounds_file  = file_mapper.fetch(listing.sound_file);

        auto map_load_task = rq::CreateMultiTask<int>(
            std::move(map_load_fun), map_read_task->get_future(), bitmaps_file);
        auto map_load = map_load_task->get_future().share();
        if(sounds_file.valid())
            rq::runtime_queue::Queue(
                rq::CreateMultiTask<void>(
                    [&e, load_is_stale](
                        int, std::shared_ptr<AsyncResource> data) mutable {
                        if(!data || load_is_stale())
                        {
                            rq::runtime_queue::CancelTask(
                                rq::runtime_queue::GetSelfId().assume_value());
                            return;
                        }
                        auto& files =
                            e.subsystem_cast<BlamFiles<halo_version>>();
                        files.sound_file = std::move(data);
                        load_sounds(
                            e,
                            blam::map_ptr(
                                static_cast<BytesConst>(*files.sound_file)));
                    },
                    map_load,
                    std::move(sounds_file)))
                .assume_value();
        rq::runtime_queue::Queue(std::move(map_load_task)).assume_value();
    } else
    {
        auto map_load_task = rq::CreateMultiTask<int>(
            [loader = std::move(map_load_fun)](result_type map) -> int {
                loader(std::move(map), {});
                return 0;
            },
            map_read_task->get_future());
        rq::runtime_queue::Queue(std::move(map_load_task)).assume_value();
    }
    BlamResources& resources = e.subsystem_cast<BlamResources>();
    rq::runtime_queue::Queue(
        resources.background_worker, std::move(map_read_task))
        .assume_value();
}

void setup_load_eventhandlers(compo::EntityContainer& e)
{
    ProfContext _;
    auto&       gbus = e.subsystem_cast<GameEventBus>();
    gbus.addEventFunction<MapLoadByNameEvent>(
        0, [&e](GameEvent&, MapLoadByNameEvent* load) {
            auto&        files = e.subsystem_cast<BlamFiles<halo_version>>();
            auto&        gbus  = e.subsystem_cast<GameEventBus>();
            GameEvent    event{GameEvent::MapLoadStart};
            MapLoadEvent map_load = {
                .origin = load->origin,
                .file   = files.map_directory /
                        Path(load->map_name).addExtension("map"),
            };
            gbus.inject(event, &map_load);
        });

    gbus.addEventFunction<MapLoadEvent>(
        0, [&e](GameEvent&, MapLoadEvent* load) {
            auto& net_state = e.subsystem_cast<NetworkState>();
            /* Reject local map changes while connected to a server */
            if(load->origin == MapLoadEvent::Local &&
               net_state.client_state == NetworkState::ClientState::Connected)
                return;
            cDebug("Starting MapLoad handler");
            open_map(e, *load);
        });

    gbus.addEventFunction<MapLoadFinishedEvent<halo_version>>(
        0,
        [&e](GameEvent&, MapLoadFinishedEvent<halo_version>* finished) mutable {
            cDebug("Starting MapFinished handler");
            init_map(e, *finished);
        });

    gbus.addEventFunction<MapRequestListingEvent>(
        0, [&e](GameEvent&, MapRequestListingEvent*) {
            auto& files = e.subsystem_cast<BlamFiles<halo_version>>();
            list_maps(
                e,
                MapLoadEvent{
                    .directory = files.map_directory,
                });
        });
}
