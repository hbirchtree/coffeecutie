#pragma once

#ifdef GL_NV_fragment_program
namespace gl::nv::fragment_program {
namespace values {
constexpr libc_types::u32 max_fragment_program_local_parameters = 0x8868;
constexpr libc_types::u32 fragment_program                      = 0x8870;
constexpr libc_types::u32 max_texture_coords                    = 0x8871;
constexpr libc_types::u32 max_texture_image_units               = 0x8872;
constexpr libc_types::u32 fragment_program_binding              = 0x8873;
constexpr libc_types::u32 program_error_string                  = 0x8874;
} // namespace values
template<class span_const_u8, class span_f64>
requires(
    semantic::concepts::Span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>> &&
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<
        std::decay_t<typename span_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_NV_fragment_program
 * \param id GLuint
 * \param len GLsizei
 * \param name const GLubyte *
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_program_named_parameter(
    u32 id, i32 len, span_const_u8 const& name, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramNamedParameterdvNV)
    }
    glGetProgramNamedParameterdvNV(
        id,
        len,
        name.size() ? reinterpret_cast<const GLubyte*>(name.data()) : nullptr,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetProgramNamedParameterdvNV"sv);
}

template<class span_const_u8, class span_f32>
requires(
    semantic::concepts::Span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_fragment_program
 * \param id GLuint
 * \param len GLsizei
 * \param name const GLubyte *
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_program_named_parameter(
    u32 id, i32 len, span_const_u8 const& name, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramNamedParameterfvNV)
    }
    glGetProgramNamedParameterfvNV(
        id,
        len,
        name.size() ? reinterpret_cast<const GLubyte*>(name.data()) : nullptr,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetProgramNamedParameterfvNV"sv);
}

template<class span_const_vec_4_u8, class vec_4_f64>
requires(
    semantic::concepts::Span<span_const_vec_4_u8> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u8::value_type, u8, 4> &&
    semantic::concepts::Vector<vec_4_f64, f64, 4>)
/*!
 * \brief Part of GL_NV_fragment_program
 * \param id GLuint
 * \param len GLsizei
 * \param name const GLubyte *
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void program_named_parameter4d(
    u32 id, i32 len, span_const_vec_4_u8 const& name, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramNamedParameter4dNV)
    }
    glProgramNamedParameter4dNV(
        id,
        len,
        reinterpret_cast<const GLubyte*>(name.data()),
        x.x(),
        x.y(),
        x.z(),
        x.w());
    detail::error_check("ProgramNamedParameter4dNV"sv);
}

template<class span_const_vec_4_f64, class span_const_vec_4_u8>
requires(
    semantic::concepts::Span<span_const_vec_4_u8> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u8::value_type, u8, 4> &&
    semantic::concepts::Span<span_const_vec_4_f64> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Part of GL_NV_fragment_program
 * \param id GLuint
 * \param len GLsizei
 * \param name const GLubyte *
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void program_named_parameter4dv(
    u32                         id,
    i32                         len,
    span_const_vec_4_u8 const&  name,
    span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramNamedParameter4dvNV)
    }
    glProgramNamedParameter4dvNV(
        id,
        len,
        reinterpret_cast<const GLubyte*>(name.data()),
        reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("ProgramNamedParameter4dvNV"sv);
}

template<class span_const_vec_4_u8, class vec_4_f32>
requires(
    semantic::concepts::Span<span_const_vec_4_u8> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u8::value_type, u8, 4> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
/*!
 * \brief Part of GL_NV_fragment_program
 * \param id GLuint
 * \param len GLsizei
 * \param name const GLubyte *
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \param w GLfloat
 * \return void
 */
STATICINLINE void program_named_parameter4f(
    u32 id, i32 len, span_const_vec_4_u8 const& name, vec_4_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramNamedParameter4fNV)
    }
    glProgramNamedParameter4fNV(
        id,
        len,
        reinterpret_cast<const GLubyte*>(name.data()),
        x.x(),
        x.y(),
        x.z(),
        x.w());
    detail::error_check("ProgramNamedParameter4fNV"sv);
}

template<class span_const_vec_4_f32, class span_const_vec_4_u8>
requires(
    semantic::concepts::Span<span_const_vec_4_u8> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u8::value_type, u8, 4> &&
    semantic::concepts::Span<span_const_vec_4_f32> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_NV_fragment_program
 * \param id GLuint
 * \param len GLsizei
 * \param name const GLubyte *
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void program_named_parameter4fv(
    u32                         id,
    i32                         len,
    span_const_vec_4_u8 const&  name,
    span_const_vec_4_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramNamedParameter4fvNV)
    }
    glProgramNamedParameter4fvNV(
        id,
        len,
        reinterpret_cast<const GLubyte*>(name.data()),
        reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("ProgramNamedParameter4fvNV"sv);
}

} // namespace gl::nv::fragment_program
#endif // GL_NV_fragment_program
namespace gl::nv::fragment_program {
constexpr auto name = "GL_NV_fragment_program";
}
