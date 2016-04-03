#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/edef/pixenum.h>

namespace Coffee{
namespace CGL{

template<typename CGhnd,typename CGenum,typename FramebufferT,typename Texture>
struct CGL_Old_Framebuffers
{
    /* Framebuffers */
    STATICINLINE void FBBind(FramebufferT t,CGhnd h){glBindFramebuffer(to_enum(t),h);}

    /*TODO: Create FramebufAttProp enum*/
    STATICINLINE void FBGetAttachParameter(FramebufferT t,CGenum att,CGenum p,int32* d)
    {glGetFramebufferAttachmentParameteriv(to_enum(t),att,p,d);}

    STATICINLINE void FBAttachTexture2D(FramebufferT t,CGenum att,Texture textrg,CGhnd h,int32 level)
    {glFramebufferTexture2D(to_enum(t),att,to_enum(textrg),h,level);}

    STATICINLINE void FBAttachTextureLayer(FramebufferT t,CGenum att,CGhnd h,int32 level,int32 layer)
    {glFramebufferTextureLayer(to_enum(t),att,h,level,layer);}
    STATICINLINE void FBAttachRenderBuffer(FramebufferT t,CGenum att,CGhnd h)
    {glFramebufferRenderbuffer(to_enum(t),att,GL_RENDERBUFFER,h);}

    STATICINLINE void FBBlit(const CRect& s, const CRect& d,CGflag m,CGenum f)
    {glBlitFramebuffer(s.x,s.y,s.w,s.h, d.x,d.y,d.w,d.h, m,f);}


    STATICINLINE void FBDrawBuffers(uint32 n,const CGenum* d){glDrawBuffers(n,d);}
    STATICINLINE void FBReadBuffer(CGenum b){glReadBuffer(b);}

    STATICINLINE void FBReadPixels(int32 x,int32 y,uint32 w,uint32 h,
                                   PixelComponents fmt,TypeEnum dt,c_ptr p)
    {glReadPixels(x,y,w,h,to_enum(fmt),to_enum(dt),p);}

    STATICINLINE bool FBValidate(FramebufferT t)
    {
        CGenum f = glCheckFramebufferStatus(to_enum(t));
        if(f!=GL_FRAMEBUFFER_COMPLETE)
            cLog(__FILE__,__LINE__,"GL33","Incomplete framebuffer",f);

        return f==GL_FRAMEBUFFER_COMPLETE;
    }

    /* Renderbuffers */
    STATICINLINE void RBufBind(FramebufferT t,CGhnd h){glBindRenderbuffer(to_enum(t),h);}

    STATICINLINE void RBufStorage(PixelFormat ifmt,uint32 w,uint32 h)
    {glRenderbufferStorage(GL_RENDERBUFFER,to_enum(ifmt),w,h);}
    STATICINLINE void RBufStorageMS(PixelFormat ifmt,uint32 samples,uint32 w,uint32 h)
    {glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples,to_enum(ifmt),w,h);}

    STATICINLINE void RBufGetParameteri(CGenum p,int32* d)
    {glGetRenderbufferParameteriv(GL_RENDERBUFFER,p,d);}
};

}
}
