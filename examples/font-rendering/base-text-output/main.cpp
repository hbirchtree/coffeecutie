#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/image/cimage.h>
#include <coffee/image/ctextrender.h>

#include <coffee/core/CDebug>

using namespace Coffee;

i32 rendering_test(i32, cstring_w*)
{
    using namespace TrueType;

    auto fontData = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"_sysfile;

    Bytes data = fontData;

    auto config = FontRenderer::LoadFontConfig(fontData);

    FontRenderer::FontProperties props;
    if(!FontRenderer::GetFontProperties(config, 64, props))
        return 1;

    Bytes textData = {};
    Size  textSize = {};

    {
        ProfContext _("Render text");
        FontRenderer::RenderText(config, props, "test", textData, textSize);
    }

    cDebug("Font name: {0}", FontRenderer::GetFontName(config));

    {
        ProfContext _("Saving image");
        auto        img = stb::image_const::From(textData, textSize, 1);

        auto outFile = "test.png"_sysfile;
        auto pngImg  = PNG::Save(img);
        outFile      = pngImg;

        {
            ProfContext _("Filesystem");
            if(!FileCommit(outFile))
                return 1;
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(rendering_test)
