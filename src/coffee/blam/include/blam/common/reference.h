#pragma once

#include "../volta/blam_atlas_type.h"
#include "../volta/blam_base_types.h"
#include "../volta/blam_versions.h"
#include "block_layout.h"
#include "endian.h"
#include "magic.h"

#include <limits>
#include <string_view>

namespace blam {

struct single_value_t
{
};

constexpr single_value_t single_value;

template<typename Layout>
struct block_storage;

/*! Halo 1 on PC: count, offset and a pad word */
template<>
struct block_storage<layout::volta_pc>
{
    u32 count;
    u32 offset;
    u32 padding;

    inline u32 size() const
    {
        return from_le(count);
    }

    inline u32 pointer() const
    {
        return from_le(offset);
    }
};

/*! Halo 1 on Xbox: count and offset */
template<>
struct block_storage<layout::volta_xbox>
{
    u32 count;
    u32 offset;

    inline u32 size() const
    {
        return from_le(count);
    }

    inline u32 pointer() const
    {
        return from_le(offset);
    }
};

/*! Halo 2: count and a virtual address into the block's heap */
template<>
struct block_storage<layout::dimeter>
{
    i32 count_raw;
    u32 pointer_raw;

    inline i32 count() const
    {
        return from_le(count_raw);
    }

    inline u32 size() const
    {
        return static_cast<u32>(count());
    }

    inline u32 pointer() const
    {
        return from_le(pointer_raw);
    }
};

/*! Halo 3: count, virtual address and a runtime-only word, big-endian */
template<>
struct block_storage<layout::elegy>
{
    be<i32> count_raw;
    be<u32> pointer_raw;
    u32     runtime;

    inline i32 count() const
    {
        return count_raw;
    }

    inline u32 size() const
    {
        return static_cast<u32>(count());
    }

    inline u32 pointer() const
    {
        return pointer_raw;
    }
};

/*!
 * \brief A block of T stored elsewhere in the map: a count plus a pointer
 * into heap Heap, laid out the way V's files store it.
 */
template<
    typename T,
    typename V        = pc_version_t,
    atlas_type_t Type = atlas_type_t::map_file,
    heap_t Heap       = heap_t::map>
struct alignas(4) reference : block_storage<typename V::block_layout>
{
    using value_type = T;
    using span_type  = Span<T const>;

    static constexpr heap_t heap_v = Heap;

    reference() = default;

    /*! A Halo 1 block from its raw (file order) count and offset words */
    constexpr reference(u32 count, u32 offset)
        requires(
            std::is_same_v<typename V::block_layout, layout::volta_pc> ||
            std::is_same_v<typename V::block_layout, layout::volta_xbox>)
    {
        this->count  = count;
        this->offset = offset;
        if constexpr(requires { this->padding; })
            this->padding = 0;
    }

    /*! The elements; empty blocks succeed, bad counts and pointers fail */
    template<typename M>
    requires(M::heap == Heap)
    inline result<span_type, error_msg> data(M const& magic) const
    {
        using namespace std::string_view_literals;

        if constexpr(requires { this->count(); })
            if(this->count() < 0)
                return stl_types::failure("negative block count"sv);
        u64 const num = this->size();
        if(num == 0)
            return stl_types::success(span_type());
        if(num * sizeof(T) > std::numeric_limits<u32>::max())
            return stl_types::failure("block pointer out of bounds"sv);
        auto const* items =
            magic.template as<T>(this->pointer(), static_cast<u32>(num));
        if(!items)
            return stl_types::failure("block pointer out of bounds"sv);
        return stl_types::success(span_type(items, static_cast<size_t>(num)));
    }

    template<typename M>
    requires(M::heap == Heap)
    inline result<T const*, error_msg> data(M const& magic, single_value_t) const
    {
        using namespace std::string_view_literals;

        if(auto v = data(magic); v.has_error())
            return stl_types::failure(v.error());
        else if(v.value().size() > 1)
            return stl_types::failure("got multiple values"sv);
        else
            return &v.value()[0];
    }

    /*! The elements, none when the block is empty or out of bounds */
    template<typename M>
    requires(M::heap == Heap)
    inline span_type span(M const& magic) const
    {
        return data(magic).value_or(span_type());
    }

    template<typename T2>
    inline auto as() const
    {
        return reinterpret_cast<reference<T2, V, Type, Heap> const*>(this);
    }
};

/*! Whether block R can be resolved through magic M */
template<typename R, typename M>
concept resolves_with = requires(R const& block, M const& magic) {
    block.data(magic);
};

} // namespace blam
