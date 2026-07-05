#include <blam/volta/blam_bitm.h>

#include "peripherals/typing/enum/pixels/format.h"
#include "peripherals/typing/enum/pixels/format_transform.h"

namespace blam::bitm {

typing::pixels::PixDesc image_t::to_fmt()
    const
{
    using typing::pixels::CompFmt;
    using typing::pixels::PixDesc;
    switch(format)
    {
    case format_t::A8:
    case format_t::Y8:
    case format_t::P8:
    case format_t::AY8:
        return PixDesc(pix_fmt::R8, bit_fmt::u8, pix_components::R);

    case format_t::A8Y8:
        return PixDesc(pix_fmt::RG8, bit_fmt::u8, pix_components::RG);
    case format_t::R5G6B5:
        return PixDesc(pix_fmt::RGB565, bit_fmt::u16_565, pix_components::RGB);
    case format_t::A1RGB5:
        return PixDesc(pix_fmt::RGB5A1, bit_fmt::u16_1555, pix_components::RGBA);
    case format_t::ARGB4:
        return PixDesc(pix_fmt::RGBA4, bit_fmt::u16_4444, pix_components::RGBA);

    case format_t::ARGB8:
        return PixDesc(pix_fmt::RGBA8, bit_fmt::u8, pix_components::RGBA);
    case format_t::XRGB8:
        return PixDesc(pix_fmt::RGBA8, bit_fmt::u8, pix_components::RGBA);

    case format_t::BC1:
        return CompFmt(pix_fmt::BCn, comp_flags::BC1);
    case format_t::BC2:
        return CompFmt(pix_fmt::BCn, comp_flags::BC2);
    case format_t::BC3:
        return CompFmt(pix_fmt::BCn, comp_flags::BC3);

    case format_t::ETC1_RGB:
        return CompFmt(pix_fmt::ETC1);
    case format_t::ETC2_RGB:
        return CompFmt(pix_fmt::ETC2, typing::pixels::pix_flags::RGB);
    case format_t::ETC2_RGBA:
        return CompFmt(pix_fmt::ETC2, typing::pixels::pix_flags::RGBA);
    case format_t::PVRTCV1_RGB:
        return CompFmt(pix_fmt::PVRTC, comp_flags::PVRTC_BPP4 | comp_flags::PVRTC_RGB);
    case format_t::PVRTCV1_RGBA:
        return CompFmt(pix_fmt::PVRTC, comp_flags::PVRTC_BPP4 | comp_flags::PVRTC_RGBA);

    default:
        return {bit_fmt::undefined, pix_components::None};
    }
}

} // namespace blam::bitm

