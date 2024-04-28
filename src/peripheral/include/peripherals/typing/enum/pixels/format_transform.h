#pragma once

#include <peripherals/base.h>
#include <peripherals/stl/standard_exceptions.h>

#include <peripherals/stl/types.h>
#include <peripherals/typing/enum/pixels/components.h>
#include <peripherals/typing/enum/pixels/format.h>

namespace typing::pixels {

struct CompFmt
{
    constexpr CompFmt()
        : base_fmt(pix_fmt::None)
        , c_flags(comp_flags::CompressionNone)
        , p_flags(pix_flags::None)
    {
    }

    constexpr CompFmt(pix_fmt base, comp_flags cf = comp_flags::CompressionNone)
        : base_fmt(base)
        , c_flags(cf)
        , p_flags(pix_flags::None)
    {
    }

    constexpr CompFmt(
        pix_fmt base, pix_flags flg, comp_flags cf = comp_flags::CompressionNone)
        : base_fmt(base)
        , c_flags(cf)
        , p_flags(flg)
    {
    }

    pix_fmt    base_fmt;
    comp_flags c_flags;
    pix_flags    p_flags;
};

namespace convert {

template<
    typename ToType,
    typename std::enable_if<std::is_same<ToType, pix_flags>::value>::type* =
        nullptr>
FORCEDINLINE constexpr pix_flags to(pix_components component)
{
    switch(component)
    {
    case pix_components::R:
        return pix_flags::R;
    case pix_components::RG:
        return pix_flags::RG;
    case pix_components::RGB:
        return pix_flags::RGB;
    case pix_components::RGBA:
    case pix_components::BGRA:
        return pix_flags::RGBA;
    case pix_components::Depth:
        return pix_flags::R | pix_flags::FloatingPoint;
    case pix_components::Stencil:
        return pix_flags::R;
    case pix_components::DepthStencil:
        return pix_flags::RG;
    default:
        return pix_flags::None;
    }
}

template<
    typename ToType,
    typename std::enable_if<std::is_same<ToType, bit_fmt>::value>::type* =
        nullptr>
FORCEDINLINE constexpr bit_fmt to(pix_fmt fmt)
{
    using P = pix_fmt;
    using B = bit_fmt;

    switch(fmt)
    {
    case P::R16F:
    case P::RG16F:
    case P::RGB16F:
    case P::RGBA16F:
        return B::f16;

    case P::R32F:
    case P::RG32F:
    case P::RGB32F:
    case P::RGBA32F:
        return B::f32;

    case P::R8I:
    case P::RG8I:
    case P::RGB8I:
    case P::RGBA8I:
        return B::i8;

    case P::R16I:
    case P::RG16I:
    case P::RGB16I:
    case P::RGBA16I:
        return B::i16;

    case P::R32I:
    case P::RG32I:
    case P::RGB32I:
    case P::RGBA32I:
        return B::i32;

    case P::RG16:
    case P::R16UI:
    case P::RG16UI:
    case P::RGB16UI:
    case P::RGBA16UI:
        return B::u16;

    case P::R32UI:
    case P::RG32UI:
    case P::RGB32UI:
    case P::RGBA32UI:
        return B::u32;

    case P::R3G3B2:
        return B::u8_332;
    case P::RGB565:
        return B::u16_565;
    case P::RGB5A1:
        return B::u16_5551;
    case P::RGBA4:
        return B::u16_4444;
    case P::RGB12:
    case P::RGB16:
    case P::RGBA12:
    case P::RGBA16:
        return B::u16;

    case P::RGB10A2:
        return B::u32_1010102;
    case P::R11G11B10F:
        return B::Scalar_11_11_10;
    case P::RGB9E5:
        return B::u32_5999;

    case P::Depth16:
        return B::u16;
    case P::Depth16F:
        return B::f16;
    case P::Depth24Stencil8:
        return B::u32_24_8;
    case P::Depth32F:
        return B::f32;
    case P::Depth32FStencil8:
        return B::f32_u32_24_8;

    default:
        return B::u8;
    }
}

/*!
 * \brief Describes sampling type when used in graphics shaders.
 * \param fmt
 * \return
 */
template<
    typename ToType,
    typename std::enable_if<std::is_same<ToType, pix_flags>::value>::type* =
        nullptr>
FORCEDINLINE constexpr pix_flags to(pix_fmt fmt)
{
    using F = pix_fmt;

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

        return pix_flags::Unsigned;

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

        return pix_flags::Signed;

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

        return pix_flags::FloatingPoint;

    case F::SRGB8:
    case F::SRGB8A8:
        return pix_flags::sRGB;

    case F::Depth16:
    case F::Depth16F:
    case F::Depth32F:
    case F::Depth24Stencil8:
    case F::Depth32FStencil8:
        return pix_flags::FloatingPoint;

    default:
        return pix_flags::None;
    }
}

template<
    typename ToType,
    typename std::enable_if<std::is_same<ToType, pix_components>::value>::type* =
        nullptr>
FORCEDINLINE constexpr pix_components to(pix_fmt fmt)
{
    using C = pix_components;
    using F = pix_fmt;

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

template<typename ToType>
requires std::is_same_v<ToType, pix_components>
FORCEDINLINE constexpr pix_components to(CompFmt fmt)
{
    switch(fmt.base_fmt)
    {
    case pix_fmt::BCn: {
        switch(fmt.p_flags)
        {
        case pix_flags::RGB:
            return pix_components::RGB;
        case pix_flags::RGBA:
            return pix_components::RGBA;
        default:
            break;
        }
        switch(fmt.c_flags)
        {
        case comp_flags::BC4:
            return pix_components::R;
        case comp_flags::BC5:
            return pix_components::RG;
        case comp_flags::BC1:
            return pix_components::RGB;
        case comp_flags::BC2:
        case comp_flags::BC3:
        case comp_flags::BC6H:
        case comp_flags::BC7:
            return pix_components::RGBA;
        default:
            break;
        }
        break;
    }

    default:
        break;
    }

    return pix_components::None;
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
requires(Prop == supports_srgb)
//
FORCEDINLINE constexpr bool get(pix_fmt f)
{
    switch(f)
    {
    case pix_fmt::SRGB8:
    case pix_fmt::SRGB8A8:
        return true;
    default:
        return false;
    }
}

template<format_property Prop>
requires(Prop == pixel_size)
//
FORCEDINLINE constexpr szptr get(bit_fmt fmt, pix_components comp, szptr pixels)
{
    using B = bit_fmt;

    /* Packed formats contain RGB/RGBA within a single value */
    bool packed = false;

    szptr pxsz = 0;
    switch(fmt)
    {
    case B::u8_332:
    case B::u8_233:
        packed = true;
        [[fallthrough]];
    case B::i8:
    case B::u8:
        pxsz = 1;
        break;
    case B::u16_4444:
    case B::u16_4444_r:
    case B::u16_565:
    case B::u16_565_r:
    case B::u16_5551:
    case B::u16_1555:
        packed = true;
        [[fallthrough]];
    case B::i16:
    case B::u16:
    case B::f16:
        pxsz = 2;
        break;
    case B::u32_5999:
    case B::u32_1010102:
    case B::u32_2101010:
    case B::Scalar_11_11_10:
        packed = true;
        [[fallthrough]];
    case B::i32:
    case B::u32:
    case B::f32:
    case B::u32_24_8:
        pxsz = 4;
        break;
    case B::f64:
    case B::f32_u32_24_8:
        pxsz = 8;
        break;

    default:
        Throw(implementation_error("size calculation not implemented"));
    }
    if(!packed)
        switch(comp)
        {
        case pix_components::R:
        case pix_components::G:
        case pix_components::B:
        case pix_components::A:
        case pix_components::Stencil:
        case pix_components::Depth:
        case pix_components::DepthStencil:
            pxsz *= 1;
            break;
        case pix_components::RG:
            pxsz *= 2;
            break;
        case pix_components::RGB:
        case pix_components::BGR:
            pxsz *= 3;
            break;
        case pix_components::RGBA:
        case pix_components::BGRA:
            pxsz *= 4;
            break;

        default:
            Throw(implementation_error("size calculation not implemented"));
        }

    return pxsz * pixels;
}

} // namespace properties

namespace convert {

} // namespace convert

namespace properties {

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
FORCEDINLINE constexpr layout_t get(pix_fmt fmt)
{
    using F = pix_fmt;

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

} // namespace properties

struct PixDesc
{
    PixDesc()
        : pixfmt(pix_fmt::None)
        , cmpflg(comp_flags::CompressionNone)
        , pixflg(pix_flags::None)
        , bfmt(bit_fmt::i8)
        , comp(pix_components::None)
    {
    }

    PixDesc(pix_fmt pixfmt)
        : pixfmt(pixfmt)
        , cmpflg(comp_flags::CompressionNone)
        , pixflg(pix_flags::None)
        , bfmt(convert::to<bit_fmt>(pixfmt))
        , comp(convert::to<pix_components>(pixfmt))
    {
    }

    PixDesc(pix_fmt pixfmt, bit_fmt bitfmt, pix_components comp)
        : pixfmt(pixfmt)
        , cmpflg(comp_flags::CompressionNone)
        , pixflg(pix_flags::None)
        , bfmt(bitfmt)
        , comp(comp)
    {
    }

    PixDesc(bit_fmt bitfmt, pix_components comp)
        : pixfmt(pix_fmt::None)
        , cmpflg(comp_flags::CompressionNone)
        , pixflg(pix_flags::None)
        , bfmt(bitfmt)
        , comp(comp)
    {
    }

    PixDesc(CompFmt cf)
        : c(cf)
        , bfmt(bit_fmt::i8)
        , comp(convert::to<pix_components>(c))
    {
    }

    union
    {
        CompFmt c;

        struct
        {
            pix_fmt    pixfmt;
            comp_flags cmpflg;
            pix_flags    pixflg;
        };
    };

    bit_fmt bfmt;
    pix_components comp;
};

} // namespace typing::pixels
