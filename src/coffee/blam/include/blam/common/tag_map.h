#pragma once

#include <peripherals/semantic/chunk.h>

#include <concepts>
#include <string_view>

namespace blam {

/*!
 * \brief A loaded map whose tags can be walked and resolved the same way in
 * every game: tags_of(), tag_at(), handle_of() and name_of() are found next
 * to each game's map_container.
 */
template<typename M>
concept tag_map = requires(
    M const& map, typename M::tag_type const& tag, typename M::handle_type id) {
    { tags_of(map) } -> std::convertible_to<semantic::Span<typename M::tag_type const>>;
    { tag_at(map, id) } -> std::same_as<typename M::tag_type const*>;
    { handle_of(map, tag) } -> std::same_as<typename M::handle_type>;
    { name_of(map, tag) } -> std::convertible_to<std::string_view>;
};

} // namespace blam
