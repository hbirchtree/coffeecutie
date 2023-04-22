#pragma once

#include "base.h"
#include "enums/InternalFormat.h"
#include "enums/PixelFormat.h"
#include "enums/PixelType.h"

#include "vk_formats.h"

namespace gl::tex {

using format_t         = gl::group::internal_format;
using texture_layout_t = gl::group::pixel_format;
using texture_type_t   = gl::group::pixel_type;

enum class profile_t
{
    core,
    es,
    web,
};

struct texture_format_t
{
    struct compression_t
    {
        texture_layout_t format;
        u8               block_width;
        u8               block_height;
        u8               block_size;
        u8               bpp;
        u8               component_count;
        bool             srgb;
    };
    struct bit_layout_t
    {
        u8 r{0};
        u8 g{0};
        u8 b{0};
        u8 a{0};
        u8 extra{0};
        u8 depth{0};
        u8 stencil{0};
    };
    struct raw_format_t
    {
        bit_layout_t     bit_layout;
        texture_type_t   type;
        texture_layout_t format;
        u16              pixel_size;
        u8               component_count;
        u8               component_size;
        bool             srgb;
        bool             floating_point;
    };

    format_t                     type{static_cast<format_t>(0)};
    vk_format_t                  vk_type{static_cast<vk_format_t>(0)};
    std::optional<compression_t> compression{};
    std::optional<raw_format_t>  raw_format{};
    struct version_t
    {
        std::optional<u16> core;
        std::optional<u16> es;
        std::optional<u16> web;
    } version{};
    struct extensions_t
    {
        std::string_view core;
        std::string_view es;
        std::string_view web;
    } extensions{};

    constexpr bool is_supported_by(profile_t profile, u32 version_) const
    {
        if(profile == profile_t::core)
            return version.core.has_value() && version_ >= *version.core;
        else if(profile == profile_t::es)
            return version.es.has_value() && version_ >= *version.es;
        else if(profile == profile_t::web)
            return version.web.has_value() && version_ >= *version.web;
        else
            return false;
    }

    constexpr bool is_compressed() const
    {
        return compression.has_value();
    }

    template<typename SizeT>
    constexpr size_t data_size(SizeT const& size) const
    {
        using value_type = typename SizeT::value_type;

        if(compression.has_value())
        {
            auto const& compression = *this->compression;
            return (size[0] / static_cast<value_type>(compression.block_width))
                   * (size[1]
                      / static_cast<value_type>(compression.block_height))
                   * compression.block_size;
        } else
        {
            auto const& raw_format = *this->raw_format;
            return size[0] * size[1] * raw_format.pixel_size;
        }
    }
};

} // namespace gl::tex
