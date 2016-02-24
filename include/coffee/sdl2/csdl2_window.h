#pragma once

#include "csdl2_context.h"
#include <coffee/core/base/renderer/windowmanagerclient.h>

namespace Coffee{
namespace CDisplay{

class SDL2Window :
        public WindowManagerClient,
        public virtual SDL2ContextUser
{
public:
    CDMonitor monitor();

    bool showWindow();
    bool hideWindow();
    bool closeWindow();

    void popErrorMessage(Severity s, cstring title, cstring msg);

protected:
    void windowPreInit(const CDProperties &p);
    void windowInit(const CDProperties& p);
    void windowPostInit(const CDProperties& p);
    void windowTerminate();

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

    bool setWindowIcon(CBitmap &icon);

    bool screensaverMode();
    void setScreensaverMode(bool m);
};

}
}