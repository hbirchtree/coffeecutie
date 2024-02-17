#pragma once

#include "cimage.h"
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

namespace Coffee {
namespace TrueType {

struct FontRendererDef
{
    struct FontProperties
    {
        scalar scale;
        int    ascent;
        int    descent;
        int    linegap;
    };

    struct FontData;
};

} // namespace TrueType

namespace StbTruetype {
struct StbFontRenderer : TrueType::FontRendererDef
{
    struct FontData;

    struct DataDeleter
    {
        void operator()(FontData* data);
    };

    using FontPtr = std::unique_ptr<FontData, DataDeleter>;

    static FontPtr LoadFontConfig(Bytes&& p);
    static void    UnloadFontConfig(FontPtr&& d);
    static cstring GetFontName(FontPtr const& d);
    static bool    GetFontProperties(
           FontPtr const& d, scalar h, FontProperties& p);

    static Bytes& RenderText(
        FontPtr const&        font,
        FontProperties const& properties,
        cstring               text,
        Bytes&                output,
        Size&                 imageSize);
};

} // namespace StbTruetype

using FontRenderer = StbTruetype::StbFontRenderer;

} // namespace Coffee
