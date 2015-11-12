#include "cdisplay.h"

namespace Coffee{
namespace CDisplay{

CDWindowProperties coffee_get_default_visual()
{
    CDWindowProperties props;

    props.flags |=
            CDWindowProperties::Resizable |
            CDWindowProperties::Windowed;

    props.contextProperties.flags |= CGLContextProperties::GLAutoResize;

    props.title = "CoffeeCore";

    props.size.w = 1280;
    props.size.h = 720;
    props.monitor = 0;

    props.contextProperties.version.major = 3;
    props.contextProperties.version.minor = 3;

    props.contextProperties.bits.alpha = 8;
    props.contextProperties.bits.red = 8;
    props.contextProperties.bits.blue = 8;
    props.contextProperties.bits.green = 8;
    props.contextProperties.bits.depth = 24;
    props.contextProperties.bits.stencil = 8;

    props.contextProperties.bits.accum.r = 8;
    props.contextProperties.bits.accum.g = 8;
    props.contextProperties.bits.accum.b = 8;
    props.contextProperties.bits.accum.a = 8;

    return props;
}

}
}
