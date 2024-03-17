#pragma once

#ifdef GL_NV_query_resource
namespace gl::nv::query_resource {
namespace values {
constexpr u32 query_resource_type_vidmem_alloc = 0x9540;
constexpr u32 query_resource_memtype_vidmem    = 0x9542;
constexpr u32 query_resource_sys_reserved      = 0x9544;
constexpr u32 query_resource_texture           = 0x9545;
constexpr u32 query_resource_renderbuffer      = 0x9546;
constexpr u32 query_resource_bufferobject      = 0x9547;
} // namespace values

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_query_resource
 * \param queryType GLenum
 * \param tagId GLint
 * \param count GLuint
 * \param buffer GLint *
 * \return GLint
 */
STATICINLINE GLint query_resource(
    GLenum      queryType,
    i32         tagId,
    span_i32    buffer,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryResourceNV)
    }
    auto out = glQueryResourceNV(
        queryType,
        tagId,
        buffer.size(),
        buffer.size() ? reinterpret_cast<GLint*>(buffer.data()) : nullptr);
    detail::error_check("QueryResourceNV"sv, check_errors);
    return out;
}

} // namespace gl::nv::query_resource
#endif // GL_NV_query_resource
namespace gl::nv::query_resource {
constexpr auto name = "GL_NV_query_resource";
}
