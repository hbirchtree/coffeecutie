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

void NDKWindow::eventHandleD(const Event &e, c_cptr data)
{

}

void NDKWindow::eventHandleI(const Input::CIEvent &e, c_cptr data)
{

}

void NDKWindow::eventHandle(const Input::CIHapticEvent &haptic, c_cptr data)
{

}

void NDKWindow::eventHandle(const Input::CIEvent &event, c_cptr data)
{

}

void NDKWindow::eventHandle(const Event &event, c_cptr data)
{

}

Input::CIControllerState NDKWindow::getControllerState(u16 index)
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

Point NDKWindow::mousePosition() const
{
    return {};
}

void NDKWindow::setMousePosition(const Point &p)
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

Monitor NDKWindow::monitor()
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

void NDKWindow::popErrorMessage(debug::Severity s, cstring title, cstring msg)
{
}

bool NDKWindow::windowPreInit(const Properties &, CString *)
{
    return true;
}

bool NDKWindow::windowInit(const Properties &, CString *)
{
    return true;
}

bool NDKWindow::windowPostInit(const Properties &, CString *)
{
    return true;
}

void NDKWindow::windowTerminate()
{
}

void NDKWindow::injectEvent(const Input::CIEvent &ev, c_cptr data)
{
    for(auto eh : m_eventhandlers_input)
        eh.func(eh.user_ptr, ev, data);
}

void NDKWindow::injectEvent(const Event &ev, c_cptr data)
{
    for(auto eh : m_eventhandlers_windw)
        eh.func(eh.user_ptr, ev, data);
}

Window *NDKWindow::window()
{
    return nullptr;
}

u32 NDKWindow::windowState() const
{
    return 0;
}

void NDKWindow::setWindowState(const Properties::State &s)
{
}

Size NDKWindow::windowSize() const
{
    int winSize[2] = {};

    CoffeeForeignSignalHandleNA(CoffeeForeign_GetWinSize,
                                winSize, nullptr, nullptr);

    return {C_FCAST<u32>(winSize[0]), C_FCAST<u32>(winSize[1])};
}

void NDKWindow::setWindowSize(const Size &s)
{
}

Point NDKWindow::windowPosition() const
{
    return {};
}

void NDKWindow::setWindowPosition(const Point &p)
{
}

CString NDKWindow::windowTitle() const
{
    return {};
}

void NDKWindow::setWindowTitle(const CString &tl)
{
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
