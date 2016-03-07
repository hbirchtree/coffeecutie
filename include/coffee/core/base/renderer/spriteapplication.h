#pragma once

#include "../types/cdisplay.h"

namespace Coffee{
namespace CDisplay{

class SpriteApplication
{
public:

    /* Types */
    using SpriteSource = CRect;

    /*! In-memory texture data, user is given handle */
    using Texture = uint64;
    /*! Renderer handle */
    using Renderer = uint64;

    /*! User-modifiable sprite located in texture */
    struct Sprite
    {
        SpriteSource rect;
        Texture source;
    };

    /* Initialization */
    virtual void spritesPreInit() = 0;
    virtual void spritesInit() = 0;
    virtual void spritesPostInit() = 0;

    virtual void spritesTerminate() = 0;

    /* Rendering */
    virtual void swapBuffers(Renderer const&) = 0;

    virtual Renderer createRenderer() = 0;
    virtual void destroyRenderer(Renderer t) = 0;

    virtual void createTexture(Renderer r, uint32 c,Texture *t, PixelFormat fmt,
                               ResourceAccess acc, CSize const& size) = 0;
    virtual void destroyTexture(uint32,Texture*) = 0;

    virtual void createSprite(Texture const&,SpriteSource const&,Sprite*) = 0;

    virtual void createSpriteAtlas(Texture const&,Vector<SpriteSource> const&,
                                   uint32&,Vector<Sprite>&) = 0;
};

}
}