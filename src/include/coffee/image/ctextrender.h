#ifndef COFFEE_IMAGE_TRUETYPE_STB_H
#define COFFEE_IMAGE_TRUETYPE_STB_H

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{
#if !defined(COFFEE_NACL)
namespace TrueType{

struct FontRendererDef
{
    struct FontProperties
    {
        scalar scale;
        int32 ascent;
        int32 descent;
        int32 linegap;
    };

    struct FontData;

    static FontData* LoadFontConfig(Bytes&){return nullptr;}
    static void UnloadFontConfig(FontData*){}
    static cstring GetFontName(FontData*){return nullptr;}
    static bool GetFontProperties(FontData*,scalar,FontProperties*){return false;}

    static bool CalcTextSize(FontData*,FontProperties const&,cstring,CRect*,uint32*){return false;}
    static bool RenderText(FontData*,FontProperties const&,cstring){return false;}
};

}
namespace StbTruetype
{
struct StbFontRenderer : TrueType::FontRendererDef
{
    struct FontData;

    static FontData* LoadFontConfig(Bytes& p);
    static void UnloadFontConfig(FontData* d);
    static cstring GetFontName(FontData* d);
    static bool GetFontProperties(FontData* d, scalar h, FontProperties* p);

    static bool CalcTextSize(FontData* d,FontProperties const& p,cstring t,CRect* b,uint32* s);
    static bool RenderText(FontData* d,FontProperties const& p,cstring t);
};

}

using FontRenderer = StbTruetype::StbFontRenderer;
#endif

}

#endif
