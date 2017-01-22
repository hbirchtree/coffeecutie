#ifndef CSDL2RENDERER_H
#define CSDL2RENDERER_H

#include <coffee/core/CDRendererBase>

#include <coffee/sdl2/graphics/csdl2_gl_renderer.h>
#include <coffee/sdl2/windowing/csdl2_window.h>
#include <coffee/sdl2/input/csdl2_eventhandler.h>
#include <coffee/graphics/apis/gleam/renderer/gleamrenderer.h>

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

template<typename Renderer> STATICINLINE
bool LoadHighestVersion(Renderer* renderer, CDProperties& properties, CString* err)
{
#if !defined(COFFEE_ANDROID)
    do{
#if defined(COFFEE_GLEAM_DESKTOP)
        properties.gl.version.major = 4;
        properties.gl.version.minor = 5;
#else
        properties.gl.version.major = 3;
        properties.gl.version.minor = 2;
#endif
        if(renderer->init(properties,err))
            break;

#if defined(COFFEE_GLEAM_DESKTOP)
        properties.gl.version.minor = 3;
#else
        properties.gl.version.major = 3;
        properties.gl.version.minor = 1;
#endif
        if(renderer->init(properties,err))
            break;

#if defined(COFFEE_GLEAM_DESKTOP)
        properties.gl.version.major = 3;
#else
        properties.gl.version.major = 3;
        properties.gl.version.minor = 0;
#endif
        if(renderer->init(properties,err))
            break;

#if !defined(COFFEE_GLEAM_DESKTOP)
        properties.gl.version.major = 2;
        if(renderer->init(properties,err))
            break;
#endif

        return false;
    } while(false);
#else
    renderer->init(properties, err);
#endif
    return true;
}

}
}

#endif // CSDL2RENDERER_H