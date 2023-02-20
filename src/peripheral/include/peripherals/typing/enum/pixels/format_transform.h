#pragma once

#include <peripherals/base.h>
#include <peripherals/stl/standard_exceptions.h>

#include <peripherals/stl/types.h>
#include <peripherals/typing/enum/pixels/components.h>
#include <peripherals/typing/enum/pixels/format.h>

namespace typing {
namespace pixels {

namespace convert {

template<
    typename ToType,
    typename std::enable_if<
        std::is_same<ToType, PixFlg>::value>::type* = nullptr>
FORCEDINLINE constexpr PixFlg to(PixCmp component)
{
    switch(component)
    {
    case PixCmp::R:
        return PixFlg::R;
    case PixCmp::RG:
        return PixFlg::RG;
    case PixCmp::RGB:
        return PixFlg::RGB;
    case PixCmp::RGBA:
    case PixCmp::BGRA:
        return PixFlg::RGBA;
    case PixCmp::Depth:
        return PixFlg::R | PixFlg::FloatingPoint;
    case PixCmp::Stencil:
        return PixFlg::R;
    case PixCmp::DepthStencil:
        return PixFlg::RG;
    default:
        return PixFlg::None;
    }
}

template<
    typename ToType,
    typename std::enable_if<
        std::is_same<ToType, BitFmt>::value>::type* = nullptr>
FORCEDINLINE constexpr BitFmt to(PixFmt fmt)
{
    using P = PixFmt;
    using B = BitFmt;

    switch(fmt)
    {
    case P::R16F:
    case P::RG16F:
    case P::RGB16F:
    case P::RGBA16F:
        return B::Scalar_16;

    case P::R32F:
    case P::RG32F:
    case P::RGB32F:
    case P::RGBA32F:
        return B::Scalar_32;

    case P::R8I:
    case P::RG8I:
    case P::RGB8I:
    case P::RGBA8I:
        return B::Byte;

    case P::R16I:
    case P::RG16I:
    case P::RGB16I:
    case P::RGBA16I:
        return B::Short;

    case P::R32I:
    case P::RG32I:
    case P::RGB32I:
    case P::RGBA32I:
        return B::Int;

    case P::RG16:
    case P::R16UI:
    case P::RG16UI:
    case P::RGB16UI:
    case P::RGBA16UI:
        return B::UShort;

    case P::R32UI:
    case P::RG32UI:
    case P::RGB32UI:
    case P::RGBA32UI:
        return B::UInt;

    case P::R3G3B2:
        return B::UByte_332;
    case P::RGB565:
        return B::UShort_565;
    case P::RGB5A1:
        return B::UShort_5551;
    case P::RGBA4:
        return B::UShort_4444;
    case P::RGB12:
    case P::RGB16:
    case P::RGBA12:
    case P::RGBA16:
        return B::UShort;

    case P::RGB10A2:
        return B::UInt_1010102;
    case P::R11G11B10F:
        return B::Scalar_11_11_10;
    case P::RGB9E5:
        return B::UInt_5999R;

    case P::Depth16:
        return B::UShort;
    case P::Depth16F:
        return B::Scalar_16;
    case P::Depth24Stencil8:
        return B::UInt24_8;
    case P::Depth32F:
        return B::Scalar_32;
    case P::Depth32FStencil8:
        return B::Scalar_32_Int_24_8;

    default:
        return B::UByte;
    }
}

/*!
 * \brief Describes sampling type when used in graphics shaders.
 * \param fmt
 * \return
 */
template<
    typename ToType,
    typename std::enable_if<
        std::is_same<ToType, PixFlg>::value>::type* = nullptr>
FORCEDINLINE constexpr PixFlg to(PixFmt fmt)
{
    using F = PixFmt;

    switch(fmt)
    {
    case F::R8UI:
    case F::R16UI:
    case F::R32UI:
    case F::RG8UI:
    case F::RG16UI:
    case F::RG32UI:
    case F::RGB8UI:
    case F::RGB16UI:
    case F::RGB32UI:
    case F::RGB10A2UI:

    case F::RGBA8UI:
    case F::RGBA16UI:
    case F::RGBA32UI:

        return PixFlg::Unsigned;

    case F::R8I:
    case F::R16I:
    case F::R32I:
    case F::RG8I:
    case F::RG16I:
    case F::RG32I:
    case F::RGB8I:
    case F::RGB16I:
    case F::RGB32I:
    case F::RGBA8I:
    case F::RGBA16I:
    case F::RGBA32I:

        return PixFlg::Signed;

    case F::R8:
    case F::RG8:
    case F::RGB8:
    case F::RGBA8:

    case F::R16:
    case F::RG16:
    case F::RGB16:
    case F::RGBA16:

    case F::RGB4:
    case F::RGB5:
    case F::RGB10:
    case F::RGB12:

    case F::RGBA2:
    case F::RGBA4:
    case F::RGBA12:

    case F::R3G3B2:
    case F::RGB565:
    case F::RGB9E5:

    case F::RGB5A1:
    case F::RGB10A2:

    case F::R16F:
    case F::RG16F:
    case F::RGB16F:
    case F::RGBA16F:
    case F::R32F:
    case F::RG32F:
    case F::RGB32F:
    case F::RGBA32F:

    case F::R11G11B10F:

        return PixFlg::FloatingPoint;

    case F::SRGB8:
    case F::SRGB8A8:
        return PixFlg::sRGB;

    case F::Depth16:
    case F::Depth16F:
    case F::Depth32F:
    case F::Depth24Stencil8:
    case F::Depth32FStencil8:
        return PixFlg::FloatingPoint;

    default:
        return PixFlg::None;
    }
}

template<
    typename ToType,
    typename std::enable_if<
        std::is_same<ToType, PixCmp>::value>::type* = nullptr>
FORCEDINLINE constexpr PixCmp to(PixFmt fmt)
{
    using C = PixCmp;
    using F = PixFmt;

    switch(fmt)
    {
    case F::R8:
    case F::R8I:
    case F::R8UI:
    case F::R16:
    case F::R16I:
    case F::R16UI:
    case F::R32I:
    case F::R32UI:
    case F::R16F:
    case F::R32F:
        return C::R;
    case F::RG8:
    case F::RG8I:
    case F::RG8UI:
    case F::RG16:
    case F::RG16I:
    case F::RG16UI:
    case F::RG32I:
    case F::RG32UI:
    case F::RG16F:
    case F::RG32F:
        return C::RG;

    case F::R3G3B2:
    case F::RGB4:
    case F::RGB5:
    case F::RGB565:
    case F::SRGB8:
    case F::RGB10:
    case F::RGB12:
    case F::RGB9E5:
    case F::R11G11B10F:

    case F::RGB8:
    case F::RGB8I:
    case F::RGB8UI:
    case F::RGB16:
    case F::RGB16I:
    case F::RGB16UI:
    case F::RGB32I:
    case F::RGB32UI:
    case F::RGB16F:
    case F::RGB32F:
        return C::RGB;

    case F::RGBA2:
    case F::RGBA4:
    case F::RGB5A1:
    case F::RGBA12:
    case F::RGB10A2:
    case F::RGB10A2UI:
    case F::SRGB8A8:

    case F::RGBA8:
    case F::RGBA8I:
    case F::RGBA8UI:
    case F::RGBA16:
    case F::RGBA16I:
    case F::RGBA16UI:
    case F::RGBA32I:
    case F::RGBA32UI:
    case F::RGBA16F:
    case F::RGBA32F:
        return C::RGBA;

    case F::Depth16:
    case F::Depth16F:
    case F::Depth32F:
        return C::Depth;

    case F::Depth24Stencil8:
    case F::Depth32FStencil8:
        return C::DepthStencil;

    default:
        return C::None;
    }
}

} // namespace convert

namespace properties {

enum format_property
{
    is_compressed,
    pixel_size,
    is_floating_point,

    layout,

    /* For compressed formats */
    block_size,
    supports_subtextures,

    supports_srgb,
};

template<format_property Prop>
requires(Prop == is_compressed) FORCEDINLINE constexpr bool get(PixFmt f)
{
    switch(f)
    {
    case PixFmt::ASTC:
    case PixFmt::S3TC:
    case PixFmt::ETC1:
    case PixFmt::ETC2:
    case PixFmt::ATC:
    case PixFmt::PVRTC:
    case PixFmt::PVRTC2:
        return true;
    default:
        return false;
    }
}

template<format_property Prop>
requires(Prop == supports_srgb) FORCEDINLINE constexpr bool get(PixFmt f)
{
    switch(f)
    {
    case PixFmt::SRGB8:
    case PixFmt::SRGB8A8:
        return true;
    default:
        return false;
    }
}

template<format_property Prop>
requires(Prop == pixel_size) FORCEDINLINE constexpr szptr
    get(BitFmt fmt, PixCmp comp, szptr pixels)
{
    using B = BitFmt;

    /* Packed formats contain RGB/RGBA within a single value */
    bool packed = false;

    szptr pxsz = 0;
    switch(fmt)
    {
    case B::UByte_332:
    case B::UByte_233R:
        packed = true;
        [[fallthrough]];
    case B::Byte:
    case B::ByteR:
    case B::UByte:
    case B::UByteR:
        pxsz = 1;
        break;
    case B::UShort_4444:
    case B::UShort_4444R:
    case B::UShort_565:
    case B::UShort_565R:
    case B::UShort_5551:
    case B::UShort_1555R:
        packed = true;
        [[fallthrough]];
    case B::Short:
    case B::ShortR:
    case B::UShort:
    case B::UShortR:
    case B::Scalar_16:
        pxsz = 2;
        break;
    case B::UInt_5999R:
    case B::UInt_1010102:
    case B::UInt_2101010R:
    case B::Scalar_11_11_10:
        packed = true;
        [[fallthrough]];
    case B::Int:
    case B::IntR:
    case B::UInt:
    case B::UIntR:
    case B::Scalar_32:
    case B::UInt24_8:
        pxsz = 4;
        break;
    case B::Scalar_64:
    case B::Scalar_32_Int_24_8:
        pxsz = 8;
        break;

    default:
        Throw(implementation_error("size calculation not implemented"));
    }
    if(!packed)
        switch(comp)
        {
        case PixCmp::R:
        case PixCmp::G:
        case PixCmp::B:
        case PixCmp::A:
        case PixCmp::Stencil:
        case PixCmp::Depth:
        case PixCmp::DepthStencil:
            pxsz *= 1;
            break;
        case PixCmp::RG:
            pxsz *= 2;
            break;
        case PixCmp::RGB:
        case PixCmp::BGR:
            pxsz *= 3;
            break;
        case PixCmp::RGBA:
        case PixCmp::BGRA:
            pxsz *= 4;
            break;

        default:
            Throw(implementation_error("size calculation not implemented"));
        }

    return pxsz * pixels;
}

} // namespace properties

struct CompFmt
{
    constexpr CompFmt() :
        base_fmt(PixFmt::None), c_flags(CompFlags::CompressionNone),
        p_flags(PixFlg::None)
    {
    }
    constexpr CompFmt(PixFmt base, CompFlags cf) :
        base_fmt(base), c_flags(cf), p_flags(PixFlg::None)
    {
    }
    constexpr CompFmt(PixFmt base, PixFlg flg, CompFlags cf) :
        base_fmt(base), c_flags(cf), p_flags(flg)
    {
    }

    PixFmt    base_fmt;
    CompFlags c_flags;
    PixFlg    p_flags;
};

namespace convert {

template<typename ToType>
requires std::is_same_v<ToType, PixCmp> FORCEDINLINE constexpr PixCmp to(
    CompFmt fmt)
{
    switch(fmt.base_fmt)
    {
    case PixFmt::BCn: {
        switch(fmt.p_flags)
        {
        case PixFlg::RGB:
            return PixCmp::RGB;
        case PixFlg::RGBA:
            return PixCmp::RGBA;
        default:
            break;
        }
        switch(fmt.c_flags)
        {
        case CompFlags::BC4:
            return PixCmp::R;
        case CompFlags::BC5:
            return PixCmp::RG;
        case CompFlags::BC1:
            return PixCmp::RGB;
        case CompFlags::BC2:
        case CompFlags::BC3:
        case CompFlags::BC6H:
        case CompFlags::BC7:
            return PixCmp::RGBA;
        default:
            break;
        }
        break;
    }

    default:
        break;
    }

    return PixCmp::None;
}

} // namespace convert

namespace properties {

struct block_dim
{
    i32 w, h;
};

template<format_property Prop>
requires(Prop == block_size) FORCEDINLINE constexpr block_dim
    get(CompFmt format)
{
    switch(format.base_fmt)
    {
    case PixFmt::ETC1:
    case PixFmt::ETC2:
    case PixFmt::S3TC:
    case PixFmt::ATC:
        return {4, 4};
    case PixFmt::ASTC: {
        static_assert(
            C_CAST<u8>(CompFlags::ASTC_4x4) == 1, "assumption broken");
        static_assert(
            C_CAST<u8>(CompFlags::ASTC_12x12) == 14, "assumption broken");

        const constexpr std::array<block_dim, 14> ASTC_Block_Sizes = {{
            {4, 4},

            {5, 4},
            {5, 5},

            {6, 5},
            {6, 6},

            {8, 5},
            {8, 6},
            {8, 8},

            {10, 5},
            {10, 6},
            {10, 8},
            {10, 10},

            {12, 10},
            {12, 12},
        }};
        const u8                                  block_idx
            = C_CAST<u8>(format.c_flags) - C_CAST<u8>(CompFlags::ASTC_4x4);

        return ASTC_Block_Sizes.at(block_idx);
    }
    case PixFmt::PVRTC2: {
        block_dim block_size = {4, 4};

        if(enum_helpers::feval(format.c_flags & CompFlags::bpp_2))
            block_size.w = 8;

        return block_size;
    }
    default:
        break;
    }

    return {1, 1};
}

template<format_property Prop>
requires(Prop == supports_subtextures) FORCEDINLINE
    constexpr bool get(CompFmt format)
{
    switch(format.base_fmt)
    {
    case PixFmt::PVRTC:
    case PixFmt::ATC:
    case PixFmt::ETC1:
        return false;

    default:
        return true;
    }
}

struct layout_t
{
    u8 r, g, b, a;

    u8 depth, stencil;

    static layout_t rgba(u8 v)
    {
        return {v, v, v, v, 0, 0};
    }
    static layout_t rgba(u8 v, u8 a)
    {
        return {v, v, v, a, 0, 0};
    }
    static layout_t ds(u8 depth, u8 stencil)
    {
        return {0, 0, 0, 0, depth, stencil};
    }
};

template<format_property Prop>
requires(Prop == layout)
    //
    FORCEDINLINE constexpr layout_t get(PixFmt fmt)
{
    using F = PixFmt;

    switch(fmt)
    {
    case F::RGBA2:
        return layout_t::rgba(2);
    case F::RGBA4:
        return layout_t::rgba(4);
    case F::RGBA8:
    case F::RGBA8I:
    case F::RGBA8UI:
        return layout_t::rgba(8);
    case F::RGBA12:
        return layout_t::rgba(12);
    case F::RGBA16:
    case F::RGBA16I:
    case F::RGBA16UI:
    case F::RGBA16F:
        return layout_t::rgba(16);
    case F::RGBA32F:
    case F::RGBA32I:
    case F::RGBA32UI:
        return layout_t::rgba(32);
    case F::RGB5A1:
        return layout_t::rgba(5, 1);
    case F::RGB10A2:
    case F::RGB10A2UI:
        return layout_t::rgba(10, 2);

    case F::R3G3B2:
        return {3, 3, 2, 0, 0, 0};
    case F::RGB4:
        return layout_t::rgba(4, 0);
    case F::RGB5:
        return layout_t::rgba(5, 0);
    case F::RGB565:
        return {5, 6, 5, 0, 0, 0};
    case F::RGB8:
        return layout_t::rgba(8, 0);
    case F::RGB10:
        return layout_t::rgba(10, 0);
    case F::RGB12:
        return layout_t::rgba(12, 0);
    case F::RGB16F:
    case F::RGB16:
        return layout_t::rgba(16, 0);
    case F::RGB9E5:
        return layout_t::rgba(9, 0);
    case F::R11G11B10F:
        return {11, 11, 10, 0, 0, 0};
    case F::RGB32F:
    case F::RGB32I:
    case F::RGB32UI:
        return layout_t::rgba(32, 0);

    case F::SRGB8:
        return {8, 8, 8, 0, 0, 0};
    case F::SRGB8A8:
        return {8, 8, 8, 8, 0, 0};

    case F::Depth16:
        return layout_t::ds(16, 0);
    case F::Depth24Stencil8:
        return layout_t::ds(24, 8);
    case F::Depth16F:
        return layout_t::ds(16, 0);
    case F::Depth32F:
        return layout_t::ds(32, 0);
    case F::Depth32FStencil8:
        return layout_t::ds(32, 8);

    default:
        Throw(undefined_behavior("layout not defined"));
    }
}

template<format_property Prop>
requires(Prop == is_floating_point)
    //
    FORCEDINLINE constexpr bool get(PixFmt fmt)
{
    using F = PixFmt;

    switch(fmt)
    {
    case F::RGBA2:
    case F::R3G3B2:
    case F::RGB4:
    case F::RGBA4:
    case F::RGB5:
    case F::RGB5A1:
    case F::RGB565:
    case F::RGB8:
    case F::SRGB8:
    case F::RGBA8:
    case F::SRGB8A8:
    case F::RGBA8I:
    case F::RGBA8UI:
    case F::RGB9E5:
    case F::RGB10:
    case F::RGB10A2:
    case F::RGB10A2UI:
    case F::RGB12:
    case F::RGBA12:
    case F::Depth16:
    case F::RGB16:
    case F::RGBA16:
    case F::RGBA16I:
    case F::RGBA16UI:
    case F::Depth24Stencil8:
    case F::RGB32I:
    case F::RGBA32I:
    case F::RGB32UI:
    case F::RGBA32UI:
        return false;
    case F::R11G11B10F:
    case F::RGB16F:
    case F::RGBA16F:
    case F::Depth16F:
    case F::RGB32F:
    case F::RGBA32F:
    case F::Depth32F:
    case F::Depth32FStencil8:
        return true;
    default:
        Throw(undefined_behavior("type not defined"));
    }
}

} // namespace properties

struct PixDesc
{
    PixDesc() :
        pixfmt(PixFmt::None), cmpflg(CompFlags::CompressionNone),
        pixflg(PixFlg::None), bfmt(BitFmt::Byte), comp(PixCmp::None)
    {
    }

    PixDesc(PixFmt pixfmt) :
        pixfmt(pixfmt), cmpflg(CompFlags::CompressionNone),
        pixflg(PixFlg::None), bfmt(convert::to<BitFmt>(pixfmt)),
        comp(convert::to<PixCmp>(pixfmt))
    {
    }

    PixDesc(PixFmt pixfmt, BitFmt bitfmt, PixCmp comp) :
        pixfmt(pixfmt), cmpflg(CompFlags::CompressionNone),
        pixflg(PixFlg::None), bfmt(bitfmt), comp(comp)
    {
    }

    PixDesc(BitFmt bitfmt, PixCmp comp) :
        pixfmt(PixFmt::None), cmpflg(CompFlags::CompressionNone),
        pixflg(PixFlg::None), bfmt(bitfmt), comp(comp)
    {
    }

    PixDesc(CompFmt cf) :
        c(cf), bfmt(BitFmt::Byte), comp(convert::to<PixCmp>(c))
    {
    }

    union
    {
        CompFmt c;
        struct
        {
            PixFmt    pixfmt;
            CompFlags cmpflg;
            PixFlg    pixflg;
        };
    };

    BitFmt bfmt;
    PixCmp comp;

    inline bool compressed() const
    {
        return properties::get<properties::is_compressed>(pixfmt);
    }
};

template<PixFmt F, BitFmt B, PixFlg Sample>
struct PixInfo
{
    static constexpr PixFmt format      = F;
    static constexpr PixCmp components  = convert::to<PixCmp>(F);
    static constexpr BitFmt bitfmt      = B;
    static constexpr PixFlg sample_type = Sample;
};

template<PixFmt F>
struct PixInfoDefault
    : PixInfo<F, convert::to<BitFmt>(F), convert::to<PixFlg>(F)>
{
};

} // namespace pixels
} // namespace typing
