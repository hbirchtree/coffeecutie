#ifndef CGLBINDINGRENDERER_H
#define CGLBINDINGRENDERER_H

#include <wrappers/cgraphicswrappers.h>
#include <coffee/core/display/csdl2renderer.h>

namespace Coffee{

namespace CGraphicsWrappers{
struct CGLReport;
}

namespace CDisplay{

/*!
 * \brief Returns true if message is accepted, false is discarded
 */
typedef std::function<bool(CGLDebugMessage const&)> CGLMessageFilter;

bool glbinding_default_filter(CGLReport *report);

class CGLBindingRenderer : public CSDL2Renderer
{
public:
    virtual ~CGLBindingRenderer();

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

    CGLMessageFilter m_msg_filter;

private:
};

}
}

#endif // CGLBINDINGRENDERER_H
