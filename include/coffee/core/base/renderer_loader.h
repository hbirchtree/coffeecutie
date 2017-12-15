#pragma once

#include <array>

#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/base/types/cdisplay.h>

namespace Coffee{
namespace Display{

template<typename Renderer> STATICINLINE
bool LoadHighestVersion(Renderer* renderer, CDProperties& properties, CString* err)
{


#if defined(COFFEE_ANDROID) || defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_GEKKO)
    return renderer->init(properties, err);
#else

    struct GLEnv
    {
        u32 requirements;
        u32 min;
        u32 maj;
    };

    static const constexpr std::array<GLEnv, 7> GLVersions = {{
#if !defined(COFFEE_ONLY_GLES20)
        {GLProperties::GLES, 3, 2},
        {GLProperties::GLES, 3, 1},
        {GLProperties::GLES, 3, 0},
#endif
        {GLProperties::GLES, 2, 0},

#if !defined(COFFEE_APPLE)
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

            if(renderer->init(c, err))
                return  true;
            else
                renderer->cleanup();
        }
    }
    return false;
#endif
}

}
}
