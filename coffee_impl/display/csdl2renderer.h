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
    CSDL2Renderer(CObject* parent);
    virtual ~CSDL2Renderer();

    // CDRendererBase interface
public:
    void init(const CDWindowProperties &);
    void cleanup();

    CString windowTitle() const;
    void setWindowTitle(const CString &title);

    double contextTime();

    CDMonitor monitor();
    CDWindow window();
    CDContextBits context();

    uint32_t windowState() const;
    void setWindowState(uint32_t state);

    CPoint windowPosition() const;
    void setWindowPosition(CPoint pos);

    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    bool closeFlag();

    int swapInterval();
    void setSwapInterval(int interval);

    CSize framebufferSize() const;
    CSize windowSize() const;
    void setWindowSize(const CSize &size);

    bool isMouseGrabbed() const;
    void setMouseGrabbing(bool grab);

    void swapBuffers();
    void pollEvents();

    bool requestGLExtension(cstring);

protected:
    CDWindowProperties m_properties;
    CString m_contextString;
private:
    CSDL2Types::CSDL2Context* m_context = nullptr;
};

}
}

#endif // CSDL2RENDERER_H
