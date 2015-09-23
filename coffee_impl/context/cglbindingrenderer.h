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

    void fetchGLExtensions();

    //Note: May hang!
    //It is wise to run this asynchronously
    bool requestGLExtension(cstring ext);

protected:
    void bindingPreInit();
    void bindingPostInit();
    void bindingTerminate();

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
    int m_libraryRevision = 0;

private:
    cstring_w m_extensions = nullptr;

};

}
}

#endif // CGLBINDINGRENDERER_H
