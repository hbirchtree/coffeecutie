#pragma once

#include <coffee/core/base/renderer/windowmanagerclient.h>

#if defined(COFFEE_USE_SDL_WINDOW)
#include "../csdl2_context.h"

namespace Coffee {
namespace Display {

class SDL2Window : public virtual WindowManagerClient,
                   public virtual SDL2ContextUser
{
  public:
    virtual ~SDL2Window()
    {
    }
    virtual Monitor monitor();

    virtual bool showWindow();
    virtual bool hideWindow();
    virtual bool closeWindow();

    virtual void popErrorMessage(debug::Severity s, cstring title, cstring msg);

  protected:
    virtual bool windowPreInit(const Properties& p, CString* err);
    virtual bool windowInit(const Properties& p, CString* err);
    virtual bool windowPostInit(const Properties& p, CString* err);
    virtual void windowTerminate();

  public:
    virtual Window* window();

    virtual u32  windowState() const;
    virtual void setWindowState(const Properties::State& s);

    virtual Size windowSize() const;
    virtual void setWindowSize(const Size& s);

    virtual Point windowPosition() const;
    virtual void  setWindowPosition(const Point& p);

    virtual CString windowTitle() const;
    virtual void    setWindowTitle(const CString& tl);

    virtual bool screensaverMode();
    virtual void setScreensaverMode(bool m);

    virtual CString windowLibrary() const;
};

} // namespace Display
} // namespace Coffee
#endif
