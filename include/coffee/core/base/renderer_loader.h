#pragma once

#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/base/types/cdisplay.h>

namespace Coffee{
namespace Display{

template<typename Renderer> STATICINLINE
bool LoadHighestVersion(Renderer* renderer, CDProperties& properties, CString* err)
{
#if !defined(COFFEE_ANDROID)
    do{
#if !defined(COFFEE_ONLY_GLES20)
        if(properties.gl.flags & (GLProperties::Flags::GLES))
        {
            properties.gl.version.major = 3;
            properties.gl.version.minor = 2;
        }else
        {
            properties.gl.version.major = 4;
            properties.gl.version.minor = 5;
        }

        if(renderer->init(properties,err))
            break;
        else
            renderer->cleanup();

        if(properties.gl.flags & (GLProperties::Flags::GLES))
        {
            properties.gl.version.major = 3;
            properties.gl.version.minor = 1;
        }else
        {
            properties.gl.version.minor = 3;
        }

        if(renderer->init(properties,err))
            break;
        else
            renderer->cleanup();

        if(properties.gl.flags & (GLProperties::Flags::GLES))
        {
            properties.gl.version.major = 3;
            properties.gl.version.minor = 0;
        }else
        {
            properties.gl.version.major = 3;
        }
        if(renderer->init(properties,err))
            break;
        else
            renderer->cleanup();

#endif
        if(properties.gl.flags & (GLProperties::Flags::GLES))
        {
            properties.gl.version.major = 2;
            properties.gl.version.minor = 0;

            if(renderer->init(properties,err))
                break;
            else
                renderer->cleanup();
        }

        return false;
    } while(false);
#else
    renderer->init(properties, err);
#endif
    return true;
}

}
}
