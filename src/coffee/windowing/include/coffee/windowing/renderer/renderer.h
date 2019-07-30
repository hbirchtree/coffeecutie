#pragma once

#include <coffee/core/CDRendererBase>
#include <coffee/windowing/stub/stub.h>

namespace Coffee {
using CDRendererBase = Display::CDRendererBase;
namespace Display {

class RendererInterface : public CDRendererBase,
                          public virtual EventApplication,
                          public virtual WindowManagerClient,
                          public virtual GLLoader,
                          public virtual GLApplication
{
  public:
    RendererInterface(CObject* parent = nullptr) :
        GLLoader(this), CDRendererBase(parent)
    {
    }
    virtual ~RendererInterface()
    {
    }
};

using CSDL2Renderer = RendererInterface;

extern CSDL2Renderer* CreateRenderer();

FORCEDINLINE UqPtr<CSDL2Renderer> CreateRendererUq()
{
    return UqPtr<CSDL2Renderer>(CreateRenderer());
}

FORCEDINLINE ShPtr<CSDL2Renderer> CreateRendererSh()
{
    return ShPtr<CSDL2Renderer>(CreateRenderer());
}

} // namespace Display
} // namespace Coffee
