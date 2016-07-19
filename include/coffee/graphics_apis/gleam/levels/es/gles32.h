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

struct CGLES32 : CGLES30,
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

    STATICINLINE bool LoadBinding(CGL_Context* ctxt, GLADloadproc fun)
    {
        if(!CGLES30::LoadBinding(ctxt,fun))
            return false;

        return true;
    }
    /* Stubbing this to avoid compilation errors */
    STATICINLINE void BufStorage(BufType,uint64,c_cptr,ResourceAccess){}
    STATICINLINE void SBufBind(CGhnd,uint32,uint32){}

    STATICINLINE bool TessellationSupported()
    {
        /* TODO: Find proper extension for this */
        return true;
    }
    STATICINLINE bool ShaderStorageSupported()
    {return false;}
};

}
}
