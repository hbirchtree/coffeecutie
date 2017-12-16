#ifndef CSDL2RENDERER_H
#define CSDL2RENDERER_H

#include <coffee/core/CDRendererBase>
#include <coffee/windowing/stub/stub.h>

namespace Coffee{

using CDRendererBase = Display::CDRendererBase;
using namespace CInput;

namespace Display{

class RendererInterface :
        public CDRendererBase,
        public virtual EventApplication,
        public virtual WindowManagerClient,
        public virtual GLLoader,
        public virtual GLApplication
{
public:
    RendererInterface(CObject* parent = nullptr) :
        CDRendererBase(parent),
        GLLoader(this)
    {}
    virtual ~RendererInterface() {}
};

using CSDL2Renderer = RendererInterface;

extern CSDL2Renderer* CreateRenderer();

FORCEDINLINE UqPtr<CSDL2Renderer> CreateRendererUq()
{
    return UqPtr<CSDL2Renderer>(CreateRenderer());
}

}
}

#endif // CSDL2RENDERER_H
