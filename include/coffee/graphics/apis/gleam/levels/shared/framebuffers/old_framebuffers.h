#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/edef/pixenum.h>

#include "../textures/old_textures.h"

namespace Coffee{
namespace CGL{

template<typename CGhnd,typename CGenum,typename FramebufferT,typename Texture>
struct CGL_Old_Framebuffers
{
    /* Framebuffers */
    STATICINLINE void FBBind(FramebufferT t,CGhnd h)
    {glBindFramebuffer(to_enum(t),h);}

    /*TODO: Create FramebufAttProp enum*/
    STATICINLINE void FBGetAttachParameter(
            FramebufferT t,CGenum att,CGenum p,int32* d)
    {glGetFramebufferAttachmentParameteriv(to_enum(t),att,p,d);}

    /* Texture operations */
    STATICINLINE void FBAttachTexture2D(
            FramebufferT t,CGenum att,Texture textrg,CGhnd h,int32 level)
    {glFramebufferTexture2D(to_enum(t),att,to_enum(textrg),h,level);}

    STATICINLINE void FBAttachTextureLayer(
            FramebufferT t,CGenum att,CGhnd h,int32 level,int32 layer)
    {glFramebufferTextureLayer(to_enum(t),att,h,level,layer);}
    STATICINLINE void FBAttachRenderBuffer(
            FramebufferT t,CGenum att,CGhnd h)
    {glFramebufferRenderbuffer(to_enum(t),att,GL_RENDERBUFFER,h);}

#if !defined(COFFEE_ONLY_GLES20)
    /* Blit */
    STATICINLINE void FBBlit(
            const CRect& s, const CRect& d,DBuffers m,Filtering f)
    {glBlitFramebuffer(s.x,s.y,s.w,s.h, d.x,d.y,d.w,d.h, to_enum2(m),to_enum(f));}
#endif

    /* Buffer management */
    STATICINLINE void FBDrawBuffers(uint32 n,const CGenum* d)
    {glDrawBuffers(n,d);}
    STATICINLINE void FBReadBuffer(CGenum b)
    {glReadBuffer(b);}

    STATICINLINE void FBReadPixels(int32 x,int32 y,uint32 w,uint32 h,
                                   PixelComponents fmt,BitFormat dt,c_ptr p)
    {glReadPixels(x,y,w,h,to_enum(fmt),to_enum(dt),p);}

    /* Validation */
    STATICINLINE bool FBValidate(FramebufferT t)
    {
        CGenum f = glCheckFramebufferStatus(to_enum(t));
        return f==GL_FRAMEBUFFER_COMPLETE;
    }

    /* Renderbuffers */
    STATICINLINE void RBufBind(CGhnd h)
    {glBindRenderbuffer(GL_RENDERBUFFER,h);}

    STATICINLINE void RBufStorage(PixelFormat ifmt,uint32 w,uint32 h)
    {
        CGenum fmt = GL_NONE;
        if(ifmt == PixelFormat::Depth
                || ifmt == PixelFormat::DepthStencil)
            fmt = to_enum(ifmt);
        else
            fmt = GL_RGBA;

        glRenderbufferStorage(GL_RENDERBUFFER,fmt,w,h);
    }
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void RBufStorageMS(PixelFormat ifmt,uint32 samples,uint32 w,uint32 h)
    {glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples,to_enum(ifmt),w,h);}
#endif

    STATICINLINE void RBufGetParameteri(CGenum p,int32* d)
    {glGetRenderbufferParameteriv(GL_RENDERBUFFER,p,d);}

    STATICINLINE CSize FBGetAttachmentSize(FramebufferT t, uint32 idx)
    {
        C_UNUSED(t);
        C_UNUSED(idx);

        CSize out = {};

#if !defined(COFFEE_ONLY_GLES20)
        using Tex = CGL_Old_Textures<CGhnd,CGenum,Texture,CompFlags>;

        int32 tmp = 0;
        int32 hnd = 0;
        /* Get object type and handle */
        FBGetAttachParameter(t,GL_COLOR_ATTACHMENT0+idx,
                             GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,&tmp);
        FBGetAttachParameter(t,GL_COLOR_ATTACHMENT0+idx,
                             GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,&hnd);
        if(tmp == GL_TEXTURE)
        {
            /* If texture, determine type of texture */
            Texture textype = Texture::T2D;
            FBGetAttachParameter(t,GL_COLOR_ATTACHMENT0+idx,
                                 GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER,
                                 &tmp);
            if(tmp == GL_TRUE)
                textype = Texture::T2DArray;

            uint32 w,h,d;
            /* Use this helper function to get the size */
            Tex::TexBind(textype,hnd);
            Tex::TexGetImageSize(textype,PixCmp::None,w,h,d,nullptr);
            Tex::TexBind(textype,0);
            /* Assign these values later, uint32 != int32 */
            out.w = w;
            out.h = h;
        }else if(tmp == GL_RENDERBUFFER){
            /* I wish textures were this easy */
            RBufBind(hnd);
            RBufGetParameteri(GL_RENDERBUFFER_WIDTH,&out.w);
            RBufGetParameteri(GL_RENDERBUFFER_HEIGHT,&out.h);
            RBufBind(0);
        }
#endif

        return out;
    }
};

}
}
