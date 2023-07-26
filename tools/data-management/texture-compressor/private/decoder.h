#pragma once

#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/types.h>

#include <coffee/image/cimage.h>

namespace compressor {

using rgba_image_t  = Coffee::stb::image_rw;
using rgbaf_image_t = Coffee::stb::image_float;

using Coffee::stb::LoadData;

template<typename ImageType>
std::optional<ImageType> map_channels(
    ImageType const& image, std::string const& channels);

extern template std::optional<rgba_image_t> map_channels<rgba_image_t>(
    rgba_image_t const& image, std::string const& channels);

extern template std::optional<rgbaf_image_t> map_channels<rgbaf_image_t>(
    rgbaf_image_t const& image, std::string const& channels);

} // namespace compressor
