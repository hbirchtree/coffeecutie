#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/image/ctextrender.h>
#include <coffee/image/cimage.h>

#include <coffee/core/CDebug>

using namespace Coffee;

i32 rendering_test(i32 argc,cstring_w* argv)
{
    using namespace TrueType;

    auto fontData = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"_sysfile;

    Bytes data = fontData;

    auto config = FontRenderer::LoadFontConfig(fontData);

    FontRenderer::FontProperties props;
    if(!FontRenderer::GetFontProperties(config, 64, props))
        return 1;

    Bytes textData = {};
    FontRenderer::RenderText(config, props, "test", textData);

    cDebug("Font name: {0}", FontRenderer::GetFontName(config));

    return 0;
}

COFFEE_APPLICATION_MAIN(rendering_test)
