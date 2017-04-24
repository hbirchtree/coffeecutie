#pragma once

#include <coffee/core/base/renderer/windowmanagerclient.h>
#include <coffee/core/base/renderer/eventapplication.h>

#if defined(COFFEE_RASPBERRY_DMX)
namespace Coffee{
namespace Display{

struct DISPMAN_Data;

class DispmanXWindow :
        public WindowManagerClient,
        public virtual EventApplication
{
    DISPMAN_Data* m_dmData;

    // WindowApplication interface
public:
    CDMonitor monitor();
    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    void popErrorMessage(Severity s, cstring title, cstring msg);

protected:
    bool windowPreInit(const CDProperties & props, CString *err);
    bool windowInit(const CDProperties & props, CString *err);
    bool windowPostInit(const CDProperties & props, CString *err);
    void windowTerminate();

    // WindowManagerClient interface
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
    CString windowLibrary() const;
};

}
}
#endif
