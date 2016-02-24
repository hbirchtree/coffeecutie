#include <coffee/core/CDisplay>

namespace Coffee{
namespace CDisplay{

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
