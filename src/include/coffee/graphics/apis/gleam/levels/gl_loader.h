#pragma once

#include "shared/gl_shared_debug.h"
#include "shared/gl_shared_include.h"
#include <coffee/core/coffee_mem_macros.h>
#include <coffee/graphics/common/gltypes.h>

namespace Coffee {
namespace CGL {

struct Loader
{
    STATICINLINE bool LoadBinding(
        CGL_Context* ctxt, GLADloadproc fun, void** ptr_check)
    {
        C_USED(fun);

        if(!ctxt->acquireContext())
            return false;

#if !defined(COFFEE_GLEAM_DESKTOP) && !defined(COFFEE_LINKED_GLES)
        if(!gladLoadGLES2Loader(fun))
            return false;
#else
        if(!gladLoadGL())
            return false;
#endif

        if(!glGetString(GL_VENDOR))
            return false;

        return static_cast<bool>(*ptr_check);
    }
};

using Debug = CGL_Shared_Debug;

struct Extensions
{
    STATICINLINE bool SRGB_Supported()
    {
        return Debug::CheckExtensionSupported("GL_EXT_sRGB")
                || Debug::CheckExtensionSupported("GL_EXT_texture_sRGB");
    }
    STATICINLINE bool GetProgramBinarySupported()
    {
        return Debug::CheckExtensionSupported("GL_ARB_get_program_binary")
                || Debug::CheckExtensionSupported("GL_OES_get_program_binary");
    }

    STATICINLINE bool ClipDistanceSupported()
    {
        return false;
    }

    STATICINLINE GL_EXT_CHECK(
        DrawParameters, "GL_ARB_shader_draw_parameters");

    STATICINLINE GL_EXT_CHECK(DirectState, "GL_ARB_direct_state_access");

    STATICINLINE GL_EXT_CHECK(BindlessTexture, "GL_ARB_bindless_texture");

    STATICINLINE GL_EXT_CHECK(CullDistance, "GL_ARB_cull_distance");

    STATICINLINE GL_EXT_CHECK(TexStorage, "GL_ARB_texture_storage");

    STATICINLINE GL_EXT_CHECK(TexView, "GL_ARB_texture_view");

    STATICINLINE GL_EXT_CHECK(ViewportArray, "GL_ARB_viewport_array");

    STATICINLINE GL_EXT_CHECK(VertexAttrib, "GL_ARB_vertex_attrib_binding");

    STATICINLINE GL_EXT_CHECK(
        SeparableShader, "GL_ARB_separate_shader_objects");

    STATICINLINE GL_EXT_CHECK(Tessellation, "GL_ARB_tessellation_shader");

    STATICINLINE GL_EXT_CHECK(
        ShaderStorage, "GL_ARB_shader_storage_buffer_object");

    STATICINLINE GL_EXT_CHECK(ShaderPrecision, "GL_ARB_shader_precision");

    STATICINLINE GL_EXT_CHECK(
        ProgramResourceInfo, "GL_ARB_program_interface_query");

    STATICINLINE GL_EXT_CHECK(ComputeShader, "GL_ARB_compute_shader");

    STATICINLINE GL_EXT_CHECK(XF2, "GL_ARB_transform_feedback2");

    STATICINLINE GL_EXT_CHECK(XF3, "GL_ARB_transform_feedback3");

    STATICINLINE GL_EXT_CHECK(
        InstancedXF, "GL_ARB_transform_feedback_instanced");

    STATICINLINE GL_EXT_CHECK(MultiDrawIndirect, "GL_ARB_multi_draw_indirect");

    STATICINLINE GL_EXT_CHECK(BufferStorage, "GL_ARB_buffer_storage");
};

} // namespace CGL
} // namespace Coffee
