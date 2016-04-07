#pragma once

#include <coffee/core/base/renderer/glapplication.h>
#include "../csdl2_context.h"

namespace Coffee{
namespace Display{

class SDL2GLRenderer : public GLApplication,
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
    CGL::CGL_ScopedContext scopedContext();

    bool contextPreInit(const GLProperties& props,CString*);
    bool contextInit(const GLProperties &, CString*err);
    bool contextPostInit(const GLProperties& props,CString*);
    void contextTerminate();
};

}
}
