#pragma once

#include "../types/cdisplay.h"

namespace Coffee{
namespace CDisplay{

class SpriteApplication
{
public:

    /* Types */
    using SpriteSource = CRect;

    struct Renderer
    {
    };
    struct Texture
    {
    };

    struct Sprite
    {
    };

    /* Initialization */
    virtual void spritesPreInit() = 0;
    virtual void spritesInit() = 0;
    virtual void spritesPostInit() = 0;

    virtual void spritesTerminate() = 0;

    /* Rendering */
    virtual void swapBuffers(Renderer* = nullptr) = 0;

    virtual Renderer* createRenderer() = 0;
    virtual void destroyRenderer(Renderer*) = 0;

    virtual void createTexture(int32,Texture**) = 0;
    virtual void destroyTexture(int32,Texture*) = 0;

    virtual Sprite* createSprite(Texture const*,SpriteSource const&) = 0;

    virtual Vector<Sprite*> createSpriteAtlas(Texture const*,Vector<SpriteSource> const&) = 0;
};

}
}