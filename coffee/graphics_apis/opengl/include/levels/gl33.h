#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_33_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_33_H

#include "glbase.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL 3.3 compliance model
 */
struct CGL_GL3 : CGL_Implementation
{
    typedef void* CGsync;

    static bool GeometryShaderSupported(){return false;}

    static bool CubemapSeamlessSupported(){return false;}

    /* Base */
    static void Enable(){}
    static void Disable(){}

    static void Clear(){}
    static void ClearColor(){}

    static void ClearDepth(){}
    static void ClearStencil(){}

    static void ClearBuffer(){}
    static void ClearBufferfi(){}

    static void CullMode(){}

    static void BlendMode(){}
    static void BlendFuncSep(){}
    static void BlendEqSep(){}

    static void DepthMode(){}
    static void DepthFuncSep(){}
    static void DepthEqSep(){}

    static void StencilMode(){}
    static void StencilFuncSep(){}
    static void StencilEqSep(){}

    static void ViewportSet(){}
    static void ScissorSet(){}
    static void DepthSet(){}

    static void Flush(){}
    static void Finish(){}

    /* SAMPLE_ALPHA_TO_COVERAGE,SAMPLE_ALPHA_TO_ONE */
    static void SampleCoverage(){}
    static void SampleMaski(){}

    /* Allocations */
    static bool TexAlloc(size_t,CGhnd*){return false;}
    static bool TexFree(size_t,CGhnd*){return false;}

    static bool FBAlloc(size_t,CGhnd*){return false;}
    static bool FBFree(size_t,CGhnd*){return false;}

    static bool XFBAlloc(size_t,CGhnd*){return false;}
    static bool XFBFree(size_t,CGhnd*){return false;}

    static bool RenderBufferAlloc(size_t,CGhnd*){return false;}
    static bool RenderBufferFree(size_t,CGhnd*){return false;}

    static bool SamplerAlloc(size_t,CGhnd*){return false;}
    static bool SamplerFree(size_t,CGhnd*){return false;}

    static bool BufAlloc(size_t,CGhnd*){return false;}
    static bool BufFree(size_t,CGhnd*){return false;}

    static bool VAOAlloc(size_t,CGhnd*){return false;}
    static bool VAOFree(size_t,CGhnd*){return false;}

    static bool QueryAlloc(size_t,CGhnd*){return false;}
    static bool QueryFree(size_t,CGhnd*){return false;}

    static bool ProgramAlloc(size_t,CGhnd*){return false;}
    static bool ProgramFree(size_t,CGhnd*){return false;}

    static bool ShaderAlloc(size_t,CGenum,CGhnd*){return false;}
    static bool ShaderFree(size_t,CGenum,CGhnd*){return false;}

    static bool FenceFree(size_t,CGhnd*){return false;}

    /* Shaders */
    static cstring ShaderGetSrc(){return nullptr;}
    static bool ShaderGetLog(){return false;}
    static void ShaderAttach(){}
    static void ShaderDetach(){}
    static bool ShaderCompile(){return false;}

    static bool ProgramLink(){return false;}
    static void ProgramGetLog(){}
    static void ProgramUse(){}

    /* Attributes */
    static void ProgramAttribGet(){}
    static void ProgramAttribLoc(){}
    static void ProgramAttribBind(){}

    /* Uniforms */
    static void ProgramUnifGetLoc(){}
    static void ProgramUnifGetiv(){}
    static void ProgramUnifGetName(){}

    /* Uniform blocks */
    static void ProgramUnifBlockGetLoc(){}
    static void ProgramUnifBlockGetiv(){}
    static void ProgramUnifBlockGetName(){}

    static void ProgramUnifBlockBinding(){}

    /* Subroutines */
    static void ProgramSubRtGetLoc(){}
    static void ProgramSubRtGetiv(){}
    static void ProgramSubRtGetName(){}
    static void ProgramSubRtBind(){}

    /* Textures */
    static void TexBind(){}
    static void TexActive(){}

    static void TexImage2D(){}
    static void TexImage3D(){}

    static void TexSubImage2D(){}
    static void TexSubImage3D(){}

    static void TexImageCompressed2D(){}
    static void TexImageCompressed3D(){}

    static void TexSubImageCompressed2D(){}
    static void TexSubImageCompressed3D(){}

    static void TexCopyImage2D(){}
    static void TexCopyImage3D(){}

    static void TexCopySubImage2D(){}
    static void TexCopySubImage3D(){}

    static void TexGetImage(){}
    static void TexGetImageCompressed(){}

    static void TexGenMipmap(){}

    /* Samplers */
    static void SamplerBind(){}

    static void SamplerParameteriv(){}
    static void SamplerParameteruiv(){}
    static void SamplerParameterfv(){}

    static void SamplerGetParameteriv(){}
    static void SamplerGetParameteruiv(){}
    static void SamplerGetParameterfv(){}

    /* Buffers */
    static void BufBind(){}

    static void BufData(){}
    static void BufSubData(){}

    static void BufCopySubData(){}

    static void BufBindRange(){}
    static void BufMapRange(){}
    static void BufUnmapRange(){}

    static void BufGetSubData(){}

    /* Queries */
    static void ConditionalRenderBegin(){}
    static void ConditionalRenderEnd(){}

    static void QueryBeginIndexed(){}
    static void QueryEndIndexed(){}

    static void QueryGetIndexediv(){}
    static void QueryGetObjectiv(){}
    static void QueryGetObjectuiv(){}
    static void QueryGetObjecti64v(){}
    static void QueryGetObjectui64v(){}

    /* FB */
    static void FBBind(){}
    static void RBufBind(){}

    static void RBufStorage(){}

    static bool FBCheckComplete(){return false;}

    static void FBGetAttachParameter(){}
    static void FBGetRBufParameter(){}

    static void FBAttachTexture(){}

    static void FBAttachTexture2D(){}
    static void FBAttachTexture3D(){}
    static void FBAttachTextureLayer(){}
    static void FBAttachRenderBuffer(){}

    static void FBBlit(){}
    static void FBClear(){}
    static void FBParameteri(){}

    static void FBDrawBuffers(){}
    static void FBReadBuffer(){}

    static void FBReadPixels(){}
    static void FBCopyPixels(){}

    /* XFB */
    static void XFBind(){}

    static void XFBegin(){}
    static void XFEnd(){}

    static void XFVaryings(){}
    static void XFGetVaryings(){}

    /* Include RASTERIZER_DISCARD */

    /* VAO */
    static void VAOBind(){}

    static void VAOEnableAttrib(){}
    static void VAOAttribPointer(){}
    static void VAOAttribIPointer(){}
    static void VAOAttribLPointer(){}
    static void VAODivisor(){}

    /* Sync */
    static CGsync FenceCreate(){return nullptr;}
    static bool FenceAwait(CGsync,uint64){return false;}
    static bool FenceServerAwait(CGsync,uint64){return false;}

    static void FenceGetiv(){}

    /* Using Uniform* */
    static void Uniform1fv(){}
    static void Uniform2fv(){}
    static void Uniform3fv(){}
    static void Uniform4fv(){}

    static void Uniform1iv(){}
    static void Uniform2iv(){}
    static void Uniform3iv(){}
    static void Uniform4iv(){}

    static void Uniform1uiv(){}
    static void Uniform2uiv(){}
    static void Uniform3uiv(){}
    static void Uniform4uiv(){}

    static void UniformMatrix2fv(){}
    static void UniformMatrix2x3fv(){}
    static void UniformMatrix3fv(){}
    static void UniformMatrix3x2fv(){}
    static void UniformMatrix3x4fv(){}
    static void UniformMatrix4fv(){}
    static void UniformMatrix4x2fv(){}
    static void UniformMatrix4x3fv(){}

    static void UniformMatrix2iv(){}
    static void UniformMatrix2x3iv(){}
    static void UniformMatrix3iv(){}
    static void UniformMatrix3x2iv(){}
    static void UniformMatrix3x4iv(){}
    static void UniformMatrix4iv(){}
    static void UniformMatrix4x2iv(){}
    static void UniformMatrix4x3iv(){}

    static void UniformMatrix2uiv(){}
    static void UniformMatrix2x3uiv(){}
    static void UniformMatrix3uiv(){}
    static void UniformMatrix3x2uiv(){}
    static void UniformMatrix3x4uiv(){}
    static void UniformMatrix4uiv(){}
    static void UniformMatrix4x2uiv(){}
    static void UniformMatrix4x3uiv(){}

    /* Drawing */
    static void DrawArrays(){}
    static void DrawArraysInstanced(){}

    static void DrawElements(){}
    static void DrawElementsInstanced(){}
    static void DrawElementsBaseVertex(){}
    static void DrawElementsInstancedBaseVertex(){}

    static void DrawRangeElementsBaseVertex(){}

    static void DrawMultiArrays(){}

    static void DrawMultiElements(){}
    static void DrawMultiElementsBaseVertex(){}
    static void DrawMultiElementsInstancedBaseVertex(){}
};

}
}

#endif
