#pragma once

#include <coffee/core/base/renderer/glapplication.h>

#if defined(COFFEE_USE_SDL_GL)
#include "../csdl2_context.h"

namespace Coffee {
namespace Display {

class SDL2GLRenderer : public virtual GLApplication,
                       public virtual SDL2ContextUser
{
  public:
    virtual ~SDL2GLRenderer()
    {
    }

    void        swapBuffers();
    Size        framebufferSize() const;
    int         swapInterval() const;
    void        setSwapInterval(const int& i);
    ContextBits context();
    ThreadId    contextThread();

    CGL::CGL_Context*       glContext();
    CGL::CGL_WorkerContext* workerContext();
    CGL::CGL_ScopedContext  scopedContext();

    bool contextPreInit(const GL::Properties& props, CString*);
    bool contextInit(const GL::Properties&, CString* err);
    bool contextPostInit(const GL::Properties& props, CString*);
    void contextTerminate();
};

} // namespace Display
} // namespace Coffee

#endif
