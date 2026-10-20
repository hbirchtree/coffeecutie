#pragma once

#include <peripherals/libc/types.h>

#include <array>
#include <bit>
#include <cstring>
#include <type_traits>

namespace blam {
namespace detail {

template<typename T, std::endian Target>
constexpr T byteswap(T value) noexcept
{
    static_assert(
        std::is_trivially_copyable_v<T>, "blam::byteswap: needs POD value");

#if defined(__cpp_lib_bit_cast)
    if constexpr(std::endian::native == Target || sizeof(T) == 1)
        return value;
    else if constexpr(sizeof(T) == 2)
    {
        auto v = std::bit_cast<libc_types::u16>(value);
        v      = static_cast<libc_types::u16>((v >> 8) | (v << 8));
        return std::bit_cast<T>(v);
    } else if constexpr(sizeof(T) == 4)
    {
        auto v = std::bit_cast<libc_types::u32>(value);
        v      = ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) |
            ((v & 0x00FF0000u) >> 8) | ((v & 0xFF000000u) >> 24);
        return std::bit_cast<T>(v);
    } else if constexpr(sizeof(T) == 8)
    {
        auto v = std::bit_cast<libc_types::u64>(value);
        v      = ((v & 0x00000000000000FFull) << 56) |
            ((v & 0x000000000000FF00ull) << 40) |
            ((v & 0x0000000000FF0000ull) << 24) |
            ((v & 0x00000000FF000000ull) << 8) |
            ((v & 0x000000FF00000000ull) >> 8) |
            ((v & 0x0000FF0000000000ull) >> 24) |
            ((v & 0x00FF000000000000ull) >> 40) |
            ((v & 0xFF00000000000000ull) >> 56);
        return std::bit_cast<T>(v);
    } else
    {
        static_assert(sizeof(T) == 0, "blam::byteswap: unsupported value size");
        return value;
    }
#else
    return value;
#endif
}

} // namespace detail

template<typename T>
constexpr T from_be(T value) noexcept
{
    return detail::byteswap<T, std::endian::big>(value);
}

template<typename T>
constexpr T to_be(T value) noexcept
{
    return detail::byteswap<T, std::endian::big>(value);
}

template<typename T>
constexpr T from_le(T value) noexcept
{
    return detail::byteswap<T, std::endian::little>(value);
}

template<typename T>
constexpr T to_le(T value) noexcept
{
    return detail::byteswap<T, std::endian::little>(value);
}

template<std::endian E, typename T>
constexpr T from_endian(T value) noexcept
{
    return detail::byteswap<T, E>(value);
}

/*!
 * \brief A field stored in a fixed byte order, read in host order. Reads are
 * free when the host matches. Vectors swap per component.
 */
template<typename T, std::endian E>
struct endian_value
{
    using value_type = T;

    static constexpr std::endian endian = E;

    std::array<libc_types::u8, sizeof(T)> raw;

    inline T value() const
    {
        if constexpr(requires { typename T::value_type; })
        {
            using C           = typename T::value_type;
            constexpr auto Nc = sizeof(T) / sizeof(C);
            static_assert(Nc * sizeof(C) == sizeof(T));
            std::array<C, Nc> parts;
            std::memcpy(parts.data(), raw.data(), sizeof(T));
            for(auto& part : parts)
                part = from_endian<E>(part);
            T out;
            std::memcpy(&out, parts.data(), sizeof(T));
            return out;
        } else
        {
            T out;
            std::memcpy(&out, raw.data(), sizeof(T));
            return from_endian<E>(out);
        }
    }

    inline operator T() const
    {
        return value();
    }
};

template<typename T>
using be = endian_value<T, std::endian::big>;
template<typename T>
using le = endian_value<T, std::endian::little>;

static_assert(sizeof(be<libc_types::u32>) == 4 && alignof(be<libc_types::u32>) == 1);

} // namespace blam
