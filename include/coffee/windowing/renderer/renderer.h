#ifndef CSDL2RENDERER_H
#define CSDL2RENDERER_H

#include <coffee/core/CDRendererBase>

#include <coffee/sdl2/graphics/csdl2_gl_renderer.h>
#include <coffee/windowing/graphics/egl/egl_renderer.h>
#include <coffee/windowing/graphics/glx/glx_renderer.h>

#include <coffee/sdl2/windowing/csdl2_window.h>
#include <coffee/windowing/windowing/x11/x11_window.h>
#include <coffee/windowing/windowing/dispmanx/dispmanx_window.h>

#include <coffee/sdl2/input/csdl2_eventhandler.h>

#include <coffee/windowing/binding/glad/gleamrenderer.h>

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
        #elif defined(COFFEE_USE_LINUX_GLX)
        public X11Window,
        public GLXRenderer,
        #elif defined(COFFEE_USE_WINDOWS_ANGLE)
        public EGLRenderer,
        public SDL2Window,
        #elif defined(COFFEE_USE_MAEMO_EGL)
        public X11Window,
        public EGLRenderer,
        #elif defined(COFFEE_USE_SDL_GL) && defined(COFFEE_USE_SDL_WINDOW)
        public SDL2Window,
        public SDL2GLRenderer,
        #else
        #error No rendering infrastructure defined
        #endif

        #if defined(COFFEE_USE_SDL_EVENT)
        public SDL2EventHandler,
        #endif

        public GLeamRenderer
{
public:
    CSDL2Renderer();

    virtual ~CSDL2Renderer();

    bool init(const CDProperties &, CString*err);
    void cleanup();
    virtual void run();

protected:
    CSDL2Renderer(CObject* parent);

#if !defined(COFFEE_USE_SDL_EVENT) && !defined(COFFEE_USE_MAEMO_X11)
    // InputApplication interface
public:
    virtual bool inputPreInit(CString *) {return true;}
    virtual bool inputInit(CString *) {return true;}
    virtual bool inputPostInit(CString *) {return true;}
    virtual void inputTerminate() {}
    virtual void eventHandleD(const CDEvent &e, c_cptr data) {}
    virtual void eventHandleI(const CIEvent &e, c_cptr data){}
    virtual void eventHandle(const CIHapticEvent &haptic, c_cptr data) {}
    virtual void eventHandle(const CIEvent &event, c_cptr data) { }
    virtual void eventHandle(const CDEvent &event, c_cptr data) { }
    virtual CIControllerState getControllerState(uint16 index) { return {}; }
    virtual bool isMouseGrabbed() const {return false;}
    virtual void setMouseGrabbing(bool m) {}
    virtual bool relativeMouse() const {return false;}
    virtual void setRelativeMouse(bool enable) { }
    virtual CPoint mousePosition() const {return {};}
    virtual void setMousePosition(const CPoint &p) {}
    virtual void setKeyboardRepeat(bool m){}
    virtual bool textInputMode() const {return false;}
    virtual void setTextInputMode(bool m) {}

    // EventApplication interface
public:
    virtual void injectEvent(const CIEvent &, c_cptr) {}
    virtual void injectEvent(const CDEvent &, c_cptr) {}
    virtual bigscalar contextTime() const
    {
        static u64 start_time = 0;
        if(start_time == 0)
            start_time = Time::CurrentTimestamp<std::chrono::microseconds>();

        return bigscalar(Time::CurrentTimestamp<std::chrono::microseconds>() - start_time) * 1_us;
    }
#endif
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
