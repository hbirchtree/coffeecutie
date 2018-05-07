#pragma once

#include <coffee/graphics/common/gltypes.h>
#include <coffee/core/types/vector_types.h>
#include <coffee/core/types/edef/dbgenum.h>
#include <coffee/core/base/types/cdisplay.h>

#include "shared/gl_shared_debug.h"
#include "shared/gl_shared_enum_convert.h"

namespace Coffee{
namespace CGL{

struct CGL_Shared_Functions
{
    using ReqVer = GLVER_33;

    using CGhnd = CGL::CGhnd;
    using BufType = CGL::BufType;
    using Texture = CGL::Texture;
    using Feature = CGL::Feature;

    using Debug = CGL_Shared_Debug;

    /* Features */
    STATICINLINE bool ClipDistanceSupported()
    {return false;}

    STATICINLINE bool Tex_SRGB_Supported()
    {
        return Debug::CheckExtensionSupported("GL_EXT_sRGB")
                || Debug::CheckExtensionSupported("GL_EXT_texture_sRGB");
    }
    STATICINLINE bool GetProgramBinarySupported()
    {
        return Debug::CheckExtensionSupported("GL_ARB_get_program_binary")
                || Debug::CheckExtensionSupported("GL_OES_get_program_binary");
    }

    /* Loading */
    STATICINLINE
    void LoadBinding(){}

    /* Basic functions */
    STATICINLINE
    void FrontFace(VertexWinding v){glFrontFace(to_enum(v));}
    STATICINLINE
    void CullMode(Face v){glCullFace(to_enum(v));}

    STATICINLINE
    void Enable(Feature e,uint32 o = 0){glEnable(to_enum(e,o));}
    STATICINLINE
    void Disable(Feature e,uint32 o = 0){glDisable(to_enum(e,o));}

#if !defined(COFFEE_ONLY_GLES20)
    GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
    STATICINLINE
    void Enablei(Feature e,uint32 i,uint32 o = 0){glEnablei(to_enum(e,o),i);}
    GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
    STATICINLINE
    void Disablei(Feature e,uint32 i,uint32 o = 0){glDisablei(to_enum(e,o),i);}
#endif

    /* Pipeline actions */
    STATICINLINE
    void Flush(){glFlush();}
    STATICINLINE
    void Finish(){glFinish();}

    /* Buffer clearing */
    STATICINLINE
    void ClearColor(Vecf4 const& c){glClearColor(c.r(),c.g(),c.b(),c.a());}
    STATICINLINE
    void ClearDepth(scalar f){glClearDepthf(f);}
    STATICINLINE
    void ClearStencil(int32 f){glClearStencil(f);}

    STATICINLINE
    void ClearBufferiv(const int32* d)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glClearBufferiv(GL_STENCIL,0,d);
#else
        ClearStencil(*d);
        glClear(GL_STENCIL_BUFFER_BIT);
#endif
    }
    STATICINLINE
    void ClearBufferfv(const scalar* d)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glClearBufferfv(GL_DEPTH,0,d);
#else
        ClearDepth(*d);
        glClear(GL_DEPTH_BUFFER_BIT);
#endif
    }
    STATICINLINE
    void ClearBufferfv(bool,int32 i,const CVec4& d)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glClearBufferfv(GL_COLOR,i,(scalar*)&d);
#else
        C_UNUSED(i);
        ClearColor(d);
        glClear(GL_COLOR_BUFFER_BIT);
#endif
    }
    STATICINLINE
    void ClearBufferfi(scalar d,int32 s)
    {
#if !defined(COFFEE_ONLY_GLES20)
        glClearBufferfi(GL_DEPTH_STENCIL,0,d,s);
#else
        ClearBufferiv(&s);
        ClearBufferfv(&d);
#endif
    }

    /* Blending */
    STATICINLINE
    void BlendFunc(CGenum v1,CGenum v2)
    {glBlendFunc(v1,v2);}
    STATICINLINE
    void BlendFuncSep(CGenum v1,CGenum v2,CGenum v3,CGenum v4)
    {glBlendFuncSeparate(v1,v2,v3,v4);}
    STATICINLINE
    void BlendEqSep(Operator v1,Operator v2)
    {glBlendEquationSeparate(to_enum(v1),to_enum(v2));}

    /* Display buffers */
    STATICINLINE
    void DepthFunc(ValueComparison f){glDepthFunc(to_enum(f));}
    STATICINLINE
    void DepthMask(bool v){glDepthMask((v) ? GL_TRUE : GL_FALSE);}
    STATICINLINE
    void DepthSet(ZField64 const& d)
    {
#ifdef COFFEE_GLEAM_DESKTOP
        glDepthRange(d.near_,d.far_);
#else
        glDepthRangef(d.near_,d.far_);
#endif
    }

    STATICINLINE
    void StencilFuncSep(Face v1,ValueComparison v2,int32 d1,uint32 d2)
    {glStencilFuncSeparate(to_enum(v1),to_enum(v2),d1,d2);}
    STATICINLINE
    void StencilOpSep(Face v1,ValueHandling sfail,ValueHandling dfail,ValueHandling dsfail)
    {glStencilOpSeparate(to_enum(v1),to_enum(sfail),to_enum(dfail),to_enum(dsfail));}
    STATICINLINE
    void StencilMaskSep(Face f,uint32 d){glStencilMaskSeparate(to_enum(f),d);}

    /* Viewport */
    STATICINLINE
    void ViewportSet(CRect64 const& r){glViewport(r.x,r.y,r.w,r.h);}
    STATICINLINE
    void ScissorSet(CRect64 const& r){glScissor(r.x,r.y,r.w,r.h);}

    /* Polygonal / line rendering */
    STATICINLINE
    void LineWidth(scalar f){glLineWidth(f);}
    STATICINLINE
    void PolyOffset(scalar f1,scalar f2){glPolygonOffset(f1,f2);}

    GL_VERSION_REQ(GLVER_33)
    STATICINLINE
    void PolyMode(Face f1,DrawMode f2)
    {
        C_UNUSED(f1);
        C_UNUSED(f2);

#ifdef COFFEE_GLEAM_DESKTOP
        glPolygonMode(to_enum(f1),to_enum(f2));
#endif
    }

    GL_VERSION_REQ(GLVER_33)
    STATICINLINE
    void PointParameteriv(CGenum f,const int32* d)
    {
        C_UNUSED(f);
        C_UNUSED(d);

#ifdef COFFEE_GLEAM_DESKTOP
        glPointParameteriv(f,d);
#endif
    }

    GL_VERSION_REQ(GLVER_33)
    STATICINLINE
    void PointParameterfv(CGenum f,const scalar* d)
    {
        C_UNUSED(f);
        C_UNUSED(d);

#ifdef COFFEE_GLEAM_DESKTOP
        glPointParameterfv(f,d);
#endif
    }

    GL_VERSION_REQ(GLVER_33)
    STATICINLINE
    void PointSize(scalar f)
    {
        C_UNUSED(f);

#ifdef COFFEE_GLEAM_DESKTOP
        glPointSize(f);
#endif
    }

    /* Sampling */
    GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
    STATICINLINE
    void SampleCoverage(scalar f,bool d){glSampleCoverage(f,(d) ? GL_TRUE : GL_FALSE);}

#if !defined(COFFEE_ONLY_GLES20)
    GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
    STATICINLINE
    void SampleMaski(uint32 d,CGflag f){glSampleMaski(d,f);}
#endif

    /* Pixel operations */
    STATICINLINE
    void PixelStore(bool pack, PixelOperation op, int32 v)
    {
        glPixelStorei(to_enum(pack,op),v);
    }

    GL_VERSION_REQ(GLVER_33)
    STATICINLINE
    void ColorMask(CColorMask op)
    {
        C_UNUSED(op);

#ifdef COFFEE_GLEAM_DESKTOP
        glColorMask(op.r,op.g,op.b,op.a);
#endif
    }
    GL_VERSION_REQ(GLVER_33)
    STATICINLINE
    void ColorMaski(uint32 i, CColorMask op)
    {
        C_UNUSED(i);
        C_UNUSED(op);

#ifdef COFFEE_GLEAM_DESKTOP
        glColorMaski(i,op.r,op.g,op.b,op.a);
#endif
    }

    GL_VERSION_REQ(GLVER_33)
    STATICINLINE
    void ColorLogicOp(LogicOp op)
    {
        C_UNUSED(op);

#ifdef COFFEE_GLEAM_DESKTOP
        glLogicOp(to_enum(op));
#endif
    }

    /* Core features! */
    STATICINLINE
    /*!
     * \brief Check if platform supports any kind of GL debugging. If this returns true, there should be a valid implementation.
     * \return
     */
#if !defined(COFFEE_ONLY_GLES20)
    bool DebuggingSupported()
    {return Debug::CheckExtensionSupported("GL_KHR_debug") && glDebugMessageCallback;}
#else
    bool DebuggingSupported()
    {return false;}
#endif
};

}
}
