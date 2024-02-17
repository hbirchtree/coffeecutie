#pragma once

#ifdef GL_ARB_base_instance
namespace gl::arb::base_instance {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_base_instance
 * \param mode GLenum
 * \param first GLint
 * \param count GLsizei
 * \param instancecount GLsizei
 * \param baseinstance GLuint
 * \return void
 */
STATICINLINE void draw_arrays_instanced_base_instance(
    group::primitive_type mode,
    i32                   first,
    i32                   count,
    i32                   instancecount,
    u32                   baseinstance)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstancedBaseInstance)
    }
    glDrawArraysInstancedBaseInstance(
        static_cast<GLenum>(mode), first, count, instancecount, baseinstance);
    detail::error_check("DrawArraysInstancedBaseInstance"sv);
}

/*!
 * \brief Part of GL_ARB_base_instance
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param instancecount GLsizei
 * \param baseinstance GLuint
 * \return void
 */
STATICINLINE void draw_elements_instanced_base_instance(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       instancecount,
    u32                       baseinstance)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedBaseInstance)
    }
    glDrawElementsInstancedBaseInstance(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount,
        baseinstance);
    detail::error_check("DrawElementsInstancedBaseInstance"sv);
}

/*!
 * \brief Part of GL_ARB_base_instance
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param instancecount GLsizei
 * \param basevertex GLint
 * \param baseinstance GLuint
 * \return void
 */
STATICINLINE void draw_elements_instanced_base_vertex_base_instance(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       instancecount,
    i32                       basevertex,
    u32                       baseinstance)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedBaseVertexBaseInstance)
    }
    glDrawElementsInstancedBaseVertexBaseInstance(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount,
        basevertex,
        baseinstance);
    detail::error_check("DrawElementsInstancedBaseVertexBaseInstance"sv);
}

} // namespace gl::arb::base_instance
#endif // GL_ARB_base_instance
namespace gl::arb::base_instance {
constexpr auto name = "GL_ARB_base_instance";
}
