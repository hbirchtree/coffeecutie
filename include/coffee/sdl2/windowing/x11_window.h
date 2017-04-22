#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#if defined(COFFEE_USE_MAEMO_X11)

namespace Coffee{
namespace Display{

struct X11_Data;

class X11Window :
        public WindowManagerClient,
        private virtual EventApplication
{
    UqPtr<X11_Data> m_xData;
    // WindowApplication interface
public:
    X11Window();
    virtual ~X11Window();

    CDMonitor monitor();
    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    void popErrorMessage(Severity s, cstring title, cstring msg);

protected:
    bool windowPreInit(const CDProperties &, CString *);
    bool windowInit(const CDProperties &props, CString * err);
    bool windowPostInit(const CDProperties &, CString *);
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

public:
    void processX11Events(InputApplication *eh);
};

}
}
#endif
