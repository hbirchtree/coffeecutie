#pragma once

#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/types.h>

#include <coffee/image/cimage.h>

namespace compressor {

using rgba_image_t = Coffee::stb::image_rw;
using rgbaf_image_t = Coffee::stb::image_float;

using Coffee::stb::LoadData;

}
