#pragma once

#include <coffee/graphics/apis/gleam/levels/gl_shared_types.h>

namespace Coffee {

#if GL_VERSION_VERIFY(0x330, 0x320)
debug::Type gl_converttype(GLuint type)
{
    switch(type)
    {
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return debug::Type::Deprecated;
    case GL_DEBUG_TYPE_ERROR:
        return debug::Type::UndefinedBehavior;
    case GL_DEBUG_TYPE_MARKER:
        return debug::Type::Marker;
    case GL_DEBUG_TYPE_OTHER:
        return debug::Type::Other;
    case GL_DEBUG_TYPE_PERFORMANCE:
        return debug::Type::Performance;
    case GL_DEBUG_TYPE_PORTABILITY:
        return debug::Type::Compliance;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return debug::Type::UndefinedBehavior;
    case GL_DEBUG_TYPE_POP_GROUP:
        return debug::Type::Information;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        return debug::Type::Information;
    default:
        return debug::Type::Information;
    }
}

debug::Severity gl_convertsev(GLuint sev)
{
    switch(sev)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        return debug::Severity::High;
    case GL_DEBUG_SEVERITY_MEDIUM:
        return debug::Severity::Medium;
    case GL_DEBUG_SEVERITY_LOW:
        return debug::Severity::Low;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return debug::Severity::Information;
    default:
        return debug::Severity::Information;
    }
}

debug::Component gl_convertcomp(GLuint src)
{
    switch(src)
    {
    case GL_DEBUG_SOURCE_API:
        return debug::Component::GraphicsAPI;
    case GL_DEBUG_SOURCE_APPLICATION:
        return debug::Component::Core;
    case GL_DEBUG_SOURCE_OTHER:
        return debug::Component::Interface;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return debug::Component::ShaderCompiler;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return debug::Component::GraphicsAPI;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return debug::Component::GraphicsAPI;
    default:
        return debug::Component::GraphicsAPI;
    }
}
#endif
}
