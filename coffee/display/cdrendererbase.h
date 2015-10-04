#ifndef COFFEE_CDISPLAY_CDRENDERERBASE_H
#define COFFEE_CDISPLAY_CDRENDERERBASE_H

#include "coffee.h"
#include "coffee/cobject.h"
#include "coffee/cdisplay.h"
#include "coffee/cinput.h"
#include "coffee/cfunctional.h"

using namespace Coffee::CInput;

namespace Coffee {

using namespace Coffee::CGraphicsWrappers;

namespace CDisplay {

class CDRendererBase : public CObject
{
    //This class and its inherited classes should remain stateless

public:
    enum VerbosityLevels{
        PrintInitializationMessages     = 0x1,
        PrintExtraData                  = 0x2,

    };

    enum RendererExitStatus{ //Accumulated to describe exit state
        StartSucess         = 0x01,
        ExitSucess          = 0x02,

        InitSucess          = 0x04,
        LoopingSucess       = 0x08,
        CleanupSucess       = 0x10,
    };

    virtual ~CDRendererBase();

    virtual std::thread::id contextThread() = 0;

    //Initialization
    virtual void init(const CDWindowProperties&) = 0;//Initializes the context manager and etc.
    virtual void run()                           = 0;
    virtual void cleanup()                       = 0;

    //Window-related
    virtual CString windowTitle() const         = 0;
    virtual void setWindowTitle(const CString&) = 0;

    virtual CDMonitor monitor()         = 0; //If it is a window, return the monitor it is on
    virtual CDWindow* window()          = 0; //Describes the window
    virtual CDContextBits context()     = 0; //Stencil bits, depth bits etc.

    virtual uint32 windowState() const          = 0;
    virtual void setWindowState(uint32)         = 0;

    virtual CPoint windowPosition() const = 0;
    virtual void setWindowPosition(const CPoint&) = 0;

    virtual bool showWindow()  = 0; //bool: whether it succeeded or not, can be a static value
    virtual bool hideWindow()  = 0;
    virtual bool closeWindow() = 0;

    virtual int  swapInterval() const    = 0;
    virtual void setSwapInterval(int)   = 0;

    virtual bool closeFlag() = 0; //Used to determine when the loop should terminate

    virtual bool screensaverMode() = 0;
    virtual void setScreensaverMode(bool) = 0;

    //Framebuffer size of context
    virtual CSize framebufferSize() const     = 0;

    virtual CSize windowSize() const          = 0;
    virtual void  setWindowSize(const CSize&) = 0;

    //Mouse controls, managed by context
    virtual bool isMouseGrabbed() const = 0;
    virtual void setMouseGrabbing(bool) = 0;

    virtual CPoint mousePosition() const                = 0;
    virtual void setMousePosition(const CPoint& pos)    = 0;

    virtual bool textInputMode() const = 0;
    virtual void setTextInputMode(bool) = 0;

    //Render loop flow
    virtual void swapBuffers() = 0;
    virtual void pollEvents()  = 0;

    //Event handling
    virtual void eventWHandle(const CDEvent*) = 0;
    virtual void eventIHandle(const CIEvent*) = 0;

    //OpenGL context functions, may be provided by binding or context manager
    virtual bool requestGLExtension(cstring)       = 0;
    virtual void bindingCallback(void*) const = 0;
    virtual void bindingPreInit()  = 0; //Called before context is created
    virtual void bindingPostInit() = 0; //Called after context is created

    virtual void bindingTerminate() = 0;

    virtual double contextTime() = 0;

protected:
    CDRendererBase(CObject* parent);
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERERBASE_H
