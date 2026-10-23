#pragma once

#include <peripherals/semantic/chunk.h>

#include <cstddef>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace blam {

/*!
 * \brief A placement, the object its palette entry names (null when the
 * index falls outside the palette) and its index in its block. Binds as
 * [placement, object].
 */
template<typename P, typename Ref>
struct placed_t
{
    P const&    placement;
    Ref const*  object;
    std::size_t index;

    template<std::size_t I>
    inline decltype(auto) get() const
    {
        if constexpr(I == 0)
            return (placement);
        else
            return object;
    }
};

/*! A block's placements with the objects `object_of(placement)` names */
template<typename P, typename ObjectOf>
inline auto placements(semantic::Span<P const> block, ObjectOf object_of)
{
    using ref_t = std::remove_cvref_t<decltype(*object_of(std::declval<P const&>()))>;
    return std::views::iota(std::size_t{0}, block.size()) |
           std::views::transform([block, object_of](std::size_t i) {
               return placed_t<P, ref_t>{block[i], object_of(block[i]), i};
           });
}

} // namespace blam

template<typename P, typename Ref>
struct std::tuple_size<blam::placed_t<P, Ref>> : std::integral_constant<std::size_t, 2>
{
};

template<typename P, typename Ref>
struct std::tuple_element<0, blam::placed_t<P, Ref>>
{
    using type = P const&;
};

template<typename P, typename Ref>
struct std::tuple_element<1, blam::placed_t<P, Ref>>
{
    using type = Ref const*;
};
