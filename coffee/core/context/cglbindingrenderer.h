#ifndef CGLBINDINGRENDERER_H
#define CGLBINDINGRENDERER_H

//#include "coffee_impl/display/cglfwrenderer.h"
#include "coffee/core/display/csdl2renderer.h"

namespace Coffee{
namespace CDisplay{

class CGLBindingRenderer : public CSDL2Renderer
{
public:
    virtual ~CGLBindingRenderer();

    void fetchGLExtensions();

    /*!
     * \brief Note: This function may hang the application if run on a different thread from the main one before the renderer is started. To avoid this, run fetchGLExtensions() early on.
     * \param ext
     * \return
     */
    bool requestGLExtension(cstring ext);

    bool printExtensions(bool doFetch = false);

protected:
    CGLBindingRenderer(CObject* parent);

    void bindingPreInit();
    void bindingPostInit();
    void bindingTerminate();

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
    int m_libraryRevision = 0;

    cstring extensions();

private:
    cstring_w m_extensions = nullptr;

};

}
}

#endif // CGLBINDINGRENDERER_H
