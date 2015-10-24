#ifndef CSDL2RENDERER_H
#define CSDL2RENDERER_H

#include "coffee_impl/functional/cqueuerunner.h"
#include "coffee/cdebug.h"
#include <mutex>

namespace Coffee{
namespace CSDL2Types{
    struct CSDL2Context;
}
namespace CDisplay{

class CSDL2Renderer : public CDQueueRendererBase
{
public:
    virtual ~CSDL2Renderer();

    virtual std::thread::id contextThread();

    void init(const CDWindowProperties &);
    void cleanup();

    CString windowTitle() const;
    void setWindowTitle(const CString &title);

    double contextTime();

    CDMonitor monitor();
    CDWindow* window();
    CDContextBits context();

    uint32 windowState() const;
    void setWindowState(uint32 state);

    bool screensaverMode();
    void setScreensaverMode(bool);

    CPoint windowPosition() const;
    void setWindowPosition(const CPoint &pos);

    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    bool closeFlag();

    int swapInterval() const;
    void setSwapInterval(int interval);

    CSize framebufferSize() const;
    CSize windowSize() const;
    void setWindowSize(const CSize &size);

    bool isMouseGrabbed() const;
    void setMouseGrabbing(bool grab);

    CPoint mousePosition() const;
    void setMousePosition(const CPoint& pos);

    bool relativeMouse() const;
    void setRelativeMouse(bool enable);

    bool textInputMode() const;
    void setTextInputMode(bool state);
    void setTextArea(const CRect& area);
    void setKeyboardRepeat(bool m);

    void eventHapticHandle(const CIHapticEvent* haptic);

    void swapBuffers();
    void pollEvents();

protected:
    CSDL2Renderer(CObject* parent);
    void _controllers_handle(const CIControllerAtomicUpdateEvent* ev);

    CDWindowProperties m_properties;
    CString m_contextString;
private:
    CSDL2Types::CSDL2Context* m_context = nullptr;
};

}
}

#endif // CSDL2RENDERER_H
