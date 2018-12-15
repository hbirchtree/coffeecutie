#pragma once

#include <coffee/core/base.h>

#if defined(COFFEE_USE_MAEMO_EGL)

#include <coffee/core/base/renderer/glapplication.h>
#include <coffee/core/types/cobject.h>

namespace Coffee{
namespace Display{

class GLKWindow;

struct EGL_Data;

struct EGL_GL_Context;
struct EGL_GL_WorkerContext;

class EGLRenderer :
        public virtual GLApplication
{
    friend struct EGL_GL_Context;
    friend struct EGL_GL_WorkerContext;
    
#if defined(COFFEE_USE_APPLE_GLKIT)
    friend class GLKWindow;
#endif

    UqPtr<EGL_Data> m_eglData;
    
    EGL_GL_Context* mContext;
    
public:
    EGLRenderer();
    virtual ~EGLRenderer();

    bool contextPreInit(const GL::Properties &props, CString *);
    bool contextInit(const GL::Properties &props, CString *err);
    bool contextPostInit(const GL::Properties &props, CString *err);
    void contextTerminate();
    void swapBuffers();
    Size framebufferSize() const;
    int swapInterval() const;
    void setSwapInterval(const int &i);
    ContextBits context();
    ThreadId contextThread();

    virtual CGL::CGL_Context *glContext();
    virtual CGL::CGL_ScopedContext scopedContext();

    virtual CGL::CGL_WorkerContext* workerContext();
};

}
}
#endif
