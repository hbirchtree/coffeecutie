#pragma once

#include "types.h"

#include <blam/common/texels.h>
#include <blam/volta/blam_bitm.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <optional>
#include <vector>

namespace world {

/*! Channel order fixes for Halo 1 formats: A8R8G8B8 is stored B,G,R,A,
 * X8R8G8B8 gets opaque alpha and V8U8 is signed */
void fix_channels(blam::bitm::format_t format, std::vector<u8>& pixels);

/*! One- and two-channel formats land in R/RG; how to spread them */
std::optional<blam::texels::swizzle_t> channel_swizzle(blam::bitm::format_t format);

/*! Has the texture read its channels as `swizzle` says */
void set_swizzle(gfx::texture_2d_t& tex, blam::texels::swizzle_t const& swizzle);

/*! A level of a channel_swizzle() format spread to RGBA8, for GPUs without
 * texture swizzle */
std::vector<u8> expand_rgba8(
    blam::bitm::format_t format, blam::texels::swizzle_t const& swizzle, std::vector<u8> const& pixels, u32 w, u32 h);

/*! A texture with a trilinear, 4x anisotropic sampler */
Texture sampled(std::shared_ptr<gfx::texture_2d_t> const& tex);

} // namespace world
