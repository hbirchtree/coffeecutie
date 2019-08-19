#include <coffee/core/types/display/properties.h>

#include <coffee/core/internal_state.h>
#include <coffee/core/platform_data.h>

namespace Coffee {
namespace Display {

Properties GetDefaultVisual(const i32& ctxtMajorVer, const i32& ctxtMinorVer)
{
    Properties props = {};

    props.flags = Properties::Resizable | Properties::Windowed |
                  Properties::Visible | Properties::HighDPI;

    props.gl.flags =
        GL::Properties::GLAutoResize | GL::Properties::GLCoreProfile;

#ifndef COFFEE_LOWFAT
    props.title = State::GetBuildInfo().default_window_name.c_str();
#endif

    props.size.w  = 1280;
    props.size.h  = 720;
    props.monitor = 0;

    props.gl.version.major = C_FCAST<u8>(ctxtMajorVer);
    props.gl.version.minor = C_FCAST<u8>(ctxtMinorVer);

    props.gl.bits.depth   = 24;
    props.gl.bits.stencil = 8;

    props.gl.bits.samples = 0;

    return props;
}

} // namespace Display
} // namespace Coffee
