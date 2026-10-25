#pragma once

#include "../components.h"
#include "../game_events.h"

#include <coffee/comp_app/file_mapper.h>

#include <memory>
#include <string_view>

namespace world {

/*! Removes the placed world and resets physics, as a BSP switch does */
void clear_objects(compo::EntityContainer& container);

/*! Tears down the previous world, any game's, before a map loads */
void clear_world(compo::EntityContainer& container);

/*! Whether side caches are read by range: always on the web, on desktop
 * when the environment variable `env` is 1 */
bool side_pread(char const* env);

/*! A file in the same directory as the map being loaded */
platform::url::Url sibling_url(MapLoadEvent const& load, std::string_view name);

/*! Reads a ranged file as (offset, destination) */
inline auto pread_reader(std::shared_ptr<comp_app::FileMapper::RangedFile> file)
{
    return [file = std::move(file)](u64 offset, semantic::Span<u8> dst) {
        return file->read(offset, gsl::span<char>(reinterpret_cast<char*>(dst.data()), dst.size()));
    };
}

/*! Tells the game's map is starting to load, before any of it is */
inline void announce_game(compo::EntityContainer& container, MapGameEvent::game_t game)
{
    GameEvent    ev{GameEvent::MapGame};
    MapGameEvent map{.game = game};
    container.subsystem_cast<GameEventBus>().process(ev, &map);
}

/*! The game a map's header (its first 0x3000 bytes) says it is from;
 * anything not Halo 2 or 3 is left to the Halo 1 loader */
MapGameEvent::game_t detect_game(semantic::BytesConst const& head);

/*! Reads the header of every map load and sends it again tagged with its
 * game, before any loader sees it; registered ahead of the loaders */
void dispatch_map_loads(compo::EntityContainer& container);

/*! Hands map loads to `loader`, consuming those it takes so the Halo 1
 * loader never sees them */
template<typename Loader>
void consume_map_loads(compo::EntityContainer& container, Loader& loader)
{
    container.subsystem_cast<GameEventBus>().addEventFunction<MapLoadEvent>(
        0, [&loader](GameEvent& ev, MapLoadEvent* load) {
            if(loader.on_map_load(*load))
                ev.type = GameEvent::None;
        });
}

} // namespace world
