#pragma once

#ifdef GL_OES_draw_buffers_indexed
#include "../enums/BlendEquationModeEXT.h"
#include "../enums/BlendingFactor.h"
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/StencilOp.h"
#include "../enums/TextureEnvMode.h"
#include "../enums/TextureSwizzle.h"
namespace gl::oes::draw_buffers_indexed {
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
        GLW_FPTR_CHECK(BlendEquationSeparateiOES)
    }
    glBlendEquationSeparateiOES(
        buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparateiOES"sv);
}

STATICINLINE void blend_equationi(u32 buf, group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationiOES)
    }
    glBlendEquationiOES(buf, static_cast<GLenum>(mode));
    detail::error_check("BlendEquationiOES"sv);
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
        GLW_FPTR_CHECK(BlendFuncSeparateiOES)
    }
    glBlendFuncSeparateiOES(
        buf,
        static_cast<GLenum>(srcRGB),
        static_cast<GLenum>(dstRGB),
        static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    detail::error_check("BlendFuncSeparateiOES"sv);
}

STATICINLINE void blend_funci(
    u32 buf, group::blending_factor src, group::blending_factor dst)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFunciOES)
    }
    glBlendFunciOES(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    detail::error_check("BlendFunciOES"sv);
}

STATICINLINE void color_maski(u32 index, bool r, bool g, bool b, bool a)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorMaskiOES)
    }
    glColorMaskiOES(index, r, g, b, a);
    detail::error_check("ColorMaskiOES"sv);
}

STATICINLINE void disablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableiOES)
    }
    glDisableiOES(static_cast<GLenum>(target), index);
    detail::error_check("DisableiOES"sv);
}

STATICINLINE void enablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableiOES)
    }
    glEnableiOES(static_cast<GLenum>(target), index);
    detail::error_check("EnableiOES"sv);
}

STATICINLINE GLboolean is_enabledi(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnablediOES)
    }
    auto out = glIsEnablediOES(static_cast<GLenum>(target), index);
    detail::error_check("IsEnablediOES"sv);
    return out;
}

} // namespace gl::oes::draw_buffers_indexed
#endif // GL_OES_draw_buffers_indexed
namespace gl::oes::draw_buffers_indexed {
constexpr auto name = "GL_OES_draw_buffers_indexed";
}
