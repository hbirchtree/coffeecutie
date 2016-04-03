#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_types.h"
#include "../gl_shared_enum_convert.h"

namespace Coffee{
namespace CGL{

template<typename CGhnd,typename ShaderStage,typename CGsync>
struct CGL_Old_Constructors
{
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
};

}
}
