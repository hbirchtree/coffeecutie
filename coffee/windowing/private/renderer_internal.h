#pragma once

#include <coffee/windowing/renderer/renderer.h>

#include <coffee/core/CDRendererBase>

#include <coffee/sdl2/graphics/csdl2_gl_renderer.h>
#include <coffee/windowing/graphics/egl/egl_renderer.h>
#include <coffee/windowing/graphics/glx/glx_renderer.h>

#include <coffee/sdl2/windowing/csdl2_window.h>
#include <coffee/windowing/windowing/x11/x11_window.h>
#include <coffee/windowing/windowing/dispmanx/dispmanx_window.h>
#include <coffee/windowing/windowing/glkit/glk_window.h>
#include <coffee/windowing/windowing/ndkwindow/ndk_window.h>

#include <coffee/sdl2/input/csdl2_eventhandler.h>

#include <coffee/windowing/binding/glad/gleamrenderer.h>

#include <coffee/core/base/renderer_loader.h>

#if defined(COFFEE_GEKKO)
#include <coffee/windowing/windowing/ogc/ogc_window.h>
#endif

namespace Coffee{

namespace SDL2{
    struct SDL2Context;
}

namespace Display{

class CSDL2Renderer_Internal final :
        public RendererInterface

        #if defined(COFFEE_ENABLE_OPENGL)
        #if defined(COFFEE_USE_SDL_GL) && defined(COFFEE_USE_SDL_WINDOW)
        , public SDL2Window,
        public SDL2GLRenderer
        #elif defined(COFFEE_USE_APPLE_GLKIT) && defined(COFFEE_USE_MAEMO_EGL)
        , public GLKWindow,
        public EGLRenderer
        #elif defined(COFFEE_USE_ANDROID_NATIVEWIN) && defined(COFFEE_USE_MAEMO_EGL)
        , public NDKWindow,
        public EGLRenderer
        #elif defined(COFFEE_RASPBERRY_DMX)
        , public DispmanXWindow,
        public EGLRenderer
        #elif defined(COFFEE_USE_LINUX_GLX) && defined(COFFEE_USE_MAEMO_X11)
        , public X11Window,
        public GLXRenderer
        #elif defined(COFFEE_USE_WINDOWS_ANGLE) && defined(COFFEE_USE_SDL_WINDOW)
        , public EGLRenderer,
        public SDL2Window
        #elif defined(COFFEE_USE_MAEMO_EGL) && defined(COFFEE_USE_MAEMO_X11)
        , public X11Window,
        public EGLRenderer
        #else
        #error No rendering infrastructure defined
        #endif

        #elif defined(COFFEE_GEKKO)
        , public OGC_Window

        #else
        #error No windowing infrastructure defined
        #endif

        #if defined(COFFEE_USE_SDL_EVENT)
        , public SDL2EventHandler
        #endif

        #if defined(COFFEE_ENABLE_OPENGL)
        , public GLeamRenderer
        #endif
{
public:
    CSDL2Renderer_Internal();

    virtual ~CSDL2Renderer_Internal();

    bool init(const CDProperties &, CString*err);
    void cleanup();
    virtual void run();

protected:
    CSDL2Renderer_Internal(CObject* parent);

#if !defined(COFFEE_USE_SDL_EVENT) && \
    !defined(COFFEE_USE_MAEMO_X11) && \
    !defined(COFFEE_USE_APPLE_GLKIT) && \
    !defined(COFFEE_USE_ANDROID_NATIVEWIN)
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
#endif
};

}
}
