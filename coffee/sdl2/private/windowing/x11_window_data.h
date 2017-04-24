#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>

namespace Coffee{
namespace Display{

struct X11_Data
{
    ::Display* display;
    ::Window window;

    XVisualInfo* visual;
};

}
}
