#pragma once

#include <coffee/core/base/renderer/glapplication.h>
#include <coffee/core/base/renderer/glloader.h>

namespace Coffee {
namespace Display {

class GLeamRenderer : public virtual GLLoader
{
  public:
    GLeamRenderer(GLApplication* app);
    virtual ~GLeamRenderer();

    virtual void bindingCallback(const void* report) const;

  protected:
    bool bindingPreInit(const GLProperties&, CString*);
    bool bindingInit(const GLProperties&, CString*);
    bool bindingPostInit(const GLProperties& p, CString*);

    void bindingTerminate();
};

} // namespace Display
} // namespace Coffee
