#pragma once

#ifdef GL_EXT_instanced_arrays
namespace gl::ext::instanced_arrays {
namespace values {
constexpr u32 vertex_attrib_array_divisor = 0x88FE;
} // namespace values
/*!
 * \brief Part of GL_EXT_instanced_arrays
 * \param mode GLenum
 * \param start GLint
 * \param count GLsizei
 * \param primcount GLsizei
 * \return void
 */
STATICINLINE void draw_arrays_instanced(
    group::primitive_type mode, i32 start, i32 count, i32 primcount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstancedEXT)
    }
    glDrawArraysInstancedEXT(
        static_cast<GLenum>(mode), start, count, primcount);
    detail::error_check("DrawArraysInstancedEXT"sv);
}

/*!
 * \brief Part of GL_EXT_instanced_arrays
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
        GLW_FPTR_CHECK(DrawElementsInstancedEXT)
    }
    glDrawElementsInstancedEXT(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        primcount);
    detail::error_check("DrawElementsInstancedEXT"sv);
}

/*!
 * \brief Part of GL_EXT_instanced_arrays
 * \param index GLuint
 * \param divisor GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_divisor(u32 index, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribDivisorEXT)
    }
    glVertexAttribDivisorEXT(index, divisor);
    detail::error_check("VertexAttribDivisorEXT"sv);
}

} // namespace gl::ext::instanced_arrays
#endif // GL_EXT_instanced_arrays
namespace gl::ext::instanced_arrays {
constexpr auto name = "GL_EXT_instanced_arrays";
}
