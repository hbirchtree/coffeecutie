#pragma once

#include "graphics/csdl2_basic_renderer.h"

#if defined(COFFEE_USE_SDL_WINDOW)
#include "csdl2_windowhost.h"

namespace Coffee{
namespace Display{

class SimpleSpriteRenderer :
        public SDL2SpriteRenderer,
        public SDL2WindowHost
{
public:
    SimpleSpriteRenderer(CObject* parent = nullptr):
        SDL2WindowHost(parent)
    {
    }

    bool init(CDProperties const& props,CString* err)
    {
        return
        hostPreInit(props,err) &&
        spritesPreInit(err) &&

        hostInit(props,err) &&
        spritesInit(err) &&

        hostPostInit(props,err) &&
        spritesPostInit(err);
    }
    void cleanup()
    {
        spritesTerminate();
        SDL2WindowHost::cleanup();
    }
};

}
}
#endif
