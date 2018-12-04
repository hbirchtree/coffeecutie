#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/pixel_format.h>
#include <coffee/core/types/rect.h>
#include <coffee/core/types/rsca.h>
#include <coffee/core/types/size.h>

namespace Coffee {
namespace Display {

class SpriteApplication
{
  public:
    /* Types */
    using SpriteSource = Rect;

    /*! In-memory texture data, user is given handle */
    using Texture = u64;
    /*! Renderer handle */
    using Renderer = u64;

    /*! User-modifiable sprite located in texture */
    struct Sprite
    {
        SpriteSource rect;
        Texture      source;
    };

    /* Initialization */
    virtual bool spritesPreInit(CString*)  = 0;
    virtual bool spritesInit(CString*)     = 0;
    virtual bool spritesPostInit(CString*) = 0;

    virtual void spritesTerminate() = 0;

    /* Rendering */
    virtual void swapBuffers(Renderer const&) = 0;

    virtual Renderer createRenderer()            = 0;
    virtual void     destroyRenderer(Renderer t) = 0;

    virtual bool createTexture(
        Renderer    r,
        u32         c,
        Texture*    t,
        PixFmt      fmt,
        RSCA        acc,
        Size const& size)                      = 0;
    virtual void destroyTexture(u32, Texture*) = 0;

    virtual bool createSprite(Texture const&, SpriteSource const&, Sprite*) = 0;

    virtual void createSpriteAtlas(
        Texture const&, Vector<SpriteSource> const&, u32&, Vector<Sprite>&) = 0;
};

} // namespace Display
} // namespace Coffee