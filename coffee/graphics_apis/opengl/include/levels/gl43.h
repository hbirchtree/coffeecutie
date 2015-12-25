#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_43_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_43_H

#include "gl33.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL 4.3 compliance model
 */
struct CGL_GL43 : CGL_GL3
{
    static bool TexStorageSupported(){return false;}

    static bool TexViewSupported(){return false;}

    static bool SeparableShaderSupported(){return false;}

    static bool TessellationSupported(){return false;}

    static bool ShaderStorageSupported(){return false;}

    static bool ProgramResourceInfoSupported(){return false;}

    static bool ComputeShaderSupported(){return false;}

    static bool InstancedXFBSupported(){return false;}

    static bool MultiDrawIndirectSupported(){return false;}

    static bool BufferStorageSupported(){return false;}

    /* Base */
    /* Use gl*Indexed gl*Arrayv */
    static void ViewportSet(){}
    static void ViewportArrayv(){}

    static void ScissorSet(){}
    static void ScissorArrayv(){}

    static void DepthSet(){}
    static void DepthArrayv(){}

    static void FrontFace(){}

    static void PointSize(){}
    static void PointParameteriv(){}
    static void PointParameterfv(){}

    static void LineWidth(){}

    static void PolyMode(){}
    static void PolyOffset(){}

    /* Resource allocation */
    static bool PipelineAlloc(size_t,CGhnd*){return false;}
    static bool PipelineFree(size_t,CGhnd*){return false;}

    /* Shaders */
    static void ShaderGetPrecisionFormat(){}
    static void ShaderReleaseCompiler(){}
    static void ShaderBinary(){}

    static void ShaderGetiv(){}

    /* Programs */
    static void ProgramBinary(){}
    static void ProgramGetBinary(){}

    static void ProgramGetInterfaceiv(){}
    static void ProgramGetResourceiv(){}

    static void ProgramGetResourceIdx(){}
    static void ProgramGetResourceName(){}
    static void ProgramGetResourceLoc(){}
    static void ProgramGetResourceLocIdx(){}

    static CGhnd ProgramCreate(){return 0;}
    static void ProgramParameter(){}

    static void ProgramGetiv(){}

    static void ProgramValidate(){}

    static CGint ProgramGetFragDataLoc(){return 0;}
    static CGint ProgramGetFragDataIdx(){return 0;}
    static void ProgramBindFragData(){}
    static void ProgramBindFragDataIndexed(){}

    /* Pipelines */
    static void PipelineBind(){}
    static bool PipelineUseStages(){return false;}
    static void PipelineActive(){}

    static void PipelineValidate(){}

    /* Textures */
    static void TexView(){}

    static bool TexStorage2D(){return false;}
    static bool TexStorage3D(){return false;}

    static void ImageBindTexture(){}
    static bool ImageCopyData(){return false;}
    static bool ImageCopySubData(){return false;}

    /* Buffers */
    static void BufClearData(){}
    static void BufClearSubData(){}

    static void BufInvalidateData(){}
    static void BufInvalidateSubData(){}

    /* Shader buffer */
    static void SBufBind(){}

    /* Queries */
    static void QueryGeti64v(){}
    static void QueryCounter(){}

    /* VAO */
    static void VAOAttribFormat(){}
    static void VAOAttribFormatI(){}
    static void VAOAttribFormatL(){}

    static void VAOElementBuffer(){}

    static void VAOBindingDivisor(){}
    static void VAOAttribBinding(){}
    static void VAOBindVertexBuffer(){}

    static void VAOPrimitiveRestart(){}

    /* XFB */
    static void XFPause(){}
    static void XFResume(){}

    /* ProgramUniform* functions */
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

    /* Memory barrier */
    static void MemoryBarrier(){}

    /* Tessellation */
    static void PatchParamterfv(){}

    /* Drawing */
    static void DrawArraysIndirect(){}
    static void DrawArraysInstancedBaseInstance(){}

    static void DrawElementsIndirect(){}
    static void DrawElementsInstancedBaseInstance(){}
    static void DrawElementsInstancedBaseVertexBaseInstance(){}

    static void DrawRangeElements(){}

    static void DrawMultiArraysIndirect(){}
    static void DrawMultiElementsIndirect(){}

    static void DrawXFB(){}
    static void DrawXFBInstanced(){}
    static void DrawXFBStream(){}
    static void DrawXFBStreamInstanced(){}

    /* Compute */
    static void ComputeDispatch(){}
    static void ComputeDispatchIndirect(){}
};

}
}

#endif
