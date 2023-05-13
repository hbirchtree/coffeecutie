#pragma once

#ifdef GL_ARB_indirect_parameters
namespace gl::arb::indirect_parameters {
namespace values {
constexpr u32 parameter_buffer         = 0x80EE;
constexpr u32 parameter_buffer_binding = 0x80EF;
} // namespace values
/*!
 * \brief Part of GL_ARB_indirect_parameters
 * \param mode GLenum
 * \param indirect const void *
 * \param drawcount GLintptr
 * \param maxdrawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_arrays_indirect_count(
    group::primitive_type mode,
    intptr_t              indirect,
    GLintptr              drawcount,
    i32                   maxdrawcount,
    i32                   stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirectCountARB)
    }
    glMultiDrawArraysIndirectCountARB(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        maxdrawcount,
        stride);
    detail::error_check("MultiDrawArraysIndirectCountARB"sv);
}

/*!
 * \brief Part of GL_ARB_indirect_parameters
 * \param mode GLenum
 * \param type GLenum
 * \param indirect const void *
 * \param drawcount GLintptr
 * \param maxdrawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_elements_indirect_count(
    group::primitive_type     mode,
    group::draw_elements_type type,
    intptr_t                  indirect,
    GLintptr                  drawcount,
    i32                       maxdrawcount,
    i32                       stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirectCountARB)
    }
    glMultiDrawElementsIndirectCountARB(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        maxdrawcount,
        stride);
    detail::error_check("MultiDrawElementsIndirectCountARB"sv);
}

} // namespace gl::arb::indirect_parameters
#endif // GL_ARB_indirect_parameters
namespace gl::arb::indirect_parameters {
constexpr auto name = "GL_ARB_indirect_parameters";
}
