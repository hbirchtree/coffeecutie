#pragma once

#ifdef GL_ARB_matrix_palette
namespace gl::arb::matrix_palette {
namespace values {
constexpr libc_types::u32 matrix_palette                 = 0x8840;
constexpr libc_types::u32 max_matrix_palette_stack_depth = 0x8841;
constexpr libc_types::u32 max_palette_matrices           = 0x8842;
constexpr libc_types::u32 current_palette_matrix         = 0x8843;
constexpr libc_types::u32 matrix_index_array             = 0x8844;
constexpr libc_types::u32 current_matrix_index           = 0x8845;
constexpr libc_types::u32 matrix_index_array_size        = 0x8846;
constexpr libc_types::u32 matrix_index_array_type        = 0x8847;
constexpr libc_types::u32 matrix_index_array_stride      = 0x8848;
constexpr libc_types::u32 matrix_index_array_pointer     = 0x8849;
} // namespace values
/*!
 * \brief Part of GL_ARB_matrix_palette
 * \param index GLint
 * \return void
 */
STATICINLINE void current_palette_matrix(i32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CurrentPaletteMatrixARB)
    }
    glCurrentPaletteMatrixARB(index);
    detail::error_check("CurrentPaletteMatrixARB"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_ARB_matrix_palette
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void matrix_index_pointer(
    i32                                  size,
    group::matrix_index_pointer_type_arb type,
    i32                                  stride,
    span_const_void const&               pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixIndexPointerARB)
    }
    glMatrixIndexPointerARB(
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("MatrixIndexPointerARB"sv);
}

template<class span_const_u8>
requires(
    semantic::concepts::Span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>>)
/*!
 * \brief Part of GL_ARB_matrix_palette
 * \param size GLint
 * \param indices const GLubyte *
 * \return void
 */
STATICINLINE void matrix_indexubv(span_const_u8 const& indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixIndexubvARB)
    }
    glMatrixIndexubvARB(
        indices.size(),
        indices.size() ? reinterpret_cast<const GLubyte*>(indices.data())
                       : nullptr);
    detail::error_check("MatrixIndexubvARB"sv);
}

template<class span_const_u32>
requires(
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_matrix_palette
 * \param size GLint
 * \param indices const GLuint *
 * \return void
 */
STATICINLINE void matrix_indexuiv(span_const_u32 const& indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixIndexuivARB)
    }
    glMatrixIndexuivARB(
        indices.size(),
        indices.size() ? reinterpret_cast<const GLuint*>(indices.data())
                       : nullptr);
    detail::error_check("MatrixIndexuivARB"sv);
}

template<class span_const_u16>
requires(
    semantic::concepts::Span<span_const_u16> &&
    std::is_same_v<
        std::decay_t<typename span_const_u16::value_type>,
        std::decay_t<u16>>)
/*!
 * \brief Part of GL_ARB_matrix_palette
 * \param size GLint
 * \param indices const GLushort *
 * \return void
 */
STATICINLINE void matrix_indexusv(span_const_u16 const& indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixIndexusvARB)
    }
    glMatrixIndexusvARB(
        indices.size(),
        indices.size() ? reinterpret_cast<const GLushort*>(indices.data())
                       : nullptr);
    detail::error_check("MatrixIndexusvARB"sv);
}

} // namespace gl::arb::matrix_palette
#endif // GL_ARB_matrix_palette
namespace gl::arb::matrix_palette {
constexpr auto name = "GL_ARB_matrix_palette";
}
