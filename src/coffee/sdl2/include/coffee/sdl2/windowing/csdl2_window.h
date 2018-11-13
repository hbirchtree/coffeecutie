#pragma once

#include <coffee/core/base/renderer/windowmanagerclient.h>

#if defined(COFFEE_USE_SDL_WINDOW)
#include "../csdl2_context.h"

namespace Coffee{
namespace Display{

class SDL2Window :
        public virtual WindowManagerClient,
        public virtual SDL2ContextUser
{
public:
    virtual ~SDL2Window() {}
    virtual CDMonitor monitor();

    virtual bool showWindow();
    virtual bool hideWindow();
    virtual bool closeWindow();

    virtual void popErrorMessage(Severity s, cstring title, cstring msg);

protected:
    virtual bool windowPreInit(const CDProperties &p, CString *err);
    virtual bool windowInit(const CDProperties& p, CString *err);
    virtual bool windowPostInit(const CDProperties& p, CString *err);
    virtual void windowTerminate();

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
};

}
}
#endif
