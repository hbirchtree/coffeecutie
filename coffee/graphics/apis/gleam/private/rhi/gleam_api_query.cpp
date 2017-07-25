#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>

#include <coffee/core/types/cdef/infotypes.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

CString GLEAM_API::GetAPIName(const GraphicsAPI::GraphicsDevice &)
{
#if defined(COFFEE_GLEAM_DESKTOP)
    return "OpenGL";
#else
    return "OpenGL ES";
#endif
}

bool GLEAM_API::GetAPIVersion(const GraphicsAPI::GraphicsDevice &d, SWVersionInfo * ver)
{
    auto gl_ver = CGL_Shared_Debug::ContextVersion();
    new (ver) SWVersionInfo(GetAPIName(d), 0, gl_ver.major, gl_ver.minor, 0,
                            StrUtil::pointerify(Level()));
    return true;
}

bool GLEAM_API::GetRendererInfo(const GraphicsAPI::GraphicsDevice &, HWDeviceInfo * dev)
{
    auto _dev = CGL_Shared_Debug::Renderer();
    new (dev) HWDeviceInfo(_dev.manufacturer, _dev.model, _dev.firmware, _dev.serial);
    return true;
}

bool GLEAM_API::GetRendererDriverInfo(const GraphicsAPI::GraphicsDevice &, SWVersionInfo * ver)
{
    auto dev = CGL_Shared_Debug::Renderer();
    if(dev.firmware.size() > 0)
    {
        new (ver) SWVersionInfo(dev.firmware, 0, 0);
        return true;
    }
    return false;
}

CString GLEAM_API::GetShaderLanguageName(const GraphicsAPI::GraphicsContext &)
{
#if defined(COFFEE_GLEAM_DESKTOP)
    return "GLSL";
#else
    return "GLSL ES";
#endif
}

bool GLEAM_API::GetShaderLanguageVersion(const GraphicsAPI::GraphicsContext &c, SWVersionInfo * ver)
{
    auto glsl_ver = CGL_Shared_Debug::ShaderLanguageVersion();
    new (ver) SWVersionInfo(GetShaderLanguageName(c), glsl_ver.major, glsl_ver.minor);
    return true;
}

}
}
}