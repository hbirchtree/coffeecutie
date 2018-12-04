#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/display/event.h>
#include <coffee/core/types/input/event_types.h>

namespace Coffee {
namespace Display {

class InputApplication
{
  public:
    virtual ~InputApplication()
    {
    }

    virtual bool inputPreInit(CString*)  = 0;
    virtual bool inputInit(CString*)     = 0;
    virtual bool inputPostInit(CString*) = 0;
    virtual void inputTerminate()        = 0;

    /*!
     * \brief Function for handling window events, implemented by the user.
     * \param e
     */
    virtual void eventHandleD(const Event& e, c_cptr data) = 0;
    /*!
     * \brief Function for handling input events, implemented by the user.
     * \param e
     */
    virtual void eventHandleI(const Input::CIEvent& e, c_cptr data) = 0;

    virtual void eventHandle(
        const Input::CIHapticEvent& haptic, c_cptr data)               = 0;
    virtual void eventHandle(const Input::CIEvent& event, c_cptr data) = 0;
    virtual void eventHandle(const Event& event, c_cptr data)          = 0;

    /*!
     * \brief Alternative way of querying controls, will grab all state.
     *  Requires more calls than event-based polling.
     * \param index
     * \return
     */
    virtual Input::CIControllerState getControllerState(uint16 index) = 0;

    /* Mouse input */

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

    virtual bool relativeMouse() const         = 0;
    virtual void setRelativeMouse(bool enable) = 0;

    /*!
     * \brief Get current mouse position
     * \return
     */
    virtual Point mousePosition() const = 0;
    /*!
     * \brief Set new mouse position
     * \param p
     */
    virtual void setMousePosition(const Point& p) = 0;

    /* Keyboard input */

    /*!
     * \brief Keyboard repeat rate determines the interval
     *  between pressing a key and it sending repeated events.
     * \param m True for enabling, false for disabling
     */
    virtual void setKeyboardRepeat(bool m) = 0;
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
};

} // namespace Display
} // namespace Coffee
