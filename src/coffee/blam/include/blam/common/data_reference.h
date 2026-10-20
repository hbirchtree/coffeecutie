#pragma once

#include "../volta/blam_base_types.h"
#include "block_layout.h"
#include "endian.h"
#include "magic.h"

#include <array>
#include <limits>
#include <string_view>

namespace blam {

template<typename Layout>
struct data_storage;

/*! Halo 1: size, flags, file offset, pointer and an unused word */
template<>
struct data_storage<layout::volta_pc>
{
    u32 count;
    u32 flags;
    u32 file_offset;
    u32 pointer_raw;
    u32 unused;

    inline i32 size() const
    {
        return static_cast<i32>(from_le(count));
    }

    inline u32 pointer() const
    {
        return from_le(pointer_raw);
    }
};

template<>
struct data_storage<layout::volta_xbox> : data_storage<layout::volta_pc>
{
};

/*! Halo 2: size and a virtual address */
template<>
struct data_storage<layout::dimeter>
{
    i32 size_raw;
    u32 pointer_raw;

    inline i32 size() const
    {
        return from_le(size_raw);
    }

    inline u32 pointer() const
    {
        return from_le(pointer_raw);
    }
};

/*! Halo 3: Halo 1's shape, big-endian */
template<>
struct data_storage<layout::elegy>
{
    be<i32>                        size_raw;
    std::array<libc_types::u32, 2> runtime_0;
    be<u32>                        pointer_raw;
    u32                            runtime_1;

    inline i32 size() const
    {
        return size_raw;
    }

    inline u32 pointer() const
    {
        return pointer_raw;
    }
};

/*! A sized run of bytes in heap Heap, laid out the way V's files store it */
template<typename V, heap_t Heap>
struct alignas(4) data_reference : data_storage<typename V::block_layout>
{
    using span_type = Span<u8 const>;

    template<typename M>
    requires(M::heap == Heap)
    inline result<span_type, error_msg> data(M const& magic) const
    {
        using namespace std::string_view_literals;

        i32 const num = this->size();
        if(num == 0)
            return stl_types::success(span_type());
        if(num < 0)
            return stl_types::failure("negative data size"sv);
        auto const* bytes = magic.template as<u8>(this->pointer(), static_cast<u32>(num));
        if(!bytes)
            return stl_types::failure("data pointer out of bounds"sv);
        return stl_types::success(span_type(bytes, static_cast<size_t>(num)));
    }

    /*! The bytes, none when empty or out of bounds */
    template<typename M>
    requires(M::heap == Heap)
    inline span_type span(M const& magic) const
    {
        return data(magic).value_or(span_type());
    }
};

} // namespace blam
