#pragma once

#include "gles30.h"

#include "../shared/constructors/arb_transform_feedback2.h"

#include "../shared/xfb/arb_xf2.h"

#include "../shared/draw/drawing_43.h"
#include "../shared/draw/arb_tessellation_shader.h"
#include "../shared/draw/arb_compute_shader.h"

#include "../shared/constructors/arb_separate_shader_programs.h"
#include "../shared/shaders/arb_program_interface_query.h"
#include "../shared/shaders/arb_separate_shader_programs.h"
#include "../shared/shaders/arb_es2_compatibility.h"
#include "../shared/shaders/arb_get_program_binary.h"

#include "../shared/vertex/arb_vertex_attrib_binding.h"

#include "../shared/framebuffers/arb_framebuffer_no_attachments.h"

namespace Coffee{
namespace CGL{

struct CGLES32 :
        CGL_Implementation,
        CGL_Old_Framebuffers<CGhnd,CGenum,FramebufferT,Texture>,
        CGL_Old_Textures<CGhnd,CGenum,Texture,CompFlags>,
        CGL_Old_Constructors<CGhnd,ShaderStage,CGsync>,
        CGL_Old_ShaderCompiler<CGhnd,CGenum>,
        CGL_Old_Buffers<CGhnd,BufType>,
        CGL_Old_VAOs<CGhnd,CGenum>,
        CGL_Basic_Draw,

        CGL_TextureStorage,

        CGL_SeparableShaderPrograms,

        CGL_SeparableShaderPrograms_Allocators,
        CGL_XF2_Allocators,

        CGL_TessellationShader,
        CGL_ComputeShader,
        CGL_Drawing_43,

        CGL_ES2Compatibility,
        CGL_ProgramInterfaceQuery,
        CGL_GetProgramBinary,

        CGL_VertexAttribBinding,

        CGL_XF2
{
    using CGL_Implementation::ViewportSet;

    STATICINLINE bool ShaderStorageSupported()
    {return false;}
    STATICINLINE bool ViewportArraySupported()
    {return false;}
    STATICINLINE bool BufferStorageSupported()
    {return true;}
    STATICINLINE bool TessellationSupported()
    {
        /* TODO: Find proper extension for this */
        return false;
    }

    STATICINLINE bool LoadBinding(CGL_Context* ctxt, GLADloadproc fun)
    {
        if(!CGLES30::LoadBinding(ctxt,fun))
            return false;

        return (bool)glVertexAttribFormat;
    }

    STATICINLINE void ViewportSet(uint32, CRectF& view)
    {
        ViewportSet(view.convert<int64>());
    }

    STATICINLINE void BlendFunci(uint32 i, CGenum v1,CGenum v2)
    {glBlendFunci(i,v1,v2);}

    /* Stubbing this to avoid compilation errors */
    template<typename... T>
    STATICINLINE void BufStorage(T...){}
    template<typename... T>
    STATICINLINE void SBufBind(T...){}

    template<typename... T>
    STATICINLINE void BufInvalidateData(T...){}
};

}
}
