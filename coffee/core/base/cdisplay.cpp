#include "cdisplay.h"

namespace Coffee{
namespace CDisplay{

CDWindowProperties coffee_get_default_visual(const int32 &ctxtMajorVer, const int32 &ctxtMinorVer)
{
    CDWindowProperties props;

    props.flags =
            CDWindowProperties::Resizable |
            CDWindowProperties::Windowed;

    props.contextProperties.flags =
            props.contextProperties.flags|CGLContextProperties::GLAutoResize;

    props.title = "CoffeeCore";

    props.size.w = 1280;
    props.size.h = 720;
    props.monitor = 0;

    props.contextProperties.version.major = ctxtMajorVer;
    props.contextProperties.version.minor = ctxtMinorVer;

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

CDWindowProperties::CDWindowProperties():
    flags(),
    monitor(0),
    size(0,0),
    contextProperties(),
    title(nullptr),
    window(nullptr)
{
}

CGLContextProperties::CGLContextProperties():
    bits(),
    flags(GLCoreProfile),
    version(3,3)
{
}

CDFocusEvent::CDFocusEvent():
    mod()
{
}

CDStateEvent::CDStateEvent():
    type()
{
}

CGLContextVersion::CGLContextVersion()
{
}

CGLContextVersion::CGLContextVersion(uint8 maj, uint8 min)
{
    this->major = maj;
    this->minor = min;
}

CDEvent::CDEvent():
    type(),
    ts(0)
{
}

CDMonitor::CDMonitor():
    screenArea(),
    name(nullptr),
    phySize(0,0),
    colorBits(),
    refresh(0),
    index(0)
{
}

CDContextBits::CDContextBits():
    accum(),
    red(0),
    green(0),
    blue(0),
    alpha(0),
    depth(0),
    stencil(0)
{
}

CDColorSpace::CDColorSpace():
    red(0),
    green(0),
    blue(0)
{
}

}
}
