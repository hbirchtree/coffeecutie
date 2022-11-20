#pragma once

#ifdef GL_ARB_multi_draw_indirect
namespace gl::arb::multi_draw_indirect {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_multi_draw_indirect
 * \param mode GLenum
 * \param indirect const void *
 * \param drawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_arrays_indirect(
    group::primitive_type mode, ptroff indirect, i32 drawcount, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirect)
    }
    glMultiDrawArraysIndirect(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        stride);
    detail::error_check("MultiDrawArraysIndirect"sv);
}

/*!
 * \brief Part of GL_ARB_multi_draw_indirect
 * \param mode GLenum
 * \param type GLenum
 * \param indirect const void *
 * \param drawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_elements_indirect(
    group::primitive_type     mode,
    group::draw_elements_type type,
    ptroff                    indirect,
    i32                       drawcount,
    i32                       stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirect)
    }
    glMultiDrawElementsIndirect(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        stride);
    detail::error_check("MultiDrawElementsIndirect"sv);
}

} // namespace gl::arb::multi_draw_indirect
#endif // GL_ARB_multi_draw_indirect
namespace gl::arb::multi_draw_indirect {
constexpr auto name = "GL_ARB_multi_draw_indirect";
}
