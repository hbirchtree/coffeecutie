#pragma once

#ifdef GL_EXT_multi_draw_indirect
namespace gl::ext::multi_draw_indirect {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_EXT_multi_draw_indirect
 * \param mode GLenum
 * \param indirect const void *
 * \param drawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_arrays_indirect(
    group::primitive_type mode, intptr_t indirect, i32 drawcount, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirectEXT)
    }
    glMultiDrawArraysIndirectEXT(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        stride);
    detail::error_check("MultiDrawArraysIndirectEXT"sv);
}

/*!
 * \brief Part of GL_EXT_multi_draw_indirect
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
    intptr_t                  indirect,
    i32                       drawcount,
    i32                       stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirectEXT)
    }
    glMultiDrawElementsIndirectEXT(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        stride);
    detail::error_check("MultiDrawElementsIndirectEXT"sv);
}

} // namespace gl::ext::multi_draw_indirect
#endif // GL_EXT_multi_draw_indirect
namespace gl::ext::multi_draw_indirect {
constexpr auto name = "GL_EXT_multi_draw_indirect";
}
