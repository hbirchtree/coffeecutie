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
    STATICINLINE bool TexAlloc(CGenum tex_type, Span<CGhnd> const& handles)
    {
        glCreateTextures(
                    tex_type,
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool BufAlloc(Span<CGhnd> const& handles)
    {
        glCreateBuffers(
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool FBAlloc(Span<CGhnd> const& handles)
    {
        glCreateFramebuffers(
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool XFAlloc(Span<CGhnd> const& handles)
    {
        glCreateTransformFeedbacks(
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool RenderBufferAlloc(Span<CGhnd> const& handles)
    {
        glCreateRenderbuffers(
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool SamplerAlloc(Span<CGhnd> const& handles)
    {
        glCreateSamplers(
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool QueryAlloc(QueryT t, Span<CGhnd> const& handles)
    {
        glCreateQueries(
                    to_enum(t),
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool VAOAlloc(Span<CGhnd> const& handles)
    {
        glCreateVertexArrays(
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool PipelineAlloc(Span<CGhnd> const& handles)
    {
        glCreateProgramPipelines(
                    C_FCAST<GLint>(handles.size),
                    handles.data
                    );
        return true;
    }

    /* Textures */
    STATICINLINE bool TexStorage2D(CGhnd,int64,CGenum,int64,int64){return false;}
    STATICINLINE bool TexStorage3D(CGhnd,int64,CGenum,int64,int64){return false;}

    STATICINLINE void TexSubImage2D(CGhnd,int32,int32,int32,
                              int64,int64,CGenum,CGenum,c_cptr){}
    STATICINLINE void TexSubImage3D(CGhnd,int32,int32,int32,int32,
                              int64,int64,int64,CGenum,CGenum,c_cptr){}

    STATICINLINE void TexImageCompressed2D(CGhnd,int32,CGenum,int64,int64,int32,c_cptr){}
    STATICINLINE void TexImageCompressed3D(CGhnd,int32,CGenum,int64,int64,int64,int32,c_cptr){}

    STATICINLINE void TexSubImageCompressed2D(CGhnd,int32,int32,int32,int64,int64,
                                        CGhnd,size_t,c_cptr){}
    STATICINLINE void TexSubImageCompressed3D(CGhnd,int32,int32,int32,int32,int64,int64,int64,
                                        CGhnd,size_t,c_cptr){}

    STATICINLINE void TexCopyImage2D(CGhnd,int32,CGenum,int32,int32,
                               int64,int64,int32){}
    STATICINLINE void TexCopySubImage2D(CGhnd,int32,CGenum,int32,int32,
                                  int32,int32,
                                  int64,int64,int32){}

    STATICINLINE void TexGetImage(CGhnd,int32,CGenum,CGenum,c_ptr){}
    STATICINLINE void TexGetImageCompressed(CGhnd,int32,c_ptr){}

    STATICINLINE void TexGenMipmap(CGhnd t){glGenerateTextureMipmap(t);}

    /* Texture handles/bindless handles */
    STATICINLINE CGhnd64 TexGetImageHandle(CGhnd,int32,bool,int32,CGenum){return 0;}

    STATICINLINE void TexMakeHandleResident(CGhnd64 h){}
    STATICINLINE void TexMakeHandleNonResident(CGhnd64 h){}

    STATICINLINE void ImageBindTextures(u32 f,Span<CGhnd> const& handles)
    {glBindImageTextures(f,C_FCAST<i32>(handles.size),handles.data);}

    STATICINLINE void ImgMakeHandleResident(CGhnd64 h,CGenum f){}
    STATICINLINE void ImgMakeHandleNonResident(CGhnd64 h){}

    STATICINLINE bool IsTexHandleResident(CGhnd64 h){return false;}
    STATICINLINE bool IsImgHandleResident(CGhnd64 h){return false;}

    /* Samplers */
    STATICINLINE CGhnd64 TexGetSamplerHandle(CGhnd,CGhnd){return 0;}

    /* Uniforms */
    STATICINLINE void UniformHandlei64(CGhnd,int32,CGhnd64);
    STATICINLINE void UniformHandlei64v(CGhnd,int32,size_t,const CGhnd64*);

    /* Buffers */
    STATICINLINE void BufStorage(CGhnd,int64,c_cptr,CGenum){}

    STATICINLINE void BufData(CGenum,size_t,c_cptr,CGenum){}
    STATICINLINE void BufSubData(CGenum,size_t,size_t,c_cptr){}
    STATICINLINE void BufGetSubData(CGenum,size_t,size_t,c_ptr){}
    STATICINLINE void BufCopySubData(CGenum,CGenum,size_t,size_t,size_t){}

    STATICINLINE void BufBindRange(CGenum,uint32,size_t,size_t){}
    STATICINLINE void BufMapRange(CGenum,size_t,size_t,CGenum){}
    STATICINLINE void BufUnmap(CGenum){}

    /* FB */
    STATICINLINE bool FBValidate(CGhnd){return false;}

    /* VAO */
    STATICINLINE void VAOAttribFormat(CGhnd,uint32,int32,CGenum,bool,uint32){}
    STATICINLINE void VAOAttribFormatI(CGhnd,uint32,int32,CGenum,uint32){}
    STATICINLINE void VAOAttribFormatL(CGhnd,uint32,int32,CGenum,uint32){}

    STATICINLINE void VAOBindingDivisor(CGhnd,uint32,uint32){}
    STATICINLINE void VAOAttribBinding(CGhnd,uint32,uint32){}

    STATICINLINE void VAOElementBuffer(CGhnd,CGhnd){}

    STATICINLINE void VAOBindVertexBuffer(CGhnd,uint32,CGhnd,int64,int64){}
    STATICINLINE void VAOBindVertexBuffers(CGhnd,uint32,CGhnd,int64,int64){}

    /* Queries */
    STATICINLINE void QueryBufferGetiv(CGhnd,CGhnd,CGenum,int64){}
    STATICINLINE void QueryBufferGeti64v(CGhnd,CGhnd,CGenum,int64){}
    STATICINLINE void QueryBufferGetui64v(CGhnd,CGhnd,CGenum,int64){}
    STATICINLINE void QueryBufferGetuiv(CGhnd,CGhnd,CGenum,int64){}

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
