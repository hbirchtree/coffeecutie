#pragma once

#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/types/edef/pixenum.h>

namespace Coffee {

#if __cpp_constexpr >= 201304
#define CONSTEXPR_EXTENDED constexpr
#else
#define CONSTEXPR_EXTENDED
#endif

FORCEDINLINE bool IsPixFmtCompressed(PixFmt f)
{
    switch(f)
    {
    case PixFmt::ASTC:
    case PixFmt::S3TC:
    case PixFmt::ETC1:
    case PixFmt::ETC2:
    case PixFmt::ATC:
        return true;
    default:
        return false;
    }
}

CONSTEXPR_EXTENDED
FORCEDINLINE BitFmt GetPreferredBitFmt(PixFmt fmt)
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

CONSTEXPR_EXTENDED
FORCEDINLINE PixelFlags GetPixSampleType(PixFmt fmt)
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

    default:
        return PixFlg::FloatingPoint;
    }
}

CONSTEXPR_EXTENDED
FORCEDINLINE PixCmp GetPixComponent(PixFmt fmt)
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
    case F::Depth32F:
        return C::Depth;

    case F::Depth24Stencil8:
    case F::Depth32FStencil8:
        return C::DepthStencil;

    default:
        return C::None;
    }
}

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

CONSTEXPR_EXTENDED
FORCEDINLINE PixCmp GetPixComponent(CompFmt fmt)
{
    switch(fmt.base_fmt)
    {
    case PixFmt::S3TC:
    {
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
        case CompFlags::S3TC_1:
            return PixCmp::RGB;
        case CompFlags::S3TC_3:
        case CompFlags::S3TC_5:
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

struct PixDesc
{
    PixDesc() :
        pixfmt(PixFmt::None), cmpflg(CompFlags::CompressionNone),
        pixflg(PixFlg::None), bfmt(BitFmt::Byte), comp(PixCmp::None)
    {
    }

    PixDesc(PixFmt pixfmt, BitFmt bitfmt, PixCmp comp) :
        pixfmt(pixfmt), cmpflg(CompFlags::CompressionNone),
        pixflg(PixFlg::None), bfmt(bitfmt), comp(comp)
    {
    }

    PixDesc(BitFmt bitfmt, PixCmp comp) :
        cmpflg(CompFlags::CompressionNone), pixflg(PixFlg::None), bfmt(bitfmt),
        comp(comp)
    {
    }

    PixDesc(CompFmt cf) : c(cf), bfmt(BitFmt::Byte), comp(GetPixComponent(c))
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
};

#if __cpp_constexpr >= 201304
template<PixFmt F, BitFmt B, PixelFlags Sample>
struct PixInfo
{
    static constexpr PixFmt     format      = F;
    static constexpr PixCmp     components  = GetPixComponent(F);
    static constexpr BitFmt     bitfmt      = B;
    static constexpr PixelFlags sample_type = Sample;
};

template<PixFmt F>
struct PixInfoDefault : PixInfo<F, GetPreferredBitFmt(F), GetPixSampleType(F)>
{
};
#endif

#undef CONSTEXPR_EXTENDED
} // namespace Coffee
