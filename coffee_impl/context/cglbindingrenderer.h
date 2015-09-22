#ifndef CGLBINDINGRENDERER_H
#define CGLBINDINGRENDERER_H

//#include "coffee_impl/display/cglfwrenderer.h"
#include "coffee_impl/display/csdl2renderer.h"

namespace Coffee{
namespace CDisplay{

class CGLBindingRenderer : public CSDL2Renderer
{
public:
    CGLBindingRenderer(CObject* parent);
    virtual ~CGLBindingRenderer();

protected:
    void bindingPreInit();
    void bindingPostInit();
    void bindingTerminate();

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
    int m_libraryRevision;
};

}
}

#endif // CGLBINDINGRENDERER_H
