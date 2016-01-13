#include <base/cdisplay.h>

namespace Coffee{
namespace CDisplay{

CDProperties GetDefaultVisual(const int32 &ctxtMajorVer, const int32 &ctxtMinorVer)
{
    CDProperties props;

    props.flags =
            CDProperties::Resizable |
            CDProperties::Windowed  |
            CDProperties::Visible;

    props.gl.flags =
            GLProperties::GLAutoResize|
            GLProperties::GLCoreProfile;

    props.title = "CoffeeCore";

    props.size.w = 1280;
    props.size.h = 720;
    props.monitor = 0;

    props.gl.version.major = ctxtMajorVer;
    props.gl.version.minor = ctxtMinorVer;

    props.gl.bits.alpha = 8;
    props.gl.bits.red = 8;
    props.gl.bits.blue = 8;
    props.gl.bits.green = 8;
    props.gl.bits.depth = 24;
    props.gl.bits.stencil = 8;

    props.gl.bits.accum.r = 8;
    props.gl.bits.accum.g = 8;
    props.gl.bits.accum.b = 8;
    props.gl.bits.accum.a = 8;

    return props;
}

CDProperties::CDProperties():
    flags(),
    monitor(0),
    size(0,0),
    gl(),
    title(nullptr),
    window(nullptr)
{
}

GLProperties::GLProperties():
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

CGLVersion::CGLVersion()
{
    this->major = 0;
    this->minor = 0;
    this->revision = 0;
}

CGLVersion::CGLVersion(uint8 maj, uint8 min)
{
    this->major = maj;
    this->minor = min;
    this->revision = 0;
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
