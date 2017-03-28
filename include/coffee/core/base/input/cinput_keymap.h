#pragma once

#include "cinput_latin1_keymap.h"

namespace Coffee{
namespace CInput{

/*!
 * \brief Xbox 360 controller mapping of buttons. Same layout as SDL2's GameController, but we do not expose all parts of SDL2.
 */
enum CIControllerButtonMapping : uint16
{
    CK_BUTTON_A         = 0x200, /*!< A button*/
    CK_BUTTON_B         = 0x201, /*!< B button*/
    CK_BUTTON_X         = 0x202, /*!< X button*/
    CK_BUTTON_Y         = 0x203, /*!< Y button*/

    CK_BUTTON_BACK      = 0x204, /*!< Back button*/
    CK_BUTTON_GUIDE     = 0x205, /*!< Guide button, also known as the Xbox button*/
    CK_BUTTON_START     = 0x206, /*!< Start button*/

    CK_BUTTON_LS        = 0x207, /*!< Left stick button*/
    CK_BUTTON_RS        = 0x208, /*!< Right stick button*/

    CK_BUTTON_LB        = 0x209, /*!< Left bumper*/
    CK_BUTTON_RB        = 0x20A, /*!< Right bumper*/

    CK_BUTTON_POV_UP    = 0x20B, /*!< Also known as D-Pad up*/
    CK_BUTTON_POV_DOWN  = 0x20C, /*!< Also known as D-Pad down*/
    CK_BUTTON_POV_LEFT  = 0x20D, /*!< Also known as D-Pad left*/
    CK_BUTTON_POV_RIGHT = 0x20E, /*!< Also known as D-Pad right*/
};

/*!
 * \brief Xbox 360 controller mapping of axes. Same mapping as SDL2's GameController.
 */
enum CIControllerAxisMapping : uint16
{
    CK_AXIS_LEFT_X          = 0x210, /*!< Left stick X-axis*/
    CK_AXIS_LEFT_Y          = 0x211, /*!< Left stick Y-axis*/

    CK_AXIS_RIGHT_X         = 0x212, /*!< Right stick X-axis*/
    CK_AXIS_RIGHT_Y         = 0x213, /*!< Right stick Y-axis*/

    CK_AXIS_TRIGGER_LEFT    = 0x214, /*!< Left trigger*/
    CK_AXIS_TRIGGER_RIGHT   = 0x215, /*!< Right trigger*/
};

/*!
 * \brief Special key mapping for use with input functions
 */
enum CISpecialKeyMapping : uint16
{
    CK_F1               = 0x100,
    CK_F2               = 0x101,
    CK_F3               = 0x102,
    CK_F4               = 0x103,
    CK_F5               = 0x104,
    CK_F6               = 0x105,
    CK_F7               = 0x106,
    CK_F8               = 0x107,
    CK_F9               = 0x108,
    CK_F10              = 0x109,
    CK_F11              = 0x10A,
    CK_F12              = 0x10B,

    /* Reserving range 0x10C-0x10F */

    CK_KP_0             = 0x110,
    CK_KP_1             = 0x111,
    CK_KP_2             = 0x112,
    CK_KP_3             = 0x113,
    CK_KP_4             = 0x114,
    CK_KP_5             = 0x115,
    CK_KP_6             = 0x116,
    CK_KP_7             = 0x117,
    CK_KP_8             = 0x118,
    CK_KP_9             = 0x119,
    CK_KP_Enter         = 0x11A,
    CK_KP_Mul           = 0x11B,
    CK_KP_Sub           = 0x11C,
    CK_KP_Add           = 0x11D,
    CK_KP_Div           = 0x11E,
    CK_KP_Comma         = 0x11F,

    CK_Insert           = 0x120,
    CK_Home             = 0x121,
    CK_End              = 0x122,
    CK_PgUp             = 0x123,
    CK_PgDn             = 0x124,
    CK_PrntScrn         = 0x125,
    CK_Pause            = 0x126,

    CK_LCtrl            = 0x127,
    CK_LShift           = 0x128,
    CK_LAlt             = 0x129,
    CK_LSuper           = 0x12A,
    CK_CapsLock         = 0x12B,
    CK_NumLock          = 0x12C,
    CK_RCtrl            = 0x12D,
    CK_RShift           = 0x12E,
    CK_RSuper           = 0x12F,

    CK_Left             = 0x130, /*!< Arrow key left*/
    CK_Right            = 0x131, /*!< Arrow key right*/
    CK_Up               = 0x132, /*!< Arrow key up*/
    CK_Down             = 0x133, /*!< Arrow key down*/

    CK_AltGr            = 0x134,

    CK_Android_Back     = CK_Escape,
    CK_Android_Menu     = CK_Home,
};

C_FLAGS(CISpecialKeyMapping,uint16);
C_FLAGS(CIControllerAxisMapping,uint16);
C_FLAGS(CIControllerButtonMapping,uint16);

using InputCode = uint16;

}
}
