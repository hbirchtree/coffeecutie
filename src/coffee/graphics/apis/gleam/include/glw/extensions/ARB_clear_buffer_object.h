#pragma once

#ifdef GL_ARB_clear_buffer_object
namespace gl::arb::clear_buffer_object {
namespace values {
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_ARB_clear_buffer_object
 * \param target GLenum
 * \param internalformat GLenum
 * \param format GLenum
 * \param type GLenum
 * \param data const void *
 * \return void
 */
STATICINLINE void clear_buffer_data(
    group::buffer_storage_target target,
    group::sized_internal_format internalformat,
    group::pixel_format          format,
    group::pixel_type            type,
    span_const_void const&       data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferData)
    }
    glClearBufferData(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearBufferData"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_ARB_clear_buffer_object
 * \param target GLenum
 * \param internalformat GLenum
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param format GLenum
 * \param type GLenum
 * \param data const void *
 * \return void
 */
STATICINLINE void clear_buffer_sub_data(
    group::buffer_target_arb     target,
    group::sized_internal_format internalformat,
    GLintptr                     offset,
    GLsizeiptr                   size,
    group::pixel_format          format,
    group::pixel_type            type,
    span_const_void const&       data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferSubData)
    }
    glClearBufferSubData(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        offset,
        size,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearBufferSubData"sv);
}

} // namespace gl::arb::clear_buffer_object
#endif // GL_ARB_clear_buffer_object
namespace gl::arb::clear_buffer_object {
constexpr auto name = "GL_ARB_clear_buffer_object";
}
