#ifndef CGLBINDINGRENDERER_H
#define CGLBINDINGRENDERER_H

#include <wrappers/cgraphicswrappers.h>
#include <csdl2renderer.h>

namespace Coffee{

namespace CDisplay{

/*!
 * \brief Returns true if message is accepted, false is discarded
 */
using CGLMessageFilter = std::function<bool(CGraphicsWrappers::CGLDebugMessage const&)>;

class CGLBindingRenderer : public CSDL2Renderer
{
public:
    virtual ~CGLBindingRenderer();

    virtual void bindingCallback(const void* report) const;

    using CSDL2Renderer::eventHandle;

protected:
    CGLBindingRenderer(CObject* parent);

    void bindingPreInit();
    void bindingPostInit();
    void bindingTerminate();

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
    int m_libraryRevision;

    CGLMessageFilter m_msg_filter;

private:
};

extern std::map<CString, CString> coffee_glbinding_get_graphics_feature_level();

}
}

#endif // CGLBINDINGRENDERER_H
