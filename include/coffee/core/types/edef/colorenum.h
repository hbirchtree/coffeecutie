#pragma once

#include "enumfun.h"

namespace Coffee{

enum class Color
{
    Neutral,

    Pink,
    Red,
    FireRed,
    Orange,
    Yellow,
    Lime,
    LeafGreen,
    Green,
    Turquoise,
    Cyan,
    SkyBlue,
    Blue,
    DeepViolet,
    Purple,

    Brown,

    Black,
    LightGray,
    Gray,
    White,
};

enum Alignment
{
    AlignUndefined = 0,

    AlignLeft = 0x1,
    AlignHCenter = 0x2,
    AlignRight = 0x4,

    AlignTop = 0x8,
    AlignVCenter = 0x10,
    AlignBottom = 0x20,

    AlignCenter = 0x12,
};
C_FLAGS(Alignment,int);

}