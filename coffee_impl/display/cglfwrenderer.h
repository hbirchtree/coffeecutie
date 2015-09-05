#ifndef COFFEE_CDISPLAY_CGLFWRENDERER_H
#define COFFEE_CDISPLAY_CGLFWRENDERER_H

#include "coffee/display/cdrendererbase.h"
#include "coffee/cdebug.h"

#include <mutex>

namespace Coffee {

namespace CGraphicsWrappers{
struct CGLReport;
}

namespace CDisplay {

using namespace CGraphicsWrappers;

struct CGLFWContext;

class CGLFWRenderer : public CDQueueRendererBase
{
public:
    CGLFWRenderer(CObject* parent);
    virtual ~CGLFWRenderer();

    CString windowTitle() const;
    void setWindowTitle(const CString &title);

    CDMonitor monitor();
    CDWindow window();
    CDContextBits context();

    uint32_t windowState() const;
    void setWindowState(uint32_t newstate);

    bool showWindow();
    bool hideWindow();
    bool closeWindow();

    int swapInterval();
    void setSwapInterval(int interval);

    CSize framebufferSize() const;

    CSize windowSize() const;
    void setWindowSize(const CSize &size);

    double contextTime() const;

    bool isMouseGrabbed() const;
    void setMouseGrabbing(bool grab);

    bool closeFlag();

    void swapBuffers();
    void pollEvents();

    void init(WindowState startState, CSize startSize, int monitorIndex);
    void cleanup();

    virtual void glbindingCallbackInternal(CGLReport* report) const = 0;

protected:
    virtual void updateJoysticks();

private:
    std::mutex m_initMutex;
    std::thread::id m_contextThread;
    CGLFWContext* m_ctxt = nullptr;

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
    CString m_contextString;

};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CGLFWRENDERER_H
