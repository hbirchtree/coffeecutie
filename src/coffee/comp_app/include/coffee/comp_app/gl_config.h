#pragma once

#include <peripherals/stl/type_list.h>

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

        ProfileMask = Core | Embedded,
    };

    PixFmt framebufferFmt = PixFmt::RGBA8;
    PixFmt depthFmt       = PixFmt::Depth24Stencil8;

    struct
    {
        libc_types::i32 major, minor;
    } version;

    libc_types::i32 swapInterval = 1;

    Profile profile = Default;

    template<libc_types::u32 Maj, libc_types::u32 Min, Profile _Profile = Core>
    struct static_version
    {
        static constexpr libc_types::u32 major = Maj, minor = Min;
        static constexpr Profile         profile = _Profile;
    };

    using valid_versions = detail::TypeList<
        static_version<3, 3>,
        static_version<4, 0>,
        static_version<4, 1>,
        static_version<4, 3>,
        static_version<4, 4>,
        static_version<4, 5>,
        static_version<4, 6>,

        static_version<2, 0, Embedded>,
        static_version<3, 0, Embedded>,
        static_version<3, 1, Embedded>,
        static_version<3, 2, Embedded>>;
};

C_FLAGS(GLConfig::Profile, libc_types::u32);

} // namespace comp_app
