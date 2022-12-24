#pragma once

#include <tuple>

namespace stl_types::tuple {
namespace detail {

template<std::size_t Offset, typename... T>
requires(Offset == 0 /*&& sizeof...(T) > 1*/)
    //
    inline auto do_slice(T&&... src)
{
    return std::make_tuple(std::forward<T>(src)...);
}

// template<std::size_t Offset, typename T>
// requires(Offset == 0)
//     //
//     inline auto do_slice(T&& src)
//{
//     return src;
// }

template<std::size_t Offset, typename Dropped, typename... T>
requires(Offset != 0) inline auto do_slice(Dropped&& /* dropped */, T&&... src)
{
    return do_slice<Offset - 1, T...>(std::forward<T>(src)...);
}

} // namespace detail

template<std::size_t Offset, typename... T>
requires(Offset != 0 && Offset <= (sizeof...(T) - 1))
    //
    inline auto slice(std::tuple<T...>&& src)
{
    return std::apply(detail::do_slice<Offset, T...>, std::move(src));
}

static_assert(
    std::is_same_v<
        std::tuple<float, double>,
        decltype(slice<1>(std::declval<std::tuple<int, float, double>>()))>);

static_assert(
    std::is_same_v<
        std::tuple<double>,
        decltype(slice<2>(std::declval<std::tuple<int, float, double>>()))>);

} // namespace stl_types::tuple
