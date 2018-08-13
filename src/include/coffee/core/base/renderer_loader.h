#pragma once

#include <coffee/core/CProfiling>
#include <coffee/core/base/renderer/initapplication.h>
#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/coffee_mem_macros.h>

namespace Coffee {
namespace Display {

template<
    typename Renderer,

    typename implements<InitApplication, Renderer>::type* = nullptr

    >
STATICINLINE bool LoadHighestVersion(
    Renderer* renderer, CDProperties& properties, CString* err)
{
#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_GEKKO)
    return renderer->init(properties, err);
#else

    struct GLEnv
    {
        u32 requirements;
        u32 maj;
        u32 min;
    };

    static const constexpr Array<GLEnv, 8> GLVersions = {{
#if !defined(COFFEE_ONLY_GLES20)
        {GLProperties::GLES, 3, 2},
        {GLProperties::GLES, 3, 1},
        {GLProperties::GLES, 3, 0},
#endif
        {GLProperties::GLES, 2, 0},

#if !defined(COFFEE_APPLE)
        {0x0, 4, 6},
        {0x0, 4, 5},
        {0x0, 4, 3},
#endif
        {0x0, 3, 3},
    }};

    for(GLEnv const& env : GLVersions)
    {
        if(env.maj == 0)
            continue;

        if(env.maj > properties.gl.version.major)
            continue;

        if(env.min > properties.gl.version.minor)
            continue;

        if((properties.gl.flags & env.requirements) == env.requirements)
        {
            CDProperties c = properties;

            c.gl.version.major = env.maj;
            c.gl.version.minor = env.min;

            DProfContext a("Attempting initialization");
            if(renderer->init(c, err))
                return true;
            else
                renderer->cleanup();
        }
    }
    return false;
#endif
}

} // namespace Display
} // namespace Coffee
