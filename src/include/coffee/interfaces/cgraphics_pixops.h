#pragma once

#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/tdef/standard_exceptions.h>

namespace Coffee {

FORCEDINLINE szptr GetPixSize(BitFormat fmt, PixelComponents comp, szptr pixels)
{
    using B = BitFormat;

    szptr pxsz = 0;
    switch(fmt)
    {
    case B::Byte:
    case B::ByteR:
    case B::UByte:
    case B::UByteR:
    case B::UByte_332:
    case B::UByte_233R:
        pxsz = 1;
        break;
    case B::Short:
    case B::ShortR:
    case B::UShort:
    case B::UShortR:
    case B::UShort_4444:
    case B::UShort_4444R:
    case B::UShort_565:
    case B::UShort_565R:
    case B::UShort_5551:
    case B::UShort_1555R:
    case B::Scalar_16:
        pxsz = 2;
        break;
    case B::Int:
    case B::IntR:
    case B::UInt:
    case B::UIntR:
    case B::UInt_5999R:
    case B::UInt_1010102:
    case B::UInt_2101010R:
    case B::Scalar_32:
    case B::Scalar_11_11_10:
    case B::UInt24_8:
        pxsz = 4;
        break;
    case B::Scalar_64:
    case B::Scalar_32_Int_24_8:
        pxsz = 8;
        break;
    }
    switch(comp)
    {
    case PixelComponents::R:
    case PixelComponents::G:
    case PixelComponents::B:
    case PixelComponents::A:
    case PixelComponents::Stencil:
    case PixelComponents::Depth:
    case PixelComponents::DepthStencil:
        pxsz *= 1;
        break;
    case PixelComponents::RG:
        pxsz *= 2;
        break;
    case PixelComponents::RGB:
    case PixelComponents::BGR:
        pxsz *= 3;
        break;
    case PixelComponents::RGBA:
    case PixelComponents::BGRA:
        pxsz *= 4;
        break;
    default:
        Throw(implementation_error("size calculation not implemented"));
    }

    return pxsz * pixels;
}

FORCEDINLINE szptr GetPixCompressedSize(CompFmt format, Size const& tex_size)
{
    auto fmt    = format.base_fmt;
    auto cflags = format.c_flags;

    switch(fmt)
    {
    case PixFmt::S3TC:
    {
        if(cflags == CompFlags::S3TC_1 || cflags == CompFlags::S3TC_5)
        {
            u32 block_size = 16;

            if(cflags == CompFlags::S3TC_1)
                block_size = 8;

            return (C_FCAST<u32>(tex_size.area()) / 16) * block_size;
        }
        break;
    }
    default:
        break;
    }

    Throw(implementation_error("size calculation not implemented"));
}

} // namespace Coffee
