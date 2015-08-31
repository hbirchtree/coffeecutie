#ifndef COFFEE_CDISPLAY_CGLFWRENDERER_H
#define COFFEE_CDISPLAY_CGLFWRENDERER_H

#include "coffee_impl/graphics/cgraphicswrappers.h"
#include "coffee/display/cdrendererbase.h"
#include "coffee/cdebug.h"

#include "cglfwnativefuncs.h"

namespace Coffee {
namespace CDisplay {

class CGLFWRenderer : public CDRendererBase
{
public:
    CGLFWRenderer(CObject* parent);
    ~CGLFWRenderer();

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

    bool isMouseGrabbed() const;
    void setMouseGrabbing(bool grab);

    bool closeFlag();

    void swapBuffers();
    void pollEvents();

    void init(WindowState startState, CSize startSize, int monitorIndex);
    void cleanup();

    static void APIENTRY glbindingCallbackDirect(GLenum source, GLenum type, GLuint id, GLenum severity,
                                            GLsizei length, const GLchar* msg, const void* userPtr);

protected:
    virtual void updateJoysticks();
    virtual void glbindingCallbackInternal(GLenum source, GLenum type, GLuint id,
                                           GLenum severity, GLsizei length, const GLchar* msg) const = 0;

private:
    GLFWwindow* m_window = nullptr;

    CString m_rendererString;
    CString m_vendorString;
    CString m_versionString;
    CString m_contextString;

};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CGLFWRENDERER_H
