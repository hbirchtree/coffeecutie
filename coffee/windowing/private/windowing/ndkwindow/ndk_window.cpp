#include <coffee/windowing/windowing/ndkwindow/ndk_window.h>

#if defined(COFFEE_USE_ANDROID_NATIVEWIN)

#include <coffee/android/android_main.h>

namespace Coffee{
namespace Display{

bool NDKWindow::inputPreInit(CString *)
{
    return true;
}

bool NDKWindow::inputInit(CString *)
{
    return true;
}

bool NDKWindow::inputPostInit(CString *)
{
    return true;
}

void NDKWindow::inputTerminate()
{
}

void NDKWindow::eventHandleD(const CDEvent &e, c_cptr data)
{

}

void NDKWindow::eventHandleI(const CIEvent &e, c_cptr data)
{

}

void NDKWindow::eventHandle(const CIHapticEvent &haptic, c_cptr data)
{

}

void NDKWindow::eventHandle(const CIEvent &event, c_cptr data)
{

}

void NDKWindow::eventHandle(const CDEvent &event, c_cptr data)
{

}

CIControllerState NDKWindow::getControllerState(uint16 index)
{
    return {};
}

bool NDKWindow::isMouseGrabbed() const
{
    return false;
}

void NDKWindow::setMouseGrabbing(bool m)
{
}

bool NDKWindow::relativeMouse() const
{
    return false;
}

void NDKWindow::setRelativeMouse(bool enable)
{
}

CPoint NDKWindow::mousePosition() const
{
    return {};
}

void NDKWindow::setMousePosition(const CPoint &p)
{
}

void NDKWindow::setKeyboardRepeat(bool m)
{
}

bool NDKWindow::textInputMode() const
{
    return false;
}

void NDKWindow::setTextInputMode(bool m)
{
}

CDMonitor NDKWindow::monitor()
{
    return {};
}

bool NDKWindow::showWindow()
{
    return false;
}

bool NDKWindow::hideWindow()
{
    return false;
}

bool NDKWindow::closeWindow()
{
    return false;
}

void NDKWindow::popErrorMessage(Severity s, cstring title, cstring msg)
{
}

bool NDKWindow::windowPreInit(const CDProperties &, CString *)
{
    return true;
}

bool NDKWindow::windowInit(const CDProperties &, CString *)
{
    return true;
}

bool NDKWindow::windowPostInit(const CDProperties &, CString *)
{
    return true;
}

void NDKWindow::windowTerminate()
{
}

void NDKWindow::injectEvent(const CIEvent &, c_cptr)
{

}

void NDKWindow::injectEvent(const CDEvent &, c_cptr)
{

}

CDWindow *NDKWindow::window()
{
    return nullptr;
}

uint32 NDKWindow::windowState() const
{
    return 0;
}

void NDKWindow::setWindowState(const CDProperties::State &s)
{
}

CSize NDKWindow::windowSize() const
{
    int winSize[2] = {};

    CoffeeForeignSignalHandleNA(CoffeeForeign_GetWinSize,
                                winSize, nullptr, nullptr);

    return {winSize[0], winSize[1]};
}

void NDKWindow::setWindowSize(const CSize &s)
{
}

CPoint NDKWindow::windowPosition() const
{
    return {};
}

void NDKWindow::setWindowPosition(const CPoint &p)
{
}

CString NDKWindow::windowTitle() const
{
    return {};
}

void NDKWindow::setWindowTitle(const CString &tl)
{
}

bool NDKWindow::setWindowIcon(CBitmap &icon)
{
    return false;
}

bool NDKWindow::screensaverMode()
{
    return false;
}

void NDKWindow::setScreensaverMode(bool m)
{
}

CString NDKWindow::windowLibrary() const
{
    return "Android Native Activity";
}

void NDKWindow::run()
{
}

bool NDKWindow::installEventHandler(EventApplication::EventHandlerI h)
{
    m_eventhandlers_input.push_back(h);
    return true;
}

bool NDKWindow::installEventHandler(EventApplication::EventHandlerD h)
{
    m_eventhandlers_windw.push_back(h);
    return true;
}

Vector<EventApplication::EventHandlerI> *NDKWindow::getEventHandlersI()
{
    return &m_eventhandlers_input;
}

Vector<EventApplication::EventHandlerD> *NDKWindow::getEventHandlersD()
{
    return &m_eventhandlers_windw;
}

}
}
#endif