#pragma once

#include "../types/cdisplay.h"
#include "glapplication.h"

namespace Coffee{
namespace CDisplay{

class GLLoader
{
public:
    /*!
     * \brief Here the loader attaches to an existing application, may be "this"
     * \param app
     */
    GLLoader(GLApplication* app):
        m_app(app)
    {
    }

    /*!
     * \brief Callback for GL errors and messages, implemented by the user.
     * \param d A pointer to the system's OpenGL report format, implementation-specific.
     */
    virtual void bindingCallback(const void*) const = 0;

protected:
    /*!
     * \brief Function called to initialize
     *  binding library like glbinding, called before context is created
     */
    virtual bool bindingPreInit(const GLProperties&,CString*) = 0;
    virtual bool bindingInit(const GLProperties&,CString*) = 0;
    /*!
     * \brief Function called to initialize binding
     *  library like glbinding, called after context is created
     */
    virtual bool bindingPostInit(const GLProperties&,CString*) = 0;
    /*!
     * \brief Function called to terminate binding library like glbinding
     */
    virtual void bindingTerminate() = 0;


protected:
    GLApplication* m_app;
};

}
}
