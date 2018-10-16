#include <coffee/sdl2/graphics/csdl2_basic_renderer.h>

#include "../types/sdl2datatypes.h"
#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>

namespace Coffee {
namespace Display {

struct SDL2SpriteRenderer::SDLSpriteContext
{
    SDLSpriteContext() : renderer_counter(0), texture_counter(0)
    {
    }

    struct TexContext
    {
        SDL_Texture*                 tex;
        SDL2SpriteRenderer::Renderer parent;
    };

    Map<SDL2SpriteRenderer::Renderer, SDL_Renderer*> renderers;
    Map<SDL2SpriteRenderer::Texture, TexContext>     textures;

    u64 renderer_counter;
    u64 texture_counter;
};

bool SDL2SpriteRenderer::spritesPreInit(CString* err)
{
    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
    {
        cTag("SDLSpriteRenderer", "Failed to init: {0}", SDL_GetError());
        if(err)
            *err = cStringFormat("Failed to init: {0}", SDL_GetError());
    }
    Profiler::DeepProfile("Initialize sprite rendering");
    return true;
}

bool SDL2SpriteRenderer::spritesInit(CString*)
{
    m_context = new SDLSpriteContext;
    return true;
}

bool SDL2SpriteRenderer::spritesPostInit(CString*)
{
    return true;
}

void SDL2SpriteRenderer::spritesTerminate()
{
    delete m_context;

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void SDL2SpriteRenderer::setClearColor(const Renderer& r, const rgba_t& color)
{
    SDL_Renderer* ren = m_context->renderers[r];
    SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
}

void SDL2SpriteRenderer::clearBuffer(const Renderer& r)
{
    SDL_Renderer* ren = m_context->renderers[r];
    SDL_RenderClear(ren);
}

void SDL2SpriteRenderer::swapBuffers(const Renderer& r)
{
    SDL_Renderer* ren = m_context->renderers[r];
    SDL_RenderPresent(ren);
}

SpriteApplication::Renderer SDL2SpriteRenderer::createRenderer()
{
    m_context->renderer_counter++;
    auto ctxt     = getSDL2Context();
    auto window   = ctxt->window;
    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
        cDebug("Failed to create renderer: {0}", SDL_GetError());
    m_context->renderers.insert(
        Pair<Renderer, SDL_Renderer*>(m_context->renderer_counter, renderer));
    return m_context->renderer_counter;
}

void SDL2SpriteRenderer::destroyRenderer(Renderer t)
{
    m_context->renderers.erase(t);
}

bool SDL2SpriteRenderer::createTexture(
    Renderer r, u32 c, Texture* t, PixelFormat fmt, RSCA acc, CSize const& size)
{
    Uint32 sdlfmt = 0;
    switch(fmt)
    {
    case PixelFormat::RGBA8:
        sdlfmt = SDL_PIXELFORMAT_ABGR8888;
        break;
    default:
        sdlfmt = SDL_PIXELFORMAT_RGBA8888;
        break;
    }
    Uint32 sdlacc = 0;
    switch(acc)
    {
    case RSCA::Persistent:
    case RSCA::Streaming:
        sdlacc = SDL_TEXTUREACCESS_STREAMING;
        break;
    case RSCA::WriteOnly:
        sdlacc = SDL_TEXTUREACCESS_TARGET;
        break;
    default:
        sdlacc = SDL_TEXTUREACCESS_STATIC;
        break;
    }

    SDL_Renderer* ren = m_context->renderers[r];
    for(u32 i = 0; i < c; i++)
    {
        SDLSpriteContext::TexContext ctxt;
        ctxt.parent = r;
        ctxt.tex    = SDL_CreateTexture(ren, sdlfmt, sdlacc, size.w, size.h);

        if(!ctxt.tex)
        {
            cDebug("Error: {0}", SDL_GetError());
            return false;
        }

        SDL_SetTextureBlendMode(ctxt.tex, SDL_BLENDMODE_BLEND);

        m_context->texture_counter++;

        m_context->textures.insert({m_context->texture_counter, ctxt});
        t[i] = m_context->texture_counter;
    }
    return true;
}

void SDL2SpriteRenderer::destroyTexture(u32 c, SpriteApplication::Texture* t)
{
    for(u32 i = 0; i < c; i++)
    {
        SDLSpriteContext::TexContext tex = m_context->textures[t[i]];
        m_context->textures.erase(t[i]);
        SDL_DestroyTexture(tex.tex);
    }
}

void SDL2SpriteRenderer::createSpriteAtlas(
    C_UNUSED(const SpriteApplication::Texture& t),
    const Vector<SpriteApplication::SpriteSource>&,
    u32&,
    Vector<SDL2SpriteRenderer::Sprite>&)
{
}

bool SDL2SpriteRenderer::createSprite(
    const Texture& t, const SpriteSource& r, Sprite* sprite)
{
    Uint32 fmt;
    int    acc;
    int    w, h;
    SDL_QueryTexture(m_context->textures[t].tex, &fmt, &acc, &w, &h);

    if(r.x + r.w > w || r.y + r.h > h)
        return false;

    sprite->rect   = r;
    sprite->source = t;

    return true;
}

bool SDL2SpriteRenderer::uploadTexture(
    Texture tex, CRect const& region, const stb::image_rw& data)
{
    SDL_Rect sec;
    sec.x = region.x;
    sec.y = region.y;
    sec.w = region.w;
    sec.h = region.h;

    return SDL_UpdateTexture(
               m_context->textures[tex].tex,
               &sec,
               data.data,
               (data.size.w) * sizeof(rgba_t)) == 0;
}

void* SDL2SpriteRenderer::mapTexture(Texture tex)
{
    void*        ptr       = nullptr;
    int          pitch     = 0;
    SDL_Texture* texhandle = m_context->textures[tex].tex;
    SDL_LockTexture(texhandle, nullptr, &ptr, &pitch);
    if(!ptr)
        cDebug("Error: {0}", SDL_GetError());
    return ptr;
}

bool SDL2SpriteRenderer::unmapTexture(SpriteApplication::Texture tex)
{
    SDL_UnlockTexture(m_context->textures[tex].tex);
    return true;
}

void SDL2SpriteRenderer::drawSprite(
    Renderer r, const CPointF& pos, const CSizeF& scale, const Sprite& sprite)
{
    SDL_Rect src;
    src.w = sprite.rect.w;
    src.h = sprite.rect.h;
    src.x = sprite.rect.x;
    src.y = sprite.rect.y;

    SDL_Rect dst;
    dst.w = src.w * scale.w;
    dst.h = src.h * scale.h;
    dst.x = pos.x;
    dst.y = pos.y;

    SDL_Renderer* ren = m_context->renderers[r];
    SDL_Texture*  tex = m_context->textures[sprite.source].tex;
    SDL_RenderCopyEx(ren, tex, &src, &dst, 0, nullptr, SDL_FLIP_NONE);
}

} // namespace Display
} // namespace Coffee
