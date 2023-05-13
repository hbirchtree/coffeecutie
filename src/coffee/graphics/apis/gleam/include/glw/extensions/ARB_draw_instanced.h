#pragma once

#ifdef GL_ARB_draw_instanced
namespace gl::arb::draw_instanced {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_draw_instanced
 * \param mode GLenum
 * \param first GLint
 * \param count GLsizei
 * \param primcount GLsizei
 * \return void
 */
STATICINLINE void draw_arrays_instanced(
    group::primitive_type mode, i32 first, i32 count, i32 primcount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstancedARB)
    }
    glDrawArraysInstancedARB(
        static_cast<GLenum>(mode), first, count, primcount);
    detail::error_check("DrawArraysInstancedARB"sv);
}

/*!
 * \brief Part of GL_ARB_draw_instanced
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param primcount GLsizei
 * \return void
 */
STATICINLINE void draw_elements_instanced(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       primcount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedARB)
    }
    glDrawElementsInstancedARB(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        primcount);
    detail::error_check("DrawElementsInstancedARB"sv);
}

} // namespace gl::arb::draw_instanced
#endif // GL_ARB_draw_instanced
namespace gl::arb::draw_instanced {
constexpr auto name = "GL_ARB_draw_instanced";
}
