#pragma once

#include <coffee/core/types/display/context_bits.h>
#include <coffee/core/types/display/gl_properties.h>
#include <coffee/core/types/size.h>
#include <coffee/graphics/common/gltypes.h>

namespace Coffee {
namespace Display {

class GLApplication
{
  public:
    virtual ~GLApplication()
    {
    }

    virtual bool contextPreInit(const GL::Properties& props, CString*)  = 0;
    virtual bool contextInit(const GL::Properties& props, CString*)     = 0;
    virtual bool contextPostInit(const GL::Properties& props, CString*) = 0;
    virtual void contextTerminate()                                     = 0;

    /*!
     * \brief Function used for swapping buffers in render loop
     */
    virtual void swapBuffers() = 0;

    /*!
     * \brief Get current framebuffer size
     * \return
     */
    virtual Size framebufferSize() const = 0;

    /*!
     * \brief Get current swap interval
     * \return
     */
    virtual int swapInterval() const = 0;
    /*!
     * \brief Set swap interval
     * \param i
     */
    virtual void setSwapInterval(const int& i) = 0;

    /*!
     * \brief Get context bit information
     * \return Struct containing context bits
     */
    virtual ContextBits context() = 0;

    /*!
     * \brief Get thread ID on which the GL context is current
     * \return
     */
    virtual ThreadId contextThread() = 0;

    virtual CGL::CGL_Context*      glContext()     = 0;
    virtual CGL::CGL_ScopedContext scopedContext() = 0;

    virtual CGL::CGL_WorkerContext* workerContext()
    {
        return nullptr;
    }
};

} // namespace Display
} // namespace Coffee
