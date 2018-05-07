#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>

#include <coffee/core/types/cdef/infotypes.h>
#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

CString GLEAM_API::GetAPIName(const GraphicsAPI::GraphicsDevice &)
{
    if(!GLEAM_FEATURES.is_gles)
        return "OpenGL";
    else
        return "OpenGL ES";
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
    if(!GLEAM_FEATURES.is_gles)
        return "GLSL";
    else
        return "GLSL ES";
}

bool GLEAM_API::GetShaderLanguageVersion(const GraphicsAPI::GraphicsContext &c, SWVersionInfo * ver)
{
    auto glsl_ver = CGL_Shared_Debug::ShaderLanguageVersion();
    new (ver) SWVersionInfo(GetShaderLanguageName(c), glsl_ver.major, glsl_ver.minor);
    return true;
}

bool GLEAM_API::TextureFormatSupport(PixFmt fmt)
{
    return CGL33::Debug::CompressedFormatSupport(
                Texture::T2D, fmt
                );
}

u32 GLSLVersionFromAPI(APILevel level)
{
    switch(level)
    {

    case GLES_2_0:
        return 100;
    case GLES_3_0:
    case GLES_3_2:
        return 300;

    case GL_3_3:
        return 330;
    case GL_4_3:
        if(GLEAM_FEATURES.base_instance)
            return 460;
        return 430;
    case GL_4_5:
    case GL_4_6:
        return 460;
    default:
        return 100;
    }
}

}
}
}
