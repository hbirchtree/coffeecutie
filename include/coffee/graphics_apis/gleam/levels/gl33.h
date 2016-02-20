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

    /* Allocations */
    STATICINLINE bool TexAlloc(uint32 l,CGhnd* d){glGenTextures(l,d); return true;}
    STATICINLINE bool TexFree(uint32 l,CGhnd* d){glDeleteTextures(l,d); return true;}

    STATICINLINE bool FBAlloc(uint32 l,CGhnd* d){glGenFramebuffers(l,d); return true;}
    STATICINLINE bool FBFree(uint32 l,CGhnd* d){glDeleteFramebuffers(l,d); return true;}

    STATICINLINE bool RenderBufferAlloc(uint32 l,CGhnd* d){glGenRenderbuffers(l,d); return true;}
    STATICINLINE bool RenderBufferFree(uint32 l,CGhnd* d){glDeleteRenderbuffers(l,d); return true;}

    STATICINLINE bool SamplerAlloc(uint32 l,CGhnd* d){glGenSamplers(l,d); return true;}
    STATICINLINE bool SamplerFree(uint32 l,CGhnd* d){glDeleteSamplers(l,d); return true;}

    STATICINLINE bool BufAlloc(uint32 l,CGhnd* d){glGenBuffers(l,d); return true;}
    STATICINLINE bool BufFree(uint32 l,CGhnd* d){glDeleteBuffers(l,d); return true;}

    STATICINLINE bool VAOAlloc(uint32 l,CGhnd* d){glGenVertexArrays(l,d); return true;}
    STATICINLINE bool VAOFree(uint32 l,CGhnd* d){glDeleteVertexArrays(l,d); return true;}

    STATICINLINE bool QueryAlloc(uint32 l,CGhnd* d){glGenQueries(l,d); return true;}
    STATICINLINE bool QueryFree(uint32 l,CGhnd* d){glDeleteQueries(l,d); return true;}

    STATICINLINE bool ProgramAlloc(uint32 l,CGhnd* d)
    {
        for(uint32 i=0;i<l;i++)
            d[i] = glCreateProgram();
        return true;
    }
    STATICINLINE bool ProgramFree(uint32 l,CGhnd* d)
    {
        for(uint32 i=0;i<l;i++)
            glDeleteProgram(d[i]);
        return true;
    }

    STATICINLINE bool ShaderAlloc(uint32 l,ShaderStage t,CGhnd* d)
    {
        for(uint32 i=0;i<l;i++)
            d[i] = glCreateShader(to_enum1(t));
        return true;
    }
    STATICINLINE bool ShaderFree(uint32 l,CGhnd* d)
    {
        for(uint32 i=0;i<l;i++)
            glDeleteShader(d[i]);
        return true;
    }

    STATICINLINE bool FenceFree(uint32 l,CGsync* d)
    {
        for(uint32 i=0;i<l;i++)
            glDeleteSync((GLsync)d[i]);
        return true;
    }

    /* Shaders */
    STATICINLINE cstring_w ShaderGetSrc(CGhnd h)
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
    STATICINLINE void ShaderSource(CGhnd h,uint32 n,cstring* src)
    {
        int32* lens = new int32[n];
        for(uint32 i=0;i<n;i++)
            lens[i] = CStrLen(src[i]+1);
        glShaderSource(h,n,src,lens);
        delete[] lens;
    }
    STATICINLINE cstring_w ShaderGetLog(CGhnd h)
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
    STATICINLINE void ShaderGetiv(CGhnd h,CGenum f,int32* d){glGetShaderiv(h,f,d);}
    STATICINLINE void ShaderAttach(CGhnd p,CGhnd s){glAttachShader(p,s);}
    STATICINLINE void ShaderDetach(CGhnd p,CGhnd s){glDetachShader(p,s);}
    STATICINLINE bool ShaderCompile(CGhnd s)
    {
        glCompileShader(s);
        int32 status;
        glGetShaderiv(s,GL_COMPILE_STATUS,&status);
        return status==GL_TRUE;
    }

    STATICINLINE void ProgramGetAttached(CGhnd p,uint32* n,CGhnd** h);
    STATICINLINE bool ProgramLink(CGhnd p)
    {
        glLinkProgram(p);
        int32 status;
        ProgramGetiv(p,GL_LINK_STATUS,&status);
        return status==GL_TRUE;
    }
    STATICINLINE cstring_w ProgramGetLog(CGhnd h);
    STATICINLINE void ProgramUse(CGhnd h)
    {
        glUseProgram(h);
    }
    STATICINLINE bool ProgramValidate(CGhnd h){glValidateProgram(h); return false;}
    /*TODO: Create ProgramProperty enum*/
    STATICINLINE void ProgramGetiv(CGhnd h,CGenum f,int32* d){glGetProgramiv(h,f,d);}

    STATICINLINE int32 ProgramGetFragDataLoc(CGhnd h,cstring n){return glGetFragDataLocation(h,n);}
    STATICINLINE int32 ProgramGetFragDataIdx(CGhnd h,cstring n){return glGetFragDataIndex(h,n);}
    STATICINLINE void ProgramBindFragData(CGhnd h,uint32 l,cstring n){glBindFragDataLocation(h,l,n);}
    STATICINLINE void ProgramBindFragDataIndexed(CGhnd h,uint32 i,uint32 l,cstring n)
    {
        glBindFragDataLocationIndexed(h,l,i,n);
    }

    /* Attributes */
    STATICINLINE void ProgramAttribGet(CGhnd h,uint32* n,cstring_w** names,CGenum** type,int32** size);
    STATICINLINE int32 ProgramAttribLoc(CGhnd h,cstring n){return glGetAttribLocation(h,n);}
    STATICINLINE void ProgramAttribBind(CGhnd h,uint32 i,cstring n){glBindAttribLocation(h,i,n);}

    /* Uniforms */
    STATICINLINE void ProgramUnifGet(CGhnd h,uint32* n,cstring_w** names,CGenum** type,int32** size);
    STATICINLINE int32 ProgramUnifGetLoc(CGhnd h,cstring n){return glGetUniformLocation(h,n);}

    /* Uniform blocks */
    /* Return name, binding, active uniform indices and size */
    STATICINLINE void ProgramUnifBlockGet(CGhnd h,uint32* n,cstring_w** names,int32** indexSize,int32*** index,int32** size);
    STATICINLINE uint32 ProgramUnifBlockGetLoc(CGhnd h,cstring n){return glGetUniformBlockIndex(h,n);}
    STATICINLINE void ProgramUnifBlockBind(CGhnd h,uint32 l,uint32 i){glUniformBlockBinding(h,l,i);}

    /* Subroutines */
    STATICINLINE void ProgramSubRtGet(CGhnd h,ShaderStage s,uint32* n,
                                cstring_w** names,int32** rtSize,int32*** rt);
    STATICINLINE uint32 ProgramSubRtGetLoc(CGhnd h,ShaderStage s,cstring n)
    {return glGetSubroutineIndex(h,to_enum1(s),n);}
    /* Binds all subroutine uniforms */
    STATICINLINE void ProgramSubRtBind(ShaderStage s,int32 n,const uint32* d)
    {glUniformSubroutinesuiv(to_enum1(s),n,d);}

    /* Textures */
    STATICINLINE void TexBind(Texture t,CGhnd h){glBindTexture(to_enum(t),h);}
    STATICINLINE void TexActive(uint32 i){glActiveTexture(GL_TEXTURE0+i);}

    STATICINLINE void TexGetLevelParami(Texture t, uint32 l, CGenum p, int32* v)
    {glGetTexLevelParameteriv(to_enum(t),l,p,v);}
    STATICINLINE void TexGetLevelParamf(Texture t, uint32 l, CGenum p, scalar* v)
    {glGetTexLevelParameterfv(to_enum(t),l,p,v);}

    STATICINLINE void TexImage2D(Texture t,uint32 level,PixelFormat ifmt,
                           uint32 w,uint32 h,int32 border,PixelComponents fmt,
                           BitFormat dt,c_cptr p)
    {glTexImage2D(to_enum(t),level,(int32)ifmt,w,h,border,to_enum(fmt),to_enum(dt),p);}
    STATICINLINE void TexImage3D(Texture t,uint32 level,PixelFormat ifmt,
                           uint32 w,uint32 h,uint32 d,int32 border,
                           PixelComponents fmt,BitFormat dt,c_cptr p)
    {glTexImage3D(to_enum(t),level,(int32)ifmt,w,h,d,border,to_enum(fmt),to_enum(dt),p);}

    STATICINLINE void TexImage2DMS(Texture t,uint32 samples,PixelFormat ifmt,
                           uint32 w,uint32 h)
    {glTexImage2DMultisample(to_enum(t),samples,to_enum(ifmt),w,h,GL_FALSE);}
    STATICINLINE void TexImage2DMS(Texture t,uint32 samples,PixelFormat ifmt,
                           uint32 w,uint32 h,uint32 d)
    {glTexImage3DMultisample(to_enum(t),samples,to_enum(ifmt),w,h,d,GL_FALSE);}

    STATICINLINE void TexSubImage2D(Texture t,uint32 level,int32 x,int32 y,
                              uint32 w,uint32 h,PixelComponents fmt,BitFormat dt,c_cptr p)
    {glTexSubImage2D(to_enum(t),level,x,y,w,h,to_enum(fmt),to_enum(dt),p);}
    STATICINLINE void TexSubImage3D(Texture t,uint32 level,int32 x,int32 y,int32 z,
                              uint32 w,uint32 h,uint32 d,PixelComponents fmt,
                              BitFormat dt,c_cptr p)
    {glTexSubImage3D(to_enum(t),level,x,y,z,w,h,d,to_enum(fmt),to_enum(dt),p);}

    STATICINLINE void TexImageCompressed2D(Texture t,int32 level,
                                     PixelFormat ifmt, PixelFlags flgs,
                                     CompFlags cflgs,
                                     uint32 w,uint32 h,int32 border,uint32 sz,c_cptr p)
    {glCompressedTexImage2D(to_enum(t),level,to_enum(ifmt,flgs,cflgs),w,h,border,sz,p);}
    STATICINLINE void TexImageCompressed3D(Texture t,int32 level,
                                     PixelFormat ifmt, PixelFlags flgs,
                                     CompFlags cflgs,
                                     uint32 w,uint32 h,uint32 d,int32 border,uint32 sz,c_cptr p)
    {glCompressedTexImage3D(to_enum(t),level,to_enum(ifmt,flgs,cflgs),w,h,d,border,sz,p);}

    STATICINLINE void TexSubImageCompressed2D(Texture t,uint32 level,int32 x,int32 y,
                                        uint32 w,uint32 h,
                                        PixelFormat fmt, PixelFlags flgs,
                                        CompFlags cflgs,
                                        int32 sz,c_cptr p)
    {glCompressedTexSubImage2D(to_enum(t),level,x,y,w,h,to_enum(fmt,flgs,cflgs),sz,p);}
    STATICINLINE void TexSubImageCompressed3D(Texture t,uint32 level,int32 x,int32 y,int32 z,
                                        uint32 w,uint32 h,uint32 d,
                                        PixelFormat fmt, PixelFlags flgs,
                                        CompFlags cflgs,
                                        uint32 sz,c_cptr p)
    {glCompressedTexSubImage3D(to_enum(t),level,x,y,z,w,h,d,to_enum(fmt,flgs,cflgs),sz,p);}

    STATICINLINE void TexCopyImage2D(Texture t,uint32 level,PixelFormat fmt,int32 x,int32 y,
                               uint32 w,uint32 h,int32 border)
    {glCopyTexImage2D(to_enum(t),level,to_enum(fmt),x,y,w,h,border);}

    STATICINLINE void TexCopySubImage2D(Texture t,uint32 level,int32 xo,int32 yo,
                                  int32 x,int32 y,
                                  uint32 w,uint32 h)
    {glCopyTexSubImage2D(to_enum(t),level,xo,yo,x,y,w,h);}
    STATICINLINE void TexCopySubImage3D(Texture t,uint32 level,int32 xo,int32 yo,int32 zo,
                                  int32 x,int32 y,
                                  uint32 w,uint32 h)
    {glCopyTexSubImage3D(to_enum(t),level,xo,yo,zo,x,y,w,h);}

    STATICINLINE void TexGetImage(Texture t,uint32 level,PixelComponents fmt,BitFormat dt,c_ptr p)
    {glGetTexImage(to_enum(t),level,to_enum(fmt),to_enum(dt),p);}
    STATICINLINE void TexGetImageCompressed(Texture t,uint32 level,c_ptr p)
    {glGetCompressedTexImage(to_enum(t),level,p);}

    STATICINLINE void TexGenMipmap(Texture t){glGenerateMipmap(to_enum(t));}

    /* Samplers */
    STATICINLINE void SamplerBind(uint32 i,CGhnd h){glBindSampler(i,h);}

    /*TODO: Create SamplerProperty enum*/
    STATICINLINE void SamplerParameteriv(CGhnd h,CGenum f,const int32* d){glSamplerParameteriv(h,f,d);}
    STATICINLINE void SamplerParameteruiv(CGhnd h,CGenum f,const uint32* d){glSamplerParameterIuiv(h,f,d);}
    STATICINLINE void SamplerParameterfv(CGhnd h,CGenum f,const scalar* d){glSamplerParameterfv(h,f,d);}

    STATICINLINE void SamplerGetParameteriv(CGhnd h,CGenum f,int32* d){glGetSamplerParameteriv(h,f,d);}
    STATICINLINE void SamplerGetParameteruiv(CGhnd h,CGenum f,uint32* d){glGetSamplerParameterIuiv(h,f,d);}
    STATICINLINE void SamplerGetParameterfv(CGhnd h,CGenum f,scalar* d){glGetSamplerParameterfv(h,f,d);}

    /* Buffers */
    STATICINLINE void BufBind(BufType t,CGhnd h){glBindBuffer(to_enum(t),h);}

    STATICINLINE void BufData(BufType t,uint64 sz,c_cptr p,ResourceAccess a)
    {glBufferData(to_enum(t),sz,p,to_enum1(a));}
    STATICINLINE void BufSubData(BufType t,int64 off,uint32 sz,c_cptr p)
    {glBufferSubData(to_enum(t),off,sz,p);}
    STATICINLINE void BufGetSubData(BufType t,int64 off,uint32 sz,c_ptr p)
    {glGetBufferSubData(to_enum(t),off,sz,p);}
    STATICINLINE void BufCopySubData(BufType t1,BufType t2,int64 off1,int64 off2,uint32 sz)
    {glCopyBufferSubData(to_enum(t1),to_enum(t2),off1,off2,sz);}

    STATICINLINE void BufBindRange(BufType t,uint32 i,CGhnd b,int64 off,uint32 sz)
    {glBindBufferRange(to_enum(t),i,b,off,sz);}
    STATICINLINE void BufMapRange(BufType t,int64 off,uint32 sz,CGflag a)
    {glMapBufferRange(to_enum(t),off,sz,a);}
    STATICINLINE void BufUnmap(BufType t){glUnmapBuffer(to_enum(t));}

    /* Queries */
    STATICINLINE void ConditionalRenderBegin(CGhnd h,QueryT m)
    {glBeginConditionalRender(h,to_enum(m));}
    STATICINLINE void ConditionalRenderEnd(){glEndConditionalRender();}

    STATICINLINE void QueryBegin(QueryT t,CGhnd h){glBeginQuery(to_enum(t),h);}
    STATICINLINE void QueryEnd(QueryT t){glEndQuery(to_enum(t));}

    /*Create QueryProperty enum*/
    STATICINLINE void QueryGetObjectiv(CGhnd h,CGenum f,int32* d){glGetQueryObjectiv(h,f,d);}
    STATICINLINE void QueryGetObjectuiv(CGhnd h,CGenum f,uint32* d){glGetQueryObjectuiv(h,f,d);}
    STATICINLINE void QueryGetObjecti64v(CGhnd h,CGenum f,int64* d){glGetQueryObjecti64v(h,f,d);}
    STATICINLINE void QueryGetObjectui64v(CGhnd h,CGenum f,uint64* d){glGetQueryObjectui64v(h,f,d);}

    STATICINLINE void QueryCounter(CGhnd h,CGenum t){glQueryCounter(h,t);}

    /* FB */
    STATICINLINE void FBBind(FramebufferT t,CGhnd h){glBindFramebuffer(to_enum(t),h);}
    STATICINLINE void RBufBind(FramebufferT t,CGhnd h){glBindRenderbuffer(to_enum(t),h);}

    STATICINLINE void RBufStorage(PixelFormat ifmt,uint32 w,uint32 h)
    {glRenderbufferStorage(GL_RENDERBUFFER,to_enum(ifmt),w,h);}
    STATICINLINE void RBufStorageMS(PixelFormat ifmt,uint32 samples,uint32 w,uint32 h)
    {glRenderbufferStorageMultisample(GL_RENDERBUFFER,samples,to_enum(ifmt),w,h);}

    STATICINLINE bool FBValidate(FramebufferT t);

    /*TODO: Create FramebufAttProp enum*/
    STATICINLINE void FBGetAttachParameter(FramebufferT t,CGenum att,CGenum p,int32* d)
    {glGetFramebufferAttachmentParameteriv(to_enum(t),att,p,d);}

    STATICINLINE void FBAttachTexture(FramebufferT t,CGenum att,CGhnd h,int32 level)
    {glFramebufferTexture(to_enum(t),att,h,level);}

    STATICINLINE void FBAttachTexture2D(FramebufferT t,CGenum att,Texture textrg,CGhnd h,int32 level)
    {glFramebufferTexture2D(to_enum(t),att,to_enum(textrg),h,level);}
    STATICINLINE void FBAttachTexture3D(FramebufferT t,CGenum att,Texture textrg,CGhnd h,
                                  int32 level,int32 z)
    {glFramebufferTexture3D(to_enum(t),att,to_enum(textrg),h,level,z);}
    STATICINLINE void FBAttachTextureLayer(FramebufferT t,CGenum att,CGhnd h,int32 level,int32 layer)
    {glFramebufferTextureLayer(to_enum(t),att,h,level,layer);}
    STATICINLINE void FBAttachRenderBuffer(FramebufferT t,CGenum att,CGhnd h)
    {glFramebufferRenderbuffer(to_enum(t),att,GL_RENDERBUFFER,h);}

    STATICINLINE void FBBlit(const CRect& s, const CRect& d,CGflag m,CGenum f)
    {glBlitFramebuffer(s.x,s.y,s.w,s.h, d.x,d.y,d.w,d.h, m,f);}

    STATICINLINE void RBufGetParameteri(CGenum p,int32* d)
    {glGetRenderbufferParameteriv(GL_RENDERBUFFER,p,d);}

    STATICINLINE void FBDrawBuffers(uint32 n,const CGenum* d){glDrawBuffers(n,d);}
    STATICINLINE void FBReadBuffer(CGenum b){glReadBuffer(b);}

    STATICINLINE void FBReadPixels(int32 x,int32 y,uint32 w,uint32 h,
                             PixelComponents fmt,TypeEnum dt,c_ptr p)
    {glReadPixels(x,y,w,h,to_enum(fmt),to_enum(dt),p);}

    /* XFB */
    STATICINLINE void XFBegin(CGenum p){glBeginTransformFeedback(p);}
    STATICINLINE void XFEnd(){glEndTransformFeedback();}

    STATICINLINE void XFVaryings(CGhnd h,int32 n,cstring* names,AttribMode a)
    {glTransformFeedbackVaryings(h,n,names,to_enum(a));}
    STATICINLINE void XFGetVaryings(CGhnd h,uint32* n,cstring_w** names,CGenum** type, int32** size);

    /* VAO */
    STATICINLINE void VAOBind(CGhnd h){glBindVertexArray(h);}

    STATICINLINE void VAOEnableAttrib(uint32 i){glEnableVertexAttribArray(i);}
    STATICINLINE void VAOAttribPointer(uint32 i,int32 s,CGenum t,bool n,int64 stride,int64 offset){glVertexAttribPointer(i,s,t,(n) ? GL_TRUE : GL_FALSE,stride,(void*)offset);}
    STATICINLINE void VAOAttribIPointer(uint32 i,int32 s,CGenum t,int64 stride,int64 offset){glVertexAttribIPointer(i,s,t,stride,(void*)offset);}
    STATICINLINE void VAODivisor(uint32 i,uint32 d){glVertexAttribDivisor(i,d);}

    STATICINLINE void VAOPrimitiveRestart(uint32 idx);

    /* Sync */
    STATICINLINE CGsync FenceCreate(){return glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);}
    STATICINLINE bool FenceAwait(CGsync s,uint64 t,CGenum *r = nullptr);
    STATICINLINE void FenceServerAwait(CGsync s){glWaitSync((GLsync)s,0,0);}

    //TODO: Create FenceProperty enum
    STATICINLINE void FenceGetiv(CGsync s,CGenum p,int32* d){glGetSynciv((GLsync)s,p,1,nullptr,d);}

    /* Using Uniform* */
    STATICINLINE void Uniformfv(int32 l,int32 c,const scalar* d){glUniform1fv(l,c,d);}
    STATICINLINE void Uniformiv(int32 l,int32 c,const int32* d){glUniform1iv(l,c,d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,const _cbasic_vec2<scalar>* d){glUniform2fv(l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(int32 l,int32 c,const _cbasic_vec2<int32>* d){glUniform2iv(l,c,(int32*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,const _cbasic_vec3<scalar>* d){glUniform3fv(l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(int32 l,int32 c,const _cbasic_vec3<int32>* d){glUniform3iv(l,c,(int32*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,const _cbasic_vec4<scalar>* d){glUniform4fv(l,c,(scalar*)d);}
    STATICINLINE void Uniformiv(int32 l,int32 c,const _cbasic_vec4<int32>* d){glUniform4iv(l,c,(int32*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,2>* d){glUniformMatrix2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,2,3>* d){glUniformMatrix2x3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,3>* d){glUniformMatrix3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,2>* d){glUniformMatrix3x2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,3,4>* d){glUniformMatrix3x4fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmatrix<scalar,4>* d){glUniformMatrix4fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,3>* d){glUniformMatrix4x3fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    STATICINLINE void Uniformfv(int32 l,int32 c,bool t,const _cbasic_tmnmatrix<scalar,4,2>* d){glUniformMatrix4x2fv(l,c,(t)?GL_TRUE:GL_FALSE,(scalar*)d);}

    /* Drawing */
    STATICINLINE void DrawArrays(CGenum p,int32 f,int64 c)
    {glDrawArrays(p,f,c);}
    STATICINLINE void DrawArraysInstanced(CGenum p,int32 f,int32 c,int32 ic)
    {glDrawArraysInstanced(p,f,c,ic);}

    STATICINLINE void DrawElements(CGenum p,int32 c,CGenum t,int64 off)
    {glDrawElements(p,c,t,(void*)off);}
    STATICINLINE void DrawElementsInstanced(CGenum p,int32 c,TypeEnum t,int64 off,uint64 pc)
    {glDrawElementsInstanced(p,c,to_enum(t),(void*)off,pc);}
    STATICINLINE void DrawElementsBaseVertex(CGenum p,int32 c,TypeEnum t,int64 off,int32 bv)
    {glDrawElementsBaseVertex(p,c,to_enum(t),(void*)off,bv);}
    STATICINLINE void DrawElementsInstancedBaseVertex(CGenum p,int32 c,TypeEnum t,int64 off,int32 pc,int32 bv)
    {glDrawElementsInstancedBaseVertex(p,c,to_enum(t),(void*)off,pc,bv);}

    STATICINLINE void DrawRangeElements(CGenum p,uint32 f,uint32 e,int32 c,TypeEnum t,int64 off)
    {glDrawRangeElements(p,f,e,c,to_enum(t),(void*)off);}
    STATICINLINE void DrawRangeElementsBaseVertex(CGenum p,uint32 f,uint32 e,int32 c,TypeEnum t,int64 off,int32 bv)
    {glDrawRangeElementsBaseVertex(p,f,e,c,to_enum(t),(void*)off,bv);}

    STATICINLINE void DrawMultiArrays(CGenum p,const int32* f,const int32* c, int32 dc)
    {glMultiDrawArrays(p,f,c,dc);}

    STATICINLINE void DrawMultiElements(CGenum p,const int32* f,TypeEnum t,const int64* off,int32 dc)
    {glMultiDrawElements(p,f,to_enum(t),(const void**)&off,dc);}
    STATICINLINE void DrawMultiElementsBaseVertex(CGenum p,const int32* c,TypeEnum t,const int64* off,int32 dc,const int32* bv)
    {glMultiDrawElementsBaseVertex(p,c,to_enum(t),(const void**)&off,dc,bv);}
};

struct CGLUtil
{
    /*!
     * \brief Dump texture to file. Dirties texture binding.
     * \param t
     * \param h
     * \param fn
     */
    STATICINLINE void DumpTexture(CGL_Implementation::Texture t, CGL_Implementation::CGhnd h,
                            uint32 l, cstring fn)
    {
        CSize tsize;
        CGL33::TexGetLevelParami(t,l,GL_TEXTURE_WIDTH,&tsize.w);
        CGL33::TexGetLevelParami(t,l,GL_TEXTURE_HEIGHT,&tsize.h);

        szptr data_size = tsize.area()*4;
        ubyte_t* data = (ubyte_t*)Alloc(data_size);

        CGL33::TexBind(t,h);
        CGL33::TexGetImage(t,0,PixelComponents::RGBA,BitFormat::Byte,data);
        CGL33::TexBind(t,0);

        CResources::CResource rsc(fn);

        CStbImageLib::CStbImageConst img;
        img.bpp = 4;
        img.data = data;
        img.size = tsize;

        CStbImageLib::SavePNG(&rsc,&img);

        CResources::FileCommit(rsc);
        CResources::FileFree(rsc);
        CFree(data);
    }
};

FORCEDINLINE void CGL33::ProgramGetAttached(CGL_Implementation::CGhnd p, uint32 *n, CGL_Implementation::CGhnd **h)
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

FORCEDINLINE cstring_w CGL33::ProgramGetLog(CGL_Implementation::CGhnd h)
{
    int32 len = 0;
    ProgramGetiv(h,GL_INFO_LOG_LENGTH,&len);
    if(len<=0)
        return nullptr;
    cstring_w s = new int8[len+1];
    glGetProgramInfoLog(h,len,nullptr,s);
    return s;
}

FORCEDINLINE void CGL33::ProgramAttribGet(CGL_Implementation::CGhnd h, uint32 *n, cstring_w **names, CGL_Implementation::CGenum **type, int32 **size)
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

FORCEDINLINE void CGL33::ProgramUnifGet(CGL_Implementation::CGhnd h, uint32 *n, cstring_w **names, CGL_Implementation::CGenum **type, int32 **size)
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

FORCEDINLINE void CGL33::ProgramUnifBlockGet(CGL_Implementation::CGhnd h, uint32 *n, cstring_w **names, int32 **indexSize, int32 ***index, int32 **size)
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

FORCEDINLINE void CGL33::ProgramSubRtGet(CGL_Implementation::CGhnd h, CGL_Implementation::ShaderStage s, uint32 *n, cstring_w **names, int32 **rtSize, int32 ***rt)
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

FORCEDINLINE bool CGL33::FBValidate(CGL_Implementation::FramebufferT t)
{
    CGenum f = glCheckFramebufferStatus(to_enum(t));
    if(f!=GL_FRAMEBUFFER_COMPLETE)
        cLog(__FILE__,__LINE__,"GL33","Incomplete framebuffer",f);

    return f==GL_FRAMEBUFFER_COMPLETE;
}

FORCEDINLINE void CGL33::XFGetVaryings(CGL_Implementation::CGhnd h, uint32 *n, cstring_w **names, CGL_Implementation::CGenum **type, int32 **size)
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

FORCEDINLINE void CGL33::VAOPrimitiveRestart(uint32 idx)
{
    glPrimitiveRestartIndex(idx);
    Enable(Feature::PrimitiveRestart);
}

FORCEDINLINE bool CGL33::FenceAwait(CGL_Implementation::CGsync s, uint64 t, CGL_Implementation::CGenum *r)
{
    CGenum status = glClientWaitSync((GLsync)s,0,t);
    if(r)
        *r = status;
    return status!=GL_TIMEOUT_EXPIRED;
}

}
}

#endif
