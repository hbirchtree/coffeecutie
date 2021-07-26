#pragma once

#ifdef GL_NV_vertex_program4
namespace gl::nv::vertex_program4 {
namespace values {
constexpr libc_types::u32 vertex_attrib_array_integer = 0x88FD;
} // namespace values
STATICINLINE void get_vertex_attrib_iiv_ext(
    u32 index, group::vertex_attrib_enum pname, i32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIivEXT)
    }
    glGetVertexAttribIivEXT(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIivEXT"sv);
}

STATICINLINE void get_vertex_attrib_iuiv_ext(
    u32 index, group::vertex_attrib_enum pname, u32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIuivEXT)
    }
    glGetVertexAttribIuivEXT(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIuivEXT"sv);
}

STATICINLINE void vertex_attrib_i1i_ext(u32 index, i32 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1iEXT)
    }
    glVertexAttribI1iEXT(index, x);
    detail::error_check("VertexAttribI1iEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void vertex_attrib_i1iv_ext(u32 index, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1ivEXT)
    }
    glVertexAttribI1ivEXT(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI1ivEXT"sv);
}

STATICINLINE void vertex_attrib_i1ui_ext(u32 index, u32 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1uiEXT)
    }
    glVertexAttribI1uiEXT(index, x);
    detail::error_check("VertexAttribI1uiEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void vertex_attrib_i1uiv_ext(u32 index, span_const_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1uivEXT)
    }
    glVertexAttribI1uivEXT(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI1uivEXT"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void vertex_attrib_i2i_ext(u32 index, vec_2_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2iEXT)
    }
    glVertexAttribI2iEXT(index, x.x(), x.y());
    detail::error_check("VertexAttribI2iEXT"sv);
}

template<class span_const_vec_2_i32>
requires(semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
             Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    STATICINLINE
    void vertex_attrib_i2iv_ext(u32 index, span_const_vec_2_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2ivEXT)
    }
    glVertexAttribI2ivEXT(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI2ivEXT"sv);
}

template<class vec_2_u32>
requires(semantic::concepts::Vector<vec_2_u32, u32, 2>) STATICINLINE
    void vertex_attrib_i2ui_ext(u32 index, vec_2_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2uiEXT)
    }
    glVertexAttribI2uiEXT(index, x.x(), x.y());
    detail::error_check("VertexAttribI2uiEXT"sv);
}

template<class span_const_vec_2_u32>
requires(semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
             Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    STATICINLINE
    void vertex_attrib_i2uiv_ext(u32 index, span_const_vec_2_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2uivEXT)
    }
    glVertexAttribI2uivEXT(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI2uivEXT"sv);
}

template<class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>) STATICINLINE
    void vertex_attrib_i3i_ext(u32 index, vec_3_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3iEXT)
    }
    glVertexAttribI3iEXT(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttribI3iEXT"sv);
}

template<class span_const_vec_3_i32>
requires(semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
             Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    STATICINLINE
    void vertex_attrib_i3iv_ext(u32 index, span_const_vec_3_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3ivEXT)
    }
    glVertexAttribI3ivEXT(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI3ivEXT"sv);
}

template<class vec_3_u32>
requires(semantic::concepts::Vector<vec_3_u32, u32, 3>) STATICINLINE
    void vertex_attrib_i3ui_ext(u32 index, vec_3_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3uiEXT)
    }
    glVertexAttribI3uiEXT(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttribI3uiEXT"sv);
}

template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    STATICINLINE
    void vertex_attrib_i3uiv_ext(u32 index, span_const_vec_3_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3uivEXT)
    }
    glVertexAttribI3uivEXT(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI3uivEXT"sv);
}

template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void vertex_attrib_i4bv_ext(u32 index, span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4bvEXT)
    }
    glVertexAttribI4bvEXT(
        index, v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttribI4bvEXT"sv);
}

template<class vec_4_i32>
requires(semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
    void vertex_attrib_i4i_ext(u32 index, vec_4_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4iEXT)
    }
    glVertexAttribI4iEXT(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribI4iEXT"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE
    void vertex_attrib_i4iv_ext(u32 index, span_const_vec_4_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4ivEXT)
    }
    glVertexAttribI4ivEXT(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI4ivEXT"sv);
}

template<class span_const_vec_4_i16>
requires(semantic::concepts::Span<span_const_vec_4_i16>&& semantic::concepts::
             Vector<typename span_const_vec_4_i16::value_type, i16, 4>)
    STATICINLINE
    void vertex_attrib_i4sv_ext(u32 index, span_const_vec_4_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4svEXT)
    }
    glVertexAttribI4svEXT(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttribI4svEXT"sv);
}

template<class span_const_u8>
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>) STATICINLINE
    void vertex_attrib_i4ubv_ext(u32 index, span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4ubvEXT)
    }
    glVertexAttribI4ubvEXT(
        index, v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttribI4ubvEXT"sv);
}

template<class vec_4_u32>
requires(semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
    void vertex_attrib_i4ui_ext(u32 index, vec_4_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4uiEXT)
    }
    glVertexAttribI4uiEXT(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribI4uiEXT"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE
    void vertex_attrib_i4uiv_ext(u32 index, span_const_vec_4_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4uivEXT)
    }
    glVertexAttribI4uivEXT(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI4uivEXT"sv);
}

template<class span_const_vec_4_u16>
requires(semantic::concepts::Span<span_const_vec_4_u16>&& semantic::concepts::
             Vector<typename span_const_vec_4_u16::value_type, u16, 4>)
    STATICINLINE
    void vertex_attrib_i4usv_ext(u32 index, span_const_vec_4_u16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4usvEXT)
    }
    glVertexAttribI4usvEXT(index, reinterpret_cast<const GLushort*>(v.data()));
    detail::error_check("VertexAttribI4usvEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void vertex_attrib_i_pointer_ext(
        u32                      index,
        i32                      size,
        group::vertex_attrib_int type,
        i32                      stride,
        span_const_void const&   pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribIPointerEXT)
    }
    glVertexAttribIPointerEXT(
        index,
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribIPointerEXT"sv);
}

} // namespace gl::nv::vertex_program4
#endif // GL_NV_vertex_program4
namespace gl::nv::vertex_program4 {
constexpr auto name = "GL_NV_vertex_program4";
}
