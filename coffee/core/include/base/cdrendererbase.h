#ifndef COFFEE_CDISPLAY_CDRENDERERBASE_H
#define COFFEE_CDISPLAY_CDRENDERERBASE_H

#include <coffee.h>
#include <base/cobject.h>
#include <base/cdisplay.h>
#include <base/cinput.h>

namespace Coffee {

using namespace CInput;

namespace CDisplay {


/*!
 * \brief The base class for creating a rendering context. It will handle window events, input events and creating a GL context for the user. The user will then work on top of this stack with Coffee-defined events and classes.
 */
class CDRendererBase : public CObject
{
public:
    /*!
     * \brief Verbosity level for messages
     */
    enum VerbosityLevels{
        PrintInitializationMessages     = 0x1, /*!< Prints init messages from binding, context and more*/
        PrintExtraData                  = 0x2,
    };

    /*!
     * \brief Describes program exit state
     */
    enum RendererExitStatus{
        StartSucess         = 0x01, /*!< Successfully started*/
        ExitSucess          = 0x02, /*!< Successfully exited*/

        InitSucess          = 0x04, /*!< Successfully initialized*/
        LoopingSucess       = 0x08, /*!< Successfully looped*/
        CleanupSucess       = 0x10, /*!< Successfully cleaned up*/
    };

    virtual ~CDRendererBase();

    /*!
     * \brief Get thread ID on which the GL context is current
     * \return
     */
    virtual std::thread::id contextThread() = 0;

    /*!
     * \brief Initialize context manager with necessary details, should not be overridden by the user.
     * \param props Hints for creating the context
     */
    virtual void init(const CDProperties& props) = 0;
    /*!
     * \brief Function driving the render loop
     */
    virtual void run() = 0;
    /*!
     * \brief Function for running cleanup, should avoid memory leaks
     */
    virtual void cleanup() = 0;

    /*!
     * \brief Get current window title if possible
     * \return
     */
    virtual CString windowTitle() const = 0;
    /*!
     * \brief Set new window title
     * \param tl
     */
    virtual void setWindowTitle(const CString& tl) = 0;

    /*!
     * \brief Get monitor information, physical size, bits etc.
     * \return Struct containing monitor information
     */
    virtual CDMonitor monitor() = 0;
    /*!
     * \brief Get window information, including window pointer, title, size, position.
     * \return Struct containing window information
     */
    virtual CDWindow* window() = 0;
    /*!
     * \brief Get context bit information
     * \return Struct containing context bits
     */
    virtual CDContextBits context() = 0;

    /*!
     * \brief Get current window state
     * \return
     */
    virtual uint32 windowState() const = 0;
    /*!
     * \brief Set new window state according
     * \param s
     */
    virtual void setWindowState(const CDProperties::State &s) = 0;

    /*!
     * \brief Get current window position
     * \return
     */
    virtual CPoint windowPosition() const = 0;
    /*!
     * \brief Set new window position
     * \param p
     */
    virtual void setWindowPosition(const CPoint& p) = 0;

    /*!
     * \brief Show window
     * \return Whether it was successful or not
     */
    virtual bool showWindow() = 0;
    /*!
     * \brief Hide window
     * \return Whether it was successful or not
     */
    virtual bool hideWindow() = 0;
    /*!
     * \brief Close window
     * \return Whether it was successful or not
     */
    virtual bool closeWindow() = 0;

    /*!
     * \brief Get current swap interval
     * \return
     */
    virtual int  swapInterval() const = 0;
    /*!
     * \brief Set swap interval
     * \param i
     */
    virtual void setSwapInterval(const int &i) = 0;

    /*!
     * \brief A flag used to determine when a render loop should terminate
     * \return Whether or not a render loop should terminate
     */
    virtual bool closeFlag() = 0; //Used to determine when the loop should terminate

    /*!
     * \brief Get current screensaver mode
     * \return True if screensaver is enabled
     */
    virtual bool screensaverMode() = 0;
    /*!
     * \brief Set new screensaver mode
     * \param m
     */
    virtual void setScreensaverMode(bool m) = 0;

    /*!
     * \brief Get current framebuffer size
     * \return
     */
    virtual CSize framebufferSize() const = 0;

    /*!
     * \brief Get current window size
     * \return
     */
    virtual CSize windowSize() const = 0;
    /*!
     * \brief Set new window size
     * \param s
     */
    virtual void  setWindowSize(const CSize& s) = 0;

    /*!
     * \brief Get current mouse grabbing state
     * \return
     */
    virtual bool isMouseGrabbed() const = 0;
    /*!
     * \brief Set new mouse grabbing state
     * \param m
     */
    virtual void setMouseGrabbing(bool m) = 0;

    /*!
     * \brief Get current mouse position
     * \return
     */
    virtual CPoint mousePosition() const = 0;
    /*!
     * \brief Set new mouse position
     * \param p
     */
    virtual void setMousePosition(const CPoint& p) = 0;

    /*!
     * \brief Get current text input mode
     * \return
     */
    virtual bool textInputMode() const = 0;
    /*!
     * \brief Set new text input mode
     * \param m
     */
    virtual void setTextInputMode(bool m) = 0;

    /*!
     * \brief Keyboard repeat rate determines the interval between pressing a key and it sending repeated events.
     * \param m True for enabling, false for disabling
     */
    virtual void setKeyboardRepeat(bool m) = 0;

    /*!
     * \brief Function used for swapping buffers in render loop
     */
    virtual void swapBuffers() = 0;
    /*!
     * \brief Function used for polling events
     */
    virtual void pollEvents() = 0;

    /*!
     * \brief Function for handling window events, implemented by the user.
     * \param e
     */
    virtual void eventHandleD(const CDEvent& e, c_cptr data) = 0;
    /*!
     * \brief Function for handling input events, implemented by the user.
     * \param e
     */
    virtual void eventHandleI(const CIEvent& e, c_cptr data) = 0;
    /*!
     * \brief Function for handling haptic events. A user will call this with a constructed event e and have it play in a fire-and-forget.
     * \param e
     */
    virtual void eventHandleH(const CIHapticEvent& e, c_cptr data) = 0;

    /*!
     * \brief Callback for GL errors and messages, implemented by the user.
     * \param d A pointer to the system's OpenGL report format, implementation-specific.
     */
    virtual void bindingCallback(const void*) const = 0;
    /*!
     * \brief Function called to initialize binding library like glbinding, called before context is created
     */
    virtual void bindingPreInit() = 0;
    /*!
     * \brief Function called to initialize binding library like glbinding, called after context is created
     */
    virtual void bindingPostInit() = 0;
    /*!
     * \brief Function called to terminate binding library like glbinding
     */
    virtual void bindingTerminate() = 0;

    /*!
     * \brief Get current context time, can be returned by the context or the system
     * \return Current context time
     */
    virtual double contextTime() = 0;

    /*!
     * \brief General function used when an error occurs and there is a need to notify the user.
     * \param s Severity of message, may change its looks
     * \param title Title of the message box
     * \param msg Messaing in the message box
     */
    virtual void popErrorMessage(Severity s, cstring title, cstring msg) = 0;

    /*!
     * \brief Alternative way of querying controls, will grab all state. Requires more calls than event-based polling.
     * \param index
     * \return
     */
    virtual CIControllerState getControllerState(uint16 index) = 0;

    /*!
     * \brief Set the window's icon for display in window manager and etc.
     * \param icon The bitmap which is to be set. Not using const& because some surface creation methods require this.
     * \return False on failure
     */
    virtual bool setWindowIcon(CBitmap& icon) = 0;

protected:
    CDRendererBase(CObject* parent);
};

inline std::future<void> coffee_display_start_async(
        std::atomic_bool *startAtomic,
        CDRendererBase* obj,
        CDProperties const& properties)
{
    std::function<void()> display_task = [=](){
        obj->init(properties);
        startAtomic->store(true);
        obj->run();
        obj->cleanup();
    };
    return CFunctional::CThreading::RunAsync(display_task);
}

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERERBASE_H
