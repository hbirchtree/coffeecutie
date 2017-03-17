#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/coffee_version.h>
#include <coffee/core/platform_data.h>

namespace Coffee{
namespace Display{

CDProperties GetDefaultVisual(const int32& ctxtMajorVer,const int32& ctxtMinorVer)
{
    CDProperties props = {};

    props.flags =
            CDProperties::Resizable |
            CDProperties::Windowed  |
            CDProperties::Visible;

    props.gl.flags =
            GLProperties::GLAutoResize
            |GLProperties::GLCoreProfile
            ;

    props.title = CoffeeDefaultWindowName.c_str();

    props.size.w = 1280;
    props.size.h = 720;
    props.monitor = 0;

    props.gl.version.major = ctxtMajorVer;
    props.gl.version.minor = ctxtMinorVer;

    props.gl.bits.depth = 24;
    props.gl.bits.stencil = 8;

    props.gl.bits.samples = 0;

    return props;
}

}
}
