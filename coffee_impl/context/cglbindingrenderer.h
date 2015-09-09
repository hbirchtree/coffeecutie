#ifndef CGLBINDINGRENDERER_H
#define CGLBINDINGRENDERER_H

#include "coffee_impl/display/cglfwrenderer.h"

namespace Coffee{
namespace CDisplay{

class CGLBindingRenderer : public CGLFWRenderer
{
public:
    CGLBindingRenderer(CObject* parent);

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
