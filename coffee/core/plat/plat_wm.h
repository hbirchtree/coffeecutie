#ifndef COFFEE_PLAT_WM
#define COFFEE_PLAT_WM

#include "coffee/core/CTypes"

namespace Coffee{
namespace CDisplay{

struct CDWindowBitmap
{
    CSize resolution;
    CRGBA *pixels;
};

struct CDWindow;
/*!
 * \brief Enable or disable window decoration
 * \param window
 * \param enabled
 */
extern void coffee_window_set_decorated(CDWindow* window, bool enabled);
/*!
 * \brief Set window to be on top
 * \param window
 * \param enabled
 */
extern void coffee_window_set_on_top(CDWindow* window, bool enabled);
/*!
 * \brief Set window icon
 * \param window
 * \param icon
 */
extern void coffee_window_set_icon(CDWindow* window, const CDWindowBitmap& icon);
/*!
 * \brief Send a program notification
 * \param window
 */
extern void coffee_window_notify(CDWindow* window);
/*!
 * \brief Send a system notification, notify-osd on Linux
 * \param window
 * \param msg
 */
extern void coffee_windowmanager_notification(CDWindow* window, cstring msg);

}
}

#endif //COFFEE_PLAT_WM
