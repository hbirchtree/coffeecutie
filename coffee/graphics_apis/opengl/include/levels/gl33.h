#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_33_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_33_H

#include "glbase.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL 3.3 compliance model
 */
struct CGL33 : CGL_Implementation
{
    typedef GLsync CGsync;

    static bool GeometryShaderSupported(){return false;}

    static bool CubemapSeamlessSupported(){return false;}

    /* Base */
    static void Enable(CGenum){}
    static void Disable(CGenum){}

    static void Clear(CGenum){}
    static void ClearColor(CVec4){}

    static void ClearDepth(scalar){}
    static void ClearStencil(scalar){}

    static void ClearBufferiv(CGenum,int32,const int32*){}
    static void ClearBufferuiv(CGenum,int32,const uint32*){}
    static void ClearBufferfv(CGenum,int32,const scalar*){}
    static void ClearBufferfi(CGenum,int32,scalar,scalar){}

    static void FrontFace(Face){}
    static void CullMode(Face){}

    static void BlendFuncSep(CGenum,CGenum,CGenum,CGenum){}
    static void BlendEqSep(CGenum,CGenum){}

    static void DepthFunc(CGenum){}
    static void DepthMask(bool){}

    static void StencilFuncSep(CGenum,CGenum,int32,uint32){}
    static void StencilOpSep(CGenum,CGenum,CGenum,CGenum){}
    static void StencilMaskSep(CGenum,uint32){}

    static void ViewportSet(CRect64 const*){}
    static void ScissorSet(CRect64 const*){}
    static void DepthSet(CZField64 const*){}

    static void Flush(){}
    static void Finish(){}

    static void PointParameteriv(CGenum,const int32*){}
    static void PointParameterfv(CGenum,const scalar*){}
    static void PointSize(scalar){}

    static void LineWidth(scalar){}

    static void PolyMode(CGenum,CGenum){}
    static void PolyOffset(scalar,scalar){}

    /* SAMPLE_ALPHA_TO_COVERAGE,SAMPLE_ALPHA_TO_ONE */
    static void SampleCoverage(scalar,bool){}
    static void SampleMaski(uint32,CGenum){}

    /* Allocations */
    static bool TexAlloc(size_t,CGhnd*){return false;}
    static bool TexFree(size_t,CGhnd*){return false;}

    static bool FBAlloc(size_t,CGhnd*){return false;}
    static bool FBFree(size_t,CGhnd*){return false;}

    static bool XFAlloc(size_t,CGhnd*){return false;}
    static bool XFFree(size_t,CGhnd*){return false;}

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
    static cstring_w ShaderGetSrc(CGhnd){return nullptr;}
    static void ShaderSource(CGhnd,size_t,cstring const*){}
    static bool ShaderGetLog(CGhnd){return false;}
    static void ShaderAttach(CGhnd,CGhnd){}
    static void ShaderDetach(CGhnd,CGhnd){}
    static bool ShaderCompile(CGhnd){return false;}

    static void ProgramGetAttached(CGhnd,size_t,CGhnd*){}
    static bool ProgramLink(CGhnd){return false;}
    static cstring_w ProgramGetLog(CGhnd){return nullptr;}
    static void ProgramUse(CGhnd){}
    static bool ProgramValidate(CGhnd){return false;}
    static void ProgramGetiv(CGhnd,CGenum,int32*){}

    static int32 ProgramGetFragDataLoc(CGhnd,cstring){return 0;}
    static int32 ProgramGetFragDataIdx(CGhnd,cstring){return 0;}
    static void ProgramBindFragData(CGhnd,uint32,cstring){}
    static void ProgramBindFragDataIndexed(CGhnd,uint32,uint32,cstring){}

    /* Attributes */
    static void ProgramAttribGet(CGhnd,size_t*,cstring_w*,CGenum*,uint32*){}
    static int32 ProgramAttribLoc(CGhnd,cstring){return 0;}
    static void ProgramAttribBind(CGhnd,uint32,cstring){}

    /* Uniforms */
    static void ProgramUnifGet(CGhnd,size_t*,cstring_w*,CGenum*,uint32*){}
    static int32 ProgramUnifGetLoc(CGhnd,cstring){return 0;}

    /* Uniform blocks */
    /* Return name, binding, active uniform indices and size */
    static void ProgramUnifBlockGet(CGhnd,size_t*,cstring_w*,int32*,int32**,int32*){}
    static uint32 ProgramUnifBlockGetLoc(CGhnd,cstring){return 0;}
    static void ProgramUnifBlockBind(CGhnd,uint32,uint32){}

    /* Subroutines */
    static void ProgramSubRtGet(CGhnd,size_t*,cstring_w*,int32*,int32*){}
    static void ProgramSubRtGetLoc(CGhnd,CGenum,cstring){}
    /* Binds all subroutine uniforms */
    static void ProgramSubRtBind(CGenum,size_t,uint32*){}

    /* Textures */
    static void TexBind(CGenum,CGhnd){}
    static void TexActive(uint32){}

    static void TexImage2D(CGenum,int32,CGenum,int64,int64,int32,CGenum,CGenum,c_cptr){}
    static void TexImage3D(CGenum,int32,CGenum,int64,int64,int64,int32,CGenum,CGenum,c_cptr){}

    static void TexSubImage2D(CGenum,int32,int32,int32,
                              int64,int64,CGenum,CGenum,c_cptr){}
    static void TexSubImage3D(CGenum,int32,int32,int32,int32,
                              int64,int64,int64,CGenum,CGenum,c_cptr){}

    static void TexImageCompressed2D(CGenum,int32,CGenum,int64,int64,int32,c_cptr){}
    static void TexImageCompressed3D(CGenum,int32,CGenum,int64,int64,int64,int32,c_cptr){}

    static void TexSubImageCompressed2D(CGenum,int32,int32,int32,int64,int64,
                                        CGenum,size_t,c_cptr){}
    static void TexSubImageCompressed3D(CGenum,int32,int32,int32,int32,int64,int64,int64,
                                        CGenum,size_t,c_cptr){}

    static void TexCopyImage2D(CGenum,int32,CGenum,int32,int32,
                               int64,int64,int32){}
    static void TexCopySubImage2D(CGenum,int32,CGenum,int32,int32,
                                  int32,int32,
                                  int64,int64,int32){}

    static void TexGetImage(CGenum,int32,CGenum,CGenum,c_ptr){}
    static void TexGetImageCompressed(CGenum,int32,c_ptr){}

    static void TexGenMipmap(CGenum){}

    /* Samplers */
    static void SamplerBind(CGhnd){}

    static void SamplerParameteriv(CGhnd,CGenum,const int32*){}
    static void SamplerParameteruiv(CGhnd,CGenum,const uint32*){}
    static void SamplerParameterfv(CGhnd,CGenum,const scalar*){}

    static void SamplerGetParameteriv(CGhnd,CGenum,int32*){}
    static void SamplerGetParameteruiv(CGhnd,CGenum,uint32*){}
    static void SamplerGetParameterfv(CGhnd,CGenum,scalar*){}

    /* Buffers */
    static void BufBind(CGenum,CGhnd){}

    static void BufData(CGenum,size_t,c_cptr,CGenum){}
    static void BufSubData(CGenum,size_t,size_t,c_cptr){}
    static void BufGetSubData(CGenum,size_t,size_t,c_ptr){}
    static void BufCopySubData(CGenum,CGenum,size_t,size_t,size_t){}

    static void BufBindRange(CGenum,uint32,size_t,size_t){}
    static void BufMapRange(CGenum,size_t,size_t,CGenum){}
    static void BufUnmap(CGenum){}

    /* Queries */
    static void ConditionalRenderBegin(CGhnd,CGenum){}
    static void ConditionalRenderEnd(){}

    static void QueryBegin(CGenum,CGhnd){}
    static void QueryEnd(CGenum){}

    static void QueryGetObjectiv(CGenum,CGenum,int32*){}
    static void QueryGetObjectuiv(CGenum,CGenum,uint32*){}
    static void QueryGetObjecti64v(CGenum,CGenum,int64*){}
    static void QueryGetObjectui64v(CGenum,CGenum,uint64*){}

    static void QueryCounter(CGhnd,CGenum){}

    /* FB */
    static void FBBind(CGenum,CGhnd){}
    static void RBufBind(CGenum,CGhnd){}

    static void RBufStorage(CGenum,CGenum,size_t,size_t){}

    static bool FBCheckComplete(CGenum){return false;}

    static void FBGetAttachParameter(CGenum,CGenum,CGenum,int32*){}

    static void FBAttachTexture(CGenum,CGenum,CGhnd,int32){}

    static void FBAttachTexture2D(CGenum,CGenum,CGenum,CGhnd,int32){}
    static void FBAttachTexture3D(CGenum,CGenum,CGenum,CGhnd,int32,int32){}
    static void FBAttachTextureLayer(CGenum,CGenum,CGhnd,int32,int32){}
    static void FBAttachRenderBuffer(CGenum,CGenum,CGenum,CGhnd){}

    static void FBBlit(int32,int32,int32,int32,
                       int32,int32,int32,int32,
                       CGenum,CGenum){}

    static void RBufParameteri(CGenum,CGenum,int32*){}
    static void FBParameteri(CGenum,CGenum,CGenum,int32*){}

    static void FBDrawBuffers(size_t,const CGenum*){}
    static void FBReadBuffer(CGenum){}

    static void FBReadPixels(int32,int32,int32,int32,CGenum,TypeEnum,c_ptr){}

    /* XFB */
    static void XFBegin(CGenum){}
    static void XFEnd(){}

    static void XFVaryings(CGhnd,size_t,cstring*,AttribMode){}
    static void XFGetVaryings(CGhnd,uint32,size_t*,size_t*,CGenum*,cstring_w*){}

    /* Include RASTERIZER_DISCARD */

    /* VAO */
    static void VAOBind(CGhnd){}

    static void VAOEnableAttrib(uint32){}
    static void VAOAttribPointer(uint32,int32,TypeEnum,bool,int64,int64){}
    static void VAOAttribIPointer(uint32,int32,TypeEnum,int64,int64){}
    static void VAODivisor(uint32,uint32){}

    /* Sync */
    static CGsync FenceCreate(){return nullptr;}
    static bool FenceAwait(CGsync,uint64){return false;}
    static bool FenceServerAwait(CGsync,uint64){return false;}

    static void FenceGetiv(CGsync,CGenum,size_t,size_t*,int32*){}

    /* Using Uniform* */
    static void Uniformfv(int32,size_t,const scalar*){}
    static void Uniformiv(int32,size_t,const int32*){}

    static void Uniformfv(int32,size_t,const _cbasic_vec2<scalar>*){}
    static void Uniformiv(int32,size_t,const _cbasic_vec2<int32>*){}

    static void Uniformfv(int32,size_t,const _cbasic_vec3<scalar>*){}
    static void Uniformiv(int32,size_t,const _cbasic_vec3<int32>*){}

    static void Uniformfv(int32,size_t,const _cbasic_vec4<scalar>*){}
    static void Uniformiv(int32,size_t,const _cbasic_vec4<int32>*){}

    static void Uniformfv(int32,size_t,const _cbasic_tmatrix<scalar,2>*){}
    static void Uniformiv(int32,size_t,const _cbasic_tmatrix<int32,2>*){}

    static void Uniformfv(int32,size_t,const _cbasic_tmnmatrix<scalar,2,3>*){}
    static void Uniformiv(int32,size_t,const _cbasic_tmnmatrix<int32,2,3>*){}

    static void Uniformfv(int32,size_t,const _cbasic_tmatrix<scalar,3>*){}
    static void Uniformiv(int32,size_t,const _cbasic_tmatrix<int32,3>*){}

    static void Uniformfv(int32,size_t,const _cbasic_tmnmatrix<scalar,3,2>*){}
    static void Uniformiv(int32,size_t,const _cbasic_tmnmatrix<int32,3,2>*){}

    static void Uniformfv(int32,size_t,const _cbasic_tmnmatrix<scalar,3,4>*){}
    static void Uniformiv(int32,size_t,const _cbasic_tmnmatrix<int32,3,4>*){}

    static void Uniformfv(int32,size_t,const _cbasic_tmatrix<scalar,4>*){}
    static void Uniformiv(int32,size_t,const _cbasic_tmatrix<int32,4>*){}

    static void Uniformfv(int32,size_t,const _cbasic_tmnmatrix<scalar,4,3>*){}
    static void Uniformiv(int32,size_t,const _cbasic_tmnmatrix<int32,4,3>*){}

    static void Uniformfv(int32,size_t,const _cbasic_tmnmatrix<scalar,4,2>*){}
    static void Uniformiv(int32,size_t,const _cbasic_tmnmatrix<int32,4,2>*){}

    /* Drawing */
    static void DrawArrays(Primitive,PrimitiveCreation,int32,size_t){}
    static void DrawArraysInstanced(Primitive,PrimitiveCreation,int32,size_t,size_t){}

    static void DrawElements(Primitive,PrimitiveCreation,size_t,TypeEnum,int64){}
    static void DrawElementsInstanced(Primitive,PrimitiveCreation,size_t,TypeEnum,int64,size_t){}
    static void DrawElementsBaseVertex(Primitive,PrimitiveCreation,size_t,TypeEnum,int64,int32){}
    static void DrawElementsInstancedBaseVertex(Primitive,PrimitiveCreation,
                                                size_t,TypeEnum,int64,size_t,int32){}

    static void DrawRangeElements(Primitive,PrimitiveCreation,uint32,uint32,size_t,TypeEnum,int64){}
    static void DrawRangeElementsBaseVertex(Primitive,PrimitiveCreation,uint32,uint32,
                                            size_t,TypeEnum,int64,int32){}

    static void DrawMultiArrays(Primitive,PrimitiveCreation,const int32*,const size_t*){}

    static void DrawMultiElements(Primitive,PrimitiveCreation,const int32*,
                                  TypeEnum,const int64* const*,size_t){}
    static void DrawMultiElementsBaseVertex(Primitive,PrimitiveCreation,const size_t*,TypeEnum,
                                            const int64* const*,size_t,const int32*){}
};

}
}

#endif
