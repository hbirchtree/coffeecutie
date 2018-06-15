#pragma once

#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/base/renderer/glapplication.h>
#include <coffee/core/base/renderer/glloader.h>
#include <coffee/core/base/renderer/windowapplication.h>
#include <coffee/core/base/renderer/inputapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>
#include <coffee/core/base/renderer/eventapplication.h>

namespace Coffee {
namespace Display {

class StubWindowManagerClient :
        public virtual WindowManagerClient
{
public:
    virtual ~StubWindowManagerClient();

public:
    virtual CDMonitor monitor();
    virtual bool showWindow();
    virtual bool hideWindow();
    virtual bool closeWindow();
    virtual void popErrorMessage(Severity, cstring, cstring);

protected:
    virtual bool windowPreInit(const CDProperties &, CString *);
    virtual bool windowInit(const CDProperties &, CString *);
    virtual bool windowPostInit(const CDProperties &, CString *);
    virtual void windowTerminate();

public:
    virtual CDWindow *window();
    virtual uint32 windowState() const;
    virtual void setWindowState(const CDProperties::State &);
    virtual CSize windowSize() const;
    virtual void setWindowSize(const CSize &);
    virtual CPoint windowPosition() const;
    virtual void setWindowPosition(const CPoint &);
    virtual CString windowTitle() const;
    virtual void setWindowTitle(const CString &);
    virtual bool screensaverMode();
    virtual void setScreensaverMode(bool);
};

class StubGLApplication :
        public virtual GLApplication
{
public:
    virtual ~StubGLApplication();

public:
    virtual bool contextPreInit(const GLProperties&, CString*);
    virtual bool contextInit(const GLProperties&, CString*);
    virtual bool contextPostInit(const GLProperties&, CString*);
    virtual void contextTerminate();

    virtual void swapBuffers();
    virtual CSize framebufferSize() const;
    virtual int swapInterval() const;
    virtual void setSwapInterval(const int &);
    virtual CDContextBits context();
    virtual ThreadId contextThread();
    virtual CGL::CGL_Context *glContext();
    virtual CGL::CGL_ScopedContext scopedContext();
};

class StubGLLoader :
        public virtual GLLoader
{
    virtual void bindingCallback(const void*) const;

public:
    StubGLLoader();
    virtual ~StubGLLoader();

    virtual bool bindingPreInit(const GLProperties&, CString*);
    virtual bool bindingInit(const GLProperties&, CString*);
    virtual bool bindingPostInit(const GLProperties&, CString*);
    virtual void bindingTerminate();
};

class StubEventApplication :
        public virtual EventApplication
{
public:
    virtual ~StubEventApplication();

    virtual bool inputPreInit(CString *) {return true;}
    virtual bool inputInit(CString *) {return true;}
    virtual bool inputPostInit(CString *) {return true;}
    virtual void inputTerminate() {}

    virtual void eventHandleD(const CDEvent &, c_cptr) {}
    virtual void eventHandleI(const CIEvent &, c_cptr){}
    virtual void eventHandle(const CIHapticEvent &, c_cptr) {}
    virtual void eventHandle(const CIEvent &, c_cptr) { }
    virtual void eventHandle(const CDEvent &, c_cptr) { }
    virtual CIControllerState getControllerState(uint16) { return {}; }

    virtual bool isMouseGrabbed() const {return false;}
    virtual void setMouseGrabbing(bool) {}
    virtual bool relativeMouse() const {return false;}
    virtual void setRelativeMouse(bool) { }
    virtual CPoint mousePosition() const {return {};}
    virtual void setMousePosition(const CPoint &) {}
    virtual void setKeyboardRepeat(bool){}
    virtual bool textInputMode() const {return false;}
    virtual void setTextInputMode(bool) {}

    virtual void injectEvent(const CIEvent &, c_cptr) {}
    virtual void injectEvent(const CDEvent &, c_cptr) {}
};

}
}
