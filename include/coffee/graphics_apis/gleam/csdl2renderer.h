#ifndef CSDL2RENDERER_H
#define CSDL2RENDERER_H

#include <coffee/core/CDRendererBase>

#include <coffee/sdl2/graphics/csdl2_gl_renderer.h>
#include <coffee/sdl2/windowing/csdl2_window.h>
#include <coffee/sdl2/input/csdl2_eventhandler.h>
#include <coffee/graphics_apis/gleam/renderer/gleamrenderer.h>

namespace Coffee{
namespace CSDL2Types{
    struct CSDL2Context;
}
namespace Display{

class CSDL2Renderer :
        public CDRendererBase,
        public SDL2Window,
        public SDL2GLRenderer,
        public SDL2EventHandler,
        public GLeamRenderer
{
public:
    virtual ~CSDL2Renderer();

    bool init(const CDProperties &, CString*err);
    void cleanup();

protected:
    CSDL2Renderer(CObject* parent);
};

}
}

#endif // CSDL2RENDERER_H
