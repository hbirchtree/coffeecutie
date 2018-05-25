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
        int ascent;
        int descent;
        int linegap;
    };

    struct FontData;
};

}
namespace StbTruetype
{
struct StbFontRenderer : TrueType::FontRendererDef
{
    struct FontData;

    struct DataDeleter
    {
        void operator()(FontData* data);
    };

    using FontPtr = UqPtr<FontData, DataDeleter>;

    static FontPtr LoadFontConfig(Bytes&& p);
    static void UnloadFontConfig(FontPtr&& d);
    static cstring GetFontName(FontPtr const& d);
    static bool GetFontProperties(FontPtr const& d, scalar h, FontProperties &p);

    static bool CalcTextSize(
            FontPtr const& d,
            FontProperties const& p,
            cstring t,
            CRect& b,
            uint32& s);
    static Bytes& RenderText(
            FontPtr const& font,
            FontProperties const& properties,
            cstring text,
            Bytes& output);
};

}

using FontRenderer = StbTruetype::StbFontRenderer;
#endif

}

#endif
