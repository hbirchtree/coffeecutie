#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_45_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_45_H

#include "gl43.h"

namespace Coffee{
namespace CGL{

/*!
 * \brief OpenGL 4.5 compliance model
 */
struct CGL_GL45 : CGL_GL43
{
    typedef CGuint64 CGhnd64;

    static bool DirectStateSupported(){return false;}

    static bool BindlessTextureSupported(){return false;}

    static bool SparseTextureSupported(){return false;}

    static bool ImageStoreLoadSupported(){return false;}

    /*
     * Most functions here should use DSA
     *
     */

    /* Allocations */
    static bool TexAlloc(size_t,CGenum,CGhnd*){return false;}

    static bool BufAlloc(size_t,CGhnd*){return false;}

    static bool FBAlloc(size_t,CGenum,CGhnd*){return false;}

    static bool XFBAlloc(size_t,CGhnd*){return false;}

    static bool RenderBufferAlloc(size_t,CGhnd*){return false;}

    static bool SamplerAlloc(size_t,CGhnd*){return false;}

    static bool QueryAlloc(size_t,CGenum,CGhnd*){return false;}

    static bool VAOAlloc(size_t,CGhnd*){return false;}

    /* Textures */
    static void TexBind(){}

    static void TexImage2D(){}
    static void TexImage3D(){}

    static void TexSubImage2D(){}
    static void TexSubImage3D(){}

    static bool TexGetImage(){return false;}

    /* Samplers */
    static void SamplerBind(){}

    static void SamplerParameteri(){}
    static void SamplerParameterf(){}

    /* Buffers */
    static void BufBind(){}

    static void BufStorage(){}

    /* FB */
    static void FBBind(){}

    static bool FBCheckComplete(){return false;}

    /* VAO */
    static void VAOBind(){}

    static void VAOAttribFormat(){}
    static void VAOAttribFormatI(){}
    static void VAOAttribFormatL(){}

    static void VAOElementBuffer(){}

    static void VAOBindingDivisor(){}
    static void VAOVertexBuffer(){}
    static void VAOVertexBuffers(){}

    /* XFB */
    static void XFBind(){}
};

}
}

#endif
