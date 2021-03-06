#include <coffee/graphics/apis/gleam/rhi/gleam_api_rhi.h>

#include "gleam_internal_types.h"
#include <peripherals/stl/string_ops.h>

namespace Coffee {
namespace RHI {
namespace GLEAM {

CString GLEAM_API::GetAPIName(const GraphicsAPI::GraphicsDevice&)
{
    if(!GLEAM_FEATURES.is_gles)
        return "OpenGL";
    else
        return "OpenGL ES";
}

bool GLEAM_API::GetAPIVersion(
    const GraphicsAPI::GraphicsDevice& d, SWVersionInfo* ver)
{
    auto gl_ver = CGL_Shared_Debug::ContextVersion();
    new(ver) SWVersionInfo(
        GetAPIName(d),
        0,
        gl_ver.major,
        gl_ver.minor,
        0,
        Strings::to_string(Level()));
    return true;
}

bool GLEAM_API::GetRendererInfo(
    const GraphicsAPI::GraphicsDevice&, HWDeviceInfo* dev)
{
    auto _dev = CGL_Shared_Debug::Renderer();
    new(dev)
        HWDeviceInfo(_dev.manufacturer, _dev.model, _dev.firmware, _dev.serial);
    return true;
}

bool GLEAM_API::GetRendererDriverInfo(
    const GraphicsAPI::GraphicsDevice&, SWVersionInfo* ver)
{
    auto dev = CGL_Shared_Debug::Renderer();
    if(dev.firmware.size() > 0)
    {
        new(ver) SWVersionInfo(dev.firmware, 0, 0);
        return true;
    }
    return false;
}

CString GLEAM_API::GetShaderLanguageName(const GraphicsAPI::GraphicsContext&)
{
    if(!GLEAM_FEATURES.is_gles)
        return "GLSL";
    else
        return "GLSL ES";
}

bool GLEAM_API::GetShaderLanguageVersion(
    const GraphicsAPI::GraphicsContext& c, SWVersionInfo* ver)
{
    auto glsl_ver = CGL_Shared_Debug::ShaderLanguageVersion();
    new(ver)
        SWVersionInfo(GetShaderLanguageName(c), glsl_ver.major, glsl_ver.minor);
    return true;
}

bool GLEAM_API::TextureFormatSupport(PixFmt fmt, CompFlags flags)
{
    switch(fmt)
    {
    case PixFmt::S3TC:
    {
        if(flags == CompFlags::BC7 || flags == CompFlags::BC6H)
        {
#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE) || \
    defined(GL_ARB_texture_compression_bptc)
            if(APILevelIsOfClass(GL_CURR_API, APIClass::GLCore))
                return Debug::CheckExtensionSupported(
                    CGL_DBG_CTXT, "GL_ARB_texture_compression_bptc");
            else
                return false;
#else
            return false;
#endif
        }
        if(flags == CompFlags::BC4 || flags == CompFlags::BC5)
        {
#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE) || \
    defined(GL_ARB_texture_compression_rgtc)
            if(APILevelIsOfClass(GL_CURR_API, APIClass::GLCore))
                return Debug::CheckExtensionSupported(
                    CGL_DBG_CTXT, "GL_ARB_texture_compression_rgtc");
            else
                return false;
#else
            return false;
#endif
        }
        if(flags == CompFlags::BC1 || flags == CompFlags::BC3)
        {
            return Debug::CheckExtensionSupported(
                CGL_DBG_CTXT, "GL_EXT_texture_compression_s3tc");
        }
        break;
    }

    case PixFmt::ASTC:
#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE) || \
    defined(GL_KHR_texture_compression_astc_hdr)
#if !defined(COFFEE_APPLE_MOBILE)
        if(APILevelIsOfClass(GL_CURR_API, APIClass::GLCore))
            return true;
        else
            /* Disabled because of API */
            return false;
#else
        /* Available on iOS */
        return true;
#endif
#else
        /* Disabled because it's not available */
        return false;
#endif

    case PixFmt::ETC1:
#if defined(GL_OES_compressed_ETC1_RGB8_texture)
        return Debug::CheckExtensionSupported(
            CGL_DBG_CTXT, "GL_OES_compressed_ETC1_RGB8_texture");
#else
        return false;
#endif
    case PixFmt::ETC2:
#if(GL_VERSION_VERIFY(0x430, 0x300) || defined(GL_ARB_ES3_compatibility)) && \
    defined(GL_COMPRESSED_RGB8_ETC2)
        return true;
#else
        return false;
#endif
    case PixFmt::ATC:
#if defined(GL_AMD_compressed_ATC_texture)
        return Debug::CheckExtensionSupported(
            CGL_DBG_CTXT, "GL_AMD_compressed_ATC_texture");
#else
        return false;
#endif

    case PixFmt::PVRTC:
#if defined(COFFEE_APPLE_MOBILE)
        return true;
#elif defined(GL_IMG_texture_compression_pvrtc)
        return Debug::CheckExtensionSupported(
            CGL_DBG_CTXT, "GL_IMG_texture_compression_pvrtc2");
#else
        return false;
#endif

    case PixFmt::PVRTC2:
#if defined(GL_IMG_texture_compression_pvrtc2)
        return Debug::CheckExtensionSupported(
            CGL_DBG_CTXT, "GL_IMG_texture_compression_pvrtc2");
#else
        return false;
#endif

    default:
        break;
    }

    return CGL::Debug::CompressedFormatSupport(CGL_DBG_CTXT, fmt);
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

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
