#pragma once

#include "../types/cdisplay.h"
#include <coffee/graphics_apis/gltypes.h>

namespace Coffee{
namespace Display{

class GLApplication
{
public:
    virtual bool contextPreInit(const GLProperties& props,CString*) = 0;
    virtual bool contextInit(const GLProperties& props,CString*) = 0;
    virtual bool contextPostInit(const GLProperties& props,CString*) = 0;
    virtual void contextTerminate() = 0;

    /*!
     * \brief Function used for swapping buffers in render loop
     */
    virtual void swapBuffers() = 0;

    /*!
     * \brief Get current framebuffer size
     * \return
     */
    virtual CSize framebufferSize() const = 0;

    /*!
     * \brief Get current swap interval
     * \return
     */
    virtual int  swapInterval() const = 0;
    /*!
     * \brief Set swap interval
     * \param i
     */
    virtual void setSwapInterval(const int &i) = 0;

    /*!
     * \brief Get context bit information
     * \return Struct containing context bits
     */
    virtual CDContextBits context() = 0;

    /*!
     * \brief Get thread ID on which the GL context is current
     * \return
     */
    virtual ThreadId contextThread() = 0;

    virtual CGL::CGL_Context* glContext() = 0;
    virtual CGL::CGL_ScopedContext scopedContext() = 0;
};

}
}
