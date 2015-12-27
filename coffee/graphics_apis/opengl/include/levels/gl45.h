#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_45_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_45_H

#include "gl43.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL 4.5 compliance model
 */
struct CGL45 : CGL43
{
    using CGhnd64 = uint64;

    static bool DirectStateSupported(){return false;}

    static bool BindlessTextureSupported(){return false;}

    static bool SparseTextureSupported(){return false;}

    static bool ImageStoreLoadSupported(){return false;}

    /*
     * Most functions here should use DSA
     *
     */

    /* Allocations, initializing variants */
    static bool TexAlloc(size_t,CGenum,CGhnd*){return false;}

    static bool BufAlloc(size_t,CGhnd*){return false;}

    static bool FBAlloc(size_t,CGenum,CGhnd*){return false;}

    static bool XFAlloc(size_t,CGhnd*){return false;}

    static bool RenderBufferAlloc(size_t,CGhnd*){return false;}

    static bool SamplerAlloc(size_t,CGhnd*){return false;}

    static bool QueryAlloc(size_t,CGenum,CGhnd*){return false;}

    static bool VAOAlloc(size_t,CGhnd*){return false;}

    static bool PipelineAlloc(size_t,CGhnd*){return false;}

    /* Textures */
    static bool TexStorage2D(CGhnd,int64,CGenum,int64,int64){return false;}
    static bool TexStorage3D(CGhnd,int64,CGenum,int64,int64){return false;}

    static void TexSubImage2D(CGhnd,int32,int32,int32,
                              int64,int64,CGenum,CGenum,c_cptr){}
    static void TexSubImage3D(CGhnd,int32,int32,int32,int32,
                              int64,int64,int64,CGenum,CGenum,c_cptr){}

    static void TexImageCompressed2D(CGhnd,int32,CGenum,int64,int64,int32,c_cptr){}
    static void TexImageCompressed3D(CGhnd,int32,CGenum,int64,int64,int64,int32,c_cptr){}

    static void TexSubImageCompressed2D(CGhnd,int32,int32,int32,int64,int64,
                                        CGhnd,size_t,c_cptr){}
    static void TexSubImageCompressed3D(CGhnd,int32,int32,int32,int32,int64,int64,int64,
                                        CGhnd,size_t,c_cptr){}

    static void TexCopyImage2D(CGhnd,int32,CGenum,int32,int32,
                               int64,int64,int32){}
    static void TexCopySubImage2D(CGhnd,int32,CGenum,int32,int32,
                                  int32,int32,
                                  int64,int64,int32){}

    static void TexGetImage(CGhnd,int32,CGenum,CGenum,c_ptr){}
    static void TexGetImageCompressed(CGhnd,int32,c_ptr){}

    static void TexGenMipmap(CGhnd){}

    /* Texture handles/bindless handles */
    static CGhnd64 TexGetImageHandle(CGhnd,int32,bool,int32,CGenum){return 0;}

    static void TexMakeHandleResident(CGhnd64){}
    static void TexMakeHandleNonResident(CGhnd64){}

    static void ImgMakeHandleResident(CGhnd64,CGenum){}
    static void ImgMakeHandleNonResident(CGhnd64){}

    static bool IsTexHandleResident(CGhnd64){return false;}
    static bool IsImgHandleResident(CGhnd64){return false;}

    /* Samplers */
    static CGhnd64 TexGetSamplerHandle(CGhnd,CGhnd){return 0;}

    /* Uniforms */
    static void UniformHandlei64(CGhnd,int32,CGhnd64);
    static void UniformHandlei64v(CGhnd,int32,size_t,const CGhnd64*);

    /* Buffers */
    static void BufStorage(CGhnd,int64,c_cptr,CGenum){}

    static void BufData(CGenum,size_t,c_cptr,CGenum){}
    static void BufSubData(CGenum,size_t,size_t,c_cptr){}
    static void BufGetSubData(CGenum,size_t,size_t,c_ptr){}
    static void BufCopySubData(CGenum,CGenum,size_t,size_t,size_t){}

    static void BufBindRange(CGenum,uint32,size_t,size_t){}
    static void BufMapRange(CGenum,size_t,size_t,CGenum){}
    static void BufUnmap(CGenum){}

    /* FB */
    static bool FBCheckComplete(CGhnd){return false;}

    /* VAO */
    static void VAOAttribFormat(CGhnd,uint32,int32,CGenum,bool,uint32){}
    static void VAOAttribFormatI(CGhnd,uint32,int32,CGenum,uint32){}
    static void VAOAttribFormatL(CGhnd,uint32,int32,CGenum,uint32){}

    static void VAOBindingDivisor(CGhnd,uint32,uint32){}
    static void VAOAttribBinding(CGhnd,uint32,uint32){}

    static void VAOElementBuffer(CGhnd,CGhnd){}

    static void VAOBindVertexBuffer(CGhnd,uint32,CGhnd,int64,int64){}
    static void VAOBindVertexBuffers(CGhnd,uint32,CGhnd,int64,int64){}

    /* Queries */
    static void QueryBufferGetiv(CGhnd,CGhnd,CGenum,int64){}
    static void QueryBufferGeti64v(CGhnd,CGhnd,CGenum,int64){}
    static void QueryBufferGetui64v(CGhnd,CGhnd,CGenum,int64){}
    static void QueryBufferGetuiv(CGhnd,CGhnd,CGenum,int64){}

    /* DSA disabling */
    static void FBBind(){}
    static void BufBind(){}
    static void TexBind(){}
    static void SamplerBind(){}
    static void XFBind(){}
};

}
}

#endif
