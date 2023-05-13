#pragma once

#ifdef GL_NV_vertex_buffer_unified_memory
namespace gl::nv::vertex_buffer_unified_memory {
namespace values {
constexpr u32 vertex_attrib_array_unified   = 0x8F1E;
constexpr u32 element_array_unified         = 0x8F1F;
constexpr u32 vertex_attrib_array_address   = 0x8F20;
constexpr u32 vertex_array_address          = 0x8F21;
constexpr u32 normal_array_address          = 0x8F22;
constexpr u32 color_array_address           = 0x8F23;
constexpr u32 index_array_address           = 0x8F24;
constexpr u32 texture_coord_array_address   = 0x8F25;
constexpr u32 edge_flag_array_address       = 0x8F26;
constexpr u32 secondary_color_array_address = 0x8F27;
constexpr u32 fog_coord_array_address       = 0x8F28;
constexpr u32 element_array_address         = 0x8F29;
constexpr u32 vertex_attrib_array_length    = 0x8F2A;
constexpr u32 vertex_array_length           = 0x8F2B;
constexpr u32 normal_array_length           = 0x8F2C;
constexpr u32 color_array_length            = 0x8F2D;
constexpr u32 index_array_length            = 0x8F2E;
constexpr u32 texture_coord_array_length    = 0x8F2F;
constexpr u32 edge_flag_array_length        = 0x8F30;
constexpr u32 secondary_color_array_length  = 0x8F31;
constexpr u32 fog_coord_array_length        = 0x8F32;
constexpr u32 element_array_length          = 0x8F33;
constexpr u32 draw_indirect_unified         = 0x8F40;
constexpr u32 draw_indirect_address         = 0x8F41;
constexpr u32 draw_indirect_length          = 0x8F42;
} // namespace values
/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param pname GLenum
 * \param index GLuint
 * \param address GLuint64EXT
 * \param length GLsizeiptr
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param stride GLsizei
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
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
requires(concepts::span<span_GLuint64EXT>&& std::is_same_v<
         std::decay_t<typename span_GLuint64EXT::value_type>,
         std::decay_t<GLuint64EXT>>)
    /*!
     * \brief Part of GL_NV_vertex_buffer_unified_memory
     * \param value GLenum
     * \param index GLuint
     * \param result GLuint64EXT *
     * \return void
     */
    STATICINLINE
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param normalized GLboolean
 * \param stride GLsizei
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
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

/*!
 * \brief Part of GL_NV_vertex_buffer_unified_memory
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \return void
 */
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
