#pragma once

#include <peripherals/libc/types.h>

#include <optional>

namespace blam {

/*! Which address space a pointer lives in; typed so magics do not mix */
enum class heap_t
{
    map,
    bsp,
    vertex,
    index,
    tag,
};

/*!
 * \brief One address space of a map: a virtual pointer names the bytes at
 * base + file_offset + (pointer - virtual_base). Pointers below the virtual
 * base wrap around and fail the bounds check.
 */
struct address_range
{
    libc_types::u8 const* base{nullptr};
    libc_types::u32       max_size{0};
    libc_types::u32       file_offset{0};
    libc_types::u32       virtual_base{0};

    inline std::optional<libc_types::u32> to_offset(
        libc_types::u32 pointer, libc_types::u32 size = 0) const
    {
        libc_types::u32 offset = file_offset + (pointer - virtual_base);
        if(offset >= max_size || size > max_size - offset)
            return std::nullopt;
        return offset;
    }

    inline libc_types::u8 const* resolve(
        libc_types::u32 pointer, libc_types::u32 size = 0) const
    {
        if(auto offset = to_offset(pointer, size); base && offset)
            return base + *offset;
        return nullptr;
    }

    template<typename T>
    inline T const* as(libc_types::u32 pointer, libc_types::u32 count = 1) const
    {
        return reinterpret_cast<T const*>(resolve(pointer, count * sizeof(T)));
    }
};

} // namespace blam
