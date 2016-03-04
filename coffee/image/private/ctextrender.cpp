#include <coffee/image/ctextrender.h>

#include <coffee/core/plat/plat_memory.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

namespace Coffee{
namespace StbTruetype{

struct StbFontRenderer::FontData
{
    stbtt_fontinfo info;
};

StbFontRenderer::FontData *StbFontRenderer::LoadFontConfig(CByteData &p)
{
    FontData* data = new FontData;
    if(!stbtt_InitFont(&data->info,p.data,0))
    {
        delete data;
        return nullptr;
    }
    return data;
}

void StbFontRenderer::UnloadFontConfig(StbFontRenderer::FontData *d)
{
    delete d;
}

cstring StbFontRenderer::GetFontName(StbFontRenderer::FontData *d)
{
    /*TODO: Fix name fetching for Stb Truetype*/
    int len;
    return stbtt_GetFontNameString(&d->info,&len,1,1,1,1);
}

bool StbFontRenderer::GetFontProperties(FontData *d, scalar h, FontProperties *p)
{
    p->scale = stbtt_ScaleForPixelHeight(&d->info,h);
    stbtt_GetFontVMetrics(&d->info,&p->ascent,&p->descent,&p->linegap);
    p->ascent *= p->scale;
    p->descent *= p->scale;
    return true;
}

/*Reference: https://github.com/justinmeiners/stb-truetype-example/blob/master/source/main.c */

bool StbFontRenderer::CalcTextSize(FontData *d, const FontProperties &p,
                                   cstring t, CRect *b, uint32 *s)
{
    uint32 max = StrLen(t);
    int32 x = 0;
    for(uint32 i=0;i<max;i++)
    {
        CRect box;
        stbtt_GetCodepointBitmapBox(&d->info,t[i],p.scale,p.scale,
                                    &box.x,&box.y,&box.w,&box.h);

        int ax;
        stbtt_GetCodepointHMetrics(&d->info,t[i],&ax,0);

        x += ax*p.scale;

        int kern = stbtt_GetCodepointKernAdvance(&d->info,t[i],t[i+1]);

        x += kern*p.scale;

        box.w = box.w-box.x;
        box.h = box.h-box.y;

        box.x += x;
        box.y = p.ascent+box.y;

        *b = b->unite(box);
    }
    return true;
}

bool StbFontRenderer::RenderText(FontData *d, const FontProperties &p, cstring t)
{
    ubyte_t* data = (ubyte_t*)Alloc(512*128);
    int32 x = 0;
    for(uint32 i=0;i<StrLen(t);i++)
    {

    }
    return false;
}

}
}