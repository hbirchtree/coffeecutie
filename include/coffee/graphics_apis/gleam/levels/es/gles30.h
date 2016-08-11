#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_ES30_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_ES30_H

#include "gles20.h"

/* Basic */
#include "../shared/draw/basic.h"

/* Extended functionality */
#include "../shared/textures/arb_texture_storage.h"
#include "../shared/shaders/arb_get_program_binary.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL ES 3.0 compliance model
 */
struct CGLES30 :
        CGLES20,
        CGL_Basic_Draw,

        CGL_TextureStorage
{
    enum FBAttach
    {
        Renderbuffer,
        ColorBuffer,
    };

    STATICINLINE bool LoadBinding(CGL_Context* ctxt, GLADloadproc fun)
    {
        if(!CGLES20::LoadBinding(ctxt,fun))
            return false;

        return (bool)glTexStorage2D;
    }
    STATICINLINE bool TexStorageSupported()
    {return true;}
};

}
}

#endif
