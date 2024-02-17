#pragma once

#include <peripherals/stl/type_list.h>

#include "services.h"

namespace comp_app {

struct GLConfig : Config<GLConfig>
{
    GLConfig()
    {
        if constexpr(versionIsFixed)
        {
            version.major = fixed_version::major;
            version.minor = fixed_version::minor;
        }
#if defined(FEATURE_ENABLE_GLAD_Core) || defined(FEATURE_ENABLE_GLAD_ES)
        else
        {
            version.major = preferred_version::major;
            version.minor = preferred_version::minor;
            profile       = preferred_version::profile;
        }
#endif
    }

    enum Profile
    {
        Core     = 0x1,
        Embedded = 0x2,
        Debug    = 0x4,
        Robust   = 0x8,

        PreferHighPerf = 0x10,
        PreferLowPerf  = 0x20,

#if defined(FEATURE_ENABLE_GLAD_Core)
        Default = Core,
#elif defined(FEATURE_ENABLE_GLAD_ES)
        Default = Embedded,
#else
        Default = 0x0,
#endif

        ProfileMask = Core | Embedded,
        PerfMask    = PreferHighPerf | PreferLowPerf,
    };

    PixFmt framebufferFmt = PixFmt::RGBA8;
    PixFmt depthFmt       = PixFmt::Depth24Stencil8;

    struct
    {
        libc_types::i32 major, minor;
    } version;

    libc_types::i32 swapInterval = 1;

    Profile profile = Default;

    struct multisample_opts
    {
        libc_types::i32 samples = 1;
        bool            enabled = false;
    } multisampling;

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

    using gl33_version = static_version<3, 3>;
    using gl46_version = static_version<4, 6>;

    using gles2_version = static_version<2, 0, Embedded>;
    using gles3_version = static_version<3, 0, Embedded>;

    static constexpr bool versionIsFixed = false;
#if defined(COFFEE_RASPBERRY) || defined(COFFEE_MAEMO)
    using fixed_version = gles2_version;
#else
    using fixed_version = static_version<0, 0>;
#endif

#if defined(FEATURE_ENABLE_GLAD_Core)
    using preferred_version = gl46_version;
#elif defined(FEATURE_ENABLE_GLAD_ES)
    using preferred_version = gles3_version;
#endif
};

C_FLAGS(GLConfig::Profile, libc_types::u32);

} // namespace comp_app
