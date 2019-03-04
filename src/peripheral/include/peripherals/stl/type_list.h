#pragma once

#include <peripherals/identify/identify.h>
#include <peripherals/stl/types.h>

namespace type_safety {
namespace type_list {
namespace detail {
/*!
 * \brief sentinel empty type_list
 */
struct empty_list
{
    using type = empty_list;
};

template<typename... Types>
struct list_creator;

} // namespace detail

template<typename T, typename... Types>
struct type_list
{
    using head = T;
    using tail = typename std::conditional<
        sizeof...(Types) >= 1,
        detail::list_creator<Types...>,
        detail::empty_list>::type::type;
};

namespace detail {
template<typename... Types>
/*!
 * \brief Provides a level of indirection between type_list and the tail,
 * avoiding instantiation of an invalid type_list.
 */
struct list_creator
{
    using type = type_list<Types...>;
};
} // namespace detail

namespace detail {
template<typename T, typename std::enable_if<false, T>::type* = nullptr>
FORCEDINLINE void in_list()
{
    /* List is empty, but not match, failure */
}

template<
    typename T,
    typename Head,
    typename Tail,
    typename std::enable_if<std::is_same<T, Head>::value>::type* = nullptr>
FORCEDINLINE void in_list()
{
    /* Type matches! Great! */
}

template<
    typename T,
    typename Head,
    typename Tail,
    typename std::enable_if<!std::is_same<T, Head>::value>::type* = nullptr,
    typename std::enable_if<!std::is_same<Tail, empty_list>::value>::type* =
        nullptr>
FORCEDINLINE void in_list()
{
    in_list<T, typename Tail::head, typename Tail::tail>();
}
} // namespace detail

template<typename T, typename Types>
/*!
 * \brief Attempts to find T in Types (a type_list<...>)
 */
FORCEDINLINE void type_in_list()
{
    detail::in_list<T, typename Types::head, typename Types::tail>();
}

namespace detail {

template<
    typename Head,
    typename Tail,
    typename std::enable_if<std::is_same<Tail, empty_list>::value>::type* =
        nullptr>
FORCEDINLINE void collect_head(stl_types::Vector<size_t>& type_hashes)
{
    type_hashes.push_back(typeid(Head).hash_code());
}

template<
    typename Head,
    typename Tail,
    typename std::enable_if<!std::is_same<Tail, empty_list>::value>::type* =
        nullptr>
FORCEDINLINE void collect_head(stl_types::Vector<size_t>& type_hashes)
{
    type_hashes.push_back(typeid(Head).hash_code());
    collect_head<typename Tail::head, typename Tail::tail>(type_hashes);
}

} // namespace detail

template<typename Types>
FORCEDINLINE void collect(stl_types::Vector<size_t>& type_hashes)
{
    detail::collect_head<typename Types::head, typename Types::tail>(
        type_hashes);
}

template<typename Types>
FORCEDINLINE stl_types::Vector<size_t> collect_list()
{
    stl_types::Vector<size_t> out;
    collect<Types>(out);
    return out;
}

} // namespace type_list
} // namespace type_safety
