#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#if defined(COFFEE_USE_APPLE_GLKIT)

#include "../../graphics/egl/egl_renderer.h"

namespace Coffee{
namespace Display{

class GLKWindow :
        public virtual WindowManagerClient,
        public virtual EventApplication
{
    friend class EGLRenderer;
    
protected:
    Properties m_properties;
    
    Vector<EventHandlerI> m_eventhandlers_input;
    Vector<EventHandlerD> m_eventhandlers_windw;
    
public:
    GLKWindow();
    virtual ~GLKWindow();
    
    // Windowing
    Monitor monitor();
    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    void popErrorMessage(debug::Severity s, cstring title, cstring msg);
    
    Window *window();
    uint32 windowState() const;
    void setWindowState(const Properties::State &s);
    Size windowSize() const;
    void setWindowSize(const Size &s);
    Point windowPosition() const;
    void setWindowPosition(const Point &p);
    CString windowTitle() const;
    void setWindowTitle(const CString &tl);
    bool screensaverMode();
    void setScreensaverMode(bool m);
    CString windowLibrary() const;


    bool windowPreInit(const Properties &, CString *err);
    bool windowInit(const Properties &props, CString * err);
    bool windowPostInit(const Properties &, CString *);
    void windowTerminate();
    
    // Event handling
    bool inputPreInit(CString*){return true;}
    bool inputInit(CString*){return true;}
    bool inputPostInit(CString*){return true;}
    void inputTerminate(){}
    
    void eventHandleD(const Event& e, c_cptr d);
    void eventHandleI(const Input::CIEvent& e, c_cptr d);
    
    void eventHandle(const Input::CIHapticEvent& e, c_cptr d);
    void eventHandle(const Event& e, c_cptr d);
    void eventHandle(const Input::CIEvent& e, c_cptr d);
    
    void injectEvent(Input::CIEvent const& e, c_cptr d);
    void injectEvent(Event const& e, c_cptr d);
    
    bool installEventHandler(EventHandlerD eh);
    bool installEventHandler(EventHandlerI eh);
    
    bool isMouseGrabbed() const {return true;}
    void setMouseGrabbing(bool){}
    
    bool relativeMouse() const {return true;}
    void setRelativeMouse(bool){}
    
    Point mousePosition() const {return {};}
    void setMousePosition(Point const&){}
    
    void setKeyboardRepeat(bool){}
    bool textInputMode() const {return false;}
    
    void setTextInputMode(bool){}
    
    Input::CIControllerState getControllerState(u16) {return {};}
};

}
}

#endif
