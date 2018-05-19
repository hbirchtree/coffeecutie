#pragma once

#include "glbase.h"

#include "../shared/buffers/old_buffers.h"
#include "../shared/constructors/oldstyle.h"
#include "../shared/draw/basic.h"
#include "../shared/draw/drawing_base_multidraw.h"
#include "../shared/framebuffers/old_framebuffers.h"
#include "../shared/queries/old_queries.h"
#include "../shared/shaders/compiling.h"
#include "../shared/shaders/uniforms.h"
#include "../shared/textures/old_textures.h"
#include "../shared/vertex/old_vaos.h"

namespace Coffee {
namespace CGL {

/*!
 * \brief OpenGL 3.3 compliance model
 */
template<typename ReqVer>
struct CGL33_Base : CGL_Implementation,
                    CGL_Old_ShaderCompiler<CGhnd, CGenum>,
                    CGL_Old_Queries<QueryT, CGhnd, CGenum>,
                    CGL_Basic_Draw,
                    CGL_Drawing_Base_MultiDraw

{
    enum FBAttach
    {
        Renderbuffer,
        ColorBuffer,
    };

    STATICINLINE bool LoadBinding(CGL_Context* ctxt,
                                  GLADloadproc procLoad = nullptr)
    {
        C_USED(procLoad);

        if(ctxt && !ctxt->acquireContext())
            return false;
#if defined(COFFEE_USE_MAEMO_EGL)
        if(!gladLoadGLLoader(procLoad))
#else
        if(!gladLoadGL())
#endif
            return false;

        if(!glGetString(GL_VENDOR))
            return false;

        if(!Debug::VerifyInit() || !glGenSamplers)
            return false;

        Debug::GetExtensions();
        Debug::InitDebugFlag();
        Debug::InitInternalFormats();
        return true;
    }

    STATICINLINE bool ClipDistanceSupported()
    {
        return true;
    }

    STATICINLINE bool InstancedGeometryShaderSupported()
    {
        return Debug::CheckExtensionSupported("GL_ARB_geometry_shader4");
    }

    STATICINLINE bool CubemapSeamlessSupported()
    {
        return Debug::CheckExtensionSupported("GL_ARB_seamless_cube_map");
    }

    STATICINLINE bool CubemapArraySupported()
    {
        return Debug::CheckExtensionSupported("GL_ARB_texture_cube_map_array");
    }

    STATICINLINE bool ASTCSupported()
    {
        return Debug::CheckExtensionSupported(
            "GL_KHR_texture_compression_astc_hdr");
    }

    STATICINLINE bool BPTCSupported()
    {
        return Debug::CheckExtensionSupported(
            "GL_ARB_texture_compression_bptc");
    }

    STATICINLINE bool RGTCSupported()
    {
        return Debug::CheckExtensionSupported(
            "GL_ARB_texture_compression_rgtc");
    }

    GL_VERSION_REQ_DESKTOP(GLVER_30)
    STATICINLINE bool DXTCSupported()
    {
        return Debug::CheckExtensionSupported(
            "GL_EXT_texture_compression_s3tc");
    }

    /* Shaders */
    /*TODO: Create ShaderProperty enum*/

    GL_VERSION_REQ_DESKTOP(GLVER_32)
    STATICINLINE int32 ProgramGetFragDataIdx(CGhnd h, cstring n)
    {
        return glGetFragDataIndex(h, n);
    }

    GL_VERSION_REQ_DESKTOP(GLVER_30)
    STATICINLINE void ProgramBindFragData(CGhnd h, uint32 l, cstring n)
    {
        glBindFragDataLocation(h, l, n);
    }

    GL_VERSION_REQ_DESKTOP(GLVER_32)
    STATICINLINE void ProgramBindFragDataIndexed(CGhnd   h,
                                                 uint32  i,
                                                 uint32  l,
                                                 cstring n)
    {
        glBindFragDataLocationIndexed(h, l, i, n);
    }

    GL_VERSION_REQ(GLVER_32)
    STATICINLINE void TexImage2DMS(
        Texture t, uint32 samples, PixelFormat ifmt, uint32 w, uint32 h)
    {
        glTexImage2DMultisample(
            to_enum(t), samples, to_enum(ifmt), w, h, GL_FALSE);
    }

    GL_VERSION_REQ(GLVER_32)
    STATICINLINE void TexImage2DMS(Texture     t,
                                   uint32      samples,
                                   PixelFormat ifmt,
                                   uint32      w,
                                   uint32      h,
                                   uint32      d)
    {
        glTexImage3DMultisample(
            to_enum(t), samples, to_enum(ifmt), w, h, d, GL_FALSE);
    }

    /* Buffers */
    GL_VERSION_REQ_DESKTOP(GLVER_30)
    STATICINLINE void BufGetSubData(BufType t, int64 off, uint32 sz, c_ptr p)
    {
        glGetBufferSubData(to_enum(t), off, sz, p);
    }

    /* Queries */
    GL_VERSION_REQ_DESKTOP(GLVER_30)
    STATICINLINE void ConditionalRenderBegin(CGhnd h, Delay m)
    {
        glBeginConditionalRender(h, to_enum1(m));
    }

    GL_VERSION_REQ_DESKTOP(GLVER_30)
    STATICINLINE void ConditionalRenderEnd()
    {
        glEndConditionalRender();
    }

    GL_VERSION_REQ_DESKTOP(GLVER_32)
    STATICINLINE void QueryCounter(CGhnd h, CGenum t)
    {
        glQueryCounter(h, t);
    }

    /* FB */
    GL_VERSION_REQ_DESKTOP(GLVER_30)
    STATICINLINE void FBAttachTexture3D(FramebufferT t,
                                        CGenum       att,
                                        Texture      textrg,
                                        CGhnd        h,
                                        int32        level,
                                        int32        z)
    {
        glFramebufferTexture3D(to_enum(t), att, to_enum(textrg), h, level, z);
    }

    /* VAO */
    STATICINLINE void VAOPrimitiveRestart(uint32 idx)
    {
        glPrimitiveRestartIndex(idx);
        Enable(Feature::PrimitiveRestart);
    }
};

struct CGL33
    : CGL33_Base<GLVER_33>,
      CGL_Old_Uniforms,
      CGL_Old_Framebuffers<CGhnd, CGenum, FramebufferT, Texture, GLVER_33>,
      CGL_Old_Textures<CGhnd, CGenum, Texture, CompFlags>,
      CGL_Old_Constructors<CGhnd, ShaderStage, CGsync>,
      CGL_Old_Buffers<CGhnd, BufType, GLVER_33>,
      CGL_Old_VAOs<CGhnd, CGenum>
{
};

} // namespace CGL
} // namespace Coffee