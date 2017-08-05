#pragma once

#include <coffee/core/types/edef/dbgenum.h>
#include <coffee/graphics/apis/gleam/levels/shared/gl_shared_include.h>

namespace Coffee{

#ifdef COFFEE_GLEAM_DESKTOP
DebugType gl_converttype(GLuint type)
{
    switch(type)
    {
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return DebugType::Deprecated;
    case GL_DEBUG_TYPE_ERROR:
        return DebugType::UndefinedBehavior;
    case GL_DEBUG_TYPE_MARKER:
        return DebugType::Marker;
    case GL_DEBUG_TYPE_OTHER:
        return DebugType::Other;
    case GL_DEBUG_TYPE_PERFORMANCE:
        return DebugType::Performance;
    case GL_DEBUG_TYPE_PORTABILITY:
        return DebugType::Compliance;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return DebugType::UndefinedBehavior;
    case GL_DEBUG_TYPE_POP_GROUP:
        return DebugType::Information;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        return DebugType::Information;
    default:
        return DebugType::Information;
    }
}

Severity gl_convertsev(GLuint sev)
{
    switch(sev)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        return Severity::High;
    case GL_DEBUG_SEVERITY_MEDIUM:
        return Severity::Medium;
    case GL_DEBUG_SEVERITY_LOW:
        return Severity::Low;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return Severity::Information;
    default:
        return Severity::Information;
    }
}

DebugComponent gl_convertcomp(GLuint src)
{
    switch(src)
    {
    case GL_DEBUG_SOURCE_API:
        return DebugComponent::GraphicsAPI;
    case GL_DEBUG_SOURCE_APPLICATION:
        return DebugComponent::Core;
    case GL_DEBUG_SOURCE_OTHER:
        return DebugComponent::Interface;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return DebugComponent::ShaderCompiler;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return DebugComponent::GraphicsAPI;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return DebugComponent::GraphicsAPI;
    default:
        return DebugComponent::GraphicsAPI;
    }
}
#endif

}
