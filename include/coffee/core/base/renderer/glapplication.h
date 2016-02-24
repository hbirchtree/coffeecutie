#pragma once

#include "../types/cdisplay.h"
#include <coffee/graphics_apis/gltypes.h>

namespace Coffee{
namespace CDisplay{

class GLApplication
{
public:
    virtual void contextPreInit(const GLProperties& props) = 0;
    virtual void contextInit(const GLProperties& props) = 0;
    virtual void contextPostInit(const GLProperties& props) = 0;
    virtual void contextTerminate() = 0;

    /*!
     * \brief Callback for GL errors and messages, implemented by the user.
     * \param d A pointer to the system's OpenGL report format, implementation-specific.
     */
    virtual void bindingCallback(const void*) const = 0;
    /*!
     * \brief Function called to initialize
     *  binding library like glbinding, called before context is created
     */
    virtual void bindingPreInit(const GLProperties&) = 0;
    virtual void bindingInit(const GLProperties&) = 0;
    /*!
     * \brief Function called to initialize binding
     *  library like glbinding, called after context is created
     */
    virtual void bindingPostInit(const GLProperties&) = 0;
    /*!
     * \brief Function called to terminate binding library like glbinding
     */
    virtual void bindingTerminate() = 0;

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