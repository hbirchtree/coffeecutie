#include <coffee/sdl2/graphics/csdl2_gl_renderer.h>

#include <coffee/core/CDebug>
#include "../windowing/sdl2helpers.h"
#include "csdl2_gl_context.h"

namespace Coffee{
namespace Display{

void SDL2GLRenderer::swapBuffers()
{
    SDL_GL_SwapWindow(getSDL2Context()->window);
}

CSize SDL2GLRenderer::framebufferSize() const
{
    CSize sz;
    SDL_GL_GetDrawableSize(getSDL2Context()->window,&sz.w,&sz.h);
    return sz;
}

int SDL2GLRenderer::swapInterval() const
{
    return SDL_GL_GetSwapInterval();
}

void SDL2GLRenderer::setSwapInterval(const int &i)
{
    SDL_GL_SetSwapInterval(i);
}

CDContextBits SDL2GLRenderer::context()
{
    return CSDL2Types::GetContextProperties().bits;
}

ThreadId SDL2GLRenderer::contextThread()
{
    return getSDL2Context()->thread;
}

CGL::CGL_Context *SDL2GLRenderer::glContext()
{
    return getSDL2Context()->context;
}

CGL::CGL_ScopedContext SDL2GLRenderer::scopedContext()
{
    return CGL::CGL_ScopedContext(getSDL2Context()->context);
}

bool SDL2GLRenderer::contextPreInit(const GLProperties& props,CString*)
{
    m_window_flags |= SDL_WINDOW_OPENGL;
    CSDL2Types::SetContextProperties(props);
    Profiler::Profile("Set context properties");
    return true;
}

bool SDL2GLRenderer::contextInit(const GLProperties&,CString* err)
{
    /* Acquire the OpenGL context from SDL2 */
    getSDL2Context()->context = new CGL_SDL_GL_Context(getSDL2Context()->window);
    Profiler::Profile("Acquire GL context");

    /* Make the GL context current to this thread */
    if(!glContext()->acquireContext())
    {
        CString m_err = cStringFormat("Failed to create SDL2 OpenGL context: {0}",SDL_GetError());
        if(err)
            *err = m_err;
        return false;
    }
    Profiler::Profile("Acquire context currency");
    return true;
}

bool SDL2GLRenderer::contextPostInit(const GLProperties& props, CString *)
{
    /* Enable VSync if requested */
    if(props.flags&GLProperties::GLVSync)
        setSwapInterval(1);
    else
        setSwapInterval(0);
    return true;
}

void SDL2GLRenderer::contextTerminate()
{
    if(!glContext())
        return;
    /* Acquire GL context first, ensures that destructor won't fail on different thread */
    glContext()->acquireContext();

    /* Delete GL context object */
    delete glContext();
}

}
}
