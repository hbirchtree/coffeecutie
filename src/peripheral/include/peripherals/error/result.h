#pragma once

#include <concepts>
#include <expected>
#include <type_traits>
#include <utility>

namespace stl_types {

template<typename T>
struct success_type
{
    T value;
};

template<>
struct success_type<void>
{
};

template<typename T, typename E>
struct result : std::expected<T, E>
{
    using base = std::expected<T, E>;
    using base::base;

    template<typename U>
    requires std::constructible_from<T, U>
    constexpr result(success_type<U>&& s)
        : base(std::in_place, std::move(s.value))
    {
    }

    constexpr result(success_type<void>)
    requires std::is_void_v<T>
        : base(std::in_place)
    {
    }

    /* A bare E must go through failure(), otherwise it may silently
     * become the value (e.g. result<posix_fd_t, int>) */
    template<typename U>
    requires(!std::same_as<T, E> && std::same_as<std::remove_cvref_t<U>, E>)
    result(U&&) = delete;

    constexpr bool has_error() const
    {
        return !this->has_value();
    }

    constexpr void assume_value() const
    {
    }
};

template<typename T>
constexpr success_type<std::decay_t<T>> success(T&& value)
{
    return {std::forward<T>(value)};
}

constexpr success_type<void> success()
{
    return {};
}

template<typename E>
constexpr std::unexpected<std::decay_t<E>> failure(E&& error)
{
    return std::unexpected<std::decay_t<E>>(std::forward<E>(error));
}

template<typename T, typename E>
inline T value_or(result<T, E>&& res, T&& v)
{
    if(res.has_value())
        return res.value();
    return v;
}

template<typename T, typename E>
inline T value_or(result<T, E>&& res, T const& v)
{
    if(res.has_value())
        return res.value();
    return v;
}

} // namespace stl_types
