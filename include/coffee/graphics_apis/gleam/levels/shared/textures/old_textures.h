#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

template<typename CGhnd,typename CGenum,typename Texture,typename CompFlags>
struct CGL_Old_Textures
{
    /* Textures */
    STATICINLINE void TexBind(Texture t,CGhnd h){glBindTexture(to_enum(t),h);}
    STATICINLINE void TexActive(uint32 i){glActiveTexture(GL_TEXTURE0+i);}

    /* TexImage */

    STATICINLINE void TexImage2D(Texture t,uint32 level,PixelFormat ifmt,
                                 uint32 w,uint32 h,int32 border,PixelComponents fmt,
                                 BitFormat dt,c_cptr p)
    {glTexImage2D(to_enum(t),level,to_enum(ifmt),w,h,border,to_enum(fmt),to_enum(dt),p);}
    STATICINLINE void TexImage3D(Texture t,uint32 level,PixelFormat ifmt,
                                 uint32 w,uint32 h,uint32 d,int32 border,
                                 PixelComponents fmt,BitFormat dt,c_cptr p)
    {glTexImage3D(to_enum(t),level,to_enum(ifmt),w,h,d,border,to_enum(fmt),to_enum(dt),p);}

    /* TexSubImage */

    STATICINLINE void TexSubImage2D(Texture t,uint32 level,int32 x,int32 y,
                                    uint32 w,uint32 h,PixelComponents fmt,BitFormat dt,c_cptr p)
    {glTexSubImage2D(to_enum(t),level,x,y,w,h,to_enum(fmt),to_enum(dt),p);}
    STATICINLINE void TexSubImage3D(Texture t,uint32 level,int32 x,int32 y,int32 z,
                                    uint32 w,uint32 h,uint32 d,PixelComponents fmt,
                                    BitFormat dt,c_cptr p)
    {glTexSubImage3D(to_enum(t),level,x,y,z,w,h,d,to_enum(fmt),to_enum(dt),p);}

    /* TexImageCompressed */

    STATICINLINE void TexImageCompressed2D(Texture t,int32 level,
                                           PixelFormat ifmt, PixelFlags flgs,
                                           CompFlags cflgs,
                                           uint32 w,uint32 h,int32 border,uint32 sz,c_cptr p)
    {glCompressedTexImage2D(to_enum(t),level,to_enum(ifmt,flgs,cflgs),w,h,border,sz,p);}
    STATICINLINE void TexImageCompressed3D(Texture t,int32 level,
                                           PixelFormat ifmt, PixelFlags flgs,
                                           CompFlags cflgs,
                                           uint32 w,uint32 h,uint32 d,int32 border,uint32 sz,c_cptr p)
    {glCompressedTexImage3D(to_enum(t),level,to_enum(ifmt,flgs,cflgs),w,h,d,border,sz,p);}

    /* TexSubImageCompressed */

    STATICINLINE void TexSubImageCompressed2D(Texture t,uint32 level,int32 x,int32 y,
                                              uint32 w,uint32 h,
                                              PixelFormat fmt, PixelFlags flgs,
                                              CompFlags cflgs,
                                              int32 sz,c_cptr p)
    {glCompressedTexSubImage2D(to_enum(t),level,x,y,w,h,to_enum(fmt,flgs,cflgs),sz,p);}
    STATICINLINE void TexSubImageCompressed3D(Texture t,uint32 level,int32 x,int32 y,int32 z,
                                              uint32 w,uint32 h,uint32 d,
                                              PixelFormat fmt, PixelFlags flgs,
                                              CompFlags cflgs,
                                              uint32 sz,c_cptr p)
    {glCompressedTexSubImage3D(to_enum(t),level,x,y,z,w,h,d,to_enum(fmt,flgs,cflgs),sz,p);}

    /* TexCopyImage* */

    STATICINLINE void TexCopyImage2D(Texture t,uint32 level,PixelFormat fmt,int32 x,int32 y,
                                     uint32 w,uint32 h,int32 border)
    {glCopyTexImage2D(to_enum(t),level,to_enum(fmt),x,y,w,h,border);}

    /* TexCopySubImage */

    STATICINLINE void TexCopySubImage2D(Texture t,uint32 level,int32 xo,int32 yo,
                                        int32 x,int32 y,
                                        uint32 w,uint32 h)
    {glCopyTexSubImage2D(to_enum(t),level,xo,yo,x,y,w,h);}
    STATICINLINE void TexCopySubImage3D(Texture t,uint32 level,int32 xo,int32 yo,int32 zo,
                                        int32 x,int32 y,
                                        uint32 w,uint32 h)
    {glCopyTexSubImage3D(to_enum(t),level,xo,yo,zo,x,y,w,h);}

    /* TexGenMipmap */

    STATICINLINE void TexGenMipmap(Texture t)
    {glGenerateMipmap(to_enum(t));}

    /* TexParameter */

    STATICINLINE void TexParameterfv(Texture t, CGenum e, const scalar* v)
    {glTexParameterfv(to_enum(t),e,v);}
    STATICINLINE void TexParameteriv(Texture t, CGenum e, const int32* v)
    {glTexParameteriv(to_enum(t),e,v);}
    STATICINLINE void TexParameteruiv(Texture t, CGenum e, const uint32* v)
    {glTexParameterIuiv(to_enum(t),e,v);}

    /* Samplers */
    STATICINLINE void SamplerBind(uint32 i,CGhnd h)
    {glBindSampler(i,h);}

    /* SamplerParameter */

    STATICINLINE void SamplerParameteri(CGhnd h,CGenum f,
                                         Filtering d1,
                                         Filtering d2 = Filtering::None)
    {glSamplerParameteri(h,f,to_enum(d1,d2));}
    STATICINLINE void SamplerParameteri(CGhnd h,CGenum f,WrapPolicy d)
    {glSamplerParameteri(h,f,to_enum(d));}

    STATICINLINE void SamplerParameterfv(CGhnd h,CGenum f,const scalar* d)
    {glSamplerParameterfv(h,f,d);}
    STATICINLINE void SamplerParameteriv(CGhnd h,CGenum f,const int32* d)
    {glSamplerParameteriv(h,f,d);}
    STATICINLINE void SamplerParameteruiv(CGhnd h,CGenum f,const uint32* d)
    {glSamplerParameterIuiv(h,f,d);}

    /* GetSamplerParameter */

    STATICINLINE void SamplerGetParameteriv(CGhnd h,CGenum f,int32* d)
    {glGetSamplerParameteriv(h,f,d);}
    STATICINLINE void SamplerGetParameterfv(CGhnd h,CGenum f,scalar* d)
    {glGetSamplerParameterfv(h,f,d);}
};

}
}
