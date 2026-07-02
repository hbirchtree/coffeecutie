#pragma once

#include <peripherals/libc/types.h>

#include <bit>
#include <type_traits>

namespace blam {

template<typename T>
constexpr T from_le(T value) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "from_le needs a POD value");

#if defined(__cpp_lib_bit_cast)
    if constexpr(std::endian::native == std::endian::little || sizeof(T) == 1)
        return value;
    else if constexpr(sizeof(T) == 2)
    {
        auto v = std::bit_cast<libc_types::u16>(value);
        v      = static_cast<libc_types::u16>((v >> 8) | (v << 8));
        return std::bit_cast<T>(v);
    }
    else if constexpr(sizeof(T) == 4)
    {
        auto v = std::bit_cast<libc_types::u32>(value);
        v = ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) |
            ((v & 0x00FF0000u) >> 8) | ((v & 0xFF000000u) >> 24);
        return std::bit_cast<T>(v);
    }
    else if constexpr(sizeof(T) == 8)
    {
        auto v = std::bit_cast<libc_types::u64>(value);
        v = ((v & 0x00000000000000FFull) << 56) |
            ((v & 0x000000000000FF00ull) << 40) |
            ((v & 0x0000000000FF0000ull) << 24) |
            ((v & 0x00000000FF000000ull) << 8) |
            ((v & 0x000000FF00000000ull) >> 8) |
            ((v & 0x0000FF0000000000ull) >> 24) |
            ((v & 0x00FF000000000000ull) >> 40) |
            ((v & 0xFF00000000000000ull) >> 56);
        return std::bit_cast<T>(v);
    }
    else
    {
        static_assert(sizeof(T) == 0, "from_le: unsupported value size");
        return value;
    }
#else
    return value;
#endif
}

template<typename T>
constexpr T to_le(T value) noexcept
{
    return from_le(value);
}

} // namespace blam
