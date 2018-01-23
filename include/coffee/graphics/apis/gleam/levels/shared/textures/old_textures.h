#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/cdef/memtypes.h>

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
    {glTexImage2D(to_enum(t),level,to_enum(ifmt),w,h,border,to_enum(fmt, ifmt),to_enum(dt),p);}
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void TexImage3D(Texture t,uint32 level,PixelFormat ifmt,
                                 uint32 w,uint32 h,uint32 d,int32 border,
                                 PixelComponents fmt,BitFormat dt,c_cptr p)
    {glTexImage3D(to_enum(t),level,to_enum(ifmt),w,h,d,border,to_enum(fmt, ifmt),to_enum(dt),p);}
#endif

    /* TexGet */
    STATICINLINE void TexGetImageSize(Texture t, PixCmp c,
                                      uint32& w, uint32& h, uint32& d,
                                      szptr* size, uint32 mip = 0)
    {
#ifdef COFFEE_GLEAM_DESKTOP
        int32 w_t,h_t,d_t;
        TexGetLevelParameteriv(t,mip,GL_TEXTURE_WIDTH,&w_t);
        TexGetLevelParameteriv(t,mip,GL_TEXTURE_HEIGHT,&h_t);
        TexGetLevelParameteriv(t,mip,GL_TEXTURE_DEPTH,&d_t);
        w = w_t, h = h_t, d = d_t;

        if(!size)
            return;

        szptr cmp_size = 0;

        switch(c)
        {
        case PixCmp::RGBA:
            cmp_size += TexGetLevelParameteri(t,mip,GL_TEXTURE_ALPHA_SIZE)/8;
        case PixCmp::RGB:
            cmp_size += TexGetLevelParameteri(t,mip,GL_TEXTURE_BLUE_SIZE)/8;
        case PixCmp::RG:
            cmp_size += TexGetLevelParameteri(t,mip,GL_TEXTURE_GREEN_SIZE)/8;
        case PixCmp::R:
        case PixCmp::G:
        case PixCmp::B:
            cmp_size += TexGetLevelParameteri(t,mip,GL_TEXTURE_RED_SIZE)/8;
            break;
        case PixCmp::BGRA:
            cmp_size += TexGetLevelParameteri(t,mip,GL_TEXTURE_ALPHA_SIZE)/8;
            break;
        case PixCmp::DepthStencil:
            cmp_size += TexGetLevelParameteri(t,mip,GL_TEXTURE_STENCIL_SIZE)/8;
        case PixCmp::Depth:
            cmp_size += TexGetLevelParameteri(t,mip,GL_TEXTURE_DEPTH_SIZE)/8;
            break;
        case PixCmp::Stencil:
            cmp_size += TexGetLevelParameteri(t,mip,GL_TEXTURE_STENCIL_SIZE)/8;
            break;
        default:
            cmp_size = 4;
            break;
        }

        *size = w * h * d * cmp_size;
#else
        C_UNUSED(t);
        C_UNUSED(c);
        C_UNUSED(mip);
        w = 0, h = 0, d = 0;
        *size = 0;
#endif
    }
#ifdef COFFEE_GLEAM_DESKTOP
    STATICINLINE void TexGetCompressedSize(
            Texture t, uint32& w, uint32& h, uint32& d, szptr& size)
    {
	TexGetParameteruiv(t,GL_TEXTURE_WIDTH,&w);
	TexGetParameteruiv(t,GL_TEXTURE_HEIGHT,&h);
	TexGetParameteruiv(t,GL_TEXTURE_DEPTH,&d);

	/*TODO: Calculate compressed size*/
	size = w * h * d * (1);
    }
#endif

#ifdef COFFEE_GLEAM_DESKTOP
    STATICINLINE void TexGetImage(
            Texture t, uint32 level, PixelComponents fmt,
            PixelFormat outfmt, Bytes& data)
    {
    glGetTexImage(to_enum(t),level,
                  to_enum(fmt, outfmt),to_enum(outfmt),data.data);
    }
    STATICINLINE void TexGetCompressedImage(
            Texture t, uint32 level, Bytes& data)
    {
	glGetCompressedTexImage(to_enum(t),level,data.data);
    }
#endif

    /* TexSubImage */

    STATICINLINE void TexSubImage2D(
            Texture t,uint32 level,int32 x,int32 y,
            uint32 w,uint32 h,PixFmt fmt,BitFormat dt,c_cptr p)
    {glTexSubImage2D(to_enum(t),level,x,y,w,h,
                     to_enum(GetPixComponent(fmt), fmt),to_enum(dt),p);}
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void TexSubImage3D(
            Texture t,uint32 level,int32 x,int32 y,int32 z,
            uint32 w,uint32 h,uint32 d,PixFmt fmt,
            BitFormat dt,c_cptr p)
    {glTexSubImage3D(to_enum(t),level,x,y,z,w,h,d,
                     to_enum(GetPixComponent(fmt), fmt),to_enum(dt),p);}
#endif

    /* TexImageCompressed */

    STATICINLINE void TexImageCompressed2D(
            Texture t,int32 level,
            PixelFormat ifmt, PixelFlags flgs, CompFlags cflgs,
            uint32 w,uint32 h,int32 border,uint32 sz,c_cptr p)
    {glCompressedTexImage2D(to_enum(t),level,to_enum(ifmt,flgs,cflgs),w,h,border,sz,p);}
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void TexImageCompressed3D(Texture t,int32 level,
                                           PixelFormat ifmt, PixelFlags flgs,
                                           CompFlags cflgs,
                                           uint32 w,uint32 h,uint32 d,int32 border,uint32 sz,c_cptr p)
    {glCompressedTexImage3D(to_enum(t),level,to_enum(ifmt,flgs,cflgs),w,h,d,border,sz,p);}
#endif

    /* TexSubImageCompressed */

    STATICINLINE void TexSubImageCompressed2D(Texture t,uint32 level,int32 x,int32 y,
                                              uint32 w,uint32 h,
                                              PixelFormat fmt, PixelFlags flgs,
                                              CompFlags cflgs,
                                              int32 sz,c_cptr p)
    {glCompressedTexSubImage2D(to_enum(t),level,x,y,w,h,to_enum(fmt,flgs,cflgs),sz,p);}
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void TexSubImageCompressed3D(Texture t,uint32 level,int32 x,int32 y,int32 z,
                                              uint32 w,uint32 h,uint32 d,
                                              PixelFormat fmt, PixelFlags flgs,
                                              CompFlags cflgs,
                                              uint32 sz,c_cptr p)
    {glCompressedTexSubImage3D(to_enum(t),level,x,y,z,w,h,d,to_enum(fmt,flgs,cflgs),sz,p);}
#endif

    /* TexCopyImage* */

    STATICINLINE void TexCopyImage2D(Texture t,uint32 level,PixelFormat fmt,int32 x,int32 y,
                                     uint32 w,uint32 h,int32 border)
    {glCopyTexImage2D(to_enum(t),level,to_enum(fmt),x,y,w,h,border);}

    /* TexCopySubImage */

    STATICINLINE void TexCopySubImage2D(Texture t,uint32 level,int32 xo,int32 yo,
                                        int32 x,int32 y,
                                        uint32 w,uint32 h)
    {glCopyTexSubImage2D(to_enum(t),level,xo,yo,x,y,w,h);}
#if !defined(COFFEE_ONLY_GLES20)
    STATICINLINE void TexCopySubImage3D(Texture t,uint32 level,int32 xo,int32 yo,int32 zo,
                                        int32 x,int32 y,
                                        uint32 w,uint32 h)
    {glCopyTexSubImage3D(to_enum(t),level,xo,yo,zo,x,y,w,h);}
#endif

    /* TexGenMipmap */

    STATICINLINE void TexGenMipmap(Texture t)
    {glGenerateMipmap(to_enum(t));}

    /* TexParameter */

    STATICINLINE void TexGetParameteriv(Texture t, CGenum f, int32* v)
    {glGetTexParameteriv(to_enum(t),f,v);}
    STATICINLINE void TexGetParameterfv(Texture t, CGenum f, scalar* v)
    {glGetTexParameteriv(to_enum(t),f,v);}
    STATICINLINE void TexGetParameteruiv(Texture t, CGenum f, uint32* v)
    {glGetTexParameterIuiv(to_enum(t),f,v);}
    STATICINLINE uint32 TexGetParameterui(Texture t, CGenum f)
    {
        uint32 v;
        glGetTexParameterIuiv(to_enum(t),f,&v);
        return v;
    }

    STATICINLINE void TexGetLevelParameteriv(Texture t, uint32 l, CGenum f, int32* v)
    {glGetTexLevelParameteriv(to_enum(t),l,f,v);}
    STATICINLINE int32 TexGetLevelParameteri(Texture t, uint32 l, CGenum f)
    {
        int32 v = 0;
        glGetTexLevelParameteriv(to_enum(t),l,f,&v);
        return v;
    }
    STATICINLINE void TexGetLevelParameterfv(Texture t, uint32 l, CGenum f, scalar* v)
    {glGetTexLevelParameteriv(to_enum(t),l,f,v);}

    STATICINLINE void TexParameterfv(Texture t, CGenum e, const scalar* v)
    {glTexParameterfv(to_enum(t),e,v);}
    STATICINLINE void TexParameteriv(Texture t, CGenum e, const int32* v)
    {glTexParameteriv(to_enum(t),e,v);}
    STATICINLINE void TexParameteruiv(Texture t, CGenum e, const uint32* v)
    {glTexParameterIuiv(to_enum(t),e,v);}


#if !defined(COFFEE_ONLY_GLES20)
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
    STATICINLINE void SamplerGetParameteruiv(CGhnd h,CGenum f,uint32* d)
    {glGetSamplerParameterIuiv(h,f,d);}
#endif
};

}
}
