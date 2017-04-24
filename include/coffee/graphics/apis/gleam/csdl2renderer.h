#ifndef CSDL2RENDERER_H
#define CSDL2RENDERER_H

#include <coffee/core/CDRendererBase>

#include <coffee/sdl2/graphics/csdl2_gl_renderer.h>
#include <coffee/sdl2/graphics/egl_renderer.h>

#include <coffee/sdl2/windowing/csdl2_window.h>
#include <coffee/sdl2/windowing/x11_window.h>
#include <coffee/sdl2/windowing/dispmanx_window.h>

#include <coffee/sdl2/input/csdl2_eventhandler.h>
#include <coffee/graphics/apis/gleam/renderer/gleamrenderer.h>

namespace Coffee{
namespace SDL2{
    struct SDL2Context;
}
namespace Display{

class CSDL2Renderer :
        public CDRendererBase,
        #if defined(COFFEE_RASPBERRY_DMX)
        public DispmanXWindow,
        public EGLRenderer,
        #elif defined(COFFEE_USE_MAEMO_EGL)
        public X11Window,
        public EGLRenderer,
        #else
        public SDL2Window,
        public SDL2GLRenderer,
        #endif
        public SDL2EventHandler,
        public GLeamRenderer
{
public:
    virtual ~CSDL2Renderer();

    bool init(const CDProperties &, CString*err);
    void cleanup();
    virtual void run();

protected:
    CSDL2Renderer(CObject* parent);
};

template<typename Renderer> STATICINLINE
bool LoadHighestVersion(Renderer* renderer, CDProperties& properties, CString* err)
{
#if !defined(COFFEE_ANDROID)
    do{
#if !defined(COFFEE_ONLY_GLES20)
        if(properties.gl.flags & (GLProperties::Flags::GLES))
        {
            properties.gl.version.major = 3;
            properties.gl.version.minor = 2;
        }else
        {
            properties.gl.version.major = 4;
            properties.gl.version.minor = 5;
        }

        if(renderer->init(properties,err))
            break;

        if(properties.gl.flags & (GLProperties::Flags::GLES))
        {
            properties.gl.version.major = 3;
            properties.gl.version.minor = 1;
        }else
        {
            properties.gl.version.minor = 3;
        }

        if(renderer->init(properties,err))
            break;

        if(properties.gl.flags & (GLProperties::Flags::GLES))
        {
            properties.gl.version.major = 3;
            properties.gl.version.minor = 0;
        }else
        {
            properties.gl.version.major = 3;
        }
        if(renderer->init(properties,err))
            break;
#endif
        if(properties.gl.flags & (GLProperties::Flags::GLES))
        {
            properties.gl.version.major = 2;

            if(renderer->init(properties,err))
                break;
        }

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
