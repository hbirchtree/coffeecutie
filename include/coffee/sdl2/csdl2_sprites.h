#pragma once

#include "graphics/csdl2_basic_renderer.h"
#include "csdl2_windowhost.h"

namespace Coffee{
namespace CDisplay{

class SimpleSpriteRenderer :
        public SDL2SpriteRenderer,
        public SDL2WindowHost
{
public:
    SimpleSpriteRenderer(CObject* parent = nullptr):
        SDL2WindowHost(parent)
    {
    }

    void init(CDProperties const& props)
    {
        hostPreInit(props);
        spritesPreInit();

        hostInit(props);
        spritesInit();

        hostPostInit(props);
        spritesPostInit();
    }
    void cleanup()
    {
        spritesTerminate();
        SDL2WindowHost::cleanup();
    }
};

}
}