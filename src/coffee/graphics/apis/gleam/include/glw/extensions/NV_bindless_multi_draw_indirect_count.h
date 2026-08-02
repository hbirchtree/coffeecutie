#pragma once

#ifdef GL_NV_bindless_multi_draw_indirect_count
namespace gl::nv::bindless_multi_draw_indirect_count {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_NV_bindless_multi_draw_indirect_count
 * \param mode GLenum
 * \param indirect const void *
 * \param drawCount GLsizei
 * \param maxDrawCount GLsizei
 * \param stride GLsizei
 * \param vertexBufferCount GLint
 * \return void
 */
STATICINLINE void multi_draw_arrays_indirect_bindless_count(
    group::primitive_type mode,
    intptr_t              indirect,
    i32                   drawCount,
    i32                   maxDrawCount,
    i32                   stride,
    i32                   vertexBufferCount,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirectBindlessCountNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glMultiDrawArraysIndirectBindlessCountNV,
        mode,
        indirect,
        drawCount,
        maxDrawCount,
        stride,
        vertexBufferCount);
#endif
    glMultiDrawArraysIndirectBindlessCountNV(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawCount,
        maxDrawCount,
        stride,
        vertexBufferCount);
    detail::error_check(
        "MultiDrawArraysIndirectBindlessCountNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_bindless_multi_draw_indirect_count
 * \param mode GLenum
 * \param type GLenum
 * \param indirect const void *
 * \param drawCount GLsizei
 * \param maxDrawCount GLsizei
 * \param stride GLsizei
 * \param vertexBufferCount GLint
 * \return void
 */
STATICINLINE void multi_draw_elements_indirect_bindless_count(
    group::primitive_type     mode,
    group::draw_elements_type type,
    intptr_t                  indirect,
    i32                       drawCount,
    i32                       maxDrawCount,
    i32                       stride,
    i32                       vertexBufferCount,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirectBindlessCountNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glMultiDrawElementsIndirectBindlessCountNV,
        mode,
        type,
        indirect,
        drawCount,
        maxDrawCount,
        stride,
        vertexBufferCount);
#endif
    glMultiDrawElementsIndirectBindlessCountNV(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawCount,
        maxDrawCount,
        stride,
        vertexBufferCount);
    detail::error_check(
        "MultiDrawElementsIndirectBindlessCountNV"sv, check_errors);
}

} // namespace gl::nv::bindless_multi_draw_indirect_count
#endif // GL_NV_bindless_multi_draw_indirect_count
namespace gl::nv::bindless_multi_draw_indirect_count {
constexpr auto name = "GL_NV_bindless_multi_draw_indirect_count";
}
