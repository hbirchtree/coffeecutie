#pragma once

#include "../csdl2_context.h"
#include <coffee/core/base/renderer/spriteapplication.h>
#include <coffee/image/cimage.h>

namespace Coffee {
namespace Display {

class SDL2SpriteRenderer : public SpriteApplication, public SDL2ContextUser
{
    struct SDLSpriteContext;

    SDLSpriteContext* m_context;

    // SpriteApplication interface
  public:
    SDL2SpriteRenderer() : m_context(nullptr)
    {
    }
    SDL2SpriteRenderer(SDL2ContextUser* parentContext) :
        SDL2ContextUser(parentContext), m_context(nullptr)
    {
    }

    bool init(CString* err)
    {
        return spritesPreInit(err) && spritesInit(err) && spritesPostInit(err);
    }
    void cleanup()
    {
        spritesTerminate();
    }

    bool spritesPreInit(CString*);
    bool spritesInit(CString*);
    bool spritesPostInit(CString*);
    void spritesTerminate();

    void setClearColor(Renderer const& r, CRGBA const& color);
    void clearBuffer(Renderer const& r);
    void swapBuffers(Renderer const& r);

    Renderer createRenderer();
    void     destroyRenderer(Renderer t);

    bool createTexture(
        Renderer     r,
        u32       c,
        Texture*     t,
        PixelFormat  fmt,
        RSCA         acc,
        CSize const& size);
    void destroyTexture(u32 c, Texture* t);

    bool createSprite(Texture const&, SpriteSource const&, Sprite*);

    void createSpriteAtlas(
        Texture const& t,
        Vector<SpriteSource> const&,
        u32&,
        Vector<Sprite>&);

    bool uploadTexture(
        Texture tex, const CRect& region, stb::image_rw const& data);

    void* mapTexture(Texture tex);
    bool  unmapTexture(Texture tex);

    void drawSprite(
        Renderer       r,
        CPointF const& pos,
        CSizeF const&  scale,
        Sprite const&  sprite);
};

} // namespace Display
} // namespace Coffee
