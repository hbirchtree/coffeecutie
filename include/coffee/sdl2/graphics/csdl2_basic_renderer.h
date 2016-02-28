#pragma once

#include <coffee/core/base/renderer/spriteapplication.h>
#include "../csdl2_context.h"

namespace Coffee{
namespace CDisplay{

class SDL2SpriteRenderer : public SpriteApplication,public SDL2ContextUser
{
    // SpriteApplication interface
public:
    void spritesPreInit();
    void spritesInit();
    void spritesPostInit();
    void spritesTerminate();

    void swapBuffers();

    void createSprites(int32, Sprite **);
    void createSpriteAtlas(const CBitmap &, const Vector<SpriteSource> &);
};

}
}