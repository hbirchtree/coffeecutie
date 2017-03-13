#pragma once

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
#include "../shared/shaders/arb_es2_compatibility.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL ES 2.0 compliance model
 * Support notes:
 *  - VertexArrayObject is present, but not supported
 *  - GetBufferParameteri64v is present, but not supported
 *  - Queries are present, but not supported
 *  - 3D textures are present but not supported
 *  - Framebuffer layers are present, but not supported
 */

#if defined(COFFEE_LINKED_GLES)
using GLADloadproc = void(*)();
#endif

struct CGLES20 :
        CGL_Implementation,
        CGL_Old_Framebuffers<CGhnd,CGenum,FramebufferT,Texture>,
        CGL_Old_Textures<CGhnd,CGenum,Texture,CompFlags>,
        CGL_Old_Constructors<CGhnd,ShaderStage,CGsync>,
        CGL_Old_ShaderCompiler<CGhnd,CGenum>,
        CGL_Old_Buffers<CGhnd,BufType>,
        CGL_Old_VAOs<CGhnd,CGenum>,
        CGL_Old_Uniforms,
        CGL_Basic_Draw
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

#if !defined(COFFEE_LINKED_GLES)
        if(!gladLoadGLES2Loader(fun))
            return false;
#endif

        if(!Debug::VerifyInit() || !glTexImage2D)
            return false;

        Debug::InitInternalFormats();

        Debug::GetExtensions();
        Debug::InitDebugFlag();
        return true;
    }
};

}
}
