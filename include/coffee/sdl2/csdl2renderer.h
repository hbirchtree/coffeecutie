#ifndef CSDL2RENDERER_H
#define CSDL2RENDERER_H

#include <coffee/core/CDRendererBase>
#include "csdl2_dialogs.h"

#include "csdl2_gl_renderer.h"
#include "csdl2_window.h"
#include "csdl2_eventhandler.h"

namespace Coffee{
namespace CSDL2Types{
    struct CSDL2Context;
}
namespace CDisplay{

class CSDL2Renderer :
        public CDRendererBase,
        public SDL2Window,
        public SDL2GLRenderer,
        public SDL2EventHandler
{
public:
    virtual ~CSDL2Renderer();

    void init(const CDProperties &);
    void cleanup();

protected:
    CSDL2Renderer(CObject* parent);
};

}
}

#endif // CSDL2RENDERER_H
