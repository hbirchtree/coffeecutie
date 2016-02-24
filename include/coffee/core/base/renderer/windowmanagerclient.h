#pragma once

#include "windowapplication.h"

namespace Coffee{
namespace CDisplay{

class WindowManagerClient : public WindowApplication
{
public:

    /* Properties */

    /*!
     * \brief Get window information, including window pointer, title, size, position.
     * \return Struct containing window information
     */
    virtual CDWindow* window() = 0;

    /* Appearance */

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
     * \brief Set the window's icon for display in window manager and etc.
     * \param icon The bitmap which is to be set. Not using const& because some surface creation methods require this.
     * \return False on failure
     */
    virtual bool setWindowIcon(CBitmap& icon) = 0;

    /* Screensaver mode */

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
};

}
}