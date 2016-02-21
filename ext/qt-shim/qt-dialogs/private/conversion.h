#pragma once

#include <coffee/core/types/edef/colorenum.h>
#include <coffee/core/types/edef/pixenum.h>

#include <Qt>
#include <QColor>

namespace CoffeeExt{
namespace QtDialogs{

using namespace Coffee;

Qt::Alignment qt_alignflag(Alignment a)
{
    Qt::Alignment o = (Qt::Alignment)0;

    if(feval(a&AlignLeft))
        o |= Qt::AlignLeft;
    if(feval(a&AlignHCenter))
        o |= Qt::AlignHCenter;
    if(feval(a&AlignRight))
        o |= Qt::AlignRight;

    if(feval(a&AlignTop))
        o |= Qt::AlignTop;
    if(feval(a&AlignVCenter))
        o |= Qt::AlignVCenter;
    if(feval(a&AlignBottom))
        o |= Qt::AlignBottom;

    return o;
}

QColor qt_colorflag(Color c)
{
    switch(c)
    {
    case Color::Pink:
    case Color::Red:
    case Color::FireRed:
        return Qt::red;
    case Color::Orange:
        return Qt::yellow;
    case Color::Yellow:
        return Qt::yellow;
    case Color::LeafGreen:
        return Qt::darkGreen;
    case Color::Lime:
    case Color::Green:
        return Qt::green;
    case Color::Turquoise:
    case Color::Cyan:
        return Qt::cyan;
    case Color::SkyBlue:
    case Color::Blue:
        return Qt::blue;
    case Color::Brown:
        return Qt::darkMagenta;
    case Color::Black:
        return Qt::black;
    case Color::LightGray:
        return Qt::lightGray;
    case Color::Gray:
        return Qt::gray;
    case Color::White:
        return Qt::white;
    default:
        return Qt::black;
    }
}

}
}