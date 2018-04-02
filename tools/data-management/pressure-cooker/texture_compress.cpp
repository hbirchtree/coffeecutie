#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/CFiles>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/CDebug>
#include <coffee/image/cimage.h>
#include <coffee/core/terminal/terminal_cursor.h>

#include <squish.h>


#if defined(HAVE_LIBTIFF)
#include <tiffio.h>
#endif

#define TEXCOMPRESS_API "TextureCooker::"

using namespace CoffeePipeline;

static Vector<CString> imageExtensions = {
    "PNG", "JPG", "TGA", "JPEG",
    #if defined(HAVE_LIBTIFF)
    "TIF", "TIFF"
    #endif
};

enum ImageProcessor
{
    ImageProc_stb,
    ImageProc_stb_rgb,
    ImageProc_tga,
#if defined(HAVE_LIBTIFF)
    ImageProc_tiff,
#endif
};

struct TextureCooker : FileProcessor
{
    virtual void process(Vector<VirtFS::VirtDesc> &files,
                         TerminalCursor& cursor);

    virtual void setBaseDirectories(const Vector<CString> &);
};

cstring compression_extension(u32 format)
{
    switch(format)
    {
    case squish::kDxt1:
        return "dxt1";
    default:
        return "dxt5";
    }
}

void TextureCooker::process(Vector<VirtFS::VirtDesc> &files,
                            TerminalCursor& cursor)
{
    Map<CString, ImageProcessor> targets;

    for(auto& desc : files)
    {
        auto path = Path(desc.filename);

        for(auto ext : imageExtensions)
            if(StrICmp(path.extension().c_str(), ext.c_str()))
            {
                if(ext == "TGA")
                    targets[desc.filename] = ImageProc_tga;
                else if(ext == "JPG")
                    targets[desc.filename] = ImageProc_stb_rgb;
#if defined(HAVE_LIBTIFF)
                else if(ext == "TIFF"  || ext == "TIF")
                    targets[desc.filename] = ImageProc_tiff;
#endif
                else
                    targets[desc.filename] = ImageProc_stb;

                continue;
            }
    }

    cursor.progress(TEXCOMPRESS_API "Compressable textures found: {0}",
                    targets.size());

    for(auto file : targets)
    {
        CResources::Resource r(MkUrl(file.first.c_str()));

        PixCmp cmp = PixCmp::RGBA;
        BitFmt bfmt;
        Bytes data;
        CSize size;
#if defined(HAVE_LIBTIFF)
        if(file.second != ImageProc_tiff)
#endif
            IMG::Load(std::move(r), cmp, bfmt, data, size);
#if defined(HAVE_LIBTIFF)
        else
        {
            /* libtiff is used for reading TIFF files, because it's a
             *  not part of stb. We use this opportunity to convert
             *  it into PNG format for packing. */
            auto timg = TIFFOpen(r.resource(), "r");

            if(!timg)
                continue;

            TIFFRGBAImage rimg;
            char emsg[1024] = {};

            if(!TIFFRGBAImageBegin(&rimg, timg, 0, emsg))
            {
                TIFFClose(timg);
                continue;
            }

            ::uint32 w, h;
            TIFFGetField(timg, TIFFTAG_IMAGEWIDTH, &w);
            TIFFGetField(timg, TIFFTAG_IMAGELENGTH, &h);

            data.size = w * h * sizeof(u32);
            data.data = CallocT<byte_t>(data.size, 1);

            Bytes::SetDestr(data, [](Bytes& d)
            {
                CFree(d.data);
            });

            if(!TIFFRGBAImageGet(&rimg, C_RCAST<u32*>(data.data), w, h))
            {
                TIFFRGBAImageEnd(&rimg);
                TIFFClose(timg);
                continue;
            }

            size.w = C_FCAST<i32>(w);
            size.h = C_FCAST<i32>(h);

            /* This is where we create the PNG file */
            auto pngPath = Path(file.first).removeExt()
                    .addExtension("png");
            files.emplace_back(
                        pngPath.internUrl.c_str(),
                        PNG::Save(data, size),
                        0
                        );

            TIFFRGBAImageEnd(&rimg);
            TIFFClose(timg);
        }
#endif

        /* IMG::Load may fail silently, just ignore it */
        if(size.area() == 0)
            continue;

        if((size.w % 4) != 0 || (size.h % 4) != 0)
            cursor.print(
                        TEXCOMPRESS_API
                        "Inadequate size for S3TC texture: {0}", size);

        auto compress = squish::kDxt5;

        /* Depending on color channels/source format, we opt for DXT1
         *  for storage efficiency. Especially when the image does
         *  not have alpha. */
        if(file.second == ImageProc_stb_rgb)
            compress = squish::kDxt1;

        auto outName = Path(file.first)
                .removeExt()
                .addExtension(compression_extension(compress));

        files.emplace_back(
                    outName.internUrl.c_str(),
                    Bytes(),
                    0
                    );
        Bytes& output = files.back().data;
        /* Allocate space for image as well as WxH parameters */
        output.size =
                C_FCAST<szptr>(
                    squish::GetStorageRequirements(
                        size.w, size.h, compress))
                + sizeof(IMG::serial_image);
        output.data = C_RCAST<byte_t*>(Calloc(output.size, 1));
        Bytes::SetDestr(output, [](Bytes& b)
        {
            CFree(b.data);
        });

        squish::CompressImage(
                    data.data, size.w, size.h,
                    &output[sizeof(IMG::serial_image)],
                compress);

        IMG::serial_image imgDesc = {};
        imgDesc.size = size.convert<u32>();
        imgDesc.fmt = PixFmt::S3TC;
        imgDesc.bit_fmt = BitFmt::Byte;
        imgDesc.comp_fmt = (compress == squish::kDxt5)
                ? CompFlags::S3TC_5
                : CompFlags::S3TC_1;

        MemCpy(output.data, &imgDesc, sizeof(IMG::serial_image));

        cursor.progress(TEXCOMPRESS_API "Compressed texture: "
                                        "{2}: {0}B (file)"
                                        " -> {3}B (raw)"
                                        " -> {1}B (compressed)",
                        r.size, output.size, file.first, data.size);
    }

#if defined(HAVE_LIBTIFF)
    auto removePred = [&](VirtFS::VirtDesc const& desc)
    {
        return targets[desc.filename] == ImageProc_tiff;
    };

    auto removeIt = std::remove_if(files.begin(), files.end(),
                                   removePred);

    for(auto it = removeIt; it != files.end(); it++)
        it->data = Bytes();

    files.erase(removeIt, files.end());
#endif
}

void TextureCooker::setBaseDirectories(const Vector<CString> &)
{
}

COFFAPI FileProcessor* CoffeeLoader()
{
    return new TextureCooker;
}
