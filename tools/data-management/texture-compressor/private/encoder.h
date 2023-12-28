#pragma once

#include "decoder.h"
#include <Etc/Etc.h>

struct ktxTexture1;

namespace compressor {

struct settings_t
{
    float    quality{100.f};
    uint32_t mipmaps{1};
};

} // namespace compressor

namespace compressor::etc2 {

using format_t = Etc::Image::Format;

std::optional<ktxTexture1*> encode(
    compressor::rgbaf_image_t const& img,
    format_t                         format   = format_t::RGBA8,
    settings_t&&                     settings = {});

} // namespace compressor::etc2

namespace compressor::bcn {

std::optional<ktxTexture1*> encode(
    compressor::rgba_image_t const& img,
    typing::pixels::CompFlags       format,
    typing::PixCmp                  channels,
    settings_t&&                    settings = {});

}
