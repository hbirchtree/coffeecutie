#pragma once

#ifdef GL_NV_vertex_array_range
namespace gl::nv::vertex_array_range {
namespace values {
constexpr libc_types::u32 vertex_array_range             = 0x851D;
constexpr libc_types::u32 vertex_array_range_length      = 0x851E;
constexpr libc_types::u32 vertex_array_range_valid       = 0x851F;
constexpr libc_types::u32 max_vertex_array_range_element = 0x8520;
constexpr libc_types::u32 vertex_array_range_pointer     = 0x8521;
} // namespace values
/*!
 * \brief Part of GL_NV_vertex_array_range

 * \return void
 */
STATICINLINE void flush_vertex_array_range()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushVertexArrayRangeNV)
    }
    glFlushVertexArrayRangeNV();
    detail::error_check("FlushVertexArrayRangeNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_NV_vertex_array_range
 * \param length GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void vertex_array_range(i32 length, span_const_void const& pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayRangeNV)
    }
    glVertexArrayRangeNV(
        length,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexArrayRangeNV"sv);
}

} // namespace gl::nv::vertex_array_range
#endif // GL_NV_vertex_array_range
namespace gl::nv::vertex_array_range {
constexpr auto name = "GL_NV_vertex_array_range";
}
