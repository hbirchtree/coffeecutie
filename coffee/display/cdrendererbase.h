#ifndef COFFEE_CDISPLAY_CDRENDERERBASE_H
#define COFFEE_CDISPLAY_CDRENDERERBASE_H

#include "coffee.h"
#include "coffee/cobject.h"
#include "coffee/cdisplay.h"

using namespace Coffee::CInput;

namespace Coffee {
namespace CDisplay {

class CDRendererBase : public CObject
{
public:
    enum WindowState{
        FullScreen          = 0b0000001,
        WindowedFullScreen  = 0b0000010,
        Windowed            = 0b0000100,

        Minimized           = 0b0001000,
        Maximized           = 0b0010000,
    };

    enum RendererExitStatus{ //Accumulated to describe exit state
        StartSucess         = 0b00001,
        ExitSucess          = 0b00010,

        InitSucess          = 0b00100,
        LoopingSucess       = 0b01000,
        CleanupSucess       = 0b10000,
    };

    CDRendererBase(CObject* parent);

    //Window-related
    virtual CString windowTitle() const               = 0;
    virtual void setWindowTitle(const CString &title) = 0;

    virtual CDMonitor monitor() = 0; //If it is a window, return the monitor it is on
    virtual CDWindow window()   = 0;

    virtual uint32_t windowState() const           = 0;
    virtual void setWindowState(uint32_t newstate) = 0;

    virtual bool showWindow()  = 0; //bool: whether it succeeded or not, can be a static value
    virtual bool hideWindow()  = 0;
    virtual bool closeWindow() = 0;

    virtual int swapInterval()     = 0;
    virtual void setSwapInterval() = 0;

    //Framebuffer size of context
    virtual CSize framebufferSize() const               = 0;
    virtual void setFramebufferSize(const CSize &size)  = 0;

    //Mouse controls, managed by context
    virtual bool isMouseGrabbed() const      = 0;
    virtual void setMouseGrabbing(bool grab) = 0;

    //Render loop flow
    virtual void swapBuffers() = 0;
    virtual void pollEvents()  = 0;
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERERBASE_H
