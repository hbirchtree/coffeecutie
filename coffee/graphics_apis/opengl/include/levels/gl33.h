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
    using CGsync = void*;

    static bool LoadBinding()
    {
        return gladLoadGL();
    }

    static bool GeometryShaderSupported(){return false;}

    static bool CubemapSeamlessSupported(){return false;}

    static bool ASTCSupported(){return false;}

    static bool BPTCSupported(){return false;}

    static bool RGTCSupported(){return false;}

    static bool DXTCSupported(){return false;}

    /* Base */
    static void Enable(CGenum e){glEnable(e);}
    static void Disable(CGenum e){glDisable(e);}

    static void Clear(CGflag f){glClear(f);}
    static void ClearColor(CVec4 c){glClearColor(c.x(),c.y(),c.z(),c.w());}

    static void ClearDepth(scalar f){glClearDepthf(f);}
    static void ClearStencil(scalar f){glClearStencil(f);}

    static void ClearBufferiv(CGenum f,int32 i,const int32* d){glClearBufferiv(f,i,d);}
    static void ClearBufferuiv(CGenum f,int32 i,const uint32* d){glClearBufferuiv(f,i,d);}
    static void ClearBufferfv(CGenum f,int32 i,const scalar* d){glClearBufferfv(f,i,d);}
    static void ClearBufferfi(CGenum f,int32 i,scalar d1,scalar d2){glClearBufferfi(f,i,d1,d2);}

    static void FrontFace(Face v)
    {
        GLenum f;
        switch(v)
        {
        case Face::Back:
            f = GL_BACK;
            break;
        case Face::Front:
            f = GL_FRONT;
            break;
        default:return;
        }
        glFrontFace(f);
    }
    static void CullMode(Face v)
    {
        GLenum f;
        switch(v)
        {
        case Face::Back:
            f = GL_BACK;
            break;
        case Face::Front:
            f = GL_FRONT;
            break;
        case Face::Both:
            f = GL_FRONT_AND_BACK;
            break;
        }
        glCullFace(f);
    }

    static void BlendFuncSep(CGenum v1,CGenum v2,CGenum v3,CGenum v4)
    {glBlendFuncSeparate(v1,v2,v3,v4);}
    static void BlendEqSep(CGenum v1,CGenum v2){glBlendEquationSeparate(v1,v2);}

    static void DepthFunc(CGenum f){glDepthFunc(f);}
    static void DepthMask(bool v){glDepthMask((v) ? GL_TRUE : GL_FALSE);}

    static void StencilFuncSep(CGenum v1,CGenum v2,int32 d1,uint32 d2){glStencilFuncSeparate(v1,v2,d1,d2);}
    static void StencilOpSep(CGenum v1,CGenum v2,CGenum v3,CGenum v4){glStencilOpSeparate(v1,v2,v3,v4);}
    static void StencilMaskSep(CGenum f,uint32 d){glStencilMaskSeparate(f,d);}

    static void ViewportSet(CRect64 const* r){glViewport(r->x,r->y,r->w,r->h);}
    static void ScissorSet(CRect64 const* r){glScissor(r->x,r->y,r->w,r->h);}
    static void DepthSet(CZField64 const* d){glDepthRange(d->near,d->far);}

    static void Flush(){glFlush();}
    static void Finish(){glFinish();}

    static void PointParameteriv(CGenum f,const int32* d){glPointParameteriv(f,d);}
    static void PointParameterfv(CGenum f,const scalar* d){glPointParameterfv(f,d);}
    static void PointSize(scalar f){glPointSize(f);}

    static void LineWidth(scalar f){glLineWidth(f);}

    static void PolyMode(CGenum f1,CGenum f2){glPolygonMode(f1,f2);}
    static void PolyOffset(scalar f1,scalar f2){glPolygonOffset(f1,f2);}

    /* SAMPLE_ALPHA_TO_COVERAGE,SAMPLE_ALPHA_TO_ONE */
    static void SampleCoverage(scalar f,bool d){glSampleCoverage(f,(d) ? GL_TRUE : GL_FALSE);}
    static void SampleMaski(uint32 d,CGflag f){glSampleMaski(d,f);}

    /* Allocations */
    static bool TexAlloc(size_t l,CGhnd* d){glGenTextures(l,d); return true;}
    static bool TexFree(size_t l,CGhnd* d){glDeleteTextures(l,d); return true;}

    static bool FBAlloc(size_t l,CGhnd* d){glGenFramebuffers(l,d); return true;}
    static bool FBFree(size_t l,CGhnd* d){glDeleteFramebuffers(l,d); return true;}

    static bool RenderBufferAlloc(size_t l,CGhnd* d){glGenRenderbuffers(l,d); return true;}
    static bool RenderBufferFree(size_t l,CGhnd* d){glDeleteRenderbuffers(l,d); return true;}

    static bool SamplerAlloc(size_t l,CGhnd* d){glGenSamplers(l,d); return true;}
    static bool SamplerFree(size_t l,CGhnd* d){glDeleteSamplers(l,d); return true;}

    static bool BufAlloc(size_t l,CGhnd* d){glGenBuffers(l,d); return true;}
    static bool BufFree(size_t l,CGhnd* d){glDeleteBuffers(l,d); return true;}

    static bool VAOAlloc(size_t l,CGhnd* d){glGenVertexArrays(l,d); return true;}
    static bool VAOFree(size_t l,CGhnd* d){glDeleteVertexArrays(l,d); return true;}

    static bool QueryAlloc(size_t l,CGhnd* d){glGenQueries(l,d); return true;}
    static bool QueryFree(size_t l,CGhnd* d){glDeleteQueries(l,d); return true;}

    static bool ProgramAlloc(size_t l,CGhnd* d)
    {
        for(size_t i=0;i<l;i++)
            d[i] = glCreateProgram();
        return true;
    }
    static bool ProgramFree(size_t l,CGhnd* d)
    {
        for(size_t i=0;i<l;i++)
            glDeleteProgram(d[i]);
        return true;
    }

    static bool ShaderAlloc(size_t l,CGenum t,CGhnd* d)
    {
        for(size_t i=0;i<l;i++)
            d[i] = glCreateShader(t);
        return true;
    }
    static bool ShaderFree(size_t l,CGhnd* d)
    {
        for(size_t i=0;i<l;i++)
            glDeleteShader(d[i]);
        return true;
    }

    static bool FenceFree(size_t l,CGsync* d)
    {
        for(size_t i=0;i<l;i++)
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
    static void ShaderSource(CGhnd h,size_t n,cstring* src)
    {
        int32* lens = new int32[n];
        for(size_t i=0;i<n;i++)
            lens[i] = c_strlen(src[i]+1);
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

    static void ProgramGetAttached(CGhnd p,size_t* n,CGhnd** h)
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
    static void ProgramGetiv(CGhnd h,CGenum f,int32* d){glGetProgramiv(h,f,d);}

    static int32 ProgramGetFragDataLoc(CGhnd h,cstring n){return glGetFragDataLocation(h,n);}
    static int32 ProgramGetFragDataIdx(CGhnd h,cstring n){return glGetFragDataIndex(h,n);}
    static void ProgramBindFragData(CGhnd h,uint32 l,cstring n){glBindFragDataLocation(h,l,n);}
    static void ProgramBindFragDataIndexed(CGhnd h,uint32 i,uint32 l,cstring n)
    {
        glBindFragDataLocationIndexed(h,l,i,n);
    }

    /* Attributes */
    static void ProgramAttribGet(CGhnd h,size_t* n,cstring_w** names,CGenum** type,int32** size)
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
    static void ProgramUnifGet(CGhnd h,size_t* n,cstring_w** names,CGenum** type,int32** size)
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
    static void ProgramUnifBlockGet(CGhnd h,size_t* n,cstring_w** names,int32** indexSize,int32*** index,int32** size)
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
        for(uint32 i=0;i<num;i++)
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

    /* Subroutines */
    static void ProgramSubRtGet(CGhnd h,CGenum s,size_t* n,
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
        for(uint32 i=0;i<num;i++)
        {
            int32 namelen = 0;
            glGetActiveSubroutineUniformiv(h,s,i,GL_UNIFORM_NAME_LENGTH,&namelen);
            glGetActiveSubroutineUniformName(h,s,i,namelen,nullptr,names[0][i]);
            glGetActiveSubroutineUniformiv(h,s,i,GL_NUM_COMPATIBLE_SUBROUTINES,&rtSize[0][i]);
            rt[0][i] = new int32[rtSize[0][i]];
            glGetActiveSubroutineUniformiv(h,s,i,GL_COMPATIBLE_SUBROUTINES,rt[0][i]);
        }
    }
    static uint32 ProgramSubRtGetLoc(CGhnd h,CGenum s,cstring n){return glGetSubroutineIndex(h,s,n);}
    /* Binds all subroutine uniforms */
    static void ProgramSubRtBind(CGenum s,size_t n,const uint32* d){glUniformSubroutinesuiv(s,n,d);}

    /* Textures */
    static void TexBind(CGenum t,CGhnd h){glBindTexture(t,h);}
    static void TexActive(uint32 i){glActiveTexture(GL_TEXTURE0+i);}

    static void TexImage2D(CGenum t,int32 level,CGenum ifmt,
                           int64 w,int64 h,int32 border,CGenum fmt,CGenum dt,c_cptr p)
    {glTexImage2D(t,level,ifmt,w,h,border,fmt,dt,p);}
    static void TexImage3D(CGenum t,int32 level,CGenum ifmt,
                           int64 w,int64 h,uint64 d,int32 border,CGenum fmt,CGenum dt,c_cptr p)
    {glTexImage3D(t,level,ifmt,w,h,d,border,fmt,dt,p);}

    static void TexSubImage2D(CGenum t,int32 level,int32 x,int32 y,
                              int64 w,int64 h,CGenum fmt,CGenum dt,c_cptr p)
    {glTexSubImage2D(t,level,x,y,w,h,fmt,dt,p);}
    static void TexSubImage3D(CGenum t,int32 level,int32 x,int32 y,int32 z,
                              int64 w,int64 h,uint64 d,CGenum fmt,CGenum dt,c_cptr p)
    {glTexSubImage3D(t,level,x,y,z,w,h,d,fmt,dt,p);}

    static void TexImageCompressed2D(CGenum t,int32 level,CGenum ifmt,
                                     int64 w,int64 h,int32 border,int64 sz,c_cptr p)
    {glCompressedTexImage2D(t,level,ifmt,w,h,border,sz,p);}
    static void TexImageCompressed3D(CGenum t,int32 level,CGenum ifmt,
                                     int64 w,int64 h,int64 d,int32 border,int64 sz,c_cptr p)
    {glCompressedTexImage3D(t,level,ifmt,w,h,d,border,sz,p);}

    static void TexSubImageCompressed2D(CGenum t,int32 level,int32 x,int32 y,
                                        int64 w,int64 h,CGenum fmt,int64 sz,c_cptr p)
    {glCompressedTexSubImage2D(t,level,x,y,w,h,fmt,sz,p);}
    static void TexSubImageCompressed3D(CGenum t,int32 level,int32 x,int32 y,int32 z,
                                        int64 w,int64 h,int64 d,CGenum fmt,int64 sz,c_cptr p)
    {glCompressedTexSubImage3D(t,level,x,y,z,w,h,d,fmt,sz,p);}

    static void TexCopyImage2D(CGenum t,int32 level,CGenum fmt,int32 x,int32 y,
                               int64 w,int64 h,int32 border)
    {glCopyTexImage2D(t,level,fmt,x,y,w,h,border);}

    static void TexCopySubImage2D(CGenum t,int32 level,int32 xo,int32 yo,
                                  int32 x,int32 y,
                                  int64 w,int64 h)
    {glCopyTexSubImage2D(t,level,xo,yo,x,y,w,h);}
    static void TexCopySubImage3D(CGenum t,int32 level,int32 xo,int32 yo,int32 zo,
                                  int32 x,int32 y,
                                  int64 w,int64 h)
    {glCopyTexSubImage3D(t,level,xo,yo,zo,x,y,w,h);}

    static void TexGetImage(CGenum t,int32 level,CGenum fmt,CGenum dt,c_ptr p)
    {glGetTexImage(t,level,fmt,dt,p);}
    static void TexGetImageCompressed(CGenum t,int32 level,c_ptr p)
    {glGetCompressedTexImage(t,level,p);}

    static void TexGenMipmap(CGenum t){glGenerateMipmap(t);}

    /* Samplers */
    static void SamplerBind(uint32 i,CGhnd h){glBindSampler(i,h);}

    static void SamplerParameteriv(CGhnd h,CGenum f,const int32* d){glSamplerParameteriv(h,f,d);}
    static void SamplerParameteruiv(CGhnd h,CGenum f,const uint32* d)
    {glSamplerParameterIuiv(h,f,d);}
    static void SamplerParameterfv(CGhnd h,CGenum f,const scalar* d){glSamplerParameterfv(h,f,d);}

    static void SamplerGetParameteriv(CGhnd h,CGenum f,int32* d){glGetSamplerParameteriv(h,f,d);}
    static void SamplerGetParameteruiv(CGhnd h,CGenum f,uint32* d)
    {glGetSamplerParameterIuiv(h,f,d);}
    static void SamplerGetParameterfv(CGhnd h,CGenum f,scalar* d){glGetSamplerParameterfv(h,f,d);}

    /* Buffers */
    static void BufBind(CGenum t,CGhnd h){glBindBuffer(t,h);}

    static void BufData(CGenum t,uint64 sz,c_cptr p,CGenum a){glBufferData(t,sz,p,a);}
    static void BufSubData(CGenum t,int64 off,uint64 sz,c_cptr p){glBufferSubData(t,off,sz,p);}
    static void BufGetSubData(CGenum t,int64 off,uint64 sz,c_ptr p)
    {glGetBufferSubData(t,off,sz,p);}
    static void BufCopySubData(CGenum t1,CGenum t2,int64 off1,int64 off2,uint64 sz)
    {glCopyBufferSubData(t1,t2,off1,off2,sz);}

    static void BufBindRange(CGenum t,uint32 i,CGhnd b,int64 off,uint64 sz)
    {glBindBufferRange(t,i,b,off,sz);}
    static void BufMapRange(CGenum t,int64 off,uint64 sz,CGflag a)
    {glMapBufferRange(t,off,sz,a);}
    static void BufUnmap(CGenum t){glUnmapBuffer(t);}

    /* Queries */
    static void ConditionalRenderBegin(CGhnd h,CGenum m){glBeginConditionalRender(h,m);}
    static void ConditionalRenderEnd(){glEndConditionalRender();}

    static void QueryBegin(CGenum t,CGhnd h){glBeginQuery(t,h);}
    static void QueryEnd(CGenum t){glEndQuery(t);}

    static void QueryGetObjectiv(CGhnd h,CGenum f,int32* d){glGetQueryObjectiv(h,f,d);}
    static void QueryGetObjectuiv(CGhnd h,CGenum f,uint32* d){glGetQueryObjectuiv(h,f,d);}
    static void QueryGetObjecti64v(CGhnd h,CGenum f,int64* d){glGetQueryObjecti64v(h,f,d);}
    static void QueryGetObjectui64v(CGhnd h,CGenum f,uint64* d){glGetQueryObjectui64v(h,f,d);}

    static void QueryCounter(CGhnd h,CGenum t){glQueryCounter(h,t);}

    /* FB */
    static void FBBind(CGenum t,CGhnd h){glBindFramebuffer(t,h);}
    static void RBufBind(CGenum t,CGhnd h){glBindRenderbuffer(t,h);}

    static void RBufStorage(CGenum t,CGenum ifmt,int64 w,int64 h)
    {glRenderbufferStorage(t,ifmt,w,h);}

    static bool FBCheckComplete(CGenum t)
    {
        CGenum f = glCheckFramebufferStatus(t);
        if(f!=GL_FRAMEBUFFER_COMPLETE)
            cLog(__FILE__,__LINE__,"GL33","Incomplete framebuffer",f);

        return f==GL_FRAMEBUFFER_COMPLETE;
    }

    static void FBGetAttachParameter(CGenum t,CGenum att,CGenum p,int32* d)
    {glGetFramebufferAttachmentParameteriv(t,att,p,d);}

    static void FBAttachTexture(CGenum t,CGenum att,CGhnd h,int32 level)
    {glFramebufferTexture(t,att,h,level);}

    static void FBAttachTexture2D(CGenum t,CGenum att,CGenum textrg,CGhnd h,int32 level)
    {glFramebufferTexture2D(t,att,textrg,h,level);}
    static void FBAttachTexture3D(CGenum t,CGenum att,CGenum textrg,CGhnd h,int32 level,int32 z)
    {glFramebufferTexture3D(t,att,textrg,h,level,z);}
    static void FBAttachTextureLayer(CGenum t,CGenum att,CGhnd h,int32 level,int32 layer)
    {glFramebufferTextureLayer(t,att,h,level,layer);}
    static void FBAttachRenderBuffer(CGenum t,CGenum att,CGenum rtrg,CGhnd h)
    {glFramebufferRenderbuffer(t,att,rtrg,h);}

    static void FBBlit(const CRect& s, const CRect& d,
                       CGflag m,CGenum f)
    {glBlitFramebuffer(s.x,s.y,s.w,s.h, d.x,d.y,d.w,d.h, m,f);}

    static void RBufGetParameteri(CGenum t,CGenum p,int32* d){glGetRenderbufferParameteriv(t,p,d);}

    static void FBDrawBuffers(size_t n,const CGenum* d){glDrawBuffers(n,d);}
    static void FBReadBuffer(CGenum b){glReadBuffer(b);}

    static void FBReadPixels(int32 x,int32 y,int32 w,int32 h,CGenum fmt,CGenum dt,c_ptr p)
    {
        glReadPixels(x,y,w,h,fmt,dt,p);
    }

    /* XFB */
    static void XFBegin(CGenum p){glBeginTransformFeedback(p);}
    static void XFEnd(){glEndTransformFeedback();}

    static void XFVaryings(CGhnd h,size_t n,cstring* names,CGenum a)
    {glTransformFeedbackVaryings(h,n,names,a);}
    static void XFGetVaryings(CGhnd h,size_t* n,cstring_w** names,CGenum** type, int32** size)
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
    }

    /* Include RASTERIZER_DISCARD */

    /* VAO */
    static void VAOBind(CGhnd h){glBindVertexArray(h);}

    static void VAOEnableAttrib(uint32 i){glEnableVertexAttribArray(i);}
    static void VAOAttribPointer(uint32 i,int32 s,CGenum t,bool n,int64 stride,int64 offset)
    {glVertexAttribPointer(i,s,t,(n) ? GL_TRUE : GL_FALSE,stride,(void*)offset);}
    static void VAOAttribIPointer(uint32 i,int32 s,CGenum t,int64 stride,int64 offset)
    {glVertexAttribIPointer(i,s,t,stride,(void*)offset);}
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
    static bool FenceServerAwait(CGsync s)
    {
        glWaitSync((GLsync)s,0,0);
        return false;
    }

    static void FenceGetiv(CGsync s,CGenum p,int32* d){glGetSynciv((GLsync)s,p,1,nullptr,d);}

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
    static void DrawArrays(CGenum p,int32 f,int64 c){glDrawArrays(p,f,c);}
    static void DrawArraysInstanced(CGenum p,int32 f,int32 c,int32 ic)
    {glDrawArraysInstanced(p,f,c,ic);}

    static void DrawElements(CGenum p,int32 c,CGenum t,int64 off)
    {glDrawElements(p,c,t,(void*)off);}
    static void DrawElementsInstanced(CGenum p,int32 c,CGenum t,int64 off,uint64 pc)
    {glDrawElementsInstanced(p,c,t,(void*)off,pc);}
    static void DrawElementsBaseVertex(CGenum p,int32 c,CGenum t,int64 off,int32 bv)
    {glDrawElementsBaseVertex(p,c,t,(void*)off,bv);}
    static void DrawElementsInstancedBaseVertex(CGenum p,int32 c,CGenum t,
                                                int64 off,int32 pc,int32 bv)
    {glDrawElementsInstancedBaseVertex(p,c,t,(void*)off,pc,bv);}

    static void DrawRangeElements(CGenum p,uint32 f,uint32 e,int32 c,CGenum t,int64 off)
    {glDrawRangeElements(p,f,e,c,t,(void*)off);}
    static void DrawRangeElementsBaseVertex(CGenum p,uint32 f,uint32 e,
                                            int32 c,CGenum t,int64 off,int32 bv)
    {glDrawRangeElementsBaseVertex(p,f,e,c,t,(void*)off,bv);}

    static void DrawMultiArrays(CGenum p,const int32* f,const int32* c, int32 dc)
    {glMultiDrawArrays(p,f,c,dc);}

    static void DrawMultiElements(CGenum p,const int32* f,CGenum t,const int64* off,int32 dc)
    {glMultiDrawElements(p,f,t,(const void**)&off,dc);}
    static void DrawMultiElementsBaseVertex(CGenum p,const int32* c,CGenum t,const int64* off,
                                            int32 dc,const int32* bv)
    {glMultiDrawElementsBaseVertex(p,c,t,(const void**)&off,dc,bv);}
};

}
}

#endif
