#pragma once

#include <coffee/core/base/renderer/windowmanagerclient.h>
#include <coffee/core/base/renderer/eventapplication.h>

#if defined(COFFEE_RASPBERRY_DMX)
namespace Coffee{
namespace Display{

struct DISPMAN_Data;

class DispmanXWindow :
        public virtual WindowManagerClient,
        public virtual EventApplication
{
    DISPMAN_Data* m_dmData;

    // WindowApplication interface
public:
    CDMonitor monitor();
    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    void popErrorMessage(Severity s, cstring title, cstring msg);

protected:
    bool windowPreInit(const CDProperties & props, CString *err);
    bool windowInit(const CDProperties & props, CString *err);
    bool windowPostInit(const CDProperties & props, CString *err);
    void windowTerminate();

    // WindowManagerClient interface
public:
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

    // InputApplication interface
public:
    bool inputPreInit(CString *) {return true;}
    bool inputInit(CString *) {return true;}
    bool inputPostInit(CString *) {return true;}
    void inputTerminate() {}
    void eventHandleD(const CDEvent &e, c_cptr) {}
    void eventHandleI(const CIEvent &e, c_cptr) {}
    void eventHandle(const CIHapticEvent &, c_cptr) {}
    void eventHandle(const CIEvent &, c_cptr) {}
    void eventHandle(const CDEvent &, c_cptr) {}
    CIControllerState getControllerState(uint16) { return {};}
    bool isMouseGrabbed() const { return false; }
    void setMouseGrabbing(bool) {}
    bool relativeMouse() const { return false; }
    void setRelativeMouse(bool) {}
    CPoint mousePosition() const {return {};}
    void setMousePosition(const CPoint &) {}
    void setKeyboardRepeat(bool) {}
    bool textInputMode() const { return false;}
    void setTextInputMode(bool) {}

    // EventApplication interface
public:
    void run() {}
    void injectEvent(const CIEvent &, c_cptr) {}
    void injectEvent(const CDEvent &, c_cptr) {}
    bigscalar contextTime() const { return 0.0; }
};

}
}
#endif
