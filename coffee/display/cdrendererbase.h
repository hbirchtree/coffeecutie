#ifndef COFFEE_CDISPLAY_CDRENDERERBASE_H
#define COFFEE_CDISPLAY_CDRENDERERBASE_H

#include "coffee.h"
#include "coffee/cobject.h"
#include "coffee/cdisplay.h"
#include "coffee_impl/functional/cqueuerunner.h"

using namespace Coffee::CInput;

namespace Coffee {
namespace CDisplay {

class CDRendererBase : public CObject
{
    //This class and its inherited classes should remain stateless

public:
    enum WindowState{
        FullScreen          = 0b001,
        WindowedFullScreen  = 0b010,
        Windowed            = 0b100,

        Minimized           = 0b01000,
        Maximized           = 0b10000,

        Focused             = 0b0100000,
        Resizable           = 0b1000000,

        Decorated           = 0b00010000000,
        Floating            = 0b00100000000,
        Visible             = 0b01000000000,
    };

    enum RendererExitStatus{ //Accumulated to describe exit state
        StartSucess         = 0b00001,
        ExitSucess          = 0b00010,

        InitSucess          = 0b00100,
        LoopingSucess       = 0b01000,
        CleanupSucess       = 0b10000,
    };

    CDRendererBase(CObject* parent);

    virtual void init(WindowState,CSize,int)    = 0; //Initializes the context manager and etc.
    virtual void run()                          = 0;
    virtual void cleanup()                      = 0;

    //Window-related
    virtual CString windowTitle() const               = 0;
    virtual void setWindowTitle(const CString &title) = 0;

    virtual CDMonitor monitor()         = 0; //If it is a window, return the monitor it is on
    virtual CDWindow window()           = 0; //Describes the window
    virtual CDContextBits context()     = 0; //Stencil bits, depth bits etc.

    virtual uint32_t windowState() const           = 0;
    virtual void setWindowState(uint32_t newstate) = 0;

    virtual bool showWindow()  = 0; //bool: whether it succeeded or not, can be a static value
    virtual bool hideWindow()  = 0;
    virtual bool closeWindow() = 0;

    virtual int swapInterval()          = 0;
    virtual void setSwapInterval(int)   = 0;

    virtual bool closeFlag() = 0;

    //Framebuffer size of context
    virtual CSize framebufferSize() const               = 0;

    virtual CSize windowSize() const                    = 0;
    virtual void setWindowSize(const CSize& size)       = 0;

    //Mouse controls, managed by context
    virtual bool isMouseGrabbed() const      = 0;
    virtual void setMouseGrabbing(bool grab) = 0;

    //Render loop flow
    virtual void swapBuffers() = 0;
    virtual void pollEvents()  = 0;

    virtual void eventHandle(CDEvent* event) = 0;
};

class CDQueueRendererBase : public CDRendererBase
{
public:
    virtual ~CDQueueRendererBase();
    virtual void queueFunction(QueueFunction func);

protected:
    CDQueueRendererBase(CObject* parent);
    CQueueRunner* m_queueRunner;
    void executeRunQueue();
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERERBASE_H
