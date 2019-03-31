#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#if defined(COFFEE_USE_ANDROID_NATIVEWIN)

namespace Coffee{
namespace Display{

class NDKWindow :
        public virtual WindowManagerClient,
        public virtual EventApplication
{
    Vector<EventHandlerI> m_eventhandlers_input;
    Vector<EventHandlerD> m_eventhandlers_windw;

public:
    virtual bool inputPreInit(CString *);
    virtual bool inputInit(CString *);
    virtual bool inputPostInit(CString *);
    virtual void inputTerminate();
    virtual void eventHandleD(const Event &e, c_cptr data);
    virtual void eventHandleI(const Input::CIEvent &e, c_cptr data);
    virtual void eventHandle(const Input::CIHapticEvent &haptic, c_cptr data);
    virtual void eventHandle(const Input::CIEvent &event, c_cptr data);
    virtual void eventHandle(const Event &event, c_cptr data);

    virtual Input::CIControllerState getControllerState(u16 index);
    virtual bool isMouseGrabbed() const;
    virtual void setMouseGrabbing(bool m);
    virtual bool relativeMouse() const;
    virtual void setRelativeMouse(bool enable);
    virtual Point mousePosition() const;
    virtual void setMousePosition(const Point &p);
    virtual void setKeyboardRepeat(bool m);
    virtual bool textInputMode() const;
    virtual void setTextInputMode(bool m);

public:
    virtual Monitor monitor();
    virtual bool showWindow();
    virtual bool hideWindow();
    virtual bool closeWindow();
    virtual void popErrorMessage(debug::Severity s, cstring title, cstring msg);

protected:
    virtual bool windowPreInit(const Properties &, CString *);
    virtual bool windowInit(const Properties &, CString *);
    virtual bool windowPostInit(const Properties &, CString *);
    virtual void windowTerminate();

public:
    virtual void injectEvent(const Input::CIEvent &ev, c_cptr data);
    virtual void injectEvent(const Event &ev, c_cptr data);

public:
    virtual Window *window();
    virtual uint32 windowState() const;
    virtual void setWindowState(const Properties::State &s);
    virtual Size windowSize() const;
    virtual void setWindowSize(const Size &s);
    virtual Point windowPosition() const;
    virtual void setWindowPosition(const Point &p);
    virtual CString windowTitle() const;
    virtual void setWindowTitle(const CString &tl);
    virtual bool screensaverMode();
    virtual void setScreensaverMode(bool m);
    virtual CString windowLibrary() const;

public:
    virtual void run();
    virtual bool installEventHandler(EventHandlerI h);
    virtual bool installEventHandler(EventHandlerD h);
    virtual Vector<EventHandlerI> *getEventHandlersI();
    virtual Vector<EventHandlerD> *getEventHandlersD();
};

}
}

#endif
