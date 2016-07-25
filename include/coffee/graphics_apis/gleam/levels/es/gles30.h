#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_ES30_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_ES30_H

#include "glbase.h"

/* Basic */
#include "../shared/constructors/oldstyle.h"
#include "../shared/shaders/compiling.h"
#include "../shared/shaders/uniforms.h"
#include "../shared/textures/old_textures.h"
#include "../shared/buffers/old_buffers.h"
#include "../shared/framebuffers/old_framebuffers.h"
#include "../shared/vertex/old_vaos.h"
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
        CGL_Implementation,
        CGL_Old_Framebuffers<CGhnd,CGenum,FramebufferT,Texture>,
        CGL_Old_Textures<CGhnd,CGenum,Texture,CompFlags>,
        CGL_Old_Constructors<CGhnd,ShaderStage,CGsync>,
        CGL_Old_ShaderCompiler<CGhnd,CGenum>,
        CGL_Old_Buffers<CGhnd,BufType>,
        CGL_Old_VAOs<CGhnd,CGenum>,
        CGL_Old_Uniforms,
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
        if(!ctxt->acquireContext())
            return false;
        if(!gladLoadGLES2Loader(fun))
            return false;

        if(!Debug::VerifyInit() || !glTexStorage2D)
	    return false;

        Debug::GetExtensions();
        Debug::InitDebugFlag();
        return true;
    }
};

}
}

#endif
