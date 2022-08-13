#pragma once

#ifdef GL_ARB_draw_buffers_blend
namespace gl::arb::draw_buffers_blend {
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
        GLW_FPTR_CHECK(BlendEquationSeparateiARB)
    }
    glBlendEquationSeparateiARB(
        buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparateiARB"sv);
}

STATICINLINE void blend_equationi(u32 buf, group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationiARB)
    }
    glBlendEquationiARB(buf, static_cast<GLenum>(mode));
    detail::error_check("BlendEquationiARB"sv);
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
        GLW_FPTR_CHECK(BlendFuncSeparateiARB)
    }
    glBlendFuncSeparateiARB(
        buf,
        static_cast<GLenum>(srcRGB),
        static_cast<GLenum>(dstRGB),
        static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    detail::error_check("BlendFuncSeparateiARB"sv);
}

STATICINLINE void blend_funci(
    u32 buf, group::blending_factor src, group::blending_factor dst)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFunciARB)
    }
    glBlendFunciARB(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    detail::error_check("BlendFunciARB"sv);
}

} // namespace gl::arb::draw_buffers_blend
#endif // GL_ARB_draw_buffers_blend
namespace gl::arb::draw_buffers_blend {
constexpr auto name = "GL_ARB_draw_buffers_blend";
}