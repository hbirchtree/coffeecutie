#pragma once

#include "../gl_shared_enum_convert.h"
#include "../gl_shared_include.h"
#include "../gl_shared_types.h"
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee {
namespace CGL {

template<typename CGhnd, typename ShaderStage, typename CGsync>
struct CGL_Old_Constructors
{
    /* Allocations */
    STATICINLINE bool TexAlloc(Span<CGhnd> const& handles)
    {
        glGenTextures(handles.elements, handles.data);
        return true;
    }
    STATICINLINE bool TexFree(Span<CGhnd> const& handles)
    {
        glDeleteTextures(handles.elements, handles.data);
        return true;
    }

    STATICINLINE bool FBAlloc(Span<CGhnd> const& handles)
    {
        glGenFramebuffers(handles.elements, handles.data);
        return true;
    }
    STATICINLINE bool FBFree(Span<CGhnd> const& handles)
    {
        glDeleteFramebuffers(handles.elements, handles.data);
        return true;
    }

    STATICINLINE bool RenderBufferAlloc(Span<CGhnd> const& handles)
    {
        glGenRenderbuffers(handles.elements, handles.data);
        return true;
    }
    STATICINLINE bool RenderBufferFree(Span<CGhnd> const& handles)
    {
        glDeleteRenderbuffers(handles.elements, handles.data);
        return true;
    }

    STATICINLINE bool BufAlloc(Span<CGhnd> const& handles)
    {
        glGenBuffers(handles.elements, handles.data);
        return true;
    }
    STATICINLINE bool BufFree(Span<CGhnd> const& handles)
    {
        glDeleteBuffers(handles.elements, handles.data);
        return true;
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE bool SamplerAlloc(Span<CGhnd> const& handles)
    {
        glGenSamplers(handles.elements, handles.data);
        return true;
    }
    STATICINLINE bool SamplerFree(Span<CGhnd> const& handles)
    {
        glDeleteSamplers(handles.elements, handles.data);
        return true;
    }

    STATICINLINE bool VAOAlloc(Span<CGhnd> const& handles)
    {
        glGenVertexArrays(handles.elements, handles.data);
        return true;
    }
    STATICINLINE bool VAOFree(Span<CGhnd> const& handles)
    {
        glDeleteVertexArrays(handles.elements, handles.data);
        return true;
    }

    STATICINLINE bool QueryAlloc(Span<CGhnd> const& handles)
    {
        glGenQueries(handles.elements, handles.data);
        return true;
    }
    STATICINLINE bool QueryFree(Span<CGhnd> const& handles)
    {
        glDeleteQueries(handles.elements, handles.data);
        return true;
    }
#endif

    STATICINLINE bool ProgramAlloc(Span<CGhnd> const& handles)
    {
        for(uint32 i = 0; i < handles.elements; i++)
            handles.data[i] = glCreateProgram();
        return true;
    }
    STATICINLINE bool ProgramFree(Span<CGhnd> const& handles)
    {
        for(uint32 i = 0; i < handles.elements; i++)
            glDeleteProgram(handles.data[i]);
        return true;
    }

    STATICINLINE bool ShaderAlloc(ShaderStage t, Span<CGhnd> const& handles)
    {
        for(uint32 i = 0; i < handles.elements; i++)
            handles.data[i] = glCreateShader(to_enum1(t));
        return true;
    }
    STATICINLINE bool ShaderFree(Span<CGhnd> const& handles)
    {
        for(uint32 i = 0; i < handles.elements; i++)
            glDeleteShader(handles.data[i]);
        return true;
    }

#if GL_VERSION_VERIFY(0x300, 0x300)
    STATICINLINE bool FenceFree(Span<GLsync> const& handles)
    {
        for(uint32 i = 0; i < handles.elements; i++)
            glDeleteSync(handles.data[i]);
        return true;
    }
#endif
};
} // namespace CGL
} // namespace Coffee
