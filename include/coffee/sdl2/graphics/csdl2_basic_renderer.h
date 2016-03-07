#pragma once

#include <coffee/core/base/renderer/spriteapplication.h>
#include "../csdl2_context.h"

namespace Coffee{
namespace CDisplay{

class SDL2SpriteRenderer : public SpriteApplication,public SDL2ContextUser
{
    struct SDLSpriteContext;

    SDLSpriteContext* m_context;

    // SpriteApplication interface
public:
    SDL2SpriteRenderer(SDL2ContextUser* parentContext):
        SDL2ContextUser(parentContext)
    {
    }

    void init()
    {
        spritesPreInit();
        spritesInit();
        spritesPostInit();
    }
    void terminate()
    {
        spritesTerminate();
    }

    void spritesPreInit();
    void spritesInit();
    void spritesPostInit();
    void spritesTerminate();

    void setClearColor(Renderer const& r, CRGBA const& color);
    void clearBuffer(Renderer const& r);
    void swapBuffers(Renderer const& r);

    Renderer createRenderer();
    void destroyRenderer(Renderer t);

    void createTexture(Renderer r, uint32 c,Texture *t, PixelFormat fmt,
                       ResourceAccess acc, CSize const& size);
    void destroyTexture(uint32 c, Texture* t);

    void createSprite(Texture const&,SpriteSource const&,Sprite*);

    void createSpriteAtlas(Texture const& t,Vector<SpriteSource> const&,
                           uint32&,Vector<Sprite>&);

    bool uploadTexture(Texture tex, const CRect &region, CBitmap const& data);

    void* mapTexture(Texture tex);
    bool unmapTexture(Texture tex);

    void drawSprite(Renderer r, CPointF const& pos,
                    CSizeF const& scale, Sprite const& sprite);
};

}
}