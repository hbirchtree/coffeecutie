#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#include <coffee/core/types/cdef/geometry.h>

#if defined(COFFEE_USE_ANDROID_NATIVEWIN)

namespace Coffee{
namespace Display{

class NDKWindow : public WindowManagerClient, public virtual EventApplication
{
    // InputApplication interface
public:
    virtual bool inputPreInit(CString *)
    {
        return true;
    }
    virtual bool inputInit(CString *)
    {
        return true;
    }
    virtual bool inputPostInit(CString *)
    {
        return true;
    }
    virtual void inputTerminate()
    {
    }
    virtual void eventHandleD(const CDEvent &e, c_cptr data)
    {
    }
    virtual void eventHandleI(const CIEvent &e, c_cptr data)
    {
    }
    virtual void eventHandle(const CIHapticEvent &haptic, c_cptr data)
    {
    }
    virtual void eventHandle(const CIEvent &event, c_cptr data)
    {
    }
    virtual void eventHandle(const CDEvent &event, c_cptr data)
    {
    }
    virtual CIControllerState getControllerState(uint16 index)
    {
        return {};
    }
    virtual bool isMouseGrabbed() const
    {
        return false;
    }
    virtual void setMouseGrabbing(bool m)
    {
    }
    virtual bool relativeMouse() const
    {
        return false;
    }
    virtual void setRelativeMouse(bool enable)
    {
    }
    virtual CPoint mousePosition() const
    {
        return {};
    }
    virtual void setMousePosition(const CPoint &p)
    {
    }
    virtual void setKeyboardRepeat(bool m)
    {
    }
    virtual bool textInputMode() const
    {
        return false;
    }
    virtual void setTextInputMode(bool m)
    {
    }

    // WindowApplication interface
public:
    virtual CDMonitor monitor()
    {
        return {};
    }
    virtual bool showWindow()
    {
        return false;
    }
    virtual bool hideWindow()
    {
        return false;
    }
    virtual bool closeWindow()
    {
        return false;
    }
    virtual void popErrorMessage(Severity s, cstring title, cstring msg)
    {
    }

protected:
    virtual bool windowPreInit(const CDProperties &, CString *)
    {
        return true;
    }
    virtual bool windowInit(const CDProperties &, CString *)
    {
        return true;
    }
    virtual bool windowPostInit(const CDProperties &, CString *)
    {
        return true;
    }
    virtual void windowTerminate()
    {
    }

    // EventApplication interface
public:
    virtual void run()
    {
    }
    virtual void injectEvent(const CIEvent &, c_cptr)
    {
    }
    virtual void injectEvent(const CDEvent &, c_cptr)
    {
    }

    // WindowManagerClient interface
public:
    virtual CDWindow *window()
    {
        return nullptr;
    }
    virtual uint32 windowState() const
    {
        return 0;
    }
    virtual void setWindowState(const CDProperties::State &s)
    {
    }
    virtual CSize windowSize() const;
    virtual void setWindowSize(const CSize &s)
    {
    }
    virtual CPoint windowPosition() const
    {
        return {};
    }
    virtual void setWindowPosition(const CPoint &p)
    {
    }
    virtual CString windowTitle() const
    {
        return {};
    }
    virtual void setWindowTitle(const CString &tl)
    {
    }
    virtual bool setWindowIcon(CBitmap &icon)
    {
        return false;
    }
    virtual bool screensaverMode()
    {
        return false;
    }
    virtual void setScreensaverMode(bool m)
    {
    }
    virtual CString windowLibrary() const
    {
        return "Android Native Activity";
    }
};

}
}

#endif
