#pragma once

#include "types.h"

#include <blam/volta/blam_bitm.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <array>
#include <optional>
#include <vector>

namespace world {

/*! Channel order fixes for Halo 1 formats: A8R8G8B8 is stored B,G,R,A,
 * X8R8G8B8 gets opaque alpha and V8U8 is signed */
void fix_channels(blam::bitm::format_t format, std::vector<u8>& pixels);

/*! One- and two-channel formats land in R/RG; how to spread them */
std::optional<std::array<gfx::textures::swizzle_t, 4>> channel_swizzle(blam::bitm::format_t format);

/*! A texture with a trilinear, 4x anisotropic sampler */
Texture sampled(std::shared_ptr<gfx::texture_2d_t> const& tex);

} // namespace world
