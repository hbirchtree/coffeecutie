#pragma once

#ifdef GL_AMD_draw_buffers_blend
namespace gl::amd::draw_buffers_blend {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_AMD_draw_buffers_blend
 * \param buf GLuint
 * \param mode GLenum
 * \return void
 */
STATICINLINE void blend_equation_indexed(
    u32 buf, group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationIndexedAMD)
    }
    glBlendEquationIndexedAMD(buf, static_cast<GLenum>(mode));
    detail::error_check("BlendEquationIndexedAMD"sv);
}

/*!
 * \brief Part of GL_AMD_draw_buffers_blend
 * \param buf GLuint
 * \param modeRGB GLenum
 * \param modeAlpha GLenum
 * \return void
 */
STATICINLINE void blend_equation_separate_indexed(
    u32                            buf,
    group::blend_equation_mode_ext modeRGB,
    group::blend_equation_mode_ext modeAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparateIndexedAMD)
    }
    glBlendEquationSeparateIndexedAMD(
        buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparateIndexedAMD"sv);
}

/*!
 * \brief Part of GL_AMD_draw_buffers_blend
 * \param buf GLuint
 * \param src GLenum
 * \param dst GLenum
 * \return void
 */
STATICINLINE void blend_func_indexed(u32 buf, GLenum src, GLenum dst)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncIndexedAMD)
    }
    glBlendFuncIndexedAMD(buf, src, dst);
    detail::error_check("BlendFuncIndexedAMD"sv);
}

/*!
 * \brief Part of GL_AMD_draw_buffers_blend
 * \param buf GLuint
 * \param srcRGB GLenum
 * \param dstRGB GLenum
 * \param srcAlpha GLenum
 * \param dstAlpha GLenum
 * \return void
 */
STATICINLINE void blend_func_separate_indexed(
    u32                    buf,
    group::blending_factor srcRGB,
    group::blending_factor dstRGB,
    group::blending_factor srcAlpha,
    group::blending_factor dstAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncSeparateIndexedAMD)
    }
    glBlendFuncSeparateIndexedAMD(
        buf,
        static_cast<GLenum>(srcRGB),
        static_cast<GLenum>(dstRGB),
        static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    detail::error_check("BlendFuncSeparateIndexedAMD"sv);
}

} // namespace gl::amd::draw_buffers_blend
#endif // GL_AMD_draw_buffers_blend
namespace gl::amd::draw_buffers_blend {
constexpr auto name = "GL_AMD_draw_buffers_blend";
}
