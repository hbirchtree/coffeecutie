#pragma once

#include "../gl_shared_include.h"
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace CGL{

template<typename CGhnd,typename CGenum>
struct CGL_Old_ShaderCompiler
{
    STATICINLINE bool ProgramValidate(CGhnd h)
    {
        /*WARNING: Do not use this with shader programs! */
        glValidateProgram(h);
        int32 outStat;
        ProgramGetiv(h,GL_VALIDATE_STATUS,&outStat);
        return outStat == GL_TRUE;
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
            lens[i] = StrLen(src[i]+1);
        glShaderSource(h,n,src,lens);
        delete[] lens;
    }
    STATICINLINE bool ProgramLink(CGhnd p)
    {
        glLinkProgram(p);
        int32 status;
        ProgramGetiv(p,GL_LINK_STATUS,&status);
        return status==GL_TRUE;
    }

    STATICINLINE void ShaderAttach(CGhnd p,CGhnd s){glAttachShader(p,s);}
    STATICINLINE void ShaderDetach(CGhnd p,CGhnd s){glDetachShader(p,s);}
    STATICINLINE bool ShaderCompile(CGhnd s)
    {
        glCompileShader(s);
        int32 status;
        glGetShaderiv(s,GL_COMPILE_STATUS,&status);
        return status==GL_TRUE;
    }

    STATICINLINE void ShaderGetiv(CGhnd h,CGenum f,int32* d){glGetShaderiv(h,f,d);}
    STATICINLINE void ProgramGetAttached(CGhnd p,uint32* n,CGhnd** h)
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

    STATICINLINE cstring_w ProgramGetLog(CGhnd h)
    {
        int32 len = 0;
        ProgramGetiv(h,GL_INFO_LOG_LENGTH,&len);
        if(len<=0)
            return nullptr;
        cstring_w s = new int8[len+1];
        glGetProgramInfoLog(h,len,nullptr,s);
        return s;
    }
    STATICINLINE void ProgramUse(CGhnd h)
    {
        glUseProgram(h);
    }

    /*TODO: Create ProgramProperty enum*/
    STATICINLINE void ProgramGetiv(CGhnd h,CGenum f,int32* d){glGetProgramiv(h,f,d);}

    STATICINLINE int32 ProgramGetFragDataLoc(CGhnd h,cstring n){return glGetFragDataLocation(h,n);}

    /* Attributes */
    STATICINLINE void ProgramAttribGet(CGhnd h,uint32* n,cstring_w** names,CGenum** type,int32** size)
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
    STATICINLINE int32 ProgramAttribLoc(CGhnd h,cstring n){return glGetAttribLocation(h,n);}
    STATICINLINE void ProgramAttribBind(CGhnd h,uint32 i,cstring n){glBindAttribLocation(h,i,n);}

    /* Uniforms */
    STATICINLINE
    void ProgramUnifGet(CGhnd h,uint32* n,cstring_w** names,CGenum** type,int32** size)
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
    STATICINLINE int32 ProgramUnifGetLoc(CGhnd h,cstring n){return glGetUniformLocation(h,n);}

    /* Uniform blocks */
    STATICINLINE
    void ProgramUnifBlockGet(CGhnd h,uint32* n,cstring_w** names,
                                          int32** indexSize,int32*** index,int32** size)
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
    STATICINLINE uint32 ProgramUnifBlockGetLoc(CGhnd h,cstring n)
    {return glGetUniformBlockIndex(h,n);}
    STATICINLINE void ProgramUnifBlockBind(CGhnd h,uint32 l,uint32 i)
    {glUniformBlockBinding(h,l,i);}
};


}
}
