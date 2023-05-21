#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <type_traits>

#include "blam_magic_data.h"
#include "blam_versions.h"

namespace blam {

using libc_types::u32;

struct single_value_t
{
};

constexpr single_value_t single_value;

template<typename T, typename V = grbx_t, typename MagicTag = void>
/*!
 * \brief Points to a chunk of memory within the file
 */
struct alignas(4) reflexive_t
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
            using pad_type
                = std::conditional_t<std::is_same_v<V, xbox_t>, u16, u32>;
            u16      overlapping;
            pad_type padding;
        } pad; /*!< Little hack to let us use the same class
                    on Xbox and PC files,
                    differentiated through templates */
    };

    /*!
     * \brief Made for simplicity when working with reflexive data. Instead of
     * several functions performing the same task, this template allows to
     * access the data much more conveniently. \param basePtr Base pointer of
     * map in most cases \param magic Magic number from tag index \return A
     * valid pointer if the reflexive is deemed valid (if the variable zero is
     * indeed zero)
     */
    result<span_type, std::string_view> data(magic_data_t const& magic) const
    {
        using namespace std::string_view_literals;

        //        auto padding = pad.padding >> 16;
        //        if(std::is_same_v<V, grbx_t> && padding != 0)
        //            return stl_types::failure("invalid reflexive_t"sv);

        if(count == 0)
            return stl_types::success(span_type{});

        if((offset - magic.magic_offset) > magic.max_size)
            return stl_types::failure("reflexive pointer out of bounds"sv);

        span_type chunk = chunk_type::of(
                              C_RCAST<T const*>(
                                  magic.base_ptr + offset - magic.magic_offset),
                              count)
                              .view;
        return stl_types::success(chunk);
    }

    result<T const*, std::string_view> data(
        magic_data_t const& magic, single_value_t) const
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
        return reinterpret_cast<reflexive_t<T2, V> const*>(this);
    }
};

static_assert(
    sizeof(reflexive_t<int, grbx_t>) == 12,
    "reflexive_t<..., pc_variant> needs to be 12 bytes");

static_assert(
    sizeof(reflexive_t<int, xbox_t>) == 8, "reflexive_t<..., xbox_variant>");

namespace reflexive_helpers {

template<typename T, typename Version>
semantic::Span<T> unwrap(
    reflexive_t<T, Version> const& pointer, magic_data_t const& magic)
{
    if(auto data = pointer.data(magic); data.has_value())
        return data.value();
    else
        return semantic::Span<T>();
}
} // namespace reflexive_helpers
} // namespace blam
