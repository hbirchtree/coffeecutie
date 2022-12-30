#pragma once

#ifdef GL_EXT_secondary_color
namespace gl::ext::secondary_color {
namespace values {
constexpr libc_types::u32 color_sum                     = 0x8458;
constexpr libc_types::u32 current_secondary_color       = 0x8459;
constexpr libc_types::u32 secondary_color_array_size    = 0x845A;
constexpr libc_types::u32 secondary_color_array_type    = 0x845B;
constexpr libc_types::u32 secondary_color_array_stride  = 0x845C;
constexpr libc_types::u32 secondary_color_array_pointer = 0x845D;
constexpr libc_types::u32 secondary_color_array         = 0x845E;
} // namespace values
template<class vec_3_i8>
requires(semantic::concepts::Vector<vec_3_i8, i8, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param red GLbyte
 * \param green GLbyte
 * \param blue GLbyte
 * \return void
 */
STATICINLINE void secondary_color3b(vec_3_i8 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3bEXT)
    }
    glSecondaryColor3bEXT(red.x(), red.y(), red.z());
    detail::error_check("SecondaryColor3bEXT"sv);
}

template<class span_const_i8>
requires(
    semantic::concepts::Span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param v const GLbyte *
 * \return void
 */
STATICINLINE void secondary_color3bv(span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3bvEXT)
    }
    glSecondaryColor3bvEXT(
        v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("SecondaryColor3bvEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param red GLdouble
 * \param green GLdouble
 * \param blue GLdouble
 * \return void
 */
STATICINLINE void secondary_color3d(vec_3_f64 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3dEXT)
    }
    glSecondaryColor3dEXT(red.x(), red.y(), red.z());
    detail::error_check("SecondaryColor3dEXT"sv);
}

template<class span_const_vec_3_f64>
requires(
    semantic::concepts::Span<span_const_vec_3_f64> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void secondary_color3dv(span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3dvEXT)
    }
    glSecondaryColor3dvEXT(reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("SecondaryColor3dvEXT"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param red GLfloat
 * \param green GLfloat
 * \param blue GLfloat
 * \return void
 */
STATICINLINE void secondary_color3f(vec_3_f32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3fEXT)
    }
    glSecondaryColor3fEXT(red.x(), red.y(), red.z());
    detail::error_check("SecondaryColor3fEXT"sv);
}

template<class span_const_vec_3_f32>
requires(
    semantic::concepts::Span<span_const_vec_3_f32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void secondary_color3fv(span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3fvEXT)
    }
    glSecondaryColor3fvEXT(reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("SecondaryColor3fvEXT"sv);
}

template<class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param red GLint
 * \param green GLint
 * \param blue GLint
 * \return void
 */
STATICINLINE void secondary_color3i(vec_3_i32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3iEXT)
    }
    glSecondaryColor3iEXT(red.x(), red.y(), red.z());
    detail::error_check("SecondaryColor3iEXT"sv);
}

template<class span_const_vec_3_i32>
requires(
    semantic::concepts::Span<span_const_vec_3_i32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param v const GLint *
 * \return void
 */
STATICINLINE void secondary_color3iv(span_const_vec_3_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3ivEXT)
    }
    glSecondaryColor3ivEXT(reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("SecondaryColor3ivEXT"sv);
}

template<class vec_3_i16>
requires(semantic::concepts::Vector<vec_3_i16, i16, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param red GLshort
 * \param green GLshort
 * \param blue GLshort
 * \return void
 */
STATICINLINE void secondary_color3s(vec_3_i16 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3sEXT)
    }
    glSecondaryColor3sEXT(red.x(), red.y(), red.z());
    detail::error_check("SecondaryColor3sEXT"sv);
}

template<class span_const_vec_3_i16>
requires(
    semantic::concepts::Span<span_const_vec_3_i16> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_i16::value_type, i16, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void secondary_color3sv(span_const_vec_3_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3svEXT)
    }
    glSecondaryColor3svEXT(reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("SecondaryColor3svEXT"sv);
}

template<class vec_3_u8>
requires(semantic::concepts::Vector<vec_3_u8, u8, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param red GLubyte
 * \param green GLubyte
 * \param blue GLubyte
 * \return void
 */
STATICINLINE void secondary_color3ub(vec_3_u8 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3ubEXT)
    }
    glSecondaryColor3ubEXT(red.x(), red.y(), red.z());
    detail::error_check("SecondaryColor3ubEXT"sv);
}

template<class span_const_u8>
requires(
    semantic::concepts::Span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param v const GLubyte *
 * \return void
 */
STATICINLINE void secondary_color3ubv(span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3ubvEXT)
    }
    glSecondaryColor3ubvEXT(
        v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("SecondaryColor3ubvEXT"sv);
}

template<class vec_3_u32>
requires(semantic::concepts::Vector<vec_3_u32, u32, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param red GLuint
 * \param green GLuint
 * \param blue GLuint
 * \return void
 */
STATICINLINE void secondary_color3ui(vec_3_u32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3uiEXT)
    }
    glSecondaryColor3uiEXT(red.x(), red.y(), red.z());
    detail::error_check("SecondaryColor3uiEXT"sv);
}

template<class span_const_vec_3_u32>
requires(
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param v const GLuint *
 * \return void
 */
STATICINLINE void secondary_color3uiv(span_const_vec_3_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3uivEXT)
    }
    glSecondaryColor3uivEXT(reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("SecondaryColor3uivEXT"sv);
}

template<class vec_3_u16>
requires(semantic::concepts::Vector<vec_3_u16, u16, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param red GLushort
 * \param green GLushort
 * \param blue GLushort
 * \return void
 */
STATICINLINE void secondary_color3us(vec_3_u16 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3usEXT)
    }
    glSecondaryColor3usEXT(red.x(), red.y(), red.z());
    detail::error_check("SecondaryColor3usEXT"sv);
}

template<class span_const_vec_3_u16>
requires(
    semantic::concepts::Span<span_const_vec_3_u16> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u16::value_type, u16, 3>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param v const GLushort *
 * \return void
 */
STATICINLINE void secondary_color3usv(span_const_vec_3_u16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColor3usvEXT)
    }
    glSecondaryColor3usvEXT(reinterpret_cast<const GLushort*>(v.data()));
    detail::error_check("SecondaryColor3usvEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_EXT_secondary_color
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void secondary_color_pointer(
    i32                       size,
    group::color_pointer_type type,
    i32                       stride,
    span_const_void const&    pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColorPointerEXT)
    }
    glSecondaryColorPointerEXT(
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("SecondaryColorPointerEXT"sv);
}

} // namespace gl::ext::secondary_color
#endif // GL_EXT_secondary_color
namespace gl::ext::secondary_color {
constexpr auto name = "GL_EXT_secondary_color";
}
