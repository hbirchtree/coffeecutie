#include <coffee/image/ctextrender.h>

#include <coffee/core/types/cdef/memsafe.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace Coffee{
namespace StbTruetype{

struct StbFontRenderer::FontData
{
    stbtt_fontinfo info;
};

StbFontRenderer::FontPtr StbFontRenderer::LoadFontConfig(Bytes &&p)
{
    if(!p)
        return FontPtr();

    auto data = MkUqDST<FontData, DataDeleter>();

    if(!stbtt_InitFont(&data->info,p.data,0))
        return FontPtr();

    return data;
}

void StbFontRenderer::UnloadFontConfig(FontPtr &&d)
{
    d = {};
}

cstring StbFontRenderer::GetFontName(const StbFontRenderer::FontPtr &d)
{
    if(!d)
        return nullptr;

    int len = 0;

    return stbtt_GetFontNameString(
                &d->info,
                &len,
                STBTT_PLATFORM_ID_UNICODE,
                STBTT_UNICODE_EID_UNICODE_1_0,
                0,
                0);
}

bool StbFontRenderer::GetFontProperties(
        const FontPtr &d,
        scalar h,
        FontProperties &p)
{
    if(!d)
        return false;

    p.scale = stbtt_ScaleForPixelHeight(&d->info, h);
    stbtt_GetFontVMetrics(&d->info, &p.ascent, &p.descent, &p.linegap);

    p.ascent *= p.scale;
    p.descent *= p.scale;

    return true;
}

bool StbFontRenderer::CalcTextSize(
        const FontPtr &d,
        const FontProperties &p,
        cstring t,
        CRect &b,
        uint32 &s)
{

}

Bytes& StbFontRenderer::RenderText(
        const FontPtr &data,
        const FontProperties &properties,
        cstring text,
        Bytes& output)
{
    auto stb_data = &data->info;
    auto scale = properties.scale;

    using irect = _cbasic_rect<int>;

    output = Bytes::Alloc(256 * 128);

    Bytes stringData = Bytes::CreateString(text);

    int x = 0;
    auto shadowChar = stringData.begin();

    for(auto c : stringData)
    {
        ++shadowChar;

        irect bbox = {};
        stbtt_GetCodepointBitmapBox(
                    stb_data, c, scale, scale,
                    &bbox.x, &bbox.y, &bbox.w, &bbox.h);


        int y = properties.ascent + bbox.y, ax = 0, kern = 0;

//        bbox.w -= bbox.x;
//        bbox.h -= bbox.y;

        stbtt_MakeCodepointBitmap(
                    stb_data, &output[x + (y * 256)],
                bbox.w, bbox.h, 256, scale, scale, c);

        stbtt_GetCodepointHMetrics(stb_data, c, &ax, nullptr);
        kern = stbtt_GetCodepointKernAdvance(stb_data, c, *shadowChar);

        x += scale * (ax + kern);
    }

    return output;
}

void StbFontRenderer::DataDeleter::operator()(FontData *data)
{
    delete data;
}

}
}
