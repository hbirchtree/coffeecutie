#pragma once

#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{

FORCEDINLINE szptr GetPixSize(BitFormat fmt, PixelComponents comp, szptr pixels)
{
    szptr pxsz = 0;
    switch(fmt)
    {
    case BitFormat::Byte:
    case BitFormat::ByteR:
    case BitFormat::UByte:
    case BitFormat::UByteR:
    case BitFormat::UByte_332:
    case BitFormat::UByte_233R:
        pxsz = 1;
        break;
    case BitFormat::Short:
    case BitFormat::ShortR:
    case BitFormat::UShort:
    case BitFormat::UShortR:
    case BitFormat::UShort_565:
    case BitFormat::UShort_565R:
    case BitFormat::UShort_5551:
    case BitFormat::UShort_1555R:
    case BitFormat::Scalar_16:
        pxsz = 2;
        break;
    case BitFormat::Int:
    case BitFormat::IntR:
    case BitFormat::UInt:
    case BitFormat::UIntR:
    case BitFormat::UInt_5999R:
    case BitFormat::UInt_1010102:
    case BitFormat::UInt_2101010R:
    case BitFormat::Scalar_32:
    case BitFormat::Scalar_11_11_10:
    case BitFormat::UInt24_8:
        pxsz = 4;
        break;
    case BitFormat::Scalar_64:
    case BitFormat::Scalar_32_Int_24_8:
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
        pxsz *= 0;
        break;
    }

    return pxsz*pixels;
}

}
