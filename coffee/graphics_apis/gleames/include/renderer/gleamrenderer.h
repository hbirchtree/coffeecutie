#ifndef COFFEE_GRAPHICS_APIS_GLEAM_RENDERER_RENDERER_H
#define COFFEE_GRAPHICS_APIS_GLEAM_RENDERER_RENDERER_H

#include <coffee/sdl2/include/csdl2renderer.h>

namespace Coffee{
namespace CDisplay{

class CGLeamRenderer : public CSDL2Renderer
{
public:
    virtual ~CGLeamRenderer();

    virtual void bindingCallback(const void * report) const;
    using CSDL2Renderer::eventHandle;
protected:
    CGLeamRenderer(CObject*parent);

    void bindingPreInit();
    void bindingPostInit();
    void bindingTerminate();

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
};

}
}

#endif
