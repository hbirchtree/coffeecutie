#pragma once

#include <coffee/core/base/renderer/glapplication.h>
#include <coffee/core/base/types/cobject.h>

#if defined(COFFEE_USE_LINUX_GLX)

namespace Coffee{
namespace Display{

struct GLX_Data;

struct GLX_Data_Deleter
{
    void operator()(GLX_Data* ptr);
};

class GLXRenderer :
        public virtual GLApplication
{
    UqPtr<GLX_Data, GLX_Data_Deleter> m_gxData;

    // GLApplication interface
public:
    bool contextPreInit(const GLProperties &props, CString *);
    bool contextInit(const GLProperties &props, CString *) {return true;}
    bool contextPostInit(const GLProperties &props, CString *);
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
