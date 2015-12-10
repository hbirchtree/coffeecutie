#ifndef CGLBINDINGRENDERER_H
#define CGLBINDINGRENDERER_H

#include "coffee/core/display/csdl2renderer.h"

namespace Coffee{

namespace CGraphicsWrappers{
struct CGLReport;
}

namespace CDisplay{

/*!
 * \brief Returns true if message is accepted, false is discarded
 */
typedef std::function<bool(CGLReport*)> CGLMessageFilter;

bool glbinding_default_filter(CGLReport *report);

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

    virtual void bindingCallback(const void* report) const;

protected:
    CGLBindingRenderer(CObject* parent);

    void bindingPreInit();
    void bindingPostInit();
    void bindingTerminate();

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
    int m_libraryRevision;

    cstring extensions();

    CGLMessageFilter m_msg_filter;

private:
    cstring_w m_extensions;
};

}
}

#endif // CGLBINDINGRENDERER_H
