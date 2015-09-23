#ifndef COFFEE_CDISPLAY_CGLFWRENDERER_H
#define COFFEE_CDISPLAY_CGLFWRENDERER_H

#include "coffee_impl/functional/cqueuerunner.h"
#include "coffee/cdebug.h"
#include <mutex>

namespace Coffee {
namespace CDisplay {

using namespace CGraphicsWrappers;

struct CGLFWContext;

class CGLFWRenderer : public CDQueueRendererBase
{
public:
    CGLFWRenderer(CObject* parent);
    virtual ~CGLFWRenderer();

    virtual thread_id contextThread();

    CString windowTitle() const;
    void setWindowTitle(const CString &title);

    CDMonitor monitor();
    CDWindow window();
    CDContextBits context();

    uint32 windowState() const;
    void setWindowState(uint32 newstate);

    bool showWindow();
    bool hideWindow();
    bool closeWindow();

    int swapInterval();
    void setSwapInterval(int interval);

    CSize framebufferSize() const;

    CSize windowSize() const;
    void setWindowSize(const CSize &size);

    //Main thread only
    CPoint windowPosition() const;
    void setWindowPosition(CPoint);

    double contextTime() const;

    bool isMouseGrabbed() const;
    void setMouseGrabbing(bool grab);

    bool closeFlag();

    virtual bool requestGLExtension(cstring extension);

    void swapBuffers();
    void pollEvents();

    void init(const CDWindowProperties &props);
    void cleanup();

protected:
    virtual void updateJoysticks();
    CDWindowProperties m_properties;
    CString m_contextString;

private:
    void _glfw_init(const CDWindowProperties &props);

    std::mutex m_initMutex;
    std::thread::id m_contextThread;
    CGLFWContext* m_ctxt = nullptr;

};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CGLFWRENDERER_H
