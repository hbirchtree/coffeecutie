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

std::optional<std::array<gfx::textures::swizzle_t, 4>> channel_swizzle(blam::bitm::format_t format)
{
    using blam::bitm::format_t;
    using gfx::textures::swizzle_t;
    switch(format)
    {
    case format_t::Y8:
        return std::array{swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::one};
    case format_t::AY8:
        return std::array{swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::red};
    case format_t::A8:
        return std::array{swizzle_t::one, swizzle_t::one, swizzle_t::one, swizzle_t::red};
    case format_t::A8Y8:
        return std::array{swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::green};
    default:
        return std::nullopt;
    }
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
