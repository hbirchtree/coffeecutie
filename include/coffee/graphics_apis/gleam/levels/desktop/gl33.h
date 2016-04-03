#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_33_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_33_H

#include "glbase.h"

#include "../shared/constructors/oldstyle.h"
#include "../shared/shaders/compiling.h"
#include "../shared/shaders/uniforms.h"
#include "../shared/textures/old_textures.h"
#include "../shared/buffers/old_buffers.h"
#include "../shared/framebuffers/old_framebuffers.h"
#include "../shared/vertex/old_vaos.h"
#include "../shared/draw/basic.h"

namespace Coffee{
namespace CGL{

/*!
 * \brief OpenGL 3.3 compliance model
 */
struct CGL33 :
        CGL_Implementation,
        CGL_Old_Framebuffers<CGhnd,CGenum,FramebufferT,Texture>,
        CGL_Old_Textures<CGhnd,CGenum,Texture,CompFlags>,
        CGL_Old_Constructors<CGhnd,ShaderStage,CGsync>,
        CGL_Old_ShaderCompiler<CGhnd,CGenum>,
        CGL_Old_Buffers<CGhnd,BufType>,
        CGL_Old_VAOs<CGhnd,CGenum>,
        CGL_Old_Uniforms,
        CGL_Basic_Draw
{
    enum FBAttach
    {
        Renderbuffer,
        ColorBuffer,
    };

    STATICINLINE bool LoadBinding(CGL_Context* ctxt)
    {
        if(!ctxt->acquireContext())
            return false;
        if(!gladLoadGL())
            return false;

        if(!Debug::VerifyInit())
            return false;

        Debug::GetExtensions();
        Debug::InitDebugFlag();
        return true;
    }

    STATICINLINE bool InstancedGeometryShaderSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_geometry_shader4");}

    STATICINLINE bool CubemapSeamlessSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_seamless_cube_map");}

    STATICINLINE bool CubemapArraySupported()
    {return Debug::CheckExtensionSupported("GL_ARB_texture_cube_map_array");}

    STATICINLINE bool ASTCSupported()
    {return Debug::CheckExtensionSupported("GL_KHR_texture_compression_astc_hdr");}

    STATICINLINE bool BPTCSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_texture_compression_bptc");}

    STATICINLINE bool RGTCSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_texture_compression_rgtc");}

    STATICINLINE bool DXTCSupported()
    {return Debug::CheckExtensionSupported("GL_EXT_texture_compression_s3tc");}

    /* Shaders */
    /*TODO: Create ShaderProperty enum*/

    STATICINLINE int32 ProgramGetFragDataIdx(CGhnd h,cstring n){return glGetFragDataIndex(h,n);}
    STATICINLINE void ProgramBindFragData(CGhnd h,uint32 l,cstring n){glBindFragDataLocation(h,l,n);}
    STATICINLINE void ProgramBindFragDataIndexed(CGhnd h,uint32 i,uint32 l,cstring n)
    {
        glBindFragDataLocationIndexed(h,l,i,n);
    }


    /* Subroutines */
    STATICINLINE void ProgramSubRtGet(CGhnd h,ShaderStage s,uint32* n,
                                      cstring_w** names,int32** rtSize,int32*** rt)
    {
        int32 num = 0;
        ProgramGetiv(h,GL_ACTIVE_SUBROUTINE_UNIFORMS,&num);
        if(num<=0)
        {
            *n = 0;
            return;
        }
        int32 namelen = 0;
        ProgramGetiv(h,GL_ACTIVE_UNIFORM_MAX_LENGTH,&namelen);

        names[0] = new cstring_w[num];
        rtSize[0] = new int32[num];
        rt[0] = new int32*[num];
        for(uint32 i=0;i<(uint32)num;i++)
        {
            int32 namelen = 0;
            glGetActiveSubroutineUniformiv(h,to_enum1(s),i,GL_UNIFORM_NAME_LENGTH,&namelen);
            glGetActiveSubroutineUniformName(h,to_enum1(s),i,namelen,nullptr,names[0][i]);
            glGetActiveSubroutineUniformiv(h,to_enum1(s),i,GL_NUM_COMPATIBLE_SUBROUTINES,&rtSize[0][i]);
            rt[0][i] = new int32[rtSize[0][i]];
            glGetActiveSubroutineUniformiv(h,to_enum1(s),i,GL_COMPATIBLE_SUBROUTINES,rt[0][i]);
        }
    }
    STATICINLINE uint32 ProgramSubRtGetLoc(CGhnd h,ShaderStage s,cstring n)
    {return glGetSubroutineIndex(h,to_enum1(s),n);}
    /* Binds all subroutine uniforms */
    STATICINLINE void ProgramSubRtBind(ShaderStage s,int32 n,const uint32* d)
    {glUniformSubroutinesuiv(to_enum1(s),n,d);}


    STATICINLINE void TexGetLevelParami(Texture t, uint32 l, CGenum p, int32* v)
    {glGetTexLevelParameteriv(to_enum(t),l,p,v);}
    STATICINLINE void TexGetLevelParamf(Texture t, uint32 l, CGenum p, scalar* v)
    {glGetTexLevelParameterfv(to_enum(t),l,p,v);}


    STATICINLINE void TexImage2DMS(Texture t,uint32 samples,PixelFormat ifmt,
                                   uint32 w,uint32 h)
    {glTexImage2DMultisample(to_enum(t),samples,to_enum(ifmt),w,h,GL_FALSE);}
    STATICINLINE void TexImage2DMS(Texture t,uint32 samples,PixelFormat ifmt,
                                   uint32 w,uint32 h,uint32 d)
    {glTexImage3DMultisample(to_enum(t),samples,to_enum(ifmt),w,h,d,GL_FALSE);}

    STATICINLINE void TexGetImage(Texture t,uint32 level,PixelComponents fmt,BitFormat dt,c_ptr p)
    {glGetTexImage(to_enum(t),level,to_enum(fmt),to_enum(dt),p);}
    STATICINLINE void TexGetImageCompressed(Texture t,uint32 level,c_ptr p)
    {glGetCompressedTexImage(to_enum(t),level,p);}

    /* Samplers */
    STATICINLINE void SamplerParameteruiv(CGhnd h,CGenum f,const uint32* d)
    {glSamplerParameterIuiv(h,f,d);}
    STATICINLINE void SamplerGetParameteruiv(CGhnd h,CGenum f,uint32* d)
    {glGetSamplerParameterIuiv(h,f,d);}

    /* Buffers */
    STATICINLINE void BufGetSubData(BufType t,int64 off,uint32 sz,c_ptr p)
    {glGetBufferSubData(to_enum(t),off,sz,p);}

    /* Queries */
    STATICINLINE void ConditionalRenderBegin(CGhnd h,QueryT m)
    {glBeginConditionalRender(h,to_enum(m));}
    STATICINLINE void ConditionalRenderEnd()
    {glEndConditionalRender();}

    /*Create QueryProperty enum*/
    STATICINLINE void QueryGetObjectiv(CGhnd h,CGenum f,int32* d)
    {glGetQueryObjectiv(h,f,d);}
    STATICINLINE void QueryGetObjecti64v(CGhnd h,CGenum f,int64* d)
    {glGetQueryObjecti64v(h,f,d);}
    STATICINLINE void QueryGetObjectui64v(CGhnd h,CGenum f,uint64* d)
    {glGetQueryObjectui64v(h,f,d);}

    STATICINLINE void QueryCounter(CGhnd h,CGenum t)
    {glQueryCounter(h,t);}

    /* FB */
    STATICINLINE void FBAttachTexture(FramebufferT t,CGenum att,CGhnd h,int32 level)
    {glFramebufferTexture(to_enum(t),att,h,level);}
    STATICINLINE void FBAttachTexture3D(FramebufferT t,CGenum att,Texture textrg,CGhnd h,
                                        int32 level,int32 z)
    {glFramebufferTexture3D(to_enum(t),att,to_enum(textrg),h,level,z);}

    /* VAO */
    STATICINLINE void VAOPrimitiveRestart(uint32 idx)
    {
        glPrimitiveRestartIndex(idx);
        Enable(Feature::PrimitiveRestart);
    }

    //TODO: Create FenceProperty enum


    /* Drawing */

    STATICINLINE void DrawElementsBaseVertex(CGenum p,int32 c,TypeEnum t,int64 off,int32 bv)
    {glDrawElementsBaseVertex(p,c,to_enum(t),(void*)off,bv);}
    STATICINLINE void DrawElementsInstancedBaseVertex(CGenum p,int32 c,TypeEnum t,int64 off,int32 pc,int32 bv)
    {glDrawElementsInstancedBaseVertex(p,c,to_enum(t),(void*)off,pc,bv);}

    STATICINLINE void DrawRangeElementsBaseVertex(CGenum p,uint32 f,uint32 e,int32 c,TypeEnum t,int64 off,int32 bv)
    {glDrawRangeElementsBaseVertex(p,f,e,c,to_enum(t),(void*)off,bv);}

    STATICINLINE void DrawMultiArrays(CGenum p,const int32* f,const int32* c, int32 dc)
    {glMultiDrawArrays(p,f,c,dc);}

    STATICINLINE void DrawMultiElements(CGenum p,const int32* f,TypeEnum t,const int64* off,int32 dc)
    {glMultiDrawElements(p,f,to_enum(t),(const void**)&off,dc);}
    STATICINLINE void DrawMultiElementsBaseVertex(CGenum p,const int32* c,TypeEnum t,const int64* off,int32 dc,const int32* bv)
    {glMultiDrawElementsBaseVertex(p,c,to_enum(t),(const void**)&off,dc,bv);}
};

}
}

#endif
