#pragma once

#include "../input/cinput.h"
#include "../types/cdisplay.h"
#include <peripherals/libc/types.h>

namespace Coffee{

using namespace CInput;

namespace Display{

class InputApplication
{
public:
    virtual ~InputApplication() {}

    virtual bool inputPreInit(CString*) = 0;
    virtual bool inputInit(CString*) = 0;
    virtual bool inputPostInit(CString*) = 0;
    virtual void inputTerminate() = 0;

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

    virtual void eventHandle(const CIHapticEvent& haptic, c_cptr data) = 0;
    virtual void eventHandle(const CIEvent &event, c_cptr data) = 0;
    virtual void eventHandle(const CDEvent &event, c_cptr data) = 0;

    /*!
     * \brief Alternative way of querying controls, will grab all state.
     *  Requires more calls than event-based polling.
     * \param index
     * \return
     */
    virtual CIControllerState getControllerState(uint16 index) = 0;

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

    virtual bool relativeMouse() const = 0;
    virtual void setRelativeMouse(bool enable) = 0;

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

}
}
