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

/*! A tag's table entry and its data read as T; false when either is missing */
template<typename T, typename Tag>
struct tagged
{
    Tag const* tag{nullptr};
    T const*   data{nullptr};

    inline explicit operator bool() const
    {
        return data != nullptr;
    }

    inline T const* operator->() const
    {
        return data;
    }

    inline T const& operator*() const
    {
        return *data;
    }
};

/*!
 * \brief The tag a handle names, read as T. When T declares its tag_class
 * and the map's classes can be trusted, a tag of another class is refused;
 * maps that cannot (Custom Edition) take the class from T.
 */
template<typename T, tag_map M>
inline tagged<T, typename M::tag_type> get(M const& map, typename M::handle_type id)
{
    auto const* tag = tag_at(map, id);
    if(!tag)
        return {};
    if constexpr(requires { T::tag_class; } && M::checks_class)
        if(class_of(map, *tag) != T::tag_class)
            return {};
    auto const* data = data_of<T>(map, *tag);
    return data ? tagged<T, typename M::tag_type>{tag, data}
                : tagged<T, typename M::tag_type>{};
}

} // namespace blam
