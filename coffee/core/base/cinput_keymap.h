#ifndef CINPUT_KEYMAP
#define CINPUT_KEYMAP

#include "coffee/core/ctypes/basetypes.h"

namespace Coffee{
namespace CInput{

/*!
 * \brief Xbox 360 controller mapping of buttons. Same layout as SDL2's GameController, but we do not expose all parts of SDL2.
 */
enum CIControllerButtonMapping : uint8
{
    CK_BUTTON_A         = 0x0, /*!< A button*/
    CK_BUTTON_B         = 0x1, /*!< B button*/
    CK_BUTTON_X         = 0x2, /*!< X button*/
    CK_BUTTON_Y         = 0x3, /*!< Y button*/

    CK_BUTTON_BACK      = 0x4, /*!< Back button*/
    CK_BUTTON_GUIDE     = 0x5, /*!< Guide button, also known as the Xbox button*/
    CK_BUTTON_START     = 0x6, /*!< Start button*/

    CK_BUTTON_LS        = 0x7, /*!< Left stick button*/
    CK_BUTTON_RS        = 0x8, /*!< Right stick button*/

    CK_BUTTON_LB        = 0x9, /*!< Left bumper*/
    CK_BUTTON_RB        = 0xA, /*!< Right bumper*/

    CK_BUTTON_POV_UP    = 0xB, /*!< Also known as D-Pad up*/
    CK_BUTTON_POV_DOWN  = 0xC, /*!< Also known as D-Pad down*/
    CK_BUTTON_POV_LEFT  = 0xD, /*!< Also known as D-Pad left*/
    CK_BUTTON_POV_RIGHT = 0xE, /*!< Also known as D-Pad right*/
};

/*!
 * \brief Xbox 360 controller mapping of axes. Same mapping as SDL2's GameController.
 */
enum CIControllerAxisMapping : uint8
{
    CK_AXIS_LEFT_X          = 0x0, /*!< Left stick X-axis*/
    CK_AXIS_LEFT_Y          = 0x1, /*!< Left stick Y-axis*/

    CK_AXIS_RIGHT_X         = 0x2, /*!< Right stick X-axis*/
    CK_AXIS_RIGHT_Y         = 0x3, /*!< Right stick Y-axis*/

    CK_AXIS_TRIGGER_LEFT    = 0x4, /*!< Left trigger*/
    CK_AXIS_TRIGGER_RIGHT   = 0x5, /*!< Right trigger*/
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
};

/*!
 * \brief A mapping of keycodes for Latin-1 for use with input functions
 */
enum CILatin1Mapping : uint8
{
    CK_Null             = 0x0,  /*!< Used for unrecognized keys*/
//    CK_                 = 0x1,
//    CK_                 = 0x2,
//    CK_                 = 0x3,
//    CK_                 = 0x4,
//    CK_                 = 0x5,
//    CK_                 = 0x6,
    CK_Bell             = 0x7,
    CK_BackSpace        = 0x8,
    CK_HTab             = 0x9,
    CK_EnterNL          = 0xA,  /*!< Enter key*/
//    CK_VTab             = 0xB,
//    CK_NewPage          = 0xC,
    CK_EnterCR          = 0xD,  /*!< Enter key?*/
//    CK_                 = 0xE,
//    CK_                 = 0xF,
//    CK_                 = 0x10,
//    CK_                 = 0x11,
//    CK_                 = 0x12,
//    CK_                 = 0x13,
//    CK_                 = 0x14,
//    CK_                 = 0x15,
//    CK_                 = 0x16,
//    CK_                 = 0x17,
//    CK_                 = 0x18,
//    CK_                 = 0x19,
//    CK_                 = 0x1A,
    CK_Escape           = 0x1B,
//    CK_                 = 0x1C,
//    CK_                 = 0x1D,
//    CK_                 = 0x1E,
//    CK_                 = 0x1F,
    CK_Space            = 0x20,
    CK_ExclamationMark  = 0x21,
    CK_QuotationMark    = 0x22,
    CK_NumberSign       = 0x23,
    CK_DollarSign       = 0x24,
    CK_Percentage       = 0x25,
    CK_Ampersand        = 0x26,
    CK_Apostrophe       = 0x27,
    CK_LParenthesis     = 0x28,
    CK_RParenthesis     = 0x29,
    CK_Asterisk         = 0x2A,
    CK_Plus             = 0x2B,
    CK_Comma            = 0x2C,
    CK_Hyphen           = 0x2D,
    CK_Period           = 0x2E,
    CK_RSlash           = 0x2F,
    CK_0                = 0x30,
    CK_1                = 0x31,
    CK_2                = 0x32,
    CK_3                = 0x33,
    CK_4                = 0x34,
    CK_5                = 0x35,
    CK_6                = 0x36,
    CK_7                = 0x37,
    CK_8                = 0x38,
    CK_9                = 0x39,
    CK_Colon            = 0x3A,
    CK_SemiColon        = 0x3B,
    CK_LessThan         = 0x3C,
    CK_Equals           = 0x3D,
    CK_GreaterThan      = 0x3E,
    CK_QuestionMark     = 0x3F,
    CK_AtSign           = 0x40,
    CK_A                = 0x41,
    CK_B                = 0x42,
    CK_C                = 0x43,
    CK_D                = 0x44,
    CK_E                = 0x45,
    CK_F                = 0x46,
    CK_G                = 0x47,
    CK_H                = 0x48,
    CK_I                = 0x49,
    CK_J                = 0x4A,
    CK_K                = 0x4B,
    CK_L                = 0x4C,
    CK_M                = 0x4D,
    CK_N                = 0x4E,
    CK_O                = 0x4F,
    CK_P                = 0x50,
    CK_Q                = 0x51,
    CK_R                = 0x52,
    CK_S                = 0x53,
    CK_T                = 0x54,
    CK_U                = 0x55,
    CK_V                = 0x56,
    CK_W                = 0x57,
    CK_X                = 0x58,
    CK_Y                = 0x59,
    CK_Z                = 0x5A,
    CK_LSBracket        = 0x5B,
    CK_LSlash           = 0x5C,
    CK_RSBracket        = 0x5D,
    CK_Caret            = 0x5E,
    CK_Underscore       = 0x5F,
    CK_RApostrophe      = 0x60,
    CK_a                = 0x61,
    CK_b                = 0x62,
    CK_c                = 0x63,
    CK_d                = 0x64,
    CK_e                = 0x65,
    CK_f                = 0x66,
    CK_g                = 0x67,
    CK_h                = 0x68,
    CK_i                = 0x69,
    CK_j                = 0x6A,
    CK_k                = 0x6B,
    CK_l                = 0x6C,
    CK_m                = 0x6D,
    CK_n                = 0x6E,
    CK_o                = 0x6F,
    CK_p                = 0x70,
    CK_q                = 0x71,
    CK_r                = 0x72,
    CK_s                = 0x73,
    CK_t                = 0x74,
    CK_u                = 0x75,
    CK_v                = 0x76,
    CK_w                = 0x77,
    CK_x                = 0x78,
    CK_y                = 0x79,
    CK_z                = 0x7A,
    CK_LCBracket        = 0x7B,
    CK_VBar             = 0x7C,
    CK_RCBracket        = 0x7D,
    CK_Tilde            = 0x7E,
    CK_Delete           = 0x7F,
//    CK_?                = 0x80,
//    CK_?                = 0x81,
//    CK_?                = 0x82,
//    CK_?                = 0x83,
//    CK_?                = 0x84,
//    CK_?                = 0x85,
//    CK_?                = 0x86,
//    CK_?                = 0x87,
//    CK_?                = 0x88,
//    CK_?                = 0x89,
//    CK_?                = 0x8A,
//    CK_?                = 0x8B,
//    CK_?                = 0x8C,
//    CK_?                = 0x8D,
//    CK_?                = 0x8E,
//    CK_?                = 0x8F,
//    CK_?                = 0x90,
//    CK_?                = 0x91,
//    CK_?                = 0x92,
//    CK_?                = 0x93,
//    CK_?                = 0x94,
//    CK_?                = 0x95,
//    CK_?                = 0x96,
//    CK_?                = 0x97,
//    CK_?                = 0x98,
//    CK_?                = 0x99,
//    CK_?                = 0x9A,
//    CK_?                = 0x9B,
//    CK_?                = 0x9C,
//    CK_?                = 0x9D,
//    CK_?                = 0x9E,
//    CK_?                = 0x9F,
    CK_NBSpace          = 0xA0,
    CK_IExclamationMark = 0xA1,
    CK_CentSign         = 0xA2,
    CK_PoundSign        = 0xA3,
    CK_CurrencySign     = 0xA4,
    CK_YenSign          = 0xA5,
    CK_BrokenVBar       = 0xA6,
    CK_SectionSign      = 0xA7,
    CK_Umlaut           = 0xA8,
    CK_CopyrightSign    = 0xA9,
    CK_FemOrdinal       = 0xAA,
    CK_LGuillemot       = 0xAB,
    CK_NegateSign       = 0xAC,
    CK_SoftHyphen       = 0xAD,
    CK_RegisteredTM     = 0xAE,
    CK_MacronAccent     = 0xAF,
    CK_DegreeSign       = 0xB0,
    CK_PlusMinus        = 0xB1,
    CK_Super2           = 0xB2,
    CK_Super3           = 0xB3,
    CK_AcuteAccent      = 0xB4,
    CK_MicroSign        = 0xB5,
    CK_ParagraphSign    = 0xB6,
    CK_MiddleDot        = 0xB7,
    CK_Cedilla          = 0xB8,
    CK_Super1           = 0xB9,
    CK_MascOrdinal      = 0xBA,
    CK_FracOneFourth    = 0xBB,
    CK_FracOneHalf      = 0xBC,
    CK_FracThreeFourths = 0xBD,
    CK_IQuestionMark    = 0xBE,
    CK_AGrave           = 0xBF,
    CK_AAcute           = 0xC0,
    CK_ACircumflex      = 0xC1,
    CK_ATilde           = 0xC2,
    CK_AUmlaut          = 0xC3,
    CK_ARing            = 0xC4,
    CK_AE               = 0xC5,
    CK_CCedilla         = 0xC6,
    CK_EGrave           = 0xC7,
    CK_EAcuta           = 0xC8,
    CK_ECircumflex      = 0xC9,
    CK_EUmlaut          = 0xCA,
    CK_IGrave           = 0xCB,
    CK_IAcute           = 0xCC,
//    CK_?                = 0xCD,
//    CK_?                = 0xCE,
//    CK_?                = 0xCF,
    CK_Eth              = 0xD0,
//    CK_?                = 0xD1,
//    CK_?                = 0xD2,
//    CK_?                = 0xD3,
//    CK_?                = 0xD4,
//    CK_?                = 0xD5,
//    CK_?                = 0xD6,
    CK_MultiplySign     = 0xD7,
//    CK_?                = 0xD8,
//    CK_?                = 0xD9,
//    CK_?                = 0xDA,
//    CK_?                = 0xDB,
//    CK_?                = 0xDC,
//    CK_?                = 0xDD,
    CK_Thorn            = 0xDE,
//    CK_?                = 0xDF,
//    CK_?                = 0xE0,
//    CK_?                = 0xE1,
//    CK_?                = 0xE2,
//    CK_?                = 0xE3,
//    CK_?                = 0xE4,
//    CK_?                = 0xE5,
    CK_ae               = 0xE6,
//    CK_?                = 0xE7,
//    CK_?                = 0xE8,
//    CK_?                = 0xE9,
//    CK_?                = 0xEA,
//    CK_?                = 0xEB,
//    CK_?                = 0xEC,
//    CK_?                = 0xED,
//    CK_?                = 0xEE,
//    CK_?                = 0xEF,
    CK_eth              = 0xF0,
//    CK_?                = 0xF1,
//    CK_?                = 0xF2,
//    CK_?                = 0xF3,
//    CK_?                = 0xF4,
//    CK_?                = 0xF5,
//    CK_?                = 0xF6,
    CK_DivideSign       = 0xF7,
    CK_OSlash           = 0xF8,
//    CK_?                = 0xF9,
//    CK_?                = 0xFA,
//    CK_?                = 0xFB,
//    CK_?                = 0xFC,
//    CK_?                = 0xFD,
    CK_thorn            = 0xFE,
//    CK_?                = 0xFF,
};

}
}

#endif // CINPUT_KEYMAP

