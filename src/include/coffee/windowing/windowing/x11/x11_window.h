#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#include "../../graphics/glx/glx_renderer.h"

#if defined(COFFEE_USE_MAEMO_X11)

namespace Coffee{
namespace Display{

struct X11_Data;

class X11Window :
        public virtual WindowManagerClient,
        private virtual EventApplication
{
    friend class GLXRenderer;

    Vector<EventHandlerI> m_ihandlers;
    Vector<EventHandlerD> m_dhandlers;

    UqPtr<X11_Data> m_xData;
    // WindowApplication interface
public:
    X11Window();
    virtual ~X11Window();

    CDMonitor monitor();
    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    void popErrorMessage(Severity s, cstring title, cstring msg);

protected:
    bool windowPreInit(const CDProperties &, CString *err);
    bool windowInit(const CDProperties &props, CString * err);
    bool windowPostInit(const CDProperties &, CString *);
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
    bool screensaverMode();
    void setScreensaverMode(bool m);
    CString windowLibrary() const;

public:
    void processX11Events(InputApplication *eh);

    // InputApplication interface
public:
    virtual void pollEvents();

    virtual bool inputPreInit(CString *)
    {return true;}
    virtual bool inputInit(CString *)
    {return true;}
    virtual bool inputPostInit(CString *)
    {return true;}
    virtual void inputTerminate()
    {}
    virtual void eventHandleD(const CDEvent &e, c_cptr data);
    virtual void eventHandleI(const CIEvent &e, c_cptr data);
    virtual void eventHandle(const CIHapticEvent &haptic, c_cptr data);
    virtual void eventHandle(const CIEvent &event, c_cptr data);
    virtual void eventHandle(const CDEvent &event, c_cptr data);
    virtual CIControllerState getControllerState(uint16 index)
    {
        return {};
    }
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
    virtual void injectEvent(const CIEvent &, c_cptr);
    virtual void injectEvent(const CDEvent &, c_cptr);

    // EventApplication interface
public:
    virtual bool installEventHandler(EventHandlerI eh);
    virtual bool installEventHandler(EventHandlerD eh);
    virtual Vector<EventHandlerI> *getEventHandlersI();
    virtual Vector<EventHandlerD> *getEventHandlersD();
};

}
}
#endif
