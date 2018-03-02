#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/CFiles>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/CDebug>
#include <coffee/core/unit_tests/graphics_framework.h>
#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/image/cimage.h>
#include <coffee/graphics/apis/CGLeam>

#define STB_DXT_IMPLEMENTATION
#define STB_DXT_STATIC
#include <stb_dxt.h>

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

using GLM = Coffee::RHI::GLEAM::GLEAM_API;

struct TextureCooker : FileProcessor
{
    virtual void process(Vector<VirtFS::VirtDesc> &files);
    virtual void receiveAssetPath(const CString &assetPath);
};


void TextureCooker::process(Vector<VirtFS::VirtDesc> &files)
{
    CoffeeTest::windowing_wrap([&](Coffee::Display::RendererInterface& r,
                               CoffeeTest::Empty*)
    {
        r.closeWindow();

        auto api = GLM::GetLoadAPI();

        if(!api(true))
            return;

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

            GLM::S_2D surface(PixFmt::RGBA8, 1);
            surface.allocate(size, cmp);
            surface.upload(bfmt, cmp, size, data);

            CGL::CGL33::TexBind(
                        CGL::Texture::T2D, surface.glTexHandle());
            szptr bsize = 0;
            u32 w = 0, h = 0, d = 0;
            CGL::CGL33::TexGetCompressedSize(
                        CGL::Texture::T2D, w, h, d, bsize);
            Vector<byte_t> cstore;
            cstore.resize(bsize);
            Bytes cdata = Bytes::CreateFrom(cstore);
            CGL::CGL33::TexGetCompressedImage(
                        CGL::Texture::T2D, 0, cdata);

            cDebug(TEXCOMPRESS_API "Texture data size: {0} ({1}x{2}x{3})",
                   cdata.size, w, h, d);

            files.erase(std::remove_if(files.begin(), files.end(),
                                       [&file](VirtFS::VirtDesc& f)
            {
                return f.filename == file.first;
            }));
        }

        r.pollEvents();

        GLM::UnloadAPI();
    });
}

void TextureCooker::receiveAssetPath(const CString &assetPath)
{
    FileResourcePrefix(assetPath.c_str());
}

COFFAPI FileProcessor* CoffeeLoader()
{
    return new TextureCooker;
}
