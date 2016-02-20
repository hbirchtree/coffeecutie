#ifndef CSDL2RENDERER_H
#define CSDL2RENDERER_H

#include <coffee/graphics_apis/gltypes.h>
#include <coffee/core/functional/cqueuerunner.h>
#include <coffee/core/display/cdialog_fun.h>

namespace Coffee{
namespace CSDL2Types{
    struct CSDL2Context;
}
namespace CDisplay{

struct SDL2Dialog : public DialogInterface
{
    static void ErrorMessage(cstring title,cstring message);
    static void WarningMessage(cstring title,cstring message);
    static void InformationMessage(cstring title,cstring message);
};

class CSDL2Renderer : public CDQueueRendererBase
{
public:
    virtual ~CSDL2Renderer();

    virtual std::thread::id contextThread();

    void init(const CDProperties &);
    void cleanup();

    CString windowTitle() const;
    void setWindowTitle(const CString &title);

    double contextTime();

    CDMonitor monitor();
    CDWindow* window();
    CDContextBits context();

    uint32 windowState() const;
    void setWindowState(const CDProperties::State &state);

    bool screensaverMode();
    void setScreensaverMode(bool);

    CPoint windowPosition() const;
    void setWindowPosition(const CPoint &pos);

    bool showWindow();
    bool hideWindow();
    bool closeWindow();
    bool closeFlag();

    int swapInterval() const;
    void setSwapInterval(const int &interval);

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

    void eventHandleH(const CIHapticEvent& haptic, c_cptr);
    void eventHandleI(const CIEvent &event, c_cptr data);
    void eventHandleD(const CDEvent &event, c_cptr data);

    CIControllerState getControllerState(uint16 index);

    void eventHandle(const CIHapticEvent& haptic, c_cptr data)
    {
        this->eventHandleH(haptic,data);
    }
    void eventHandle(const CIEvent &event, c_cptr data)
    {
        this->eventHandleI(event,data);
    }
    void eventHandle(const CDEvent &event, c_cptr data)
    {
        this->eventHandleD(event,data);
    }

    void swapBuffers();
    void pollEvents();

    void popErrorMessage(Severity s, cstring title, cstring msg);

    CGL::CGL_Context* glContext();
    CGL::CGL_ScopedContext scopedContext();

    bool setWindowIcon(CBitmap& icon);

protected:
    CSDL2Renderer(CObject* parent);
    /*!
     * \brief Takes care of opening, closing and remapping controllers as they come and go. This method is mandatory if you want to open controllers under the SDL2 context.
     * \param ev The atomic update event for this event
     */
    void _sdl2_controllers_handle(const CIControllerAtomicUpdateEvent* ev);

    CDProperties m_properties;
    CString m_contextString;
private:
    CSDL2Types::CSDL2Context* m_context = nullptr;
};

}
}

#endif // CSDL2RENDERER_H
