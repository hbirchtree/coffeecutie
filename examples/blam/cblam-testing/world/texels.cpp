#include "texels.h"

#include <utility>

namespace world {

void fix_channels(blam::bitm::format_t format, std::vector<u8>& pixels)
{
    using blam::bitm::format_t;
    if(format == format_t::ARGB8 || format == format_t::XRGB8)
        for(size_t i = 0; i + 3 < pixels.size(); i += 4)
        {
            std::swap(pixels[i], pixels[i + 2]);
            if(format == format_t::XRGB8)
                pixels[i + 3] = 0xFF;
        }
    if(format == format_t::V8U8)
        for(auto& b : pixels)
            b = static_cast<u8>(b + 0x80);
}

std::optional<blam::texels::swizzle_t> channel_swizzle(blam::bitm::format_t format)
{
    using blam::bitm::format_t;
    using c = blam::texels::channel_t;
    switch(format)
    {
    case format_t::Y8:
        return blam::texels::swizzle_t{c::red, c::red, c::red, c::one};
    case format_t::AY8:
        return blam::texels::swizzle_t{c::red, c::red, c::red, c::red};
    case format_t::A8:
        return blam::texels::swizzle_t{c::one, c::one, c::one, c::red};
    case format_t::A8Y8:
        return blam::texels::swizzle_t{c::red, c::red, c::red, c::green};
    default:
        return std::nullopt;
    }
}

void set_swizzle(gfx::texture_2d_t& tex, blam::texels::swizzle_t const& swizzle)
{
    using gfx::textures::swizzle_t;
    auto gl = [](blam::texels::channel_t c) {
        switch(c)
        {
        case blam::texels::channel_t::red:
            return swizzle_t::red;
        case blam::texels::channel_t::green:
            return swizzle_t::green;
        case blam::texels::channel_t::one:
            return swizzle_t::one;
        default:
            return swizzle_t::zero;
        }
    };
    tex.set_swizzle(gl(swizzle[0]), gl(swizzle[1]), gl(swizzle[2]), gl(swizzle[3]));
}

std::vector<u8> expand_rgba8(
    blam::bitm::format_t format, blam::texels::swizzle_t const& swizzle, std::vector<u8> const& pixels, u32 w, u32 h)
{
    u32 const channels = format == blam::bitm::format_t::A8Y8 ? 2 : 1;
    return blam::texels::expand_rgba8(
        semantic::Span<const u8>(pixels.data(), pixels.size()), size_t(w) * h, channels, swizzle);
}

Texture sampled(std::shared_ptr<gfx::texture_2d_t> const& tex)
{
    Texture out{.texture = tex, .sampler = tex->sampler()};
    out.sampler->alloc();
    out.sampler->set_filtering(typing::Filtering::Linear, typing::Filtering::Linear, typing::Filtering::Linear);
    out.sampler->set_anisotropic(4);
    return out;
}

} // namespace world
