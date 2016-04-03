#ifndef COFFEE_GRAPHICS_APIS_GLEAM_RENDERER_RENDERER_H
#define COFFEE_GRAPHICS_APIS_GLEAM_RENDERER_RENDERER_H

#include <coffee/core/base/renderer/glloader.h>
#include <coffee/core/base/renderer/glapplication.h>

namespace Coffee{
namespace CDisplay{

class GLeamRenderer : public GLLoader
{
public:
    GLeamRenderer(GLApplication* app);
    virtual ~GLeamRenderer();

    virtual void bindingCallback(const void * report) const;
protected:

    bool bindingPreInit(const GLProperties&,CString*);
    bool bindingInit(const GLProperties&,CString*);
    bool bindingPostInit(const GLProperties &p,CString*);

    void bindingTerminate();

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
};

}
}

#endif
