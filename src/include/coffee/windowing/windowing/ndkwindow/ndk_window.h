#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#include <coffee/core/types/cdef/geometry.h>

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
    virtual void eventHandleD(const CDEvent &e, c_cptr data);
    virtual void eventHandleI(const CIEvent &e, c_cptr data);
    virtual void eventHandle(const CIHapticEvent &haptic, c_cptr data);
    virtual void eventHandle(const CIEvent &event, c_cptr data);
    virtual void eventHandle(const CDEvent &event, c_cptr data);

    virtual CIControllerState getControllerState(uint16 index);
    virtual bool isMouseGrabbed() const;
    virtual void setMouseGrabbing(bool m);
    virtual bool relativeMouse() const;
    virtual void setRelativeMouse(bool enable);
    virtual CPoint mousePosition() const;
    virtual void setMousePosition(const CPoint &p);
    virtual void setKeyboardRepeat(bool m);
    virtual bool textInputMode() const;
    virtual void setTextInputMode(bool m);

public:
    virtual CDMonitor monitor();
    virtual bool showWindow();
    virtual bool hideWindow();
    virtual bool closeWindow();
    virtual void popErrorMessage(Severity s, cstring title, cstring msg);

protected:
    virtual bool windowPreInit(const CDProperties &, CString *);
    virtual bool windowInit(const CDProperties &, CString *);
    virtual bool windowPostInit(const CDProperties &, CString *);
    virtual void windowTerminate();

public:
    virtual void injectEvent(const CIEvent &ev, c_cptr data);
    virtual void injectEvent(const CDEvent &ev, c_cptr data);

public:
    virtual CDWindow *window();
    virtual uint32 windowState() const;
    virtual void setWindowState(const CDProperties::State &s);
    virtual CSize windowSize() const;
    virtual void setWindowSize(const CSize &s);
    virtual CPoint windowPosition() const;
    virtual void setWindowPosition(const CPoint &p);
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
