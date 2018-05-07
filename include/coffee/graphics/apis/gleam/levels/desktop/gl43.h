#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_43_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_43_H

#include "gl33.h"

#include "../shared/buffers/arb_invalidate_subdata.h"
#include "../shared/buffers/arb_clear_buffer_object.h"
#include "../shared/buffers/arb_buffer_storage.h"

#include "../shared/constructors/arb_separate_shader_programs.h"
#include "../shared/constructors/arb_transform_feedback2.h"

#include "../shared/draw/arb_multi_draw_indirect.h"
#include "../shared/draw/arb_tessellation_shader.h"
#include "../shared/draw/arb_compute_shader.h"
#include "../shared/draw/drawing_43.h"

#include "../shared/framebuffers/arb_viewport_array.h"
#include "../shared/framebuffers/arb_invalidate_subdata.h"
#include "../shared/framebuffers/arb_framebuffer_no_attachments.h"
#include "../shared/framebuffers/framebuffer_43.h"

#include "../shared/shaders/arb_es2_compatibility.h"
#include "../shared/shaders/arb_program_interface_query.h"
#include "../shared/shaders/arb_separate_shader_programs.h"
#include "../shared/shaders/arb_shader_storage_buffer_object.h"
#include "../shared/shaders/arb_get_program_binary.h"

#include "../shared/textures/arb_texture_storage_multisample.h"
#include "../shared/textures/arb_texture_storage.h"
#include "../shared/textures/arb_texture_view.h"
#include "../shared/textures/arb_shader_image_load_store.h"
#include "../shared/textures/arb_copy_image.h"

#include "../shared/vertex/arb_vertex_attrib_binding.h"

#include "../shared/xfb/arb_xf2.h"
#include "../shared/xfb/arb_xf3.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL 4.3 compliance model
 */
template<typename ReqVer>
struct CGL43_Base :

        CGL33_Base<ReqVer>,

        CGL_ClearBuffer,

        CGL_SeparableShaderPrograms,

        CGL_SeparableShaderPrograms_Allocators,
        CGL_XF2_Allocators,

        CGL_MultiDrawIndirect,
        CGL_TessellationShader,
        CGL_ComputeShader,
        CGL_Drawing_43,

        CGL_ViewportArray,
        CGL_FramebufferInvalidateSubData,
        CGL_FramebufferNoAttachments,
        CGL_Framebuffers_43,

        CGL_ES2Compatibility,
        CGL_ProgramInterfaceQuery,
        CGL_SSBO,
        CGL_GetProgramBinary,

        CGL_ShaderImageLoadStore,
        CGL_CopyImage,

        CGL_XF2,
        CGL_XF3
{
    STATICINLINE bool LoadBinding(CGL_Context *ctxt = nullptr, GLADloadproc procLoad = nullptr)
    {
        bool status = CGL33::LoadBinding(ctxt, procLoad);
	if(!status)
            return false;

        Debug::InitInternalFormats();
        return status;
    }

    //TODO: Add more extensions to check for, and check for them in functions

    STATICINLINE GL_EXT_CHECK(TexStorage,
                              "GL_ARB_texture_storage")

    STATICINLINE GL_EXT_CHECK(TexView,
                              "GL_ARB_texture_view")

    STATICINLINE GL_EXT_CHECK(ViewportArray,
                              "GL_ARB_viewport_array")

    STATICINLINE GL_EXT_CHECK(VertexAttrib,
                              "GL_ARB_vertex_attrib_binding")

    STATICINLINE GL_EXT_CHECK(SeparableShader,
                              "GL_ARB_separate_shader_objects")

    STATICINLINE GL_EXT_CHECK(Tessellation,
                              "GL_ARB_tessellation_shader")

    STATICINLINE GL_EXT_CHECK(ShaderStorage,
                              "GL_ARB_shader_storage_buffer_object")

    STATICINLINE GL_EXT_CHECK(ShaderPrecision,
                              "GL_ARB_shader_precision")

    STATICINLINE GL_EXT_CHECK(ProgramResourceInfo,
                              "GL_ARB_program_interface_query")

    STATICINLINE GL_EXT_CHECK(ComputeShader,
                              "GL_ARB_compute_shader")

    STATICINLINE GL_EXT_CHECK(XF2,
                              "GL_ARB_transform_feedback2")

    STATICINLINE GL_EXT_CHECK(XF3,
                              "GL_ARB_transform_feedback3")

    STATICINLINE GL_EXT_CHECK(InstancedXF,
                              "GL_ARB_transform_feedback_instanced")

    STATICINLINE GL_EXT_CHECK(MultiDrawIndirect,
                              "GL_ARB_multi_draw_indirect")

    STATICINLINE GL_EXT_CHECK(BufferStorage,
                              "GL_ARB_buffer_storage")

    STATICINLINE void BlendFunci(uint32 i, CGenum v1,CGenum v2)
    {glBlendFunci(i,v1,v2);}

    /* Overriding GL3.3 functions */
    using CGL_SeparableShaderPrograms::Uniformfv;
    using CGL_SeparableShaderPrograms::Uniformiv;
    using CGL_SeparableShaderPrograms::Uniformuiv;

    /* Overriding GL3.3 functions */
    using CGL_ViewportArray::ViewportSet;
    STATICINLINE void ViewportSet(CRect64 const& v)
    {
        ViewportSet(0,v.convert<scalar>());
    }

    struct Debug : CGL33::Debug
    {
        STATICINLINE void InitInternalFormats()
        {
        }

        STATICINLINE void FreeInternalFormats()
        {
        }

        STATICINLINE bool InternalFormatSupport(Texture tt, PixelFormat t)
        {
            /* TODO: GL_COMPRESSED_TEXTURE_FORMATS */
            int32 supp = GL_FALSE;
            glGetInternalformativ(to_enum(tt),to_enum(t),GL_INTERNALFORMAT_SUPPORTED,sizeof(supp),&supp);
            return supp == GL_TRUE;
        }
        STATICINLINE ColBits InternalFormatDepths(Texture, PixelFormat)
        {
    //        int32 supp;
    //        glGetInternalformativ(to_enum(tt),to_enum(t),GL_INTERNALFORMAT_SUPPORTED,sizeof(supp),&supp);
            return {};
        }
        STATICINLINE CSize InternalFormatMaxResolution2D(Texture tt, PixelFormat t)
        {
            CSize sz;
            glGetInternalformativ(to_enum(tt),to_enum(t),GL_MAX_WIDTH,sizeof(sz.w),&sz.w);
            glGetInternalformativ(to_enum(tt),to_enum(t),GL_MAX_HEIGHT,sizeof(sz.h),&sz.h);
            return sz;
        }
    };
};

struct CGL43 :
        CGL43_Base<GLVER_43>,

        /* GL 4.3 features */
        CGL_BufferInvalidateSubData,
        CGL_BufferStorage<GLVER_43>,
        CGL_TextureStorage,
        CGL_TextureStorageMultisample,
        CGL_TextureView,
        CGL_VertexAttribBinding<GLVER_43>,

        /* GL 3.3 features */
        CGL_Old_Framebuffers<CGhnd,CGenum,FramebufferT,Texture, GLVER_33>,
        CGL_Old_Textures<CGhnd,CGenum,Texture,CompFlags>,
        CGL_Old_Constructors<CGhnd,ShaderStage,CGsync>,
        CGL_Old_Buffers<CGhnd,BufType, GLVER_33>,
        CGL_Old_VAOs<CGhnd,CGenum>
{
};

}
}

#endif
