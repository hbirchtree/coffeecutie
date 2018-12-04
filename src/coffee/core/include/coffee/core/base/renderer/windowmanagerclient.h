#pragma once

#include "windowapplication.h"
#include <coffee/core/types/point.h>

namespace Coffee {
namespace Display {

class WindowManagerClient : public WindowApplication
{
  public:
    /* Properties */

    /*!
     * \brief Get window information, including window pointer, title, size,
     * position. \return Struct containing window information
     */
    virtual Window* window() = 0;

    /* Appearance */

    /*!
     * \brief Get current window state
     * \return
     */
    virtual u32 windowState() const = 0;
    /*!
     * \brief Set new window state according
     * \param s
     */
    virtual void setWindowState(const Properties::State& s) = 0;

    /*!
     * \brief Get current window size
     * \return
     */
    virtual Size windowSize() const = 0;
    /*!
     * \brief Set new window size
     * \param s
     */
    virtual void setWindowSize(const Size& s) = 0;

    /*!
     * \brief Get current window position
     * \return
     */
    virtual Point windowPosition() const = 0;
    /*!
     * \brief Set new window position
     * \param p
     */
    virtual void setWindowPosition(const Point& p) = 0;

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

    virtual CString windowLibrary() const = 0;
};

} // namespace Display
} // namespace Coffee