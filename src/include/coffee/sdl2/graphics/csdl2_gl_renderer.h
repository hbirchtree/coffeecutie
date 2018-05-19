#pragma once

#include <coffee/core/base/renderer/glapplication.h>

#if defined(COFFEE_USE_SDL_GL)
#include "../csdl2_context.h"

namespace Coffee{
namespace Display{

class SDL2GLRenderer :
        public virtual GLApplication,
        public virtual SDL2ContextUser
{
public:
    virtual ~SDL2GLRenderer()
    {
    }

    void swapBuffers();
    CSize framebufferSize() const;
    int swapInterval() const;
    void setSwapInterval(const int &i);
    CDContextBits context();
    ThreadId contextThread();

    CGL::CGL_Context *glContext();
    CGL::CGL_WorkerContext *workerContext();
    CGL::CGL_ScopedContext scopedContext();

    bool contextPreInit(const GLProperties& props,CString*);
    bool contextInit(const GLProperties &, CString*err);
    bool contextPostInit(const GLProperties& props,CString*);
    void contextTerminate();
};

}
}

#endif
