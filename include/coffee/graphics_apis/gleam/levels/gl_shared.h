#pragma once

#include "../../gltypes.h"
#include <coffee/core/types/vector_types.h>
#include <coffee/core/types/edef/dbgenum.h>
#include <coffee/core/base/types/cdisplay.h>

#include "shared/gl_shared_debug.h"
#include "shared/gl_shared_enum_convert.h"

namespace Coffee{
namespace CGL{

struct CGL_Shared_Functions
{
    using CGhnd = CGL::CGhnd;
    using BufType = CGL::BufType;
    using ShaderStage = CGL::ShaderStage;
    using Texture = CGL::Texture;
    using Feature = CGL::Feature;
    using FramebufferT = CGL::FramebufferT;

    STATICINLINE
    void LoadBinding(){}

    /* Basic functions */

    STATICINLINE
    void ClearDepth(scalar f){glClearDepthf(f);}
    STATICINLINE
    void ClearStencil(int32 f){glClearStencil(f);}

    STATICINLINE
    void FrontFace(Face v){glFrontFace(to_enum(v));}
    STATICINLINE
    void CullMode(Face v){glCullFace(to_enum(v));}

    STATICINLINE
    void Flush(){glFlush();}
    STATICINLINE
    void Finish(){glFinish();}

    STATICINLINE
    void ClearBufferiv(const int32* d){glClearBufferiv(GL_STENCIL,0,d);}
    STATICINLINE
    void ClearBufferfv(bool color,int32 i,const scalar* d)
    {glClearBufferfv((color) ? GL_COLOR : GL_DEPTH,i,d);}
    STATICINLINE
    void ClearBufferfv(bool color,int32 i,const CVec4& d)
    {glClearBufferfv((color) ? GL_COLOR : GL_DEPTH,i,(scalar*)&d);}
    STATICINLINE
    void ClearBufferfi(scalar d,int32 s){glClearBufferfi(GL_DEPTH_STENCIL,0,d,s);}

    STATICINLINE
    void BlendFunc(CGenum v1,CGenum v2)
    {glBlendFunc(v1,v2);}
    STATICINLINE
    void BlendFuncSep(CGenum v1,CGenum v2,CGenum v3,CGenum v4)
    {glBlendFuncSeparate(v1,v2,v3,v4);}
    STATICINLINE
    void BlendEqSep(Operator v1,Operator v2)
    {glBlendEquationSeparate(to_enum(v1),to_enum(v2));}

    STATICINLINE
    void DepthFunc(ValueComparison f){glDepthFunc(to_enum(f));}
    STATICINLINE
    void DepthMask(bool v){glDepthMask((v) ? GL_TRUE : GL_FALSE);}

    STATICINLINE
    void StencilFuncSep(Face v1,ValueComparison v2,int32 d1,uint32 d2)
    {glStencilFuncSeparate(to_enum(v1),to_enum(v2),d1,d2);}
    STATICINLINE
    void StencilOpSep(Face v1,ValueHandling sfail,ValueHandling dfail,ValueHandling dsfail)
    {glStencilOpSeparate(to_enum(v1),to_enum(sfail),to_enum(dfail),to_enum(dsfail));}
    STATICINLINE
    void StencilMaskSep(Face f,uint32 d){glStencilMaskSeparate(to_enum(f),d);}

    STATICINLINE
    void ViewportSet(CRect64 const* r){glViewport(r->x,r->y,r->w,r->h);}
    STATICINLINE
    void ScissorSet(CRect64 const* r){glScissor(r->x,r->y,r->w,r->h);}

    STATICINLINE
    void LineWidth(scalar f){glLineWidth(f);}
    STATICINLINE
    void PolyOffset(scalar f1,scalar f2){glPolygonOffset(f1,f2);}

    STATICINLINE
    void SampleCoverage(scalar f,bool d){glSampleCoverage(f,(d) ? GL_TRUE : GL_FALSE);}

    STATICINLINE
    void PixelStore(PixelOperation op, int32 v)
    {
        glPixelStorei(to_enum(false,op),v);
        glPixelStorei(to_enum(true,op),v);
    }

    /* Core features! */
    STATICINLINE
    /*!
     * \brief Check if platform supports any kind of GL debugging. If this returns true, there should be a valid implementation.
     * \return
     */
    bool DebuggingSupported()
    {return Debug::CheckExtensionSupported("GL_KHR_debug");}

    using Debug = CGL_Shared_Debug;
};

}
}
