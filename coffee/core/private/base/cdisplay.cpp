#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/coffee_version.h>

namespace Coffee{
namespace Display{

CDProperties GetDefaultVisual(const int32& ctxtMajorVer,const int32& ctxtMinorVer)
{
    CDProperties props = {};

    props.flags =
            CDProperties::Resizable |
            CDProperties::Windowed  |
            CDProperties::Visible;

#ifdef COFFEE_ANDROID
    props.flags |= CDProperties::FullScreen;
#endif

    props.gl.flags =
            GLProperties::GLAutoResize|
            GLProperties::GLCoreProfile;

    props.title = CoffeeDefaultWindowName.c_str();

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

    return props;
}

}
}
