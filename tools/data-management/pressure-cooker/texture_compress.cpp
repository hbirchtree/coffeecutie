#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/CFiles>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/CDebug>
#include <coffee/image/cimage.h>

#include <squish.h>

#define TEXCOMPRESS_API "TextureCooker::"

using namespace CoffeePipeline;

static Vector<CString> imageExtensions = {
    "PNG", "JPG", "TGA", "JPEG"
};

enum ImageProcessor
{
    ImageProc_stb,
    ImageProc_tga,
};

struct TextureCooker : FileProcessor
{
    virtual void process(Vector<VirtFS::VirtDesc> &files);
    virtual void receiveAssetPath(const CString &assetPath);
};


void TextureCooker::process(Vector<VirtFS::VirtDesc> &files)
{
    Map<CString, ImageProcessor> targets;

    for(auto& desc : files)
    {
        auto path = Path(desc.filename);

        for(auto ext : imageExtensions)
            if(StrICmp(path.extension().c_str(), ext.c_str()))
            {
                bool is_tga = ext == "TGA";
                targets[desc.filename] = (is_tga)
                        ? ImageProc_tga : ImageProc_stb;
                continue;
            }
    }

    cDebug(TEXCOMPRESS_API "Compressable textures found: {0}",
           targets.size());

    for(auto file : targets)
    {
        CResources::Resource r(MkUrl(file.first.c_str()));

        PixCmp cmp = PixCmp::RGBA;
        BitFmt bfmt;
        Bytes data;
        CSize size;
        IMG::Load(std::move(r), cmp, bfmt, data, size);

        if(size.area() == 0)
            continue;

        auto outName = Path(file.first).removeExt().addExtension("dx5");
        files.emplace_back(
                    outName.internUrl.c_str(),
                    Bytes(),
                    0
                    );
        Bytes& output = files.back().data;
        output.size = C_FCAST<szptr>(
                    squish::GetStorageRequirements(
                        size.w, size.h, squish::kDxt5));
        output.data = C_RCAST<byte_t*>(Calloc(output.size, 1));
        Bytes::SetDestr(output, [](Bytes& b)
        {
            CFree(b.data);
        });

        squish::CompressImage(data.data, size.w, size.h,
                              output.data, squish::kDxt5);

        cDebug(TEXCOMPRESS_API "Compressed texture: {2} {0} -> {1}",
               r.size, output.size, file.first);

        files.erase(std::find_if(files.begin(), files.end(),
                                   [&](VirtFS::VirtDesc& f)
        {
                        return f.filename == file.first;
        }));
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
