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
    static bool LoadBinding(CGL_Context *ctxt)
    {
        bool status = CGL33::LoadBinding(ctxt);
	if(!status)
	    return false;
	const CDisplay::CGLVersion targetVer(4,3);
        CDisplay::CGLVersion ver = Debug::ContextVersion();
        if(ver<targetVer)
        {
	    const _cbasic_version<uint8>& rv = ver;
	    const _cbasic_version<uint8>& tv = targetVer;
            cLog(__FILE__,__LINE__,CFStrings::Graphics_GLeam_Library_Name,
                 CFStrings::Graphics_GLeam_Library_CoreVersionError,
                 rv,tv);
        }
        return status;
    }

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

    enum class PatchProperty
    {
        Vertices = GL_PATCH_VERTICES,
        DefOuterLevel = GL_PATCH_DEFAULT_OUTER_LEVEL,
        DefInnerLevel = GL_PATCH_DEFAULT_INNER_LEVEL,
    };

    using CGL::CGL33::to_enum;

    inline static CGenum to_enum(PatchProperty f)
    {
        return (CGenum)f;
    }

    //TODO: Add more extensions to check for, and check for them in functions

    static bool TexStorageSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_texture_storage");}

    static bool TexViewSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_texture_view");}

    static bool ViewportArraySupported()
    {return Debug::CheckExtensionSupported("GL_ARB_viewport_array");}

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

    //TODO: Create off-thread program compiler when binary storage is available
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
    static int32 ProgramGetResourceLoc(CGhnd h,CGenum e,cstring n)
    {return glGetProgramResourceLocation(h,e,n);}
    static uint32 ProgramGetResourceLocIdx(CGhnd h,CGenum e,cstring n)
    {return glGetProgramResourceLocationIndex(h,e,n);}

    //TODO: Create ProgramParameter enum
    static void ProgramParameter(CGhnd h,CGenum e,int32 v)
    {glProgramParameteri(h,e,v);}

    static bool ProgramValidate(CGhnd h)
    {
	/*WARNING: Do not use this with shader programs! */
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

    static void ImageBindTexture(int32 tx,CGhnd t,uint32 lev,bool lay,
                                 uint32 ln,ResourceAccess acc,PixelFormat fmt)
    {glBindImageTexture(tx,t,lev,lay,ln,to_enum3(acc),to_enum(fmt));}
    static void ImageBindTextures(int32 f,uint32 c, const CGhnd* h)
    {glBindImageTextures(f,c,h);}
    static bool ImageCopySubData(CGhnd s,Texture st,uint32 sl,int32 sx,int32 sy,int32 sz,
                                 CGhnd d,Texture dt,uint32 dl,int32 dx,int32 dy,int32 dz,
                                 uint32 sw,uint32 sh,uint32 sd)
    {
        glCopyImageSubData(s,to_enum(st),sl,sx,sy,sz,d,to_enum(dt),dl,dx,dy,dz,sw,sh,sd);
        return true;
    }

    /* Buffers */
    static void BufClearData(BufType t,PixelFormat ifmt,TypeEnum dtb,
                             TypeEnum dts,c_cptr d)
    {glClearBufferData(to_enum(t),to_enum(ifmt),to_enum(dtb),to_enum(dts),d);}
    static void BufClearSubData(BufType t,PixelFormat ifmt,int64 offset,uint32 size,
                                PixelFormat df,TypeEnum dt,c_cptr d)
    {glClearBufferSubData(to_enum(t),to_enum(ifmt),offset,size,to_enum(df),to_enum(dt),d);}

    static void BufInvalidateData(CGhnd h){glInvalidateBufferData(h);}
    static void BufInvalidateSubData(CGhnd h,int64 offset,uint32 size)
    {glInvalidateBufferSubData(h,offset,size);}

    /* Shader buffer */
    static void SBufBind(CGhnd h,uint32 i,uint32 b)
    {glShaderStorageBlockBinding(h,i,b);}

    /* Queries */
    static void QueryIndexedBegin(QueryT t,uint32 i,CGhnd h)
    {glBeginQueryIndexed(to_enum(t),i,h);}
    static void QueryIndexedEnd(QueryT t,uint32 i){glEndQueryIndexed(to_enum(t),i);}
    //TODO: Create QueryProperty enum
    static void QueryIndexedGetiv(QueryT t,uint32 i,CGenum e,int32* v)
    {glGetQueryIndexediv(to_enum(t),i,e,v);}

    /* VAO */
    static void VAOAttribFormat(uint32 i,int32 s,TypeEnum t,bool n,uint32 off)
    {glVertexAttribFormat(i,s,to_enum(t),(n)?GL_TRUE:GL_FALSE,off);}
    static void VAOAttribFormatI(uint32 i,int32 s,TypeEnum t,uint32 off)
    {glVertexAttribIFormat(i,s,to_enum(t),off);}
    static void VAOAttribFormatL(uint32 i,int32 s,TypeEnum t,uint32 off)
    {glVertexAttribLFormat(i,s,to_enum(t),off);}

    static void VAOBindingDivisor(uint32 attr,uint32 div)
    {glVertexBindingDivisor(attr,div);}
    static void VAOAttribBinding(uint32 attr,uint32 idx)
    {glVertexAttribBinding(attr,idx);}
    static void VAOBindVertexBuffer(uint32 idx,CGhnd h,uint64 off,int32 stride)
    {glBindVertexBuffer(idx,h,off,stride);}
    static void VAOBindVertexBuffers(uint32 idx_f, uint32 c,const CGhnd* h,
                                     const int64* off, const int32* stride)
    {glBindVertexBuffers(idx_f,c,h,off,stride);}

    /* Just to hide and disable outdated functionality */
    static void VAOAttribPointer(){}
    static void VAOAttribIPointer(){}
    static void VAOAttribLPointer(){}
    static void VAODivisor(){}

    /* Buffers */
    static void BufStorage(BufType t,int64 s,c_cptr d,ResourceAccess a)
    {glBufferStorage(to_enum(t),s,d,to_enum2(a));}

    /* Framebuffer */
    static void FBInvalidate(CGenum,size_t,const CGenum*){}
    static void FBParameteri(CGenum t,CGenum p,int32 d)
    {glFramebufferParameteri(t,p,d);}

    /* XFB */
    static void XFBind(CGhnd h){glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,h);}

    static void XFPause(){glPauseTransformFeedback();}
    static void XFResume(){glResumeTransformFeedback();}

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
    static void PatchParameteri(PatchProperty p,int32 v)
    {glPatchParameteri(to_enum(p),v);}
    static void PatchParameterfv(PatchProperty p,const scalar* v)
    {glPatchParameterfv(to_enum(p),v);}

    /* Drawing */
    /*!
     * \brief DrawArraysIndirect
     * \param p Primitive type
     * \param c Primitive creation method
     * \param off Offset into buffer
     */
    static void DrawArraysIndirect(Prim p,PrimCre c,uint64 off)
    {glDrawArraysIndirect(to_enum(p,c),(c_cptr)off);}
    /*!
     * \brief DrawArraysInstancedBaseInstance
     * \param p Primitive type
     * \param c Primitive creation method
     * \param vf First vertex
     * \param vc Number of vertices
     * \param ic Number of instances
     * \param bi Index of first instance to draw
     */
    static void DrawArraysInstancedBaseInstance(Prim p,PrimCre c,int32 vf,
                                                uint32 vc,uint32 ic,uint32 bi)
    {glDrawArraysInstancedBaseInstance(to_enum(p,c),vf,vc,ic,bi);}

    /*!
     * \brief DrawElementsIndirect
     * \param p Primitive type
     * \param c Primitive creation method
     * \param d Element data type
     * \param off Offset into indirect buffer
     */
    static void DrawElementsIndirect(Prim p,PrimCre c,
                                     TypeEnum d,
                                     uint64 off)
    {glDrawElementsIndirect(to_enum(p,c),to_enum(d),(c_cptr)off);}
    /*!
     * \brief DrawElementsInstancedBaseInstance
     * \param p Primitive type
     * \param c Primitive creation method
     * \param ec Number of elements to draw per instance
     * \param d Element data type
     * \param off Offset into element buffer
     * \param bc Number of instances to draw
     * \param bi Base instance applied to all instances for attribute fetching
     */
    static void DrawElementsInstancedBaseInstance(
            Prim p,PrimCre c,uint32 ec,TypeEnum d,uint64 off,uint32 bc,uint32 bi)
    {glDrawElementsInstancedBaseInstance(to_enum(p,c),ec,to_enum(d),(c_cptr)off,bc,bi);}
    /*!
     * \brief DrawElementsInstancedBaseVertexBaseInstance
     * \param p Primitive type
     * \param c Primitive creation method
     * \param ec Number of elements to draw
     * \param d Element data type
     * \param off Offset into element buffer
     * \param ic Number of instances to draw from base instance
     * \param bv Value added to each element in the element buffer for this call
     * \param bi Index of first instance to draw
     */
    static void DrawElementsInstancedBaseVertexBaseInstance(
            Prim p,PrimCre c,uint32 ec,TypeEnum d,uint64 off,uint32 ic,uint32 bv,uint32 bi)
    {glDrawElementsInstancedBaseVertexBaseInstance(to_enum(p,c),ec,to_enum(d),
                                                   (c_cptr)off,ic,bv,bi);}

    /*!
     * \brief DrawRangeElements
     * \param p Primitive type
     * \param c Primitive creation method
     * \param f First element in range to draw
     * \param e Last element in range to draw
     * \param vc Number of elements to draw
     * \param d Element data type
     * \param off Offset into index buffer
     */
    static void DrawRangeElements(
            Prim p,PrimCre c,uint32 f,uint32 e,uint32 vc,TypeEnum d,uint64 off)
    {glDrawRangeElements(to_enum(p,c),f,e,vc,to_enum(d),(c_cptr)off);}
    /*!
     * \brief DrawRangeElementsBaseVertex
     * \param p Primitive type
     * \param c Primitive creation method
     * \param f First element in range to draw
     * \param e Last element in range to draw
     * \param vc Number of elements to draw
     * \param d Element data type
     * \param off Offset into index buffer
     * \param bv Index of first vertex to draw
     */
    static void DrawRangeElementsBaseVertex(
            Prim p,PrimCre c,uint32 f,uint32 e,uint32 vc,TypeEnum d,uint64 off,int32 bv)
    {glDrawRangeElementsBaseVertex(to_enum(p,c),f,e,vc,to_enum(d),(c_cptr)off,bv);}

    /*!
     * \brief DrawMultiArraysIndirect
     * \param p Primitive type
     * \param c Primitive creation method
     * \param off Offset into indirect buffer
     * \param dc Number of indirect calls to process
     * \param s Stride of draw parameter structure
     */
    static void DrawMultiArraysIndirect(Prim p,PrimCre c,uint64 off,uint32 dc,uint64 s)
    {glMultiDrawArraysIndirect(to_enum(p,c),(c_cptr)off,dc,s);}
    /*!
     * \brief DrawMultiElementsIndirect
     * \param p Primitive type
     * \param c Primitive creation method
     * \param d Element data type
     * \param off Offset into indirect buffer
     * \param dc Number of indirect calls to process
     * \param s Stride of draw parameter structure
     */
    static void DrawMultiElementsIndirect(
            Prim p,PrimCre c,TypeEnum d,uint64 off,uint32 dc,uint32 s)
    {glMultiDrawElementsIndirect(to_enum(p,c),to_enum(d),(c_cptr)off,dc,s);}

    /*!
     * \brief DrawXF
     * \param p Primitive type
     * \param c Primitive creation method
     * \param h Transform feedback handle
     */
    static void DrawXF(Prim p,PrimCre c,CGhnd h)
    {glDrawTransformFeedback(to_enum(p,c),h);}
    /*!
     * \brief DrawXFStream
     * \param p Primitive type
     * \param c Primitive creation method
     * \param h Transform feedback handle
     * \param s Stream index
     */
    static void DrawXFStream(Prim p,PrimCre c,CGhnd h,uint32 s)
    {glDrawTransformFeedbackStream(to_enum(p,c),h,s);}

    /*!
     * \brief DrawXFInstanced
     * \param p Primitive type
     * \param c Primitive creation method
     * \param h Transform feedback handle
     * \param ic Instance count
     */
    static void DrawXFInstanced(Prim p,PrimCre c,CGhnd h,uint32 ic)
    {glDrawTransformFeedbackInstanced(to_enum(p,c),h,ic);}
    /*!
     * \brief DrawXFStreamInstanced
     * \param p Primitive type
     * \param c Primitive creation method
     * \param h Transform feedback handle
     * \param s Stream index
     * \param ic Instance count
     */
    static void DrawXFStreamInstanced(Prim p,PrimCre c,CGhnd h,uint32 s,uint32 ic)
    {glDrawTransformFeedbackStreamInstanced(to_enum(p,c),h,s,ic);}

    /* Compute */
    /*!
     * \brief ComputeDispatch
     * \param x Number of X workgroups
     * \param y Number of Y workgroups
     * \param z Number of Z workgroups
     */
    static void ComputeDispatch(uint32 x,uint32 y,uint32 z){glDispatchCompute(x,y,z);}
    /*!
     * \brief ComputeDispatchIndirect
     * \param o Offset into dispatch buffer for command
     */
    static void ComputeDispatchIndirect(uint64 o){glDispatchComputeIndirect(o);}
};

}
}

#endif
