#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/coffee_version.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/internal_state.h>

namespace Coffee{
namespace Display{

CDProperties GetDefaultVisual(const i32& ctxtMajorVer,const i32& ctxtMinorVer)
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

#ifndef COFFEE_LOWFAT
    props.title = State::GetBuildInfo().default_window_name.c_str();
#endif

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
