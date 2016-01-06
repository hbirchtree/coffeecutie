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

    static bool TexStorageSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_texture_storage");}

    static bool TexViewSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_texture_view");}

    static bool SeparableShaderSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_separate_shader_objects");}

    static bool TessellationSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_tessellation_shader");}

    static bool ShaderStorageSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_shader_storage_buffer_object");}

    static bool ProgramResourceInfoSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_program_interface_query");}

    static bool ComputeShaderSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_compute_shader");}

    static bool InstancedXFSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_transform_feedback_instanced");}

    static bool MultiDrawIndirectSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_multi_draw_indirect");}

    static bool BufferStorageSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_buffer_storage");}

    /* Base */
    static void MemoryBarrier(GLbitfield f){glMemoryBarrier(f);}

    /* Use gl*Indexed gl*Arrayv */
    static void ViewportArrayv(uint32 i,size_t c,CRectF const* v)
    {glViewportArrayv(i,c,(scalar*)v);}
    static void ViewportSet(uint32 i,CRectF const* v){ViewportArrayv(i,1,v);}

    static void ScissorArrayv(uint32 i,size_t c,CRect const* v)
    {glScissorArrayv(i,c,(int32*)v);}
    static void ScissorSet(uint32 i,CRect const* v){ScissorArrayv(i,1,v);}

    static void DepthArrayv(uint32 i,size_t c,CZField64 const* v)
    {glDepthRangeArrayv(i,c,(bigscalar*)v);}
    static void DepthSet(uint32 i,CZField64 const* v){DepthArrayv(i,1,v);}

    /* Resource allocation */
    static bool PipelineAlloc(uint32 c,CGhnd* h)
    {
        glGenProgramPipelines(c,h);
        return true;
    }
    static bool PipelineFree(uint32 c,CGhnd* h)
    {
        glDeleteProgramPipelines(c,h);
        return true;
    }

    static bool XFAlloc(uint32 l,CGhnd* d){glGenTransformFeedbacks(l,d); return true;}
    static bool XFFree(uint32 l,CGhnd* d){glDeleteTransformFeedbacks(l,d); return true;}

    static CGhnd ProgramCreate(ShaderStage t,uint32 c,cstring* d)
    {return glCreateShaderProgramv(to_enum1(t),c,d);}

    /* Shaders */
    static void ShaderGetPrecisionFormat(ShaderStage s,CGenum e,int32* r,int32* p)
    {glGetShaderPrecisionFormat(to_enum1(s),e,r,p);}
    static void ShaderReleaseCompiler()
    {glReleaseShaderCompiler();}
    static void ShaderBinary(uint32 n,const CGhnd* h,CGenum t,c_cptr d,int32 s)
    {glShaderBinary(n,h,t,d,s);}

    /* Programs */
    static void ProgramBinary(CGhnd h,CGenum t,uint64 size,c_cptr d)
    {glProgramBinary(h,t,d,size);}
    static void ProgramGetBinary(CGhnd h,int64 bsize,int32* size,CGenum* t,c_ptr d)
    {glGetProgramBinary(h,bsize,size,t,d);}

    //TODO: Create PInterfaceProperty enum
    static void ProgramGetInterfaceiv(CGhnd h,CGenum i,CGenum e,int32* v)
    {glGetProgramInterfaceiv(h,i,e,v);}
    //TODO: Add glGetProgramResourceiv()

    static uint32 ProgramGetResourceIdx(CGhnd h,CGenum i,cstring n)
    {return glGetProgramResourceIndex(h,i,n);}
    static cstring_w ProgramGetResourceName(CGhnd h,CGenum i,uint32 d)
    {
        int32 len = 0;
        ProgramGetInterfaceiv(h,i,GL_MAX_NAME_LENGTH,&len);
        cstring_w name = new int8[len+1];
        name[len] = 0;
        glGetProgramResourceName(h,i,d,len+1,nullptr,name);
        return name;
    }
    static uint32 ProgramGetResourceLoc(CGhnd h,CGenum e,cstring n)
    {return glGetProgramResourceLocation(h,e,n);}
    static uint32 ProgramGetResourceLocIdx(CGhnd h,CGenum e,cstring n)
    {return glGetProgramResourceLocationIndex(h,e,n);}

    //TODO: Create ProgramParameter enum
    static void ProgramParameter(CGhnd h,CGenum e,int32 v)
    {glProgramParameteri(h,e,v);}

    static bool ProgramValidate(CGhnd h)
    {
        glValidateProgram(h);
        int32 outStat;
        ProgramGetiv(h,GL_VALIDATE_STATUS,&outStat);
        return outStat == GL_TRUE;
    }

    /* Pipelines */
    static void PipelineBind(CGhnd h){glBindProgramPipeline(h);}
    static bool PipelineUseStages(CGhnd p,ShaderStage e,CGhnd prg)
    {
        glUseProgramStages(p,to_enum2(e),prg);
        //TODO: Add strict check in debug mode
        return true;
    }
    //TODO: Create PipelineProperty enum
    static void PipelineGetiv(CGhnd h,CGenum e,int32* v){glGetProgramPipelineiv(h,e,v);}
    static cstring_w PipelineGetLog(CGhnd h)
    {
        int32 len = 0;
        PipelineGetiv(h,GL_INFO_LOG_LENGTH,&len);
        if(len<=0)
            return nullptr;
        cstring_w s = new int8[len+1];
        glGetProgramPipelineInfoLog(h,len,nullptr,s);
        return s;
    }

    static bool PipelineValidate(CGhnd h)
    {
        glValidateProgramPipeline(h);
        int32 outStat;
        PipelineGetiv(h,GL_VALIDATE_STATUS,&outStat);
        return outStat == GL_TRUE;
    }

    /* Textures */
    static void TexView(CGhnd vh,Texture t,CGhnd th,PixelFormat ifmt,
                        uint32 ml,uint32 nl,uint32 lm,uint32 ln)
    {glTextureView(vh,to_enum(t),th,to_enum(ifmt),ml,nl,lm,ln);}

    static bool TexStorage2D(Texture t,uint32 l,PixelFormat ifmt,uint32 w,uint32 h)
    {
        glTexStorage2D(to_enum(t),l,to_enum(ifmt),w,h);
        //TODO: Add verification in debug mode
        return TexStorageSupported();
    }
    static bool TexStorage3D(Texture t,uint32 l,PixelFormat ifmt,uint32 w,uint32 h, uint32 d)
    {
        glTexStorage3D(to_enum(t),l,to_enum(ifmt),w,h,d);
        //TODO: Add verification in debug mode
        return TexStorageSupported();
    }
    static bool TexStorage2DMS(Texture t,uint32 samples,uint32 l,
                               PixelFormat ifmt,uint32 w,uint32 h)
    {
        glTexStorage2DMultisample(to_enum(t),samples,l,to_enum(ifmt),w,h);
        //TODO: Add verification in debug mode
        return TexStorageSupported();
    }
    static bool TexStorage3DMS(Texture t,uint32 samples,uint32 l,
                               PixelFormat ifmt,uint32 w,uint32 h, uint32 d)
    {
        glTexStorage3DMultisample(to_enum(t),samples,l,to_enum(ifmt),w,h,d);
        //TODO: Add verification in debug mode
        return TexStorageSupported();
    }

    static void ImageBindTexture(uint32 tx,CGhnd t,uint32 lev,bool lay,
                                 uint32 ln,ResourceAccess acc,PixelFormat fmt)
    {glBindImageTexture(tx,t,lev,lay,ln,to_enum3(acc),to_enum(fmt));}
    static void ImageBindTextures(uint32 f,uint32 c, const CGhnd* h)
    {glBindImageTextures(f,c,h);}
    static bool ImageCopySubData(CGhnd s,Texture st,uint32 sl,int32 sx,int32 sy,int32 sz,
                                 CGhnd d,Texture dt,uint32 dl,int32 dx,int32 dy,int32 dz,
                                 uint32 sw,uint32 sh,uint32 sd)
    {
        glCopyImageSubData(s,to_enum(st),sl,sx,sy,sz,d,to_enum(dt),dl,dx,dy,dz,sw,sh,sd);
        return true;
    }

    /* Buffers */
    static void BufClearData(DataType t,PixelFormat ifmt,TypeEnum dtb,
                             TypeEnum dts,c_cptr d)
    {glClearBufferData(to_enum(t),to_enum(ifmt),to_enum(dtb),to_enum(dts),d);}
    static void BufClearSubData(DataType t,PixelFormat ifmt,int64 offset,uint32 size,
                                PixelFormat df,TypeEnum dt,c_cptr d)
    {glClearBufferSubData(to_enum(t),to_enum(ifmt),offset,size,to_enum(df),to_enum(dt),d);}

    static void BufInvalidateData(CGhnd h){glInvalidateBufferData(h);}
    static void BufInvalidateSubData(CGhnd h,int64 offset,uint32 size)
    {glInvalidateBufferSubData(h,offset,size);}

    /* Shader buffer */
    static void SBufBind(CGhnd h,uint32 i,uint32 b){glShaderStorageBlockBinding(h,i,b);}

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
    static void Uniformfv(CGhnd h,int32 l,int32 c,const scalar* d)
    {glProgramUniform1fv(h,l,c,d);}
    static void Uniformiv(CGhnd h,int32 l,int32 c,const int32* d)
    {glProgramUniform1iv(h,l,c,d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,const _cbasic_vec2<scalar>* d)
    {glProgramUniform2fv(h,l,c,(scalar*)d);}
    static void Uniformiv(CGhnd h,int32 l,int32 c,const _cbasic_vec2<int32>* d)
    {glProgramUniform2iv(h,l,c,(int32*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,const _cbasic_vec3<scalar>* d)
    {glProgramUniform3fv(h,l,c,(scalar*)d);}
    static void Uniformiv(CGhnd h,int32 l,int32 c,const _cbasic_vec3<int32>* d)
    {glProgramUniform3iv(h,l,c,(int32*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,const _cbasic_vec4<scalar>* d)
    {glProgramUniform4fv(h,l,c,(scalar*)d);}
    static void Uniformiv(CGhnd h,int32 l,int32 c,const _cbasic_vec4<int32>* d)
    {glProgramUniform4iv(h,l,c,(int32*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,2>* d)
    {glProgramUniformMatrix2fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,2,3>* d)
    {glProgramUniformMatrix2x3fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,3>* d)
    {glProgramUniformMatrix3fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,2>* d)
    {glProgramUniformMatrix3x2fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,4>* d)
    {glProgramUniformMatrix3x4fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,4>* d)
    {glProgramUniformMatrix4fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,3>* d)
    {glProgramUniformMatrix4x3fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(CGhnd h,int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,2>* d)
    {glProgramUniformMatrix4x2fv(h,l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

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
