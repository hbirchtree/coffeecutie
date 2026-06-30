#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <type_traits>

#include "blam_atlas_type.h"
#include "blam_endian.h"
#include "blam_magic_data.h"
#include "blam_versions.h"

namespace blam {

using libc_types::u32;

struct single_value_t
{
};

constexpr single_value_t single_value;

template<
    typename T,
    typename V        = grbx_t,
    atlas_type_t Type = atlas_type_t::map_file>
/*!
 * \brief Points to a chunk of memory within the file
 */
struct alignas(4) reference
{
    using value_type = T;
    using chunk_type = semantic::mem_chunk<T const>;
    using span_type  = typename chunk_type::span_type;

    u32 count; /*!< Number of elements */

    union
    {
        u32 offset; /*!< Offset to data within file (this will only refer to
                       data within the map file)*/

        struct
        {
            using pad_type =
                std::conditional_t<std::is_same_v<V, xbox_t>, u16, u32>;
            u16      overlapping;
            pad_type padding;
        } pad; /*!< Little hack to let us use the same class
                    on Xbox and PC files,
                    differentiated through templates */
    };

    /*!
     * \brief Function for dereferencing reflexive data. Will do basic error
     * checking eg. the pointer is within the map file and that count > 0
     */
    result<span_type, std::string_view> data(map_ptr const& magic) const
    {
        using namespace std::string_view_literals;

        //        auto padding = pad.padding >> 16;
        //        if(std::is_same_v<V, grbx_t> && padding != 0)
        //            return stl_types::failure("invalid reflexive_t"sv);

        // count/offset are stored little-endian in the map; from_le() corrects
        // them on big-endian hosts (no-op on little-endian).
        u32 const host_count  = from_le(count);
        u32 const host_offset = from_le(offset);

        if(host_count == 0)
            return stl_types::success(span_type());

        auto computed_offset = host_offset - magic.file_offset;
        if(computed_offset > magic.max_size)
            return stl_types::failure("reflexive pointer out of bounds"sv);

        span_type chunk =
            chunk_type::of(
                C_RCAST<T const*>(
                    magic.base_ptr + host_offset - magic.file_offset),
                host_count)
                .view;
        return stl_types::success(chunk);
    }

    result<T const*, std::string_view> data(
        map_ptr const& magic, single_value_t) const
    {
        using namespace std::string_view_literals;

        if(auto v = data(magic); v.has_error())
            return v.error();
        else if(v.value().size() > 1)
            return "got multiple values"sv;
        else
            return &v.value()[0];
    }

    template<typename T2>
    inline auto as() const
    {
        return reinterpret_cast<reference<T2, V> const*>(this);
    }

    /*! Element count in host byte order (the raw \c count field is stored
     *  little-endian and must not be read directly on big-endian hosts). */
    inline u32 size() const
    {
        return from_le(count);
    }
};

static_assert(
    sizeof(reference<int, grbx_t>) == 12,
    "reference_t<..., grbx_t> needs to be 12 bytes");

static_assert(
    sizeof(reference<int, xbox_t>) == 8,
    "reference_t<..., xbox_t> needs to be 8 bytes");

} // namespace blam
