#include <blam/common/texels.h>

namespace blam::texels {

std::vector<u8> expand_rgba8(
    semantic::Span<const u8> pixels, size_t count, u32 channels, swizzle_t const& swizzle)
{
    std::vector<u8> rgba(count * 4);
    if(pixels.size() < count * channels)
        return rgba;
    for(size_t i = 0; i < count; i++)
        for(size_t c = 0; c < 4; c++)
        {
            auto const sc   = swizzle[c];
            rgba[i * 4 + c] = sc == channel_t::red     ? pixels[i * channels]
                              : sc == channel_t::green ? pixels[i * channels + 1]
                              : sc == channel_t::one   ? u8(255)
                                                       : u8(0);
        }
    return rgba;
}

} // namespace blam::texels
