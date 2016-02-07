#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_ES30_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_ES30_H

#include "glbase.h"

namespace Coffee{
namespace CGL{
/*!
 * \brief OpenGL 3.3 compliance model
 */
struct CGLES30 : CGL_Implementation
{
    enum FBAttach
    {
        Renderbuffer,
        ColorBuffer,
    };

    static bool LoadBinding(CGL_Context* ctxt, GLADloadproc fun)
    {
        if(!ctxt->acquireContext())
            return false;
        if(!gladLoadGLES2Loader(fun))
            return false;

	if(!Debug::VerifyInit())
	    return false;

        Debug::GetExtensions();
        Debug::InitDebugFlag();
        return true;
    }

    /* Allocations */
    static bool TexAlloc(uint32 l,CGhnd* d){glGenTextures(l,d); return true;}
    static bool TexFree(uint32 l,CGhnd* d){glDeleteTextures(l,d); return true;}

    static bool FBAlloc(uint32 l,CGhnd* d){glGenFramebuffers(l,d); return true;}
    static bool FBFree(uint32 l,CGhnd* d){glDeleteFramebuffers(l,d); return true;}

    static bool RenderBufferAlloc(uint32 l,CGhnd* d){glGenRenderbuffers(l,d); return true;}
    static bool RenderBufferFree(uint32 l,CGhnd* d){glDeleteRenderbuffers(l,d); return true;}

    static bool SamplerAlloc(uint32 l,CGhnd* d){glGenSamplers(l,d); return true;}
    static bool SamplerFree(uint32 l,CGhnd* d){glDeleteSamplers(l,d); return true;}

    static bool BufAlloc(uint32 l,CGhnd* d){glGenBuffers(l,d); return true;}
    static bool BufFree(uint32 l,CGhnd* d){glDeleteBuffers(l,d); return true;}

    static bool VAOAlloc(uint32 l,CGhnd* d){glGenVertexArrays(l,d); return true;}
    static bool VAOFree(uint32 l,CGhnd* d){glDeleteVertexArrays(l,d); return true;}

    static bool QueryAlloc(uint32 l,CGhnd* d){glGenQueries(l,d); return true;}
    static bool QueryFree(uint32 l,CGhnd* d){glDeleteQueries(l,d); return true;}

    static bool ProgramAlloc(uint32 l,CGhnd* d)
    {
        for(uint32 i=0;i<l;i++)
            d[i] = glCreateProgram();
        return true;
    }
    static bool ProgramFree(uint32 l,CGhnd* d)
    {
        for(uint32 i=0;i<l;i++)
            glDeleteProgram(d[i]);
        return true;
    }

    static bool ShaderAlloc(uint32 l,ShaderStage t,CGhnd* d)
    {
        for(uint32 i=0;i<l;i++)
            d[i] = glCreateShader(to_enum1(t));
        return true;
    }
    static bool ShaderFree(uint32 l,CGhnd* d)
    {
        for(uint32 i=0;i<l;i++)
            glDeleteShader(d[i]);
        return true;
    }

    static bool FenceFree(uint32 l,CGsync* d)
    {
        for(uint32 i=0;i<l;i++)
            glDeleteSync((GLsync)d[i]);
        return true;
    }

    /* Shaders */
    static cstring_w ShaderGetSrc(CGhnd h)
    {
        int32 len = 0;
        ShaderGetiv(h,GL_SHADER_SOURCE_LENGTH,&len);
        if(len<=0)
            return nullptr;
        cstring_w s = new int8[len+1];
        glGetShaderSource(h,len,nullptr,s);
        s[len] = 0;
        return s;
    }
    static void ShaderSource(CGhnd h,uint32 n,cstring* src)
    {
        int32* lens = new int32[n];
        for(uint32 i=0;i<n;i++)
            lens[i] = CStrLen(src[i]+1);
        glShaderSource(h,n,src,lens);
        delete[] lens;
    }
    static cstring_w ShaderGetLog(CGhnd h)
    {
        int32 len = 0;
        ShaderGetiv(h,GL_INFO_LOG_LENGTH,&len);
        if(len<=0)
            return nullptr;
        cstring_w s = new int8[len+1];
        glGetShaderInfoLog(h,len,nullptr,s);
        s[len] = 0;
        return s;
    }
    /*TODO: Create ShaderProperty enum*/
    static void ShaderGetiv(CGhnd h,CGenum f,int32* d){glGetShaderiv(h,f,d);}
    static void ShaderAttach(CGhnd p,CGhnd s){glAttachShader(p,s);}
    static void ShaderDetach(CGhnd p,CGhnd s){glDetachShader(p,s);}
    static bool ShaderCompile(CGhnd s)
    {
        glCompileShader(s);
        int32 status;
        glGetShaderiv(s,GL_COMPILE_STATUS,&status);
        return status==GL_TRUE;
    }

    static void ProgramGetAttached(CGhnd p,uint32* n,CGhnd** h)
    {
        int32 num = 0;
        ProgramGetiv(p,GL_ATTACHED_SHADERS,&num);
        if(num<=0)
        {
            *n = 0;
            return;
        }
        CGhnd* attached = new CGhnd[num];
        glGetAttachedShaders(p,num,nullptr,attached);

        *n = num;
        *h = attached;
    }
    static bool ProgramLink(CGhnd p)
    {
        glLinkProgram(p);
        int32 status;
        ProgramGetiv(p,GL_LINK_STATUS,&status);
        return status==GL_TRUE;
    }
    static cstring_w ProgramGetLog(CGhnd h)
    {
        int32 len = 0;
        ProgramGetiv(h,GL_INFO_LOG_LENGTH,&len);
        if(len<=0)
            return nullptr;
        cstring_w s = new int8[len+1];
        glGetProgramInfoLog(h,len,nullptr,s);
        return s;
    }
    static void ProgramUse(CGhnd h)
    {
        glUseProgram(h);
    }
    static bool ProgramValidate(CGhnd h){glValidateProgram(h); return false;}
    /*TODO: Create ProgramProperty enum*/
    static void ProgramGetiv(CGhnd h,CGenum f,int32* d){glGetProgramiv(h,f,d);}

    static int32 ProgramGetFragDataLoc(CGhnd h,cstring n){return glGetFragDataLocation(h,n);}

    /* Attributes */
    static void ProgramAttribGet(CGhnd h,uint32* n,cstring_w** names,CGenum** type,int32** size)
    {
        int32 num = 0;
        ProgramGetiv(h,GL_ACTIVE_ATTRIBUTES,&num);
        if(num<=0)
        {
            *n = 0;
            return;
        }
        int32 namelen = 0;
        ProgramGetiv(h,GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,&namelen);

        names[0] = new cstring_w[num];
        type[0] = new CGenum[num];
        size[0] = new int32[num];
        for(int32 i=0;i<num;i++)
            names[0][i] = new int8[namelen];
        for(int32 i=0;i<num;i++)
            glGetActiveAttrib(h,i,namelen,nullptr,&size[0][i],&type[0][i],names[0][i]);
    }
    static int32 ProgramAttribLoc(CGhnd h,cstring n){return glGetAttribLocation(h,n);}
    static void ProgramAttribBind(CGhnd h,uint32 i,cstring n){glBindAttribLocation(h,i,n);}

    /* Uniforms */
    static void ProgramUnifGet(CGhnd h,uint32* n,cstring_w** names,CGenum** type,int32** size)
    {
        int32 num = 0;
        ProgramGetiv(h,GL_ACTIVE_UNIFORMS,&num);
        if(num<=0)
        {
            *n = 0;
            return;
        }
        int32 namelen = 0;
        ProgramGetiv(h,GL_ACTIVE_UNIFORM_MAX_LENGTH,&namelen);

        names[0] = new cstring_w[num];
        type[0] = new CGenum[num];
        size[0] = new int32[num];
        for(int32 i=0;i<num;i++)
            names[0][i] = new int8[namelen];
        for(int32 i=0;i<num;i++)
            glGetActiveUniform(h,i,namelen,nullptr,&size[0][i],&type[0][i],names[0][i]);
    }
    static int32 ProgramUnifGetLoc(CGhnd h,cstring n){return glGetUniformLocation(h,n);}

    /* Uniform blocks */
    /* Return name, binding, active uniform indices and size */
    static void ProgramUnifBlockGet(CGhnd h,uint32* n,cstring_w** names,int32** indexSize,int32*** index,int32** size)
    {
        int32 num = 0;
        ProgramGetiv(h,GL_ACTIVE_UNIFORM_BLOCKS,&num);
        if(num<=0)
        {
            *n = 0;
            return;
        }
        int32 namelen = 0;
        ProgramGetiv(h,GL_ACTIVE_UNIFORM_MAX_LENGTH,&namelen);

        names[0] = new cstring_w[num];
        indexSize[0] = new int32[num];
        index[0] = new int32*[num];
        size[0] = new int32[num];
        for(int32 i=0;i<num;i++)
            names[0][i] = new int8[namelen];
	for(uint32 i=0;i<(uint32)num;i++)
        {
            glGetActiveUniformBlockName(h,i,namelen,nullptr,names[0][i]);
            glGetActiveUniformBlockiv(h,i,GL_UNIFORM_BLOCK_DATA_SIZE,&size[0][i]);
            glGetActiveUniformBlockiv(h,i,GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,&indexSize[0][i]);
            index[0][i] = new int32[indexSize[0][i]];
            glGetActiveUniformBlockiv(h,i,GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,index[0][i]);
        }
    }
    static uint32 ProgramUnifBlockGetLoc(CGhnd h,cstring n){return glGetUniformBlockIndex(h,n);}
    static void ProgramUnifBlockBind(CGhnd h,uint32 l,uint32 i){glUniformBlockBinding(h,l,i);}

    /* Textures */
    static void TexBind(Texture t,CGhnd h){glBindTexture(to_enum(t),h);}
    static void TexActive(uint32 i){glActiveTexture(GL_TEXTURE0+i);}

    static void TexImage2D(Texture t,uint32 level,PixelFormat ifmt,
                           uint32 w,uint32 h,int32 border,PixelComponents fmt,
                           BitFormat dt,c_cptr p)
    {glTexImage2D(to_enum(t),level,(int32)ifmt,w,h,border,to_enum(fmt),to_enum(dt),p);}
    static void TexImage3D(Texture t,uint32 level,PixelFormat ifmt,
                           uint32 w,uint32 h,uint32 d,int32 border,
                           PixelComponents fmt,BitFormat dt,c_cptr p)
    {glTexImage3D(to_enum(t),level,(int32)ifmt,w,h,d,border,to_enum(fmt),to_enum(dt),p);}


    static void TexSubImage2D(Texture t,uint32 level,int32 x,int32 y,
                              uint32 w,uint32 h,PixelComponents fmt,BitFormat dt,c_cptr p)
    {glTexSubImage2D(to_enum(t),level,x,y,w,h,to_enum(fmt),to_enum(dt),p);}
    static void TexSubImage3D(Texture t,uint32 level,int32 x,int32 y,int32 z,
                              uint32 w,uint32 h,uint32 d,PixelComponents fmt,
                              BitFormat dt,c_cptr p)
    {glTexSubImage3D(to_enum(t),level,x,y,z,w,h,d,to_enum(fmt),to_enum(dt),p);}

    static void TexImageCompressed2D(Texture t,int32 level,
                                     PixelFormat ifmt, PixelFlags flgs,
                                     CompFlags cflgs,
                                     uint32 w,uint32 h,int32 border,uint32 sz,c_cptr p)
    {glCompressedTexImage2D(to_enum(t),level,to_enum(ifmt,flgs,cflgs),w,h,border,sz,p);}
    static void TexImageCompressed3D(Texture t,int32 level,
                                     PixelFormat ifmt, PixelFlags flgs,
                                     CompFlags cflgs,
                                     uint32 w,uint32 h,uint32 d,int32 border,uint32 sz,c_cptr p)
    {glCompressedTexImage3D(to_enum(t),level,to_enum(ifmt,flgs,cflgs),w,h,d,border,sz,p);}

    static void TexSubImageCompressed2D(Texture t,uint32 level,int32 x,int32 y,
                                        uint32 w,uint32 h,
                                        PixelFormat fmt, PixelFlags flgs,
                                        CompFlags cflgs,
                                        int32 sz,c_cptr p)
    {glCompressedTexSubImage2D(to_enum(t),level,x,y,w,h,to_enum(fmt,flgs,cflgs),sz,p);}
    static void TexSubImageCompressed3D(Texture t,uint32 level,int32 x,int32 y,int32 z,
                                        uint32 w,uint32 h,uint32 d,
                                        PixelFormat fmt, PixelFlags flgs,
                                        CompFlags cflgs,
                                        uint32 sz,c_cptr p)
    {glCompressedTexSubImage3D(to_enum(t),level,x,y,z,w,h,d,to_enum(fmt,flgs,cflgs),sz,p);}

    static void TexCopyImage2D(Texture t,uint32 level,PixelFormat fmt,int32 x,int32 y,
                               uint32 w,uint32 h,int32 border)
    {glCopyTexImage2D(to_enum(t),level,to_enum(fmt),x,y,w,h,border);}

    static void TexCopySubImage2D(Texture t,uint32 level,int32 xo,int32 yo,
                                  int32 x,int32 y,
                                  uint32 w,uint32 h)
    {glCopyTexSubImage2D(to_enum(t),level,xo,yo,x,y,w,h);}
    static void TexCopySubImage3D(Texture t,uint32 level,int32 xo,int32 yo,int32 zo,
                                  int32 x,int32 y,
                                  uint32 w,uint32 h)
    {glCopyTexSubImage3D(to_enum(t),level,xo,yo,zo,x,y,w,h);}

//    static void TexGetImageCompressed(Texture t,uint32 level,c_ptr p)
//    {glGetCompressedTexImage(to_enum(t),level,p);}

    static void TexGenMipmap(Texture t){glGenerateMipmap(to_enum(t));}

    /* Samplers */
    static void SamplerBind(uint32 i,CGhnd h){glBindSampler(i,h);}

    /*TODO: Create SamplerProperty enum*/
    static void SamplerParameteriv(CGhnd h,CGenum f,const int32* d){glSamplerParameteriv(h,f,d);}
    static void SamplerParameterfv(CGhnd h,CGenum f,const scalar* d){glSamplerParameterfv(h,f,d);}

    static void SamplerGetParameteriv(CGhnd h,CGenum f,int32* d){glGetSamplerParameteriv(h,f,d);}
    static void SamplerGetParameterfv(CGhnd h,CGenum f,scalar* d){glGetSamplerParameterfv(h,f,d);}

    /* Buffers */
    static void BufBind(BufType t,CGhnd h){glBindBuffer(to_enum(t),h);}

    static void BufData(BufType t,uint64 sz,c_cptr p,ResourceAccess a)
    {glBufferData(to_enum(t),sz,p,to_enum1(a));}
    static void BufSubData(BufType t,int64 off,uint32 sz,c_cptr p)
    {glBufferSubData(to_enum(t),off,sz,p);}
    static void BufCopySubData(BufType t1,BufType t2,int64 off1,int64 off2,uint32 sz)
    {glCopyBufferSubData(to_enum(t1),to_enum(t2),off1,off2,sz);}

    static void BufBindRange(BufType t,uint32 i,CGhnd b,int64 off,uint32 sz)
    {glBindBufferRange(to_enum(t),i,b,off,sz);}
    static void BufMapRange(BufType t,int64 off,uint32 sz,CGflag a)
    {glMapBufferRange(to_enum(t),off,sz,a);}
    static void BufUnmap(BufType t){glUnmapBuffer(to_enum(t));}

    /* Queries */
    static void QueryBegin(QueryT t,CGhnd h){glBeginQuery(to_enum(t),h);}
    static void QueryEnd(QueryT t){glEndQuery(to_enum(t));}

    /*Create QueryProperty enum*/
    static void QueryGetObjectuiv(CGhnd h,CGenum f,uint32* d){glGetQueryObjectuiv(h,f,d);}

    /* FB */
    static void FBBind(FramebufferT t,CGhnd h){glBindFramebuffer(to_enum(t),h);}
    static void RBufBind(FramebufferT t,CGhnd h){glBindRenderbuffer(to_enum(t),h);}

    static void RBufStorage(PixelFormat ifmt,uint32 w,uint32 h)
    {glRenderbufferStorage(GL_RENDERBUFFER,to_enum(ifmt),w,h);}
    static void RBufStorageMS(PixelFormat ifmt,uint32 samples,uint32 w,uint32 h)
    {glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples,to_enum(ifmt),w,h);}

    static bool FBCheckComplete(FramebufferT t)
    {
        CGenum f = glCheckFramebufferStatus(to_enum(t));
        if(f!=GL_FRAMEBUFFER_COMPLETE)
            cLog(__FILE__,__LINE__,"GL33","Incomplete framebuffer",f);

        return f==GL_FRAMEBUFFER_COMPLETE;
    }

    /*TODO: Create FramebufAttProp enum*/
    static void FBGetAttachParameter(FramebufferT t,CGenum att,CGenum p,int32* d)
    {glGetFramebufferAttachmentParameteriv(to_enum(t),att,p,d);}

    static void FBAttachTexture2D(FramebufferT t,CGenum att,Texture textrg,CGhnd h,int32 level)
    {glFramebufferTexture2D(to_enum(t),att,to_enum(textrg),h,level);}
    static void FBAttachTextureLayer(FramebufferT t,CGenum att,CGhnd h,int32 level,int32 layer)
    {glFramebufferTextureLayer(to_enum(t),att,h,level,layer);}
    static void FBAttachRenderBuffer(FramebufferT t,CGenum att,CGhnd h)
    {glFramebufferRenderbuffer(to_enum(t),att,GL_RENDERBUFFER,h);}

    static void FBBlit(const CRect& s, const CRect& d,CGflag m,CGenum f)
    {glBlitFramebuffer(s.x,s.y,s.w,s.h, d.x,d.y,d.w,d.h, m,f);}

    static void RBufGetParameteri(CGenum p,int32* d)
    {glGetRenderbufferParameteriv(GL_RENDERBUFFER,p,d);}

    static void FBDrawBuffers(uint32 n,const CGenum* d){glDrawBuffers(n,d);}
    static void FBReadBuffer(CGenum b){glReadBuffer(b);}

    static void FBReadPixels(int32 x,int32 y,uint32 w,uint32 h,
                             PixelComponents fmt,TypeEnum dt,c_ptr p)
    {glReadPixels(x,y,w,h,to_enum(fmt),to_enum(dt),p);}

    /* XFB */
    static void XFBegin(CGenum p){glBeginTransformFeedback(p);}
    static void XFEnd(){glEndTransformFeedback();}

    static void XFVaryings(CGhnd h,int32 n,cstring* names,AttribMode a)
    {glTransformFeedbackVaryings(h,n,names,to_enum(a));}
    static void XFGetVaryings(CGhnd h,uint32* n,cstring_w** names,CGenum** type, int32** size)
    {
        int32 num = 0;
        ProgramGetiv(h,GL_TRANSFORM_FEEDBACK_VARYINGS,&num);

        type[0] = new CGenum[num];
        size[0] = new int32[num];
        names[0] = new cstring_w[num];
        int32 namelen = 0;
        ProgramGetiv(h,GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH,&namelen);
        for(int32 i=0;i<num;i++)
            names[0][i] = new int8[namelen];

        for(int32 i=0;i<num;i++)
            glGetTransformFeedbackVarying(h,i,namelen,nullptr,&size[0][i],&type[0][i],names[0][i]);
        *n = num;
    }

    /* VAO */
    static void VAOBind(CGhnd h){glBindVertexArray(h);}

    static void VAOEnableAttrib(uint32 i){glEnableVertexAttribArray(i);}
    static void VAOAttribPointer(uint32 i,int32 s,CGenum t,bool n,int64 stride,int64 offset){glVertexAttribPointer(i,s,t,(n) ? GL_TRUE : GL_FALSE,stride,(void*)offset);}
    static void VAOAttribIPointer(uint32 i,int32 s,CGenum t,int64 stride,int64 offset){glVertexAttribIPointer(i,s,t,stride,(void*)offset);}
    static void VAODivisor(uint32 i,uint32 d){glVertexAttribDivisor(i,d);}

    /* Sync */
    static CGsync FenceCreate(){return glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);}
    static bool FenceAwait(CGsync s,uint64 t,CGenum *r = nullptr)
    {
        CGenum status = glClientWaitSync((GLsync)s,0,t);
        if(r)
            *r = status;
        return status!=GL_TIMEOUT_EXPIRED;
    }
    static void FenceServerAwait(CGsync s){glWaitSync((GLsync)s,0,0);}

    //TODO: Create FenceProperty enum
    static void FenceGetiv(CGsync s,CGenum p,int32* d){glGetSynciv((GLsync)s,p,1,nullptr,d);}

    /* Using Uniform* */
    static void Uniformfv(int32 l,int32 c,const scalar* d){glUniform1fv(l,c,d);}
    static void Uniformiv(int32 l,int32 c,const int32* d){glUniform1iv(l,c,d);}

    static void Uniformfv(int32 l,int32 c,const _cbasic_vec2<scalar>* d){glUniform2fv(l,c,(scalar*)d);}
    static void Uniformiv(int32 l,int32 c,const _cbasic_vec2<int32>* d){glUniform2iv(l,c,(int32*)d);}

    static void Uniformfv(int32 l,int32 c,const _cbasic_vec3<scalar>* d){glUniform3fv(l,c,(scalar*)d);}
    static void Uniformiv(int32 l,int32 c,const _cbasic_vec3<int32>* d){glUniform3iv(l,c,(int32*)d);}

    static void Uniformfv(int32 l,int32 c,const _cbasic_vec4<scalar>* d){glUniform4fv(l,c,(scalar*)d);}
    static void Uniformiv(int32 l,int32 c,const _cbasic_vec4<int32>* d){glUniform4iv(l,c,(int32*)d);}

    static void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,2>* d){glUniformMatrix2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,2,3>* d){glUniformMatrix2x3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,3>* d){glUniformMatrix3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,2>* d){glUniformMatrix3x2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,4>* d){glUniformMatrix3x4fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,4>* d){glUniformMatrix4fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,3>* d){glUniformMatrix4x3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    static void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,2>* d){glUniformMatrix4x2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    /* Drawing */
    static void DrawArrays(CGenum p,int32 f,int64 c)
    {glDrawArrays(p,f,c);}
    static void DrawArraysInstanced(CGenum p,int32 f,int32 c,int32 ic)
    {glDrawArraysInstanced(p,f,c,ic);}

    static void DrawElements(CGenum p,int32 c,CGenum t,int64 off)
    {glDrawElements(p,c,t,(void*)off);}
    static void DrawElementsInstanced(CGenum p,int32 c,TypeEnum t,int64 off,uint64 pc)
    {glDrawElementsInstanced(p,c,to_enum(t),(void*)off,pc);}

    static void DrawRangeElements(CGenum p,uint32 f,uint32 e,int32 c,TypeEnum t,int64 off)
    {glDrawRangeElements(p,f,e,c,to_enum(t),(void*)off);}
};

}
}

#endif
