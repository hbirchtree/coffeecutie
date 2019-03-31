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
    bool bindingPreInit(const GL::Properties&, CString*);
    bool bindingInit(const GL::Properties&, CString*);
    bool bindingPostInit(const GL::Properties& p, CString*);

    void bindingTerminate();
};

} // namespace Display
} // namespace Coffee
