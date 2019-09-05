#pragma once

#include "services.h"

namespace comp_app {

struct GLConfig : Config<GLConfig>
{
    enum Profile
    {
        Core     = 0x1,
        Embedded = 0x2,
        Debug    = 0x4,

        Default = Core,
    };

    PixFmt framebufferFmt = PixFmt::RGBA8;
    PixFmt depthFmt       = PixFmt::Depth24Stencil8;

    struct
    {
        libc_types::i32 major, minor;
    } version;

    libc_types::i32 swapInterval = 1;

    Profile profile = Default;
};

C_FLAGS(GLConfig::Profile, libc_types::u32);

} // namespace comp_app
