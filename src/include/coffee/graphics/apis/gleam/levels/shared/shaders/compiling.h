#pragma once

#include "../gl_shared_types.h"
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee {
namespace CGL {

template <typename CGhnd, typename CGenum>
struct CGL_Old_ShaderCompiler {
    STATICINLINE bool ProgramValidate(CGhnd h)
    {
        /*WARNING: Do not use this with shader programs! */
        glValidateProgram(h);
        int32 outStat;
        ProgramGetiv(h, GL_VALIDATE_STATUS, &outStat);
        return outStat == GL_TRUE;
    }

    /* Shaders */

    STATICINLINE void ShaderGetiv(CGhnd h, CGenum f, int32* d)
    {
        glGetShaderiv(h, f, d);
    }
    STATICINLINE cstring_w ShaderGetLog(CGhnd h)
    {
        int32 len = 0;
        ShaderGetiv(h, GL_INFO_LOG_LENGTH, &len);
        if (len <= 0)
            return nullptr;
        cstring_w s = new char[len + 1];
        glGetShaderInfoLog(h, len, nullptr, s);
        return s;
    }
    STATICINLINE void ShaderSource(CGhnd h, uint32 n, cstring* src)
    {
        int32* lens = new int32[n];
        /* ShaderSource wants to know all the string lengths */
        for (uint32 i = 0; i < n; i++)
            lens[i] = StrLen(src[i] + 1);
        glShaderSource(h, n, src, lens);
        delete[] lens;
    }
    STATICINLINE void ShaderSource(CGhnd h, uint32 n, int32* len, cstring* src)
    {
        glShaderSource(h, n, src, len);
    }
    STATICINLINE bool ShaderCompile(CGhnd s)
    {
        glCompileShader(s);
        int32 status = GL_TRUE;
        glGetShaderiv(s, GL_COMPILE_STATUS, &status);
        return status == GL_TRUE;
    }

    STATICINLINE cstring_w ShaderGetSrc(CGhnd h)
    {
        int32 len = 0;
        ShaderGetiv(h, GL_SHADER_SOURCE_LENGTH, &len);
        if (len <= 0)
            return nullptr;
        cstring_w s = new char[len + 1];
        glGetShaderSource(h, len, nullptr, s);
        s[len] = 0;
        return s;
    }

    STATICINLINE void ShaderAttach(CGhnd p, CGhnd s) { glAttachShader(p, s); }
    STATICINLINE void ShaderDetach(CGhnd p, CGhnd s) { glDetachShader(p, s); }

    STATICINLINE bool ProgramLink(CGhnd p)
    {
        glLinkProgram(p);
        int32 status;
        ProgramGetiv(p, GL_LINK_STATUS, &status);
        return status == GL_TRUE;
    }

    STATICINLINE void ProgramGetAttached(CGhnd p, uint32* n, CGhnd** h)
    {
        int32 num = 0;
        ProgramGetiv(p, GL_ATTACHED_SHADERS, &num);
        if (num <= 0) {
            *n = 0;
            return;
        }
        CGhnd* attached = new CGhnd[num];
        glGetAttachedShaders(p, num, nullptr, attached);

        *n = num;
        *h = attached;
    }

    STATICINLINE cstring_w ProgramGetLog(CGhnd h)
    {
        int32 len = 0;
        ProgramGetiv(h, GL_INFO_LOG_LENGTH, &len);
        if (len <= 0)
            return nullptr;
        cstring_w s = new char[len + 1];
        glGetProgramInfoLog(h, len, nullptr, s);
        return s;
    }
    STATICINLINE void ProgramUse(CGhnd h) { glUseProgram(h); }

    /*TODO: Create ProgramProperty enum*/
    STATICINLINE void ProgramGetiv(CGhnd h, CGenum f, int32* d)
    {
        glGetProgramiv(h, f, d);
    }

    STATICINLINE int32 ProgramGetFragDataLoc(CGhnd h, cstring n)
    {
        return glGetFragDataLocation(h, n);
    }

    /* Attributes */
    STATICINLINE void ProgramAttribGet(CGhnd h, uint32* n, cstring_w** names,
                                       CGenum** type, int32** size)
    {
        int32 num = 0;
        ProgramGetiv(h, GL_ACTIVE_ATTRIBUTES, &num);
        if (num <= 0) {
            *n = 0;
            return;
        }
        *n = num;

        int32 namelen = 0;
        ProgramGetiv(h, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &namelen);

        names[0] = new cstring_w[num];
        type[0] = new CGenum[num];
        size[0] = new int32[num];
        for (int32 i = 0; i < num; i++)
            names[0][i] = new char[namelen];
        for (int32 i = 0; i < num; i++)
            glGetActiveAttrib(h, i, namelen, nullptr, &size[0][i], &type[0][i],
                              names[0][i]);
    }
    STATICINLINE int32 ProgramAttribLoc(CGhnd h, cstring n)
    {
        return glGetAttribLocation(h, n);
    }
    STATICINLINE void ProgramAttribBind(CGhnd h, uint32 i, cstring n)
    {
        glBindAttribLocation(h, i, n);
    }

    /* Uniforms */
    struct UnifValInfo {
        cstring_w name;
        CGenum type;
        int32 size;
    };

    STATICINLINE
    void ProgramUnifGet(CGhnd h, uint32* n, UnifValInfo** values)
    {
        int32 num = 0;
        ProgramGetiv(h, GL_ACTIVE_UNIFORMS, &num);
        if (num <= 0) {
            *n = 0;
            return;
        }
        *n = num;

        int32 namelen = 0;
        ProgramGetiv(h, GL_ACTIVE_UNIFORM_MAX_LENGTH, &namelen);

        *values = new UnifValInfo[num];

        for (int32 i = 0; i < num; i++)
            (*values)[i].name = new char[namelen];
        for (int32 i = 0; i < num; i++)
            glGetActiveUniform(h, i, namelen, nullptr, &(*values)[i].size,
                               &(*values)[i].type, (*values)[i].name);
    }
    STATICINLINE int32 ProgramUnifGetLoc(CGhnd h, cstring n)
    {
        return glGetUniformLocation(h, n);
    }

    /* Uniform blocks */
    struct UnifBlkInfo {
        cstring_w name;
        int32* index;
        int32 indexSize;
        int32 size;
    };

#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE
    void ProgramUnifBlockGet(CGhnd h, uint32* n, UnifBlkInfo** blocks)
    {
        int32 num = 0;
        ProgramGetiv(h, GL_ACTIVE_UNIFORM_BLOCKS, &num);
        if (num <= 0) {
            *n = 0;
            return;
        }
        *n = num;

        *blocks = new UnifBlkInfo[num];

        int32 namelen = 0;
        for (int32 i = 0; i < num; i++) {
            glGetActiveUniformBlockiv(h, i, GL_UNIFORM_BLOCK_NAME_LENGTH,
                                      &namelen);
            (*blocks)[i].name = new int8[namelen];
        }
        for (uint32 i = 0; i < (uint32)num; i++) {
            glGetActiveUniformBlockName(h, i, namelen, nullptr,
                                        (*blocks)[i].name);
            glGetActiveUniformBlockiv(h, i, GL_UNIFORM_BLOCK_DATA_SIZE,
                                      &((*blocks)[i].size));
            glGetActiveUniformBlockiv(h, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
                                      &((*blocks)[i].indexSize));
            (*blocks)[i].index = new int32[(*blocks)[i].indexSize];
            glGetActiveUniformBlockiv(h, i,
                                      GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
                                      (*blocks)[i].index);
        }
    }

    STATICINLINE uint32 ProgramUnifBlockGetLoc(CGhnd h, cstring n)
    {
        return glGetUniformBlockIndex(h, n);
    }
    STATICINLINE void ProgramUnifBlockBind(CGhnd h, uint32 l, uint32 i)
    {
        glUniformBlockBinding(h, l, i);
    }
#endif
};
}
}
