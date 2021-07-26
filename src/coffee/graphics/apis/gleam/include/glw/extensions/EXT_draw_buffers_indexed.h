#pragma once

#ifdef GL_EXT_draw_buffers_indexed
#include "../enums/BlendEquationModeEXT.h"
#include "../enums/BlendingFactor.h"
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/StencilOp.h"
#include "../enums/TextureEnvMode.h"
#include "../enums/TextureSwizzle.h"
namespace gl::ext::draw_buffers_indexed {
using gl::group::blend_equation_mode_ext;
using gl::group::blending_factor;
using gl::group::enable_cap;
using gl::group::get_prop;
using gl::group::stencil_op;
using gl::group::texture_env_mode;
using gl::group::texture_swizzle;
namespace values {
} // namespace values
STATICINLINE void blend_equation_separatei(
    u32                            buf,
    group::blend_equation_mode_ext modeRGB,
    group::blend_equation_mode_ext modeAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparateiEXT)
    }
    glBlendEquationSeparateiEXT(
        buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparateiEXT"sv);
}

STATICINLINE void blend_equationi(u32 buf, group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationiEXT)
    }
    glBlendEquationiEXT(buf, static_cast<GLenum>(mode));
    detail::error_check("BlendEquationiEXT"sv);
}

STATICINLINE void blend_func_separatei(
    u32                    buf,
    group::blending_factor srcRGB,
    group::blending_factor dstRGB,
    group::blending_factor srcAlpha,
    group::blending_factor dstAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncSeparateiEXT)
    }
    glBlendFuncSeparateiEXT(
        buf,
        static_cast<GLenum>(srcRGB),
        static_cast<GLenum>(dstRGB),
        static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    detail::error_check("BlendFuncSeparateiEXT"sv);
}

STATICINLINE void blend_funci(
    u32 buf, group::blending_factor src, group::blending_factor dst)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFunciEXT)
    }
    glBlendFunciEXT(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    detail::error_check("BlendFunciEXT"sv);
}

STATICINLINE void color_maski(u32 index, bool r, bool g, bool b, bool a)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorMaskiEXT)
    }
    glColorMaskiEXT(index, r, g, b, a);
    detail::error_check("ColorMaskiEXT"sv);
}

STATICINLINE void disablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableiEXT)
    }
    glDisableiEXT(static_cast<GLenum>(target), index);
    detail::error_check("DisableiEXT"sv);
}

STATICINLINE void enablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableiEXT)
    }
    glEnableiEXT(static_cast<GLenum>(target), index);
    detail::error_check("EnableiEXT"sv);
}

STATICINLINE GLboolean is_enabledi(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnablediEXT)
    }
    auto out = glIsEnablediEXT(static_cast<GLenum>(target), index);
    detail::error_check("IsEnablediEXT"sv);
    return out;
}

} // namespace gl::ext::draw_buffers_indexed
#endif // GL_EXT_draw_buffers_indexed
namespace gl::ext::draw_buffers_indexed {
constexpr auto name = "GL_EXT_draw_buffers_indexed";
}
