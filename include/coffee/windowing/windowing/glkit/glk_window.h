#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#if defined(COFFEE_USE_APPLE_GLKIT)

#include "../../graphics/egl/egl_renderer.h"

namespace Coffee{
namespace Display{

class GLKWindow :
        public WindowManagerClient,
        public virtual EventApplication
{
    friend class EGLRenderer;
    
protected:
    CDProperties m_properties;
    
public:
    GLKWindow();
    virtual ~GLKWindow();
    
    // Windowing
    CDMonitor monitor();
    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    void popErrorMessage(Severity s, cstring title, cstring msg);
    
    CDWindow *window();
    uint32 windowState() const;
    void setWindowState(const CDProperties::State &s);
    CSize windowSize() const;
    void setWindowSize(const CSize &s);
    CPoint windowPosition() const;
    void setWindowPosition(const CPoint &p);
    CString windowTitle() const;
    void setWindowTitle(const CString &tl);
    bool setWindowIcon(CBitmap &icon);
    bool screensaverMode();
    void setScreensaverMode(bool m);
    CString windowLibrary() const;

protected:
    bool windowPreInit(const CDProperties &, CString *err);
    bool windowInit(const CDProperties &props, CString * err);
    bool windowPostInit(const CDProperties &, CString *);
    void windowTerminate();
    
    // Event handling
    bool inputPreInit(CString*){return true;}
    bool inputInit(CString*){return true;}
    bool inputPostInit(CString*){return true;}
    void inputTerminate(){}
    
    void eventHandleD(const CDEvent&, c_cptr){}
    void eventHandleI(const CIEvent&, c_cptr){}
    
    void eventHandle(const CIHapticEvent&, c_cptr){}
    void eventHandle(const CDEvent&, c_cptr){}
    void eventHandle(const CIEvent&, c_cptr){}
    
    void injectEvent(CIEvent const&, c_cptr) {}
    void injectEvent(CDEvent const&, c_cptr) {}
    
    bool isMouseGrabbed() const {return true;}
    void setMouseGrabbing(bool){}
    
    bool relativeMouse() const {return true;}
    void setRelativeMouse(bool){}
    
    CPoint mousePosition() const {return {};}
    void setMousePosition(CPoint const&){}
    
    void setKeyboardRepeat(bool){}
    bool textInputMode() const {return false;}
    
    void setTextInputMode(bool){}
    
    bigscalar contextTime() const {return 0.0;}
    
    CIControllerState getControllerState(uint16) {return {};}
};

}
}

#endif
