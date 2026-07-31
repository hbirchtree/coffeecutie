#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/decl_member_function.h>
#include <peripherals/stl/type_list.h>

#include <type_traits>
#include <typeinfo>
#include <vector>

namespace compo {
using type_hash = declreturntype(std::type_info::hash_code);
} // namespace compo

namespace compo::access {

using libc_types::u8;

/*!
 * \brief Declared access to a resource in a manifest type list.
 */
enum class mode : u8
{
    none  = 0,
    read  = 1,
    write = 3, /* write implies read */
};

constexpr bool is_write(mode m)
{
    return (static_cast<u8>(m) & static_cast<u8>(mode::write)) ==
           static_cast<u8>(mode::write);
}

template<typename T, typename List>
constexpr bool writable_v = type_safety::type_list::type_in_list_v<T, List>;

template<typename T, typename List>
constexpr bool readable_v =
    type_safety::type_list::type_in_list_v<T, List> ||
    type_safety::type_list::type_in_list_v<const T, List>;

template<typename T, typename List>
constexpr mode mode_of_v = writable_v<T, List> ? mode::write
                           : readable_v<T, List>
                               ? mode::read
                               : mode::none;

/*!
 * \brief A component tag's payload type, const-qualified unless the list
 * declares write access to it.
 */
template<typename Tag, typename List>
using value_t = std::conditional_t<
    writable_v<Tag, List>,
    typename Tag::value_type,
    typename Tag::value_type const>;

/*!
 * \brief A subsystem/service tag's wrapped type, const-qualified unless the
 * list declares write access to it.
 */
template<typename Tag, typename List>
using system_t = std::conditional_t<
    writable_v<Tag, List>,
    typename Tag::type,
    typename Tag::type const>;

/*!
 * \brief value_t, but with `void` as the list meaning "unrestricted".
 */
template<typename Tag, typename List>
struct projected_value
{
    using type = value_t<Tag, List>;
};

template<typename Tag>
struct projected_value<Tag, void>
{
    using type = typename Tag::value_type;
};

template<typename Tag, typename List>
using projected_value_t = typename projected_value<Tag, List>::type;

/*!
 * \brief readable_v, but an unrestricted (`void`) list permits everything.
 */
template<typename T, typename List>
struct projected_writable
{
    static constexpr bool value = writable_v<T, List>;
};

template<typename T>
struct projected_writable<T, void>
{
    static constexpr bool value = true;
};

template<typename T, typename List>
constexpr bool projected_writable_v = projected_writable<T, List>::value;

template<typename T, typename List>
struct projected_readable
{
    static constexpr bool value = readable_v<T, List>;
};

template<typename T>
struct projected_readable<T, void>
{
    static constexpr bool value = true;
};

template<typename T, typename List>
constexpr bool projected_readable_v = projected_readable<T, List>::value;

struct entry
{
    type_hash hash;
    mode      access;
    /*! Storage keeps more than one frame, so a writer (producing frame N+1)
     *  and a reader (consuming N) do not conflict */
    bool buffered{false};
};

/*!
 * \brief Whether a tag's storage keeps more than one frame.
 */
template<typename T, typename = void>
struct is_buffered : std::false_type
{
};

template<typename T>
struct is_buffered<
    T,
    std::void_t<decltype(T::type::frame_count_v)>>
    : std::bool_constant<(T::type::frame_count_v > 1)>
{
};

namespace detail {

struct collect_operator
{
    collect_operator(std::vector<entry>& out)
        : out(out)
    {
    }

    template<typename T>
    void operator()()
    {
        using bare = std::remove_const_t<T>;

        /* empty_list_t is type_list_t<void>, a list *containing* void */
        if constexpr(!std::is_void_v<bare>)
            out.push_back(
                {typeid(bare).hash_code(),
                 std::is_const_v<T> ? mode::read : mode::write,
                 is_buffered<bare>::value});
    }

    std::vector<entry>& out;
};

} // namespace detail

/*!
 * \brief Hash + declared access for every type in a manifest list.
 */
template<typename List>
std::vector<entry> collect()
{
    std::vector<entry> out;
    type_safety::type_list::for_each<List>(detail::collect_operator(out));
    return out;
}

/*!
 * \brief Access a subsystem took that its manifest cannot express.
 */
struct runtime_flags
{
    /*! Created or removed entities */
    bool structural_mutation{false};
    /*! Reached the container outside the manifest */
    bool opaque{false};
};

/*!
 * \brief Flags of the subsystem whose frame hook is running on this thread.
 */
inline runtime_flags*& current_scope()
{
    static thread_local runtime_flags* scope = nullptr;
    return scope;
}

inline void note_structural_mutation()
{
    if(auto* scope = current_scope())
        scope->structural_mutation = true;
}

inline void note_opaque_access()
{
    if(auto* scope = current_scope())
        scope->opaque = true;
}

/*!
 * \brief Points current_scope() at a subsystem for the duration of its hook.
 */
struct scope_guard
{
    scope_guard(runtime_flags& flags)
        : previous(current_scope())
    {
        current_scope() = &flags;
    }

    ~scope_guard()
    {
        current_scope() = previous;
    }

    scope_guard(scope_guard const&) = delete;

    runtime_flags* previous;
};

inline std::vector<entry> const& nothing()
{
    static const std::vector<entry> empty;
    return empty;
}

inline mode find(std::vector<entry> const& set, type_hash hash)
{
    for(auto const& e : set)
        if(e.hash == hash)
            return e.access;
    return mode::none;
}

} // namespace compo::access
