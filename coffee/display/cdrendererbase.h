#ifndef COFFEE_CDISPLAY_CDRENDERERBASE_H
#define COFFEE_CDISPLAY_CDRENDERERBASE_H

#include "coffee.h"
#include "coffee/cobject.h"
#include "coffee/cdisplay.h"
#include "coffee/cinput.h"

using namespace Coffee::CInput;

namespace Coffee {

using namespace Coffee::CGraphicsWrappers;

namespace CGraphicsWrappers{
struct CGLReport;
}

namespace CDisplay {

class CDRendererBase : public CObject
{
    //This class and its inherited classes should remain stateless

public:
    enum VerbosityLevels{
        PrintInitializationMessages     = 0b1,
        PrintExtraData                  = 0b10,

    };

    enum RendererExitStatus{ //Accumulated to describe exit state
        StartSucess         = 0b00001,
        ExitSucess          = 0b00010,

        InitSucess          = 0b00100,
        LoopingSucess       = 0b01000,
        CleanupSucess       = 0b10000,
    };

    CDRendererBase(CObject* parent);
    virtual ~CDRendererBase();

    //Initialization
    virtual void init(const CDWindowProperties&) = 0;//Initializes the context manager and etc.
    virtual void run()                           = 0;
    virtual void cleanup()                       = 0;

    //Window-related
    virtual CString windowTitle() const         = 0;
    virtual void setWindowTitle(const CString&) = 0;

    virtual CDMonitor monitor()         = 0; //If it is a window, return the monitor it is on
    virtual CDWindow window()           = 0; //Describes the window
    virtual CDContextBits context()     = 0; //Stencil bits, depth bits etc.

    virtual uint32_t windowState() const           = 0;
    virtual void     setWindowState(uint32_t)      = 0;

    virtual CPoint windowPosition() const = 0;
    virtual void setWindowPosition(CPoint) = 0;

    virtual bool showWindow()  = 0; //bool: whether it succeeded or not, can be a static value
    virtual bool hideWindow()  = 0;
    virtual bool closeWindow() = 0;

    virtual int  swapInterval()          = 0;
    virtual void setSwapInterval(int)   = 0;

    virtual bool closeFlag() = 0;

    //Framebuffer size of context
    virtual CSize framebufferSize() const     = 0;

    virtual CSize windowSize() const          = 0;
    virtual void  setWindowSize(const CSize&) = 0;

    //Mouse controls, managed by context
    virtual bool isMouseGrabbed() const = 0;
    virtual void setMouseGrabbing(bool) = 0;

    //Render loop flow
    virtual void swapBuffers() = 0;
    virtual void pollEvents()  = 0;

    //Event handling
    virtual void eventWHandle(CDEvent*) = 0;
    virtual void eventIHandle(CIEvent*) = 0;

    //OpenGL context functions, may be provided by binding or context manager
    virtual bool requestGLExtension(cstring)       = 0;
    virtual void bindingCallback(CGLReport*) const = 0;
    virtual void bindingPreInit()  = 0; //Called before context is created
    virtual void bindingPostInit() = 0; //Called after context is created

    virtual void bindingTerminate() = 0;
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERERBASE_H
