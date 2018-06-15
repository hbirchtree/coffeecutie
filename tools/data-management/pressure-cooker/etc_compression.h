#pragma once

#include "texture_common.h"

#if defined(HAVE_ETC2COMP)
#include <Etc.h>
#include <EtcConfig.h>
#include <EtcFilter.h>

using EtcImFormat = Etc::Image::Format;

static cstring GetETCExtension(EtcImFormat fmt)
{
    switch(fmt)
    {
    case EtcImFormat::ETC1:
        return "etc1";
    case EtcImFormat::RGBA8:
        return "etc2";
    case EtcImFormat::RGB8:
        return "etc2";
    default:
        return "etc";
    }
}

static CompFmt GetETCFormat(EtcImFormat fmt)
{
    switch(fmt)
    {
    case EtcImFormat::ETC1:
        return {PixFmt::ETC1, PixFlg::None, CompFlags::CompressionNone};
    case EtcImFormat::RGB8A1:
        return {PixFmt::ETC2, PixFlg::RGBA, CompFlags::CompressionNone};
    }
}

static void CompressETC_With(
    common_tools_t&   t,
    Size const&       size,
    Path const&       outName,
    u32               num_mipmaps,
    stb::image_float& local_img,
    EtcImFormat       format = EtcImFormat::ETC1,
    scalar            effort = 1.f)
{
    Vector<Etc::RawImage> mipmaps;

    mipmaps.resize(num_mipmaps);

    int  encoding_time = 0;
    auto img_size      = size.convert<u32>();

    Etc::EncodeMipmaps(
        local_img.data,
        img_size.w,
        img_size.h,
        format,
        Etc::ErrorMetric::REC709,
        effort,
        1,
        1,
        num_mipmaps,
        0,
        mipmaps.data(),
        &encoding_time);

    for(auto const& mip : mipmaps)
    {
        auto mipName = outName.addExtension(cast_pod(mip.uiExtendedWidth))
                           .addExtension(GetETCExtension(format));
        auto mipData =
            Bytes::From(mip.paucEncodingBits.get(), mip.uiEncodingBitsBytes);

        IMG::serial_image imgDesc = {};
        imgDesc.size =
            _cbasic_size_2d<i32>(mip.uiExtendedWidth, mip.uiExtendedHeight)
                .convert<u32>();
        imgDesc.fmt =
            (format == EtcImFormat::ETC1) ? PixFmt::ETC1 : PixFmt::ETC2;

        t.cursor.progress(TEXCOMPRESS_API "Saving mipmap: {0}", mipName);

        t.cooker->cacheFile(mipName, mipData);

        t.files.emplace_back(mipName, std::move(mipData), 0);

        mipData.disown();
    }
}

static bool CompressETC12(
    common_tools_t& t,
    CSize const&    size,
    Bytes const&    inputData,
    Path const&     outName)
{
    u32 num_mipmaps = 1;
    {
        i32 tex_size = CMath::min(size.w, t.compress.max_size);

        while(tex_size > t.compress.min_size)
        {
            num_mipmaps++;
            tex_size /= 2;
        }
    }

    stb::image_float local_img =
        stb::ToFloat(stb::image_const::From(inputData, size, 4));

    CompressETC_With(
        t, size, outName, num_mipmaps, local_img, EtcImFormat::ETC1, 1.f);

    auto etc2_format = EtcImFormat::RGB8;

    if(t.compress.channels == 4)
        etc2_format = EtcImFormat::RGBA8;

    CompressETC_With(
        t, size, outName, num_mipmaps, local_img, etc2_format, 1.f);

    return false;
}
#endif
