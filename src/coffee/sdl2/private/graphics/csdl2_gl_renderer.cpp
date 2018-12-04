#include <coffee/sdl2/graphics/csdl2_gl_renderer.h>

#if defined(COFFEE_USE_SDL_GL)

#include "../windowing/sdl2helpers.h"
#include "csdl2_gl_context.h"
#include <coffee/core/CProfiling>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace Display {

void SDL2GLRenderer::swapBuffers()
{
    SDL_GL_SwapWindow(getSDL2Context()->window);
}

Size SDL2GLRenderer::framebufferSize() const
{
    size_2d<i32> sz;
    SDL_GL_GetDrawableSize(getSDL2Context()->window, &sz.w, &sz.h);
    return sz.convert<u32>();
}

int SDL2GLRenderer::swapInterval() const
{
    return SDL_GL_GetSwapInterval();
}

void SDL2GLRenderer::setSwapInterval(const int& i)
{
    SDL_GL_SetSwapInterval(i);
}

ContextBits SDL2GLRenderer::context()
{
    return SDL2::GetContextProperties().bits;
}

ThreadId SDL2GLRenderer::contextThread()
{
    return getSDL2Context()->thread;
}

CGL::CGL_Context* SDL2GLRenderer::glContext()
{
    if(!getSDL2Context())
        setSDL2Context(new Context);

    if(getSDL2Context() && !getSDL2Context()->context)
        getSDL2Context()->context =
            new CGL_SDL_GL_Context(getSDL2Context()->window);

    if(!getSDL2Context() || !getSDL2Context()->context)
        return nullptr;

    return getSDL2Context()->context;
}

CGL::CGL_WorkerContext* SDL2GLRenderer::workerContext()
{
    if(!getSDL2Context() || !getSDL2Context()->window)
        return nullptr;

    return new CGL_SDL_GL_Context(getSDL2Context()->window);
}

CGL::CGL_ScopedContext SDL2GLRenderer::scopedContext()
{
    return CGL::CGL_ScopedContext(getSDL2Context()->context);
}

bool SDL2GLRenderer::contextPreInit(const GL::Properties& props, CString*)
{
    DProfContext a(SDLGL_API "Set context properties");

    m_window_flags |= SDL_WINDOW_OPENGL;
    SDL2::SetContextProperties(props);
    return true;
}

bool SDL2GLRenderer::contextInit(const GL::Properties&, CString* err)
{
    /* Acquire the OpenGL context from SDL2 */
    DProfContext a(SDLGL_API "Acquire GL context");

    /* Make the GL context current to this thread */
    if(glContext() && glContext()->acquireContext())
    {
        Profiler::DeepProfile(SDLGL_API "Acquire context currency");
        return true;
    } else
    {
        CString m_err = cStringFormat(
            "Failed to create SDL2 OpenGL context: {0}", SDL_GetError());
        cDebug("{0}", m_err);
        if(err)
            *err = m_err;
        return false;
    }
}

bool SDL2GLRenderer::contextPostInit(const GL::Properties& props, CString*)
{
    DProfContext a(SDLGL_API "Set swapping interval");

    /* Enable VSync if requested */
    if(props.flags & GL::Properties::GLVSync)
        setSwapInterval(1);
    else
        setSwapInterval(0);
    return true;
}

void SDL2GLRenderer::contextTerminate()
{
    cVerbose(10, SDLGL_API "Context pointer: {0}", C_CAST<void*>(glContext()));
    if(!glContext())
        return;
    /* Acquire GL context first, ensures that destructor won't fail on different
     * thread */
    glContext()->acquireContext();

    /* Delete GL context object */
    delete glContext();

    getSDL2Context()->context = nullptr;
}

} // namespace Display
} // namespace Coffee

#endif
