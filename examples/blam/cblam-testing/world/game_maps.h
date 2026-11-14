#pragma once

#include "loading/blam_files.h"
#include "data.h"
#include "game_events.h"
#include "selected_version.h"

#include <blam/common/tag_map.h>
#include <blam/volta/blam_unit.h>

#if defined(BLAM_DIMETER)
#include "h2/h2_map.h"

#include <blam/dimeter/objects.h>
#endif

#if defined(BLAM_ELEGY)
#include "h3/h3_map.h"

#include <blam/elegy/objects.h>
#endif

namespace world {

/*! A system's services, and those it needs to look tags up in any game's map */
template<typename... Services>
using with_game_maps_t = type_list_t<
    Services...,
#if defined(BLAM_DIMETER)
    const h2::H2Map,
#endif
#if defined(BLAM_ELEGY)
    const h3::H3Map,
#endif
    const BlamFiles<halo_version>>;

/*! Which game a map container is, and its tag structs */
template<typename M>
struct game_of;

template<typename V>
struct game_of<blam::map_container<V>>
{
    static constexpr auto game = MapGameEvent::halo1;
    using weapon               = blam::scn::weapon;
};

#if defined(BLAM_DIMETER)
template<typename V>
struct game_of<blam::dimeter::map_container<V>>
{
    static constexpr auto game = MapGameEvent::halo2;
    using weapon               = blam::dimeter::obj::weapon_t;
};
#endif

#if defined(BLAM_ELEGY)
template<>
struct game_of<blam::elegy::map_container>
{
    static constexpr auto game = MapGameEvent::halo3;
    using weapon               = blam::elegy::weapon_t;
};
#endif

/*! f(map) with `game`'s loaded map as its own container; not called when
 * that game is not built in or has nothing loaded */
template<typename Proxy, typename F>
inline void with_map(Proxy& p, MapGameEvent::game_t game, F&& f)
{
    switch(game)
    {
    case MapGameEvent::halo1:
        f(p.template subsystem<BlamFiles<halo_version>>().container);
        break;
#if defined(BLAM_DIMETER)
    case MapGameEvent::halo2:
        p.template subsystem<h2::H2Map>().visit(f);
        break;
#endif
#if defined(BLAM_ELEGY)
    case MapGameEvent::halo3:
        p.template subsystem<h3::H3Map>().visit(f);
        break;
#endif
    default:
        break;
    }
}

/*! The tag `id` names in `map`, its game's, read as T */
template<typename T, typename M, blam::tag_class_t... Classes>
inline auto lookup(M const& map, GameTag<Classes...> const& id)
{
    return blam::get<T>(map, M::handle_type::of(id.tag));
}

/*! Some game's world is loaded: Halo 1's once all of it is, another's once
 * its map is held */
template<typename Proxy>
inline bool world_loaded(Proxy& p)
{
    if(p.template subsystem<LoadingStatus>().loaded_map == LoadingStatus::loaded)
        return true;
#if defined(BLAM_DIMETER)
    if(p.template subsystem<h2::H2Map>().loaded())
        return true;
#endif
#if defined(BLAM_ELEGY)
    if(p.template subsystem<h3::H3Map>().loaded())
        return true;
#endif
    return false;
}

} // namespace world
