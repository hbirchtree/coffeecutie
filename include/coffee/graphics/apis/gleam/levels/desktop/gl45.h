#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_45_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_45_H

#include "gl43.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL 4.5 compliance model
 */
template<typename ReqVer>
struct CGL45_Base : CGL43_Base<ReqVer>
{
    using Debug = typename CGL43::Debug;

    STATICINLINE bool DirectStateSupported()
    {
        return Debug::CheckExtensionSupported("GL_ARB_direct_state_access");
    }

    STATICINLINE bool BindlessTextureSupported(){return false;}

    STATICINLINE bool SparseTextureSupported(){return false;}

    STATICINLINE bool ImageStoreLoadSupported(){return false;}

    STATICINLINE bool CullDistanceSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_cull_distance");}

    /*
     * Most functions here should use DSA
     *
     */
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void MemoryBarrierByRegion(GLbitfield f){glMemoryBarrierByRegion(f);}

    /* Allocations, initializing variants */

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool TexAlloc(Texture tex_type, Span<CGhnd> const& handles)
    {
        glCreateTextures(
                    to_enum(tex_type),
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool BufAlloc(Span<CGhnd> const& handles)
    {
        glCreateBuffers(
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool FBAlloc(Span<CGhnd> const& handles)
    {
        glCreateFramebuffers(
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool XFAlloc(Span<CGhnd> const& handles)
    {
        glCreateTransformFeedbacks(
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool RenderBufferAlloc(Span<CGhnd> const& handles)
    {
        glCreateRenderbuffers(
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool SamplerAlloc(Span<CGhnd> const& handles)
    {
        glCreateSamplers(
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool QueryAlloc(QueryT t, Span<CGhnd> const& handles)
    {
        glCreateQueries(
                    to_enum(t),
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool VAOAlloc(Span<CGhnd> const& handles)
    {
        glCreateVertexArrays(
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool PipelineAlloc(Span<CGhnd> const& handles)
    {
        glCreateProgramPipelines(
                    C_FCAST<GLint>(handles.elements),
                    handles.data
                    );
        return true;
    }

    /* Textures */
    GL_VERSION_REQ(GLVER_44)
    STATICINLINE bool TexStorage2D(CGhnd hnd,
                                   i32 levels,
                                   PixFmt ifmt,
                                   i32 w,
                                   i32 h
                                   )
    {
        glTextureStorage2D(hnd, levels, to_enum(ifmt), w, h);

        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool TexStorage3D(CGhnd hnd,
                                   i32 levels,
                                   PixFmt ifmt,
                                   i32 w,
                                   i32 h,
                                   i32 d)
    {
        glTextureStorage3D(hnd, levels, to_enum(ifmt), w, h, d);

        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexSubImage2D(CGhnd hnd,
                                    i32 level,
                                    i32 x,i32 y,
                                    i32 w,i32 h,
                                    PixFmt fmt,
                                    BitFmt bfmt,
                                    c_cptr data)
    {
        glTextureSubImage2D(hnd, level,
                            x, y, w, h,
                            to_enum(fmt), to_enum(bfmt), data);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexSubImage3D(CGhnd hnd, i32 level,
                                    i32 x,i32 y,i32 z,
                                    i32 w,i32 h,i32 d,
                                    PixFmt fmt, BitFmt bfmt, c_cptr data)
    {
        glTextureSubImage3D(hnd, level,
                            x, y, z, w, h, d,
                            to_enum(fmt), to_enum(bfmt), data);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexSubImageCompressed2D(CGhnd hnd, i32 level,
                                              i32 x,i32 y,
                                              i32 w,i32 h,
                                              CompFmt cfmt,
                                              i32 data_size,c_cptr data)
    {
        glCompressedTextureSubImage2D(hnd, level,
                                      x, y, w, h,
                                      to_enum(cfmt.base_fmt,
                                              cfmt.p_flags,
                                              cfmt.c_flags),
                                      data_size, data);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexSubImageCompressed3D(CGhnd hnd, i32 level,
                                              i32 x,i32 y, i32 z,
                                              i32 w,i32 h, i32 d,
                                              CompFmt cfmt,
                                              i32 data_size,c_cptr data)
    {
        glCompressedTextureSubImage3D(hnd, level,
                                      x, y, z, w, h,d,
                                      to_enum(cfmt.base_fmt,
                                              cfmt.p_flags,
                                              cfmt.c_flags),
                                      data_size, data);
    }

    STATICINLINE void TexCopyImage2D(CGhnd,i32,CGenum,i32,i32,
                               i64,i64,i32){}
    STATICINLINE void TexCopySubImage2D(CGhnd,i32,CGenum,i32,i32,
                                  i32,i32,
                                  i64,i64,i32){}

    STATICINLINE void TexGetImage(CGhnd,i32,CGenum,CGenum,c_ptr){}
    STATICINLINE void TexGetImageCompressed(CGhnd,i32,c_ptr){}

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexGenMipmap(CGhnd t)
    {
        glGenerateTextureMipmap(t);
    }

    GL_VERSION_REQ(GLVER_44)
    STATICINLINE void ImageBindTextures(u32 f,Span<CGhnd> const& handles)
    {
        glBindImageTextures(f,C_FCAST<i32>(handles.size),handles.data);
    }

    /* Texture handles/bindless handles */
    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE CGhnd64 TexGetImageHandle(CGhnd hnd,i32 level,
                                           bool layered,i32 layer,
                                           PixFmt fmt)
    {
        return glGetImageHandleARB(hnd, level, layered, layer, to_enum(fmt));
    }

    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE void TexMakeHandleResident(CGhnd64 h)
    {
        glMakeTextureHandleResidentARB(h);
    }

    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE void TexMakeHandleNonResident(CGhnd64 h)
    {
        glMakeTextureHandleNonResidentARB(h);
    }

    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE void ImgMakeHandleResident(CGhnd64 h,PixFmt fmt)
    {
        glMakeImageHandleResidentARB(h, to_enum(fmt));
    }
    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE void ImgMakeHandleNonResident(CGhnd64 h){}

    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE bool IsTexHandleResident(CGhnd64 h){return false;}
    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE bool IsImgHandleResident(CGhnd64 h){return false;}

    /* Samplers */
    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE CGhnd64 TexGetSamplerHandle(CGhnd,CGhnd){return 0;}

    /* Uniforms */
    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE void UniformHandlei64(CGhnd,i32,CGhnd64);
    GL_EXTENSION_REQ("GL_ARB_bindless_texture")
    STATICINLINE void UniformHandlei64v(CGhnd,i32,size_t,const CGhnd64*);

    /* Buffers */
    STATICINLINE void BufStorage(CGhnd h, Bytes const& data, RSCA storage)
    {
        glNamedBufferStorage(
                    h,
                    C_FCAST<i64>(data.size), data.data,
                    to_enum2(storage)
                    );
    }

    STATICINLINE void BufData(CGenum,size_t,c_cptr,CGenum){}
    STATICINLINE void BufSubData(CGenum,size_t,size_t,c_cptr){}
    STATICINLINE void BufGetSubData(CGenum,size_t,size_t,c_ptr){}
    STATICINLINE void BufCopySubData(CGenum,CGenum,size_t,size_t,size_t){}

    STATICINLINE void BufBindRange(CGenum,u32,size_t,size_t){}
    STATICINLINE void BufMapRange(CGenum,size_t,size_t,CGenum){}
    STATICINLINE void BufUnmap(CGenum){}

    /* FB */
    STATICINLINE bool FBValidate(CGhnd){return false;}

    /* VAO */
    STATICINLINE void VAOAttribFormat(CGhnd,u32,i32,CGenum,bool,u32){}
    STATICINLINE void VAOAttribFormatI(CGhnd,u32,i32,CGenum,u32){}
    STATICINLINE void VAOAttribFormatL(CGhnd,u32,i32,CGenum,u32){}

    STATICINLINE void VAOBindingDivisor(CGhnd,u32,u32){}
    STATICINLINE void VAOAttribBinding(CGhnd,u32,u32){}

    STATICINLINE void VAOElementBuffer(CGhnd,CGhnd){}

    STATICINLINE void VAOBindVertexBuffer(CGhnd,u32,CGhnd,i64,i64){}
    STATICINLINE void VAOBindVertexBuffers(CGhnd,u32,CGhnd,i64,i64){}

    /* Queries */
    STATICINLINE void QueryBufferGetiv(CGhnd,CGhnd,CGenum,i64){}
    STATICINLINE void QueryBufferGeti64v(CGhnd,CGhnd,CGenum,i64){}
    STATICINLINE void QueryBufferGetui64v(CGhnd,CGhnd,CGenum,i64){}
    STATICINLINE void QueryBufferGetuiv(CGhnd,CGhnd,CGenum,i64){}

    /* DSA disabling */
    STATICINLINE void FBBind(){}
    STATICINLINE void TexBind(){}
    STATICINLINE void SamplerBind(){}
    STATICINLINE void XFBind(){}
};

using CGL45 = CGL45_Base<GLVER_45>;

}
}

#endif
