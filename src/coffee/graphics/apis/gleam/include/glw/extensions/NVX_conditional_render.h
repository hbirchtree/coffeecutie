#pragma once

#ifdef GL_NVX_conditional_render
namespace gl::nvx::conditional_render {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_NVX_conditional_render
 * \param id GLuint
 * \return void
 */
STATICINLINE void begin_conditional_render(
    u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BeginConditionalRenderNVX)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBeginConditionalRenderNVX, id);
#endif
    glBeginConditionalRenderNVX(id);
    detail::error_check("BeginConditionalRenderNVX"sv, check_errors);
}

/*!
 * \brief Part of GL_NVX_conditional_render

 * \return void
 */
STATICINLINE void end_conditional_render(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(EndConditionalRenderNVX)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEndConditionalRenderNVX);
#endif
    glEndConditionalRenderNVX();
    detail::error_check("EndConditionalRenderNVX"sv, check_errors);
}

} // namespace gl::nvx::conditional_render
#endif // GL_NVX_conditional_render
namespace gl::nvx::conditional_render {
constexpr auto name = "GL_NVX_conditional_render";
}
