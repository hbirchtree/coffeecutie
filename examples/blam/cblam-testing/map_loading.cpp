#include "bitmap_cache.h"
#include "blam_files.h"
#include "caching.h"
#include "components.h"
#include "data.h"
#include "loading.h"
#include "resource_creation.h"
#include "selected_version.h"
#include "shader_cache.h"
#include "ui.h"

#include <coffee/comp_app/file_mapper.h>
#include <coffee/components/entity_container.h>
#include <coffee/core/debug/formatting.h>

#include <fmt_extensions/url_types.h>

using Coffee::cDebug;
using Coffee::Resource;
using platform::url::Path;
using platform::url::Url;
using semantic::RSCA;

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

static void init_map(
    compo::EntityContainer& e, MapLoadFinishedEvent<halo_version>& finished)
{
    auto& loading_status = e.subsystem_cast<LoadingStatus>();
    auto& bitmaps        = e.subsystem_cast<BitmapCache<halo_version>>();
    auto& bsps           = e.subsystem_cast<BSPCache<halo_version>>();
    auto& models         = e.subsystem_cast<ModelCache<halo_version>>();
    auto& shaders        = e.subsystem_cast<ShaderCache<halo_version>>();
    auto& sounds         = e.subsystem_cast<SoundCache<halo_version>>();

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

    bitmaps.load_from(changed.container);
    bsps.load_from(changed.container);
    models.load_from(changed.container);
    shaders.load_from(changed.container);
    sounds.load_from(changed.container);

    load_scenario_bsp(e, changed);
    load_scenario_scenery(e, changed);
    load_ui_items(e, changed);

    // For debugging: go through all the bitmaps
    if constexpr(!compile_info::platform::is_32bit)
    {
        u32 num_snds{0}, num_bitms{0}, num_mod{0};
        for(blam::tag_t const& tag : blam::tag_index_view(files.container))
        {
            if(tag.matches(blam::tag_class_t::bitm))
            {
                // cDebug(
                //     "Loading bitmap {0}",
                //     tag.to_name().to_string(files.container.magic));
                // bitmaps.predict(tag.as_ref(), 0);
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
        }

        cDebug(
            "Map tag summary: {} bitmaps, {} sounds, {} models",
            num_bitms,
            num_snds,
            num_mod);
    }

    create_camera(
        e,
        changed.scenario->mp.player_spawns.data(changed.container.magic)
            .value());

    if(auto window_config = e.service<comp_app::WindowInfo>())
    {
        auto map_name = files.container.map->full_mapname();
        window_config->setName(fmt::format("Blam! : {0}", map_name));
    }
    // TODO: Move to MapResourcesReady handler
    if(!compile_info::platform::is_32bit &&
       (files.bitmap_file ||
        files.container.map->version == blam::version_t::xbox))
    {
        ProfContext _("Texture allocation");
        bitmaps.allocate_storage();
        loading_status.loaded_bitmaps = true;
    }
    if(files.sound_file)
    {
        sounds.process_sounds();
    }

    /* Depending on when we're done loading, stop the spinner */
    auto& api = e.subsystem_cast<gleam::system>();
    if(auto tex_queue = api.queue<gleam::system::queues::texture_decode>())
    {
        [[maybe_unused]] auto res = rq::runtime_queue::QueueImmediate(
            tex_queue, rq::detail::duration(), [load = &loading_status] {
                load->loading = false;
            });
    } else
        loading_status.loading = false;
}

static void open_map(compo::EntityContainer& e, MapLoadEvent const& load)
{
    /* clear entities, evict cache entries */
    e.remove_entity_if([](compo::Entity const& e) {
        return stl_types::any_flag_of(e.tags, ObjectGC);
    });

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

    GameEvent list{GameEvent::MapListing};
    e.subsystem_cast<GameEventBus>().inject(list, &listing);

    if(!load.file)
        return;

    cDebug("map={}, bitmap={}", load.file, listing.bitmap_file);

    ProfContext _;

    using result_type = blam::map_container<halo_version>::result_type;

    LoadingStatus& loading = e.subsystem_cast<LoadingStatus>();
    auto&          files   = e.subsystem_cast<BlamFiles<halo_version>>();

    files.bitmap_file.reset();
    files.sound_file.reset();

    loading.loading        = true;
    loading.loaded_bitmaps = false;
    loading.loaded_sounds  = false;

    auto& file_mapper = e.subsystem_cast<comp_app::FileMapper>();

    using AsyncResource = comp_app::FileMapper::Resource;
    auto map_loader     = [&e](std::shared_ptr<AsyncResource>* data) mutable {
        ProfContext    _("Launching map decode");
        BlamResources& resources = e.subsystem_cast<BlamResources>();
        LoadingStatus& loading   = e.subsystem_cast<LoadingStatus>();
        auto&          files     = e.subsystem_cast<BlamFiles<halo_version>>();

        std::function<void(std::string_view, i16)> progress_cb =
            [&loading](std::string_view status, i16 progress) {
                loading.status   = std::string(status.begin(), status.end());
                loading.progress = progress;
                cDebug("Map loading status: {}%: {}", progress, status);
            };
        files.map_file = *data;
        auto map_data  = blam::map_container<halo_version>::from_bytes_async(
            resources.background_worker,
            **data,
            halo_version_v,
            rq::runtime_queue::BindToQueue(progress_cb));

        [[maybe_unused]] auto res = rq::runtime_queue::Queue(
            resources.background_worker,
            rq::dependent_task<result_type, void>::CreateSink(
                std::move(map_data), [&e, &files](result_type* map) mutable {
                    ProfContext _("Notifying systems of new map");
                    if(map->has_error())
                    {
                        cWarning(
                            "Failed to load map: {}",
                            magic_enum::enum_name(map->error()));
                        return;
                    }
                    // files.bitmap_file = bitmap.get();
                    files.container = std::move(map->value());

                    auto&            gbus = e.subsystem_cast<GameEventBus>();
                    GameEvent        event{GameEvent::MapDataLoad};
                    MapDataLoadEvent loaded = {
                            .map = *files.map_file,
                        // .bitmap = *files.bitmap_file,
                    };
                    gbus.inject(event, &loaded);
                    event.type = GameEvent::MapLoadFinished;
                    MapLoadFinishedEvent<halo_version> finished = {
                            .container = &files.container,
                    };
                    finished.map_name  = files.container.internal_name();
                    finished.map_title = files.container.name();
                    gbus.inject(event, &finished);
                }));
    };

    rq::runtime_queue::Queue(
        rq::dependent_task<std::shared_ptr<AsyncResource>, void>::CreateSink(
            file_mapper.fetch(*load.file), std::move(map_loader)))
        .assume_value();

    if(std::is_same_v<halo_version, blam::xbox_version_t>)
        return;

    rq::runtime_queue::Queue(
        rq::dependent_task<std::shared_ptr<AsyncResource>, void>::CreateSink(
            file_mapper.fetch(listing.bitmap_file),
            [&e](std::shared_ptr<AsyncResource>* data) mutable {
                auto& files       = e.subsystem_cast<BlamFiles<halo_version>>();
                files.bitmap_file = std::move(*data);
                GameEvent         event{GameEvent::MapResourcesReady};
                MapResourcesReady ready = {
                    .bitmap_file = blam::magic_data_t(
                        static_cast<BytesConst>(*files.bitmap_file)),
                };
                auto& gbus = e.subsystem_cast<GameEventBus>();
                gbus.inject(event, &ready);
            }))
        .assume_value();

    rq::runtime_queue::Queue(
        rq::dependent_task<std::shared_ptr<AsyncResource>, void>::CreateSink(
            file_mapper.fetch(listing.sound_file),
            [&e](std::shared_ptr<AsyncResource>* data) mutable {
                auto& files      = e.subsystem_cast<BlamFiles<halo_version>>();
                files.sound_file = std::move(*data);
                GameEvent         event{GameEvent::MapResourcesReady};
                MapResourcesReady ready = {
                    .sound_file = blam::magic_data_t(
                        static_cast<BytesConst>(*files.sound_file)),
                };
                auto& gbus = e.subsystem_cast<GameEventBus>();
                gbus.inject(event, &ready);
            }))
        .assume_value();
}

void setup_load_eventhandlers(compo::EntityContainer& e)
{
    auto& gbus = e.subsystem_cast<GameEventBus>();
    gbus.addEventFunction<MapLoadEvent>(
        0, [&e](GameEvent&, MapLoadEvent* load) {
            cDebug("Starting MapLoad handler");
            open_map(e, *load);
        });

    gbus.addEventFunction<MapLoadFinishedEvent<halo_version>>(
        0,
        rq::runtime_queue::BindToQueue(
            rq::runtime_queue::await_task,
            std::function(
                [&e](GameEvent&, MapLoadFinishedEvent<halo_version>* finished) {
                    cDebug("Starting MapFinished handler");
                    init_map(e, *finished);
                })));

    gbus.addEventFunction<MapResourcesReady>(
        0, [&e](GameEvent&, MapResourcesReady* ready) {
            cDebug("Map resources loaded");
            auto& bitmaps = e.subsystem_cast<BitmapCache<halo_version>>();
            auto& sounds  = e.subsystem_cast<SoundCache<halo_version>>();
            auto& loading_status = e.subsystem_cast<LoadingStatus>();

            if(ready->bitmap_file)
            {
                bitmaps.load_bitmaps_from(*ready->bitmap_file);
                loading_status.loaded_bitmaps = true;
            }
            if(ready->sound_file)
            {
                sounds.load_sounds_from(*ready->sound_file);
                loading_status.loaded_sounds = true;
            }

            if(!loading_status.loading)
            {
                if(ready->bitmap_file && !compile_info::platform::is_32bit)
                {
                    bitmaps.allocate_storage();
                }
                if(ready->sound_file)
                {
                    sounds.process_sounds();
                }
            }
        });
}
