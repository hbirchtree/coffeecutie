#pragma once

#include <fmt/format.h>
#include <optional>
#include <string_view>

#include <peripherals/libc/types.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include "texture_format.h"

namespace gl::tex {

namespace detail {

using namespace std::string_view_literals;

#define COMPRESSED_FORMAT_TYPE texture_format_t::compression_t
#define RAW_FORMAT_TYPE texture_format_t::raw_format_t

constexpr texture_format_t texture_formats[] = {
#include "texture_formats.inl"
};
constexpr size_t num_texture_formats =
    sizeof(texture_formats) / sizeof(texture_format_t);

#undef COMPRESSED_FORMAT_TYPE
#undef RAW_FORMAT_TYPE

} // namespace detail

using texture_format_span =
    gsl::span<const texture_format_t, detail::num_texture_formats>;

inline constexpr texture_format_span texture_formats_view()
{
    return texture_format_span(detail::texture_formats);
}

inline constexpr bool is_compiled(format_t fmt)
{
    for(auto const& def : texture_formats_view())
        if(def.type == fmt)
            return true;
    return false;
}

inline constexpr texture_format_t const& format_of(format_t fmt)
{
    for(auto const& def : texture_formats_view())
        if(def.type == fmt)
            return def;
    throw std::out_of_range(
        "format not found: " + std::to_string(static_cast<u32>(fmt)));
}

inline constexpr texture_format_t const& format_of(vk_format_t fmt)
{
    for(auto const& def : texture_formats_view())
        if(def.vk_type == fmt)
            return def;
    throw std::out_of_range(
        "format not found" + std::to_string(static_cast<u32>(fmt)));
}

using typing::pixels::PixDesc;

inline texture_format_t const& format_of(PixDesc const& desc);

inline PixDesc desc_of(texture_format_t const& fmt);

} // namespace gl::tex
