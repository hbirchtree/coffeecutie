#ifndef CGLBINDINGRENDERER_H
#define CGLBINDINGRENDERER_H

#include "wrappers/cgraphicswrappers.h"
#include <coffee/sdl2/csdl2renderer.h>

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

    void bindingPreInit(const GLProperties&);
    void bindingInit(const GLProperties&);
    void bindingPostInit(const GLProperties&p);
    void bindingTerminate();

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
    int m_libraryRevision;

    CGLMessageFilter m_msg_filter;

private:
};

//TODO: Externalize this to a glxml library for introspection of GL features
extern std::map<CString, CString> coffee_glbinding_get_graphics_feature_level();

}
}

#endif // CGLBINDINGRENDERER_H
