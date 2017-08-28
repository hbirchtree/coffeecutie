#pragma once

#include <coffee/core/plat/plat_quirks_toggling.h>

#if defined(COFFEE_USE_MAEMO_EGL)

#include <coffee/core/base/renderer/glapplication.h>
#include <coffee/core/base/types/cobject.h>

namespace Coffee{
namespace Display{

struct EGL_Data;

struct EGL_GL_Context;
struct EGL_GL_WorkerContext;

class EGLRenderer : public GLApplication
{
    friend struct EGL_GL_Context;
    friend struct EGL_GL_WorkerContext;

    UqPtr<EGL_Data> m_eglData;
public:
    EGLRenderer();
    virtual ~EGLRenderer();

    bool contextPreInit(const GLProperties &props, CString *);
    bool contextInit(const GLProperties &props, CString *err);
    bool contextPostInit(const GLProperties &props, CString *err);
    void contextTerminate();
    void swapBuffers();
    CSize framebufferSize() const;
    int swapInterval() const;
    void setSwapInterval(const int &i);
    CDContextBits context();
    ThreadId contextThread();

    virtual CGL::CGL_Context *glContext();
    virtual CGL::CGL_ScopedContext scopedContext();

    virtual CGL::CGL_WorkerContext* workerContext();
};

}
}
#endif