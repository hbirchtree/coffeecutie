#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_43_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_43_H

#include "gl33.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL 4.3 compliance model
 */
struct CGL43 : CGL33
{

    enum class Feature
    {
        Blend,

        DebugOutput,
        DebugOutputSynchronous,

        DepthClamp,
        DepthTest,
        Dither,

        FramebufferSRGB,

        LineSmooth,
        PolygonSmooth,

        PrimitiveRestart,
        PrimitiveRestartFixedIdx,

        SampleAlphaToCoverage,
        SampleShading,
        SampleMask,

        ScissorTest,
        StencilTest,

        PointSize,
    };

    static bool TexStorageSupported(){return false;}

    static bool TexViewSupported(){return false;}

    static bool SeparableShaderSupported(){return false;}

    static bool TessellationSupported(){return false;}

    static bool ShaderStorageSupported(){return false;}

    static bool ProgramResourceInfoSupported(){return false;}

    static bool ComputeShaderSupported(){return false;}

    static bool InstancedXFSupported(){return false;}

    static bool MultiDrawIndirectSupported(){return false;}

    static bool BufferStorageSupported(){return false;}

    /* Base */
    static void MemoryBarrier(GLbitfield f){glMemoryBarrier(f);}

    /* Use gl*Indexed gl*Arrayv */
    static void ViewportSet(uint32,CRect const*){}
    static void ViewportArrayv(uint32,size_t,CRect const*){}

    static void ScissorSet(uint32,CRect const*){}
    static void ScissorArrayv(uint32,size_t,CRect const*){}

    static void DepthSet(uint32,CZField64 const*){}
    static void DepthArrayv(uint32,size_t,CZField64 const*){}

    /* Resource allocation */
    static bool PipelineAlloc(int32,CGhnd*){return false;}
    static bool PipelineFree(int32,CGhnd*){return false;}

    static bool XFAlloc(int32 l,CGhnd* d){glGenTransformFeedbacks(l,d); return false;}
    static bool XFFree(int32 l,CGhnd* d){glDeleteTransformFeedbacks(l,d); return false;}

    /* Shaders */
    static void ShaderGetPrecisionFormat(CGenum,CGenum,int32*,int32*){}
    static void ShaderReleaseCompiler(){}
    static void ShaderBinary(size_t,const CGhnd*,CGenum,c_cptr,int64){}

    static void ShaderGetiv(CGhnd,CGenum,int32*){}

    /* Programs */
    static void ProgramBinary(CGhnd,CGenum,c_cptr){}
    static void ProgramGetBinary(CGhnd,int64,int64*,CGenum*,c_ptr){}

    static void ProgramGetInterfaceiv(CGhnd,CGenum,CGenum,int32*){}

    static uint32 ProgramGetResourceIdx(CGhnd,CGenum,cstring){return 0;}
    static cstring_w ProgramGetResourceName(CGhnd,CGenum,uint32){return nullptr;}
    static uint32 ProgramGetResourceLoc(CGhnd,CGenum,cstring){return 0;}
    static uint32 ProgramGetResourceLocIdx(CGhnd,CGenum,cstring){return 0;}

    static CGhnd ProgramCreate(CGenum,size_t,cstring const*){return 0;}
    static void ProgramParameter(CGhnd,CGenum,int32){}

    static bool ProgramValidate(CGhnd){return false;}

    /* Pipelines */
    static void PipelineBind(CGhnd){}
    static bool PipelineUseStages(){return false;}
    static void PipelineActive(CGhnd,CGenum,CGhnd){}

    static void PipelineGetiv(CGhnd,CGenum,int32*){}
    static cstring_w PipelineGetLog(CGhnd){return nullptr;}

    static bool PipelineValidate(CGhnd){return false;}

    /* Textures */
    static void TexView(CGhnd,CGenum,CGhnd,CGenum,uint32,uint32,uint32,uint32){}

    static bool TexStorage2D(CGenum,int64,CGenum,int64,int64){return false;}
    static bool TexStorage3D(CGenum,int64,CGenum,int64,int64){return false;}

    static void ImageBindTexture(CGhnd,CGhnd,int32,bool,int32,CGenum,CGenum){}
    static bool ImageCopySubData(CGhnd,CGenum,int32,int32,int32,int32,
                                 CGhnd,CGenum,int32,int32,int32,int32,
                                 int64,int64,int64){return false;}

    /* Buffers */
    static void BufClearData(CGenum,CGenum,CGenum,TypeEnum,c_cptr){}
    static void BufClearSubData(CGenum,CGenum,int64,int64,CGenum,TypeEnum,c_cptr){}

    static void BufInvalidateData(CGhnd){}
    static void BufInvalidateSubData(CGhnd,int64,int64){}

    /* Shader buffer */
    static void SBufBind(CGhnd,uint32,uint32){}

    /* Queries */
    static void QueryIndexedBegin(CGenum,uint32,CGhnd){}
    static void QueryIndexedEnd(CGenum,uint32){}

    static void QueryIndexedGetiv(CGenum,uint32,CGenum,int32*){}

    /* VAO */
    static void VAOAttribFormat(uint32,int32,CGenum,bool,uint32){}
    static void VAOAttribFormatI(uint32,int32,CGenum,uint32){}
    static void VAOAttribFormatL(uint32,int32,CGenum,uint32){}

    static void VAOBindingDivisor(uint32,uint32){}
    static void VAOAttribBinding(uint32,uint32){}
    static void VAOBindVertexBuffer(uint32,CGhnd,int64,int64){}

    static void VAOPrimitiveRestart(uint32){}

    /* Just to hide and disable outdated functionality */
    static void VAOAttribPointer(){}
    static void VAOAttribIPointer(){}
    static void VAOAttribLPointer(){}
    static void VAODivisor(){}

    /* Buffers */
    static void BufStorage(CGenum,int64,c_cptr,CGenum){}

    /* Framebuffer */
    static void FBInvalidate(CGenum,size_t,const CGenum*){}
    static void FBParameteri(CGenum t,CGenum p,int32 d)
    {glFramebufferParameteri(t,p,d);}

    /* XFB */
    static void XFBind(CGhnd){}

    static void XFPause(){}
    static void XFResume(){}

    /* ProgramUniform* functions */
    static void Uniformfv(CGhnd,int32,size_t,const scalar*){}
    static void Uniformiv(CGhnd,int32,size_t,const int32*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_vec2<scalar>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_vec2<int32>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_vec3<scalar>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_vec3<int32>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_vec4<scalar>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_vec4<int32>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_tmatrix<scalar,2>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_tmatrix<int32,2>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<scalar,2,3>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<int32,2,3>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_tmatrix<scalar,3>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_tmatrix<int32,3>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<scalar,3,2>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<int32,3,2>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<scalar,3,4>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<int32,3,4>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_tmatrix<scalar,4>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_tmatrix<int32,4>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<scalar,4,3>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<int32,4,3>*){}

    static void Uniformfv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<scalar,4,2>*){}
    static void Uniformiv(CGhnd,int32,size_t,const _cbasic_tmnmatrix<int32,4,2>*){}

    /* Memory barrier */
    static void MemoryBarrier(CGenum){}

    /* Tessellation */
    static void PatchParamteri(CGenum,int32){}
    static void PatchParamterfv(CGenum,const scalar*){}

    /* Drawing */
    static void DrawArraysIndirect(Primitive,PrimitiveCreation,uint64){}
    static void DrawArraysInstancedBaseInstance(Primitive,PrimitiveCreation,int32,
                                                int64,int64,uint32){}

    static void DrawElementsIndirect(Primitive,PrimitiveCreation,TypeEnum,uint64,int64,int64){}
    static void DrawElementsInstancedBaseInstance(Primitive,PrimitiveCreation,int64,
                                                  TypeEnum,uint64,int64,int64){}
    static void DrawElementsInstancedBaseVertexBaseInstance(Primitive,PrimitiveCreation,int64,
                                                            TypeEnum,uint64,int64,int64,
                                                            int32,int32){}

    static void DrawRangeElements(Primitive,PrimitiveCreation,uint32,uint32,int64,TypeEnum,uint64){}

    static void DrawMultiArraysIndirect(Primitive,PrimitiveCreation,uint64,int64,int64){}
    static void DrawMultiElementsIndirect(Primitive,PrimitiveCreation,TypeEnum,uint64,int64,int64){}

    static void DrawXF(Primitive,PrimitiveCreation,CGhnd){}
    static void DrawXFStream(Primitive,PrimitiveCreation,CGhnd,uint32){}

    static void DrawXFInstanced(Primitive,PrimitiveCreation,CGhnd,int64){}
    static void DrawXFStreamInstanced(Primitive,PrimitiveCreation,CGhnd,uint32,int64){}

    /* Compute */
    static void ComputeDispatch(uint32,uint32,uint32){}
    static void ComputeDispatchIndirect(int64){}
};

}
}

#endif
