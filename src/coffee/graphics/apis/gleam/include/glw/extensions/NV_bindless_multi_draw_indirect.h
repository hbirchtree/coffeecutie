#pragma once

#ifdef GL_NV_bindless_multi_draw_indirect
namespace gl::nv::bindless_multi_draw_indirect {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_NV_bindless_multi_draw_indirect
 * \param mode GLenum
 * \param indirect const void *
 * \param drawCount GLsizei
 * \param stride GLsizei
 * \param vertexBufferCount GLint
 * \return void
 */
STATICINLINE void multi_draw_arrays_indirect_bindless(
    group::primitive_type mode,
    intptr_t              indirect,
    i32                   drawCount,
    i32                   stride,
    i32                   vertexBufferCount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirectBindlessNV)
    }
    glMultiDrawArraysIndirectBindlessNV(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawCount,
        stride,
        vertexBufferCount);
    detail::error_check("MultiDrawArraysIndirectBindlessNV"sv);
}

/*!
 * \brief Part of GL_NV_bindless_multi_draw_indirect
 * \param mode GLenum
 * \param type GLenum
 * \param indirect const void *
 * \param drawCount GLsizei
 * \param stride GLsizei
 * \param vertexBufferCount GLint
 * \return void
 */
STATICINLINE void multi_draw_elements_indirect_bindless(
    group::primitive_type     mode,
    group::draw_elements_type type,
    intptr_t                  indirect,
    i32                       drawCount,
    i32                       stride,
    i32                       vertexBufferCount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirectBindlessNV)
    }
    glMultiDrawElementsIndirectBindlessNV(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawCount,
        stride,
        vertexBufferCount);
    detail::error_check("MultiDrawElementsIndirectBindlessNV"sv);
}

} // namespace gl::nv::bindless_multi_draw_indirect
#endif // GL_NV_bindless_multi_draw_indirect
namespace gl::nv::bindless_multi_draw_indirect {
constexpr auto name = "GL_NV_bindless_multi_draw_indirect";
}
