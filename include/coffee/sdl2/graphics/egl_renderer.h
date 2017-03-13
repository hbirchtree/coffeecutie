#pragma once

#include <coffee/core/plat/plat_quirks_toggling.h>

#if defined(COFFEE_USE_MAEMO_EGL)

#include <coffee/core/base/renderer/glapplication.h>

namespace Coffee{
namespace Display{

struct EGL_Data;

class EGLRenderer : public GLApplication
{
    std::unique_ptr<EGL_Data> m_eglData;
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
    CGL::CGL_Context *glContext();
    CGL::CGL_ScopedContext scopedContext();
};

}
}
#endif
