#pragma once

#ifdef GL_NV_vertex_buffer_unified_memory
namespace gl::nv::vertex_buffer_unified_memory {
namespace values {
constexpr libc_types::u32 vertex_attrib_array_unified   = 0x8F1E;
constexpr libc_types::u32 element_array_unified         = 0x8F1F;
constexpr libc_types::u32 vertex_attrib_array_address   = 0x8F20;
constexpr libc_types::u32 vertex_array_address          = 0x8F21;
constexpr libc_types::u32 normal_array_address          = 0x8F22;
constexpr libc_types::u32 color_array_address           = 0x8F23;
constexpr libc_types::u32 index_array_address           = 0x8F24;
constexpr libc_types::u32 texture_coord_array_address   = 0x8F25;
constexpr libc_types::u32 edge_flag_array_address       = 0x8F26;
constexpr libc_types::u32 secondary_color_array_address = 0x8F27;
constexpr libc_types::u32 fog_coord_array_address       = 0x8F28;
constexpr libc_types::u32 element_array_address         = 0x8F29;
constexpr libc_types::u32 vertex_attrib_array_length    = 0x8F2A;
constexpr libc_types::u32 vertex_array_length           = 0x8F2B;
constexpr libc_types::u32 normal_array_length           = 0x8F2C;
constexpr libc_types::u32 color_array_length            = 0x8F2D;
constexpr libc_types::u32 index_array_length            = 0x8F2E;
constexpr libc_types::u32 texture_coord_array_length    = 0x8F2F;
constexpr libc_types::u32 edge_flag_array_length        = 0x8F30;
constexpr libc_types::u32 secondary_color_array_length  = 0x8F31;
constexpr libc_types::u32 fog_coord_array_length        = 0x8F32;
constexpr libc_types::u32 element_array_length          = 0x8F33;
constexpr libc_types::u32 draw_indirect_unified         = 0x8F40;
constexpr libc_types::u32 draw_indirect_address         = 0x8F41;
constexpr libc_types::u32 draw_indirect_length          = 0x8F42;
} // namespace values
STATICINLINE void buffer_address_range(
    GLenum pname, u32 index, GLuint64EXT address, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferAddressRangeNV)
    }
    glBufferAddressRangeNV(pname, index, address, length);
    detail::error_check("BufferAddressRangeNV"sv);
}

STATICINLINE void color_format(i32 size, GLenum type, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorFormatNV)
    }
    glColorFormatNV(size, type, stride);
    detail::error_check("ColorFormatNV"sv);
}

STATICINLINE void edge_flag_format(i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EdgeFlagFormatNV)
    }
    glEdgeFlagFormatNV(stride);
    detail::error_check("EdgeFlagFormatNV"sv);
}

STATICINLINE void fog_coord_format(GLenum type, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogCoordFormatNV)
    }
    glFogCoordFormatNV(type, stride);
    detail::error_check("FogCoordFormatNV"sv);
}

template<class span_GLuint64EXT>
requires(semantic::concepts::Span<span_GLuint64EXT>&& std::is_same_v<
         std::decay_t<typename span_GLuint64EXT::value_type>,
         std::decay_t<GLuint64EXT>>) STATICINLINE
    void get_integerui64i_v(GLenum value, u32 index, span_GLuint64EXT result)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegerui64i_vNV)
    }
    glGetIntegerui64i_vNV(
        value,
        index,
        result.size() ? reinterpret_cast<GLuint64EXT*>(result.data())
                      : nullptr);
    detail::error_check("GetIntegerui64i_vNV"sv);
}

STATICINLINE void index_format(GLenum type, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IndexFormatNV)
    }
    glIndexFormatNV(type, stride);
    detail::error_check("IndexFormatNV"sv);
}

STATICINLINE void normal_format(GLenum type, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NormalFormatNV)
    }
    glNormalFormatNV(type, stride);
    detail::error_check("NormalFormatNV"sv);
}

STATICINLINE void secondary_color_format(
    i32 size, group::color_pointer_type type, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColorFormatNV)
    }
    glSecondaryColorFormatNV(size, static_cast<GLenum>(type), stride);
    detail::error_check("SecondaryColorFormatNV"sv);
}

STATICINLINE void tex_coord_format(i32 size, GLenum type, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordFormatNV)
    }
    glTexCoordFormatNV(size, type, stride);
    detail::error_check("TexCoordFormatNV"sv);
}

STATICINLINE void vertex_attrib_format(
    u32                       index,
    i32                       size,
    group::vertex_attrib_type type,
    bool                      normalized,
    i32                       stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribFormatNV)
    }
    glVertexAttribFormatNV(
        index, size, static_cast<GLenum>(type), normalized, stride);
    detail::error_check("VertexAttribFormatNV"sv);
}

STATICINLINE void vertex_attrib_i_format(
    u32 index, i32 size, group::vertex_attrib_int type, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribIFormatNV)
    }
    glVertexAttribIFormatNV(index, size, static_cast<GLenum>(type), stride);
    detail::error_check("VertexAttribIFormatNV"sv);
}

STATICINLINE void vertex_format(
    i32 size, group::vertex_pointer_type type, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexFormatNV)
    }
    glVertexFormatNV(size, static_cast<GLenum>(type), stride);
    detail::error_check("VertexFormatNV"sv);
}

} // namespace gl::nv::vertex_buffer_unified_memory
#endif // GL_NV_vertex_buffer_unified_memory
namespace gl::nv::vertex_buffer_unified_memory {
constexpr auto name = "GL_NV_vertex_buffer_unified_memory";
}
