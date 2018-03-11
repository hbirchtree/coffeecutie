#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/CFiles>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/CDebug>
#include <coffee/image/cimage.h>
#include <coffee/core/terminal/terminal_cursor.h>

#include <squish.h>

#define TEXCOMPRESS_API "TextureCooker::"

using namespace CoffeePipeline;

static Vector<CString> imageExtensions = {
    "PNG", "JPG", "TGA", "JPEG"
};

enum ImageProcessor
{
    ImageProc_stb,
    ImageProc_stb_rgb,
    ImageProc_tga,
};

struct TextureCooker : FileProcessor
{
    virtual void process(Vector<VirtFS::VirtDesc> &files,
                         TerminalCursor& cursor);
    virtual void receiveAssetPath(const CString &assetPath);
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
        IMG::Load(std::move(r), cmp, bfmt, data, size);

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
                + sizeof(u32) * 2;
        output.data = C_RCAST<byte_t*>(Calloc(output.size, 1));
//        Bytes::SetDestr(output, [](Bytes& b)
//        {
//            CFree(b.data);
//        });

        squish::CompressImage(
                    data.data, size.w, size.h,
                    &output[sizeof(u32) * 2],
                compress);

        u32* sizeParam = C_RCAST<u32*>(output.data);
        sizeParam[0] = C_FCAST<u32>(size.w);
        sizeParam[1] = C_FCAST<u32>(size.h);

        cursor.progress(TEXCOMPRESS_API "Compressed texture: "
                                        "{2}: {0}B (file)"
                                        " -> {3}B (raw)"
                                        " -> {1}B (compressed)",
                        r.size, output.size, file.first, data.size);
    }
}

void TextureCooker::receiveAssetPath(const CString &assetPath)
{
    FileResourcePrefix(assetPath.c_str());
}

COFFAPI FileProcessor* CoffeeLoader()
{
    return new TextureCooker;
}
