#pragma once

#include <coffee/core/datastorage/binary/virtualfs.h>
#include <squish.h>

#include "texture_common.h"

using namespace Coffee;

static void CompressDXT(
    common_tools_t&                      t,
    Pair<CString, ImageProcessor> const& file,
    CSize const&                         size,
    Bytes const&                         inputData,
    Path const&                          outName)
{
    bool    fail   = false;
    cstring reason = nullptr;

    if(size.w != size.h)
    {
        fail   = true;
        reason = "{1}:0: Non-square texture is non-optimal: {0}";
    }

    if((size.w % 4) != 0 || (size.h % 4) != 0)
    {
        fail   = true;
        reason = "{1}:0: Inadequate size for S3TC texture: {0}";
    }

    if(fail)
    {
        auto offender = Path(GetFileResourcePrefix()) + file.first;
        t.cursor.print(reason, size, offender.internUrl);
        return;
    }

    Bytes data = Bytes();
    data.data  = inputData.data;
    data.size  = inputData.size;

    auto imsize  = size;
    auto newSize = imsize;
    while(newSize.w >= t.compress.min_size && newSize.h >= t.compress.min_size)
    {
        /* The step will generate a bitmap of size newSize,
         *  imsize is source size */

        auto ext             = "dxt5";
        auto compression_fmt = squish::kDxt5;

        if(t.compress.channels == 3)
        {
            ext             = "dxt1";
            compression_fmt = squish::kDxt1;
        }

        auto sizeString = cast_pod(newSize.w);
        auto targetImg =
            outName.addExtension(sizeString.c_str()).addExtension(ext);
        auto mipImg = outName.addExtension(sizeString.c_str());

        if(t.compress.channels == 3)
            mipImg = mipImg.addExtension("jpg");
        else
            mipImg = mipImg.addExtension("png");

        if(newSize.w > t.compress.max_size || newSize.h > t.compress.max_size)
        {
            t.cursor.progress(
                TEXCOMPRESS_API "Skipping texture, over max size: {0}",
                targetImg.internUrl);
            imsize = newSize;
        } else if(t.cooker->isCached(targetImg) && t.cooker->isCached(mipImg))
        {
            t.cursor.progress(
                TEXCOMPRESS_API "Using cache texture: {0}",
                targetImg.internUrl);
            t.files.emplace_back(targetImg, t.cooker->getCached(targetImg), 0);
            t.files.emplace_back(mipImg, t.cooker->getCached(mipImg), 0);

            imsize = newSize;
        } else
        {
            if(imsize.w != newSize.w)
            {
                auto newData = stb::Resize(
                    stb::CStbImage::From(inputData, size), newSize, 4);
                data = std::move(newData.data_owner);
                newData.disown();
                imsize = newSize;
            }

            t.files.emplace_back(targetImg, Bytes(), 0);
            t.files.emplace_back(mipImg, Bytes(), 0);

            Bytes& output    = t.files.at(t.files.size() - 2).data;
            Bytes& pngOutput = t.files.at(t.files.size() - 1).data;

            /* Allocate space for image as well as WxH parameters */
            output = Bytes::Alloc(
                C_FCAST<szptr>(squish::GetStorageRequirements(
                    imsize.w, imsize.h, compression_fmt)) +
                sizeof(IMG::serial_image));

            t.cursor.progress(
                TEXCOMPRESS_API "Compressing texture: "
                                "{1}: {2}B (raw)"
                                " -> {0}B (compressed)",
                output.size,
                targetImg.internUrl,
                data.size);
            squish::CompressImage(
                data.data,
                imsize.w,
                imsize.h,
                &output[sizeof(IMG::serial_image)],
                compression_fmt);

            IMG::serial_image imgDesc = {};
            imgDesc.size              = imsize.convert<u32>();
            imgDesc.fmt               = PixFmt::S3TC;
            imgDesc.bit_fmt           = BitFmt::Byte;
            imgDesc.comp_fmt          = compression_fmt == squish::kDxt5
                                   ? CompFlags::S3TC_5
                                   : CompFlags::S3TC_1;

            MemCpy(Bytes::Create(imgDesc), output);

            /* Cache DXT-compressed data */
            t.cooker->cacheFile(targetImg, output);

            if(t.compress.channels == 3)
                pngOutput = JPG::Save(stb::image_const::From(data, imsize));
            else
                pngOutput = PNG::Save(stb::image_const::From(data, imsize));

            /* Cache regular image format */
            t.cooker->cacheFile(mipImg, pngOutput);
        }

        newSize = imsize / 2;
    }
}
