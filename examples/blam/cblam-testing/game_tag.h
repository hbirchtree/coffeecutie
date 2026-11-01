#pragma once

#include "game_events.h"

#include <blam/common/tag_index.h>

/*! A tag in whichever game's map is loaded: the game, and the tag's handle
 * there. The classes say which tags it may name */
template<blam::tag_class_t... Classes>
struct GameTag
{
    MapGameEvent::game_t game{MapGameEvent::halo1};
    libc_types::u32      tag{0xFFFFFFFF};

    template<std::endian E, blam::tag_class_t... Other>
    static constexpr GameTag of(
        MapGameEvent::game_t game, blam::basic_tag_index<E, Other...> const& id)
    {
        return {game, id.value()};
    }

    constexpr bool valid() const
    {
        return tag != 0xFFFFFFFF;
    }

    /*! Game in the top bits (0x4831 Halo 1, 0x4832 Halo 2, 0x4833 Halo 3),
     * tag below */
    constexpr libc_types::u64 key() const
    {
        return (static_cast<libc_types::u64>(0x4831 + game) << 48) | tag;
    }

    constexpr bool operator==(GameTag const&) const = default;
};

/*! The tag `id` names in `map`, its game's, read as T */
template<typename T, typename M, blam::tag_class_t... Classes>
inline auto lookup(M const& map, GameTag<Classes...> const& id)
{
    return blam::get<T>(map, M::handle_type::of(id.tag));
}
