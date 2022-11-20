#pragma once

#ifdef GL_INTEL_parallel_arrays
namespace gl::intel::parallel_arrays {
namespace values {
constexpr libc_types::u32 parallel_arrays                       = 0x83F4;
constexpr libc_types::u32 vertex_array_parallel_pointers        = 0x83F5;
constexpr libc_types::u32 normal_array_parallel_pointers        = 0x83F6;
constexpr libc_types::u32 color_array_parallel_pointers         = 0x83F7;
constexpr libc_types::u32 texture_coord_array_parallel_pointers = 0x83F8;
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_INTEL_parallel_arrays
     * \param size GLint
     * \param type GLenum
     * \param pointer const void **
     * \return void
     */
    STATICINLINE void color_pointerv(
        i32                        size,
        group::vertex_pointer_type type,
        span_const_void const&     pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorPointervINTEL)
    }
    glColorPointervINTEL(
        size,
        static_cast<GLenum>(type),
        pointer.size() ? reinterpret_cast<const void**>(pointer.data())
                       : nullptr);
    detail::error_check("ColorPointervINTEL"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_INTEL_parallel_arrays
     * \param type GLenum
     * \param pointer const void **
     * \return void
     */
    STATICINLINE void normal_pointerv(
        group::normal_pointer_type type, span_const_void const& pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NormalPointervINTEL)
    }
    glNormalPointervINTEL(
        static_cast<GLenum>(type),
        pointer.size() ? reinterpret_cast<const void**>(pointer.data())
                       : nullptr);
    detail::error_check("NormalPointervINTEL"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_INTEL_parallel_arrays
     * \param size GLint
     * \param type GLenum
     * \param pointer const void **
     * \return void
     */
    STATICINLINE void tex_coord_pointerv(
        i32                        size,
        group::vertex_pointer_type type,
        span_const_void const&     pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordPointervINTEL)
    }
    glTexCoordPointervINTEL(
        size,
        static_cast<GLenum>(type),
        pointer.size() ? reinterpret_cast<const void**>(pointer.data())
                       : nullptr);
    detail::error_check("TexCoordPointervINTEL"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_INTEL_parallel_arrays
     * \param size GLint
     * \param type GLenum
     * \param pointer const void **
     * \return void
     */
    STATICINLINE void vertex_pointerv(
        i32                        size,
        group::vertex_pointer_type type,
        span_const_void const&     pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexPointervINTEL)
    }
    glVertexPointervINTEL(
        size,
        static_cast<GLenum>(type),
        pointer.size() ? reinterpret_cast<const void**>(pointer.data())
                       : nullptr);
    detail::error_check("VertexPointervINTEL"sv);
}

} // namespace gl::intel::parallel_arrays
#endif // GL_INTEL_parallel_arrays
namespace gl::intel::parallel_arrays {
constexpr auto name = "GL_INTEL_parallel_arrays";
}
