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

#ifdef COFFEE_ANDROID
    props.flags ^= CDProperties::Windowed;
    props.flags = CDProperties::FullScreen;
#endif



    props.gl.flags =
            GLProperties::GLAutoResize
            |GLProperties::GLCoreProfile
#if !defined(COFFEE_ANDROID) && !defined(COFFEE_RASPBERRYPI)
            |GLProperties::GLSRGB
#endif
            ;

    props.title = CoffeeDefaultWindowName.c_str();

    props.size.w = 1280;
    props.size.h = 720;
    props.monitor = 0;

    props.gl.version.major = ctxtMajorVer;
    props.gl.version.minor = ctxtMinorVer;

#if defined(COFFEE_MAEMO)
    props.gl.bits.alpha = 0;
    props.gl.bits.red = 5;
    props.gl.bits.green = 6;
    props.gl.bits.blue = 5;
#else
    props.gl.bits.alpha = 8;
    props.gl.bits.red = 8;
    props.gl.bits.blue = 8;
    props.gl.bits.green = 8;
#endif
    props.gl.bits.depth = 24;
    props.gl.bits.stencil = 8;

    props.gl.bits.samples = 0;

    return props;
}

}
}
