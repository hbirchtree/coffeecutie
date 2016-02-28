#pragma once

#include <coffee/graphics_apis/gltypes.h>
#include "../types/sdl2datatypes.h"

namespace Coffee{
namespace CDisplay{

struct CGL_SDL_GL_Context : CGL::CGL_Context
{
    CGL_SDL_GL_Context(SDL_Window* win):
        m_window(win)
    {
        m_context = SDL_GL_CreateContext(win);
        acquireContext();
    }
    ~CGL_SDL_GL_Context()
    {
        releaseContext();
        SDL_GL_DeleteContext(m_context);
    }

    C_DELETE_COPY_CONSTRUCTOR(CGL_SDL_GL_Context);

    bool acquireContext()
    {
        new (&m_threadId) ThreadId;
        return SDL_GL_MakeCurrent(m_window,m_context)==0;
    }
    bool releaseContext()
    {
        return SDL_GL_MakeCurrent(nullptr,nullptr)==0;
    }
    const ThreadId &currentThread()
    {
        return m_threadId;
    }
protected:
    ThreadId m_threadId;
private:
    SDL_Window* m_window;
    SDL_GLContext m_context;
};

}
}