#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/inputapplication.h>
#include <coffee/core/base/renderer/windowapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#if defined(FEATURE_ENABLE_CoffeeGLeamCommon)
#include <coffee/core/base/renderer/glapplication.h>
#include <coffee/core/base/renderer/glloader.h>
#endif

namespace Coffee {
namespace Display {

class StubWindowManagerClient : public virtual WindowManagerClient
{
  public:
    virtual ~StubWindowManagerClient();

  public:
    virtual Monitor monitor();
    virtual bool    showWindow();
    virtual bool    hideWindow();
    virtual bool    closeWindow();
    virtual void    popErrorMessage(debug::Severity, cstring, cstring);

  protected:
    virtual bool windowPreInit(const Properties&, CString*);
    virtual bool windowInit(const Properties&, CString*);
    virtual bool windowPostInit(const Properties&, CString*);
    virtual void windowTerminate();

  public:
    virtual Window* window();
    virtual u32     windowState() const;
    virtual void    setWindowState(const Properties::State&);
    virtual Size    windowSize() const;
    virtual void    setWindowSize(const Size&);
    virtual Point   windowPosition() const;
    virtual void    setWindowPosition(const Point&);
    virtual CString windowTitle() const;
    virtual void    setWindowTitle(const CString&);
    virtual bool    screensaverMode();
    virtual void    setScreensaverMode(bool);
};

#if defined(FEATURE_ENABLE_CoffeeGLeamCommon)
class StubGLApplication : public virtual GLApplication
{
  public:
    virtual ~StubGLApplication();

  public:
    virtual bool contextPreInit(const GL::Properties&, CString*);
    virtual bool contextInit(const GL::Properties&, CString*);
    virtual bool contextPostInit(const GL::Properties&, CString*);
    virtual void contextTerminate();

    virtual void                   swapBuffers();
    virtual Size                   framebufferSize() const;
    virtual int                    swapInterval() const;
    virtual void                   setSwapInterval(const int&);
    virtual ContextBits            context();
    virtual ThreadId               contextThread();
    virtual CGL::CGL_Context*      glContext();
    virtual CGL::CGL_ScopedContext scopedContext();
};

class StubGLLoader : public virtual GLLoader
{
    virtual void bindingCallback(const void*) const;

  public:
    StubGLLoader();
    virtual ~StubGLLoader();

    virtual bool bindingPreInit(const GL::Properties&, CString*);
    virtual bool bindingInit(const GL::Properties&, CString*);
    virtual bool bindingPostInit(const GL::Properties&, CString*);
    virtual void bindingTerminate();
};
#endif

class StubEventApplication : public virtual EventApplication
{
  public:
    virtual ~StubEventApplication();

    virtual bool inputPreInit(CString*)
    {
        return true;
    }
    virtual bool inputInit(CString*)
    {
        return true;
    }
    virtual bool inputPostInit(CString*)
    {
        return true;
    }
    virtual void inputTerminate()
    {
    }

    virtual void eventHandleD(const Event&, c_cptr)
    {
    }
    virtual void eventHandleI(const Input::CIEvent&, c_cptr)
    {
    }
    virtual void eventHandle(const Input::CIHapticEvent&, c_cptr)
    {
    }
    virtual void eventHandle(const Input::CIEvent&, c_cptr)
    {
    }
    virtual void eventHandle(const Event&, c_cptr)
    {
    }
    virtual Input::CIControllerState getControllerState(uint16)
    {
        return {};
    }

    virtual bool isMouseGrabbed() const
    {
        return false;
    }
    virtual void setMouseGrabbing(bool)
    {
    }
    virtual bool relativeMouse() const
    {
        return false;
    }
    virtual void setRelativeMouse(bool)
    {
    }
    virtual Point mousePosition() const
    {
        return {};
    }
    virtual void setMousePosition(const Point&)
    {
    }
    virtual void setKeyboardRepeat(bool)
    {
    }
    virtual bool textInputMode() const
    {
        return false;
    }
    virtual void setTextInputMode(bool)
    {
    }

    virtual void injectEvent(const Input::CIEvent&, c_cptr)
    {
    }
    virtual void injectEvent(const Event&, c_cptr)
    {
    }
};

} // namespace Display
} // namespace Coffee
