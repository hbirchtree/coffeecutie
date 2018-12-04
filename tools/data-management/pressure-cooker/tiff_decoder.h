#pragma once

#include "texture_common.h"

#if defined(HAVE_LIBTIFF)
#include <tiffio.h>

namespace TexCompress {

bool TiffDecode(
    FileProcessor*                       cooker,
    Pair<CString, ImageProcessor> const& file,
    PixCmp                               cmp,
    BitFmt&                              bfmt,
    Size&                                size,
    Bytes&                               data,
    Resource&                            r)
{
    do
    {
        auto pngPath = Path(file.first).removeExt().addExtension("png");

        if(cooker->isCached(pngPath))
        {
            IMG::Load(cooker->getCached(pngPath), cmp, bfmt, data, size);
            break;
        }

        /* libtiff is used for reading TIFF files, because it's a
         *  not part of stb. We use this opportunity to convert
         *  it into PNG format for packing. */
        auto timg = TIFFOpen(r.resource(), "r");

        if(!timg)
            return false;

        TIFFRGBAImage rimg;
        char          emsg[1024] = {};

        if(!TIFFRGBAImageBegin(&rimg, timg, 0, emsg))
        {
            TIFFClose(timg);
            return false;
        }

        ::uint32 w, h;
        TIFFGetField(timg, TIFFTAG_IMAGEWIDTH, &w);
        TIFFGetField(timg, TIFFTAG_IMAGELENGTH, &h);

        data = Bytes::Alloc(w * h * sizeof(u32));

        if(!TIFFRGBAImageGet(&rimg, C_RCAST<u32*>(data.data), w, h))
        {
            TIFFRGBAImageEnd(&rimg);
            TIFFClose(timg);
            return false;
        }

        size.w = C_FCAST<i32>(w);
        size.h = C_FCAST<i32>(h);
        bfmt   = BitFmt::UByte;

        /* This is where we create the PNG file */
        auto pngData = PNG::Save(stb::image_const::From(data, size));
        cooker->cacheFile(pngPath, pngData);

        TIFFRGBAImageEnd(&rimg);
        TIFFClose(timg);
    } while(false);

    return true;
}

} // namespace TexCompress

#endif
