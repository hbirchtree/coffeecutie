#pragma once

#include <peripherals/identify/identify.h>

#include <cstddef>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

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
    using tail = typename std::conditional_t<
        sizeof...(Types) >= 1,
        detail::list_creator<Types...>,
        detail::empty_list>::type;
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

template<typename... Types>
struct list_inspector
{
    constexpr list_inspector(type_list<Types...>)
    {
    }

    static constexpr size_t num_types = sizeof...(Types);

    template<typename T>
    struct has_type
    {
        static constexpr bool value = (std::is_same_v<T, Types> || ...);
    };

    template<class UnaryOperator>
    struct for_each
    {
        for_each(UnaryOperator&& op)
            : op(std::move(op))
        {
        }

        void operator()()
        {
            (op.template operator()<Types>(), ...);
        }

        UnaryOperator op;
    };

    template<class UnaryOperator>
    struct for_each_rev
    {
        for_each_rev(UnaryOperator&& op)
            : op(std::move(op))
        {
        }

        void operator()()
        {
            int dummy;
            (dummy = ... = (op.template operator()<Types>(), 0));
        }

        UnaryOperator op;
    };

    template<typename... MoreTypes>
    struct concat_list
    {
        constexpr concat_list(type_list<MoreTypes...>)
        {
        }

        using type = type_list<Types..., MoreTypes...>;
    };
};

struct collect_operator
{
    collect_operator(std::vector<size_t>& hashes)
        : hashes(hashes)
    {
    }

    template<typename T>
    void operator()()
    {
        hashes.push_back(typeid(T).hash_code());
    }

    std::vector<size_t>& hashes;
};

} // namespace detail

template<typename List>
using list_inspector = decltype(detail::list_inspector(std::declval<List>()));

template<typename T, typename List>
constexpr bool type_in_list_v =
    list_inspector<List>::template has_type<T>::value;

template<typename T1, typename T2>
using concat_lists = typename decltype(list_inspector<T1>::concat_list(
    std::declval<T2>()))::type;

template<typename List>
FORCEDINLINE void collect(std::vector<size_t>& type_hashes)
{
    using for_each = typename list_inspector<List>::template for_each<
        detail::collect_operator>;
    for_each(detail::collect_operator(type_hashes))();
}

template<typename Types>
FORCEDINLINE std::vector<size_t> collect_list()
{
    std::vector<size_t> out;
    collect<Types>(out);
    return out;
}

template<typename List, typename Operator>
void for_each(Operator&& op)
{
    using for_each = typename list_inspector<List>::template for_each<Operator>;
    for_each(std::move(op))();
}

template<typename List, typename Operator>
void for_each_rev(Operator&& op)
{
    using for_each =
        typename list_inspector<List>::template for_each_rev<Operator>;
    for_each(std::move(op))();
}

} // namespace type_list

template<typename T>
concept is_type_list =
    std::is_same_v<T, type_list::detail::empty_list> ||
    (std::is_same_v<typename T::head, typename T::head> &&
     (std::is_same_v<typename T::tail, type_list::detail::empty_list> ||
      std::is_same_v<typename T::tail::head, typename T::tail::head>));

template<typename... T>
using type_list_t = type_list::type_list<T...>;

using empty_list_t = type_list_t<void>;

} // namespace type_safety
