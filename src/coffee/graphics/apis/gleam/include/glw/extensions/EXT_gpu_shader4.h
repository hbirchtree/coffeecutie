#pragma once

#ifdef GL_EXT_gpu_shader4
#include "../enums/VertexAttribPropertyARB.h"

namespace gl::ext::gpu_shader4 {
using gl::group::vertex_attrib_property_arb;

namespace values {
constexpr u32 min_program_texel_offset      = 0x8904;
constexpr u32 max_program_texel_offset      = 0x8905;
constexpr u32 sampler_1d_array              = 0x8DC0;
constexpr u32 sampler_2d_array              = 0x8DC1;
constexpr u32 sampler_buffer                = 0x8DC2;
constexpr u32 sampler_1d_array_shadow       = 0x8DC3;
constexpr u32 sampler_2d_array_shadow       = 0x8DC4;
constexpr u32 sampler_cube_shadow           = 0x8DC5;
constexpr u32 unsigned_int_vec2             = 0x8DC6;
constexpr u32 unsigned_int_vec3             = 0x8DC7;
constexpr u32 unsigned_int_vec4             = 0x8DC8;
constexpr u32 int_sampler_1d                = 0x8DC9;
constexpr u32 int_sampler_2d                = 0x8DCA;
constexpr u32 int_sampler_3d                = 0x8DCB;
constexpr u32 int_sampler_cube              = 0x8DCC;
constexpr u32 int_sampler_2d_rect           = 0x8DCD;
constexpr u32 int_sampler_1d_array          = 0x8DCE;
constexpr u32 int_sampler_2d_array          = 0x8DCF;
constexpr u32 int_sampler_buffer            = 0x8DD0;
constexpr u32 unsigned_int_sampler_1d       = 0x8DD1;
constexpr u32 unsigned_int_sampler_2d       = 0x8DD2;
constexpr u32 unsigned_int_sampler_3d       = 0x8DD3;
constexpr u32 unsigned_int_sampler_cube     = 0x8DD4;
constexpr u32 unsigned_int_sampler_2d_rect  = 0x8DD5;
constexpr u32 unsigned_int_sampler_1d_array = 0x8DD6;
constexpr u32 unsigned_int_sampler_2d_array = 0x8DD7;
constexpr u32 unsigned_int_sampler_buffer   = 0x8DD8;
} // namespace values

/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param program GLuint
 * \param color GLuint
 * \param name const GLchar *
 * \return void
 */
STATICINLINE void bind_frag_data_location(
    u32                     program,
    u32                     color,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocationEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glBindFragDataLocationEXT(program, color, name.data());
    detail::error_check("BindFragDataLocationEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param program GLuint
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_frag_data_location(
    u32                     program,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataLocationEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetFragDataLocationEXT(program, name.data());
    detail::error_check("GetFragDataLocationEXT"sv, check_errors);
    return out;
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param program GLuint
 * \param location GLint
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_uniformuiv(
    u32         program,
    i32         location,
    span_u32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformuivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformuivEXT(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetUniformuivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param location GLint
 * \param v0 GLuint
 * \return void
 */
STATICINLINE void uniform(
    i32 location, u32 v0, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1uiEXT)
    }
    glUniform1uiEXT(location, v0);
    detail::error_check("Uniform1uiEXT"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void uniform(
    i32                   location,
    span_const_u32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1uivEXT)
    }
    glUniform1uivEXT(
        location, value.size(), reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform1uivEXT"sv, check_errors);
}

template<class vec_2_u32>
requires(concepts::vector<vec_2_u32, u32, 2>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_2_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2uiEXT)
    }
    glUniform2uiEXT(location, v0[0], v0[1]);
    detail::error_check("Uniform2uiEXT"sv, check_errors);
}

template<class span_const_vec_2_u32>
requires(
    concepts::span<span_const_vec_2_u32> &&
    concepts::vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_2_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2uivEXT)
    }
    glUniform2uivEXT(
        location, value.size(), reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform2uivEXT"sv, check_errors);
}

template<class vec_3_u32>
requires(concepts::vector<vec_3_u32, u32, 3>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \param v2 GLuint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_3_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3uiEXT)
    }
    glUniform3uiEXT(location, v0[0], v0[1], v0[2]);
    detail::error_check("Uniform3uiEXT"sv, check_errors);
}

template<class span_const_vec_3_u32>
requires(
    concepts::span<span_const_vec_3_u32> &&
    concepts::vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_3_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3uivEXT)
    }
    glUniform3uivEXT(
        location, value.size(), reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform3uivEXT"sv, check_errors);
}

template<class vec_4_u32>
requires(concepts::vector<vec_4_u32, u32, 4>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \param v2 GLuint
 * \param v3 GLuint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_4_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4uiEXT)
    }
    glUniform4uiEXT(location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("Uniform4uiEXT"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_4_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4uivEXT)
    }
    glUniform4uivEXT(
        location, value.size(), reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform4uivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_vertex_attrib_iiv(
    u32                       index,
    group::vertex_attrib_enum pname,
    i32&                      params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIivEXT)
    }
    glGetVertexAttribIivEXT(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_vertex_attrib_iuiv(
    u32                       index,
    group::vertex_attrib_enum pname,
    u32&                      params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIuivEXT)
    }
    glGetVertexAttribIuivEXT(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIuivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param x GLint
 * \return void
 */
STATICINLINE void vertex_attrib_i1i(
    u32 index, i32 x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1iEXT)
    }
    glVertexAttribI1iEXT(index, x);
    detail::error_check("VertexAttribI1iEXT"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void vertex_attrib_i1iv(
    u32                   index,
    span_const_i32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1ivEXT)
    }
    glVertexAttribI1ivEXT(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI1ivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param x GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_i1ui(
    u32 index, u32 x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1uiEXT)
    }
    glVertexAttribI1uiEXT(index, x);
    detail::error_check("VertexAttribI1uiEXT"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_i1uiv(
    u32                   index,
    span_const_u32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1uivEXT)
    }
    glVertexAttribI1uivEXT(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI1uivEXT"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param x GLint
 * \param y GLint
 * \return void
 */
STATICINLINE void vertex_attrib_i2i(
    u32 index, vec_2_i32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2iEXT)
    }
    glVertexAttribI2iEXT(index, x[0], x[1]);
    detail::error_check("VertexAttribI2iEXT"sv, check_errors);
}

template<class span_const_vec_2_i32>
requires(
    concepts::span<span_const_vec_2_i32> &&
    concepts::vector<typename span_const_vec_2_i32::value_type, i32, 2>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void vertex_attrib_i2iv(
    u32                         index,
    span_const_vec_2_i32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2ivEXT)
    }
    glVertexAttribI2ivEXT(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI2ivEXT"sv, check_errors);
}

template<class vec_2_u32>
requires(concepts::vector<vec_2_u32, u32, 2>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param x GLuint
 * \param y GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_i2ui(
    u32 index, vec_2_u32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2uiEXT)
    }
    glVertexAttribI2uiEXT(index, x[0], x[1]);
    detail::error_check("VertexAttribI2uiEXT"sv, check_errors);
}

template<class span_const_vec_2_u32>
requires(
    concepts::span<span_const_vec_2_u32> &&
    concepts::vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_i2uiv(
    u32                         index,
    span_const_vec_2_u32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2uivEXT)
    }
    glVertexAttribI2uivEXT(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI2uivEXT"sv, check_errors);
}

template<class vec_3_i32>
requires(concepts::vector<vec_3_i32, i32, 3>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param x GLint
 * \param y GLint
 * \param z GLint
 * \return void
 */
STATICINLINE void vertex_attrib_i3i(
    u32 index, vec_3_i32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3iEXT)
    }
    glVertexAttribI3iEXT(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttribI3iEXT"sv, check_errors);
}

template<class span_const_vec_3_i32>
requires(
    concepts::span<span_const_vec_3_i32> &&
    concepts::vector<typename span_const_vec_3_i32::value_type, i32, 3>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void vertex_attrib_i3iv(
    u32                         index,
    span_const_vec_3_i32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3ivEXT)
    }
    glVertexAttribI3ivEXT(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI3ivEXT"sv, check_errors);
}

template<class vec_3_u32>
requires(concepts::vector<vec_3_u32, u32, 3>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param x GLuint
 * \param y GLuint
 * \param z GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_i3ui(
    u32 index, vec_3_u32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3uiEXT)
    }
    glVertexAttribI3uiEXT(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttribI3uiEXT"sv, check_errors);
}

template<class span_const_vec_3_u32>
requires(
    concepts::span<span_const_vec_3_u32> &&
    concepts::vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_i3uiv(
    u32                         index,
    span_const_vec_3_u32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3uivEXT)
    }
    glVertexAttribI3uivEXT(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI3uivEXT"sv, check_errors);
}

template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLbyte *
 * \return void
 */
STATICINLINE void vertex_attrib_i4bv(
    u32                  index,
    span_const_i8 const& v,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4bvEXT)
    }
    glVertexAttribI4bvEXT(
        index, v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttribI4bvEXT"sv, check_errors);
}

template<class vec_4_i32>
requires(concepts::vector<vec_4_i32, i32, 4>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param x GLint
 * \param y GLint
 * \param z GLint
 * \param w GLint
 * \return void
 */
STATICINLINE void vertex_attrib_i4i(
    u32 index, vec_4_i32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4iEXT)
    }
    glVertexAttribI4iEXT(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttribI4iEXT"sv, check_errors);
}

template<class span_const_vec_4_i32>
requires(
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void vertex_attrib_i4iv(
    u32                         index,
    span_const_vec_4_i32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4ivEXT)
    }
    glVertexAttribI4ivEXT(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI4ivEXT"sv, check_errors);
}

template<class span_const_vec_4_i16>
requires(
    concepts::span<span_const_vec_4_i16> &&
    concepts::vector<typename span_const_vec_4_i16::value_type, i16, 4>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void vertex_attrib_i4sv(
    u32                         index,
    span_const_vec_4_i16 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4svEXT)
    }
    glVertexAttribI4svEXT(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttribI4svEXT"sv, check_errors);
}

template<class span_const_u8>
requires(
    concepts::span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLubyte *
 * \return void
 */
STATICINLINE void vertex_attrib_i4ubv(
    u32                  index,
    span_const_u8 const& v,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4ubvEXT)
    }
    glVertexAttribI4ubvEXT(
        index, v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttribI4ubvEXT"sv, check_errors);
}

template<class vec_4_u32>
requires(concepts::vector<vec_4_u32, u32, 4>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param x GLuint
 * \param y GLuint
 * \param z GLuint
 * \param w GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_i4ui(
    u32 index, vec_4_u32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4uiEXT)
    }
    glVertexAttribI4uiEXT(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttribI4uiEXT"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_i4uiv(
    u32                         index,
    span_const_vec_4_u32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4uivEXT)
    }
    glVertexAttribI4uivEXT(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI4uivEXT"sv, check_errors);
}

template<class span_const_vec_4_u16>
requires(
    concepts::span<span_const_vec_4_u16> &&
    concepts::vector<typename span_const_vec_4_u16::value_type, u16, 4>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param v const GLushort *
 * \return void
 */
STATICINLINE void vertex_attrib_i4usv(
    u32                         index,
    span_const_vec_4_u16 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4usvEXT)
    }
    glVertexAttribI4usvEXT(index, reinterpret_cast<const GLushort*>(v.data()));
    detail::error_check("VertexAttribI4usvEXT"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_gpu_shader4
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void vertex_attrib_i_pointer(
    u32                      index,
    i32                      size,
    group::vertex_attrib_int type,
    i32                      stride,
    span_const_void const&   pointer,
    error_check              check_errors = error_check::on)
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
    detail::error_check("VertexAttribIPointerEXT"sv, check_errors);
}

} // namespace gl::ext::gpu_shader4
#endif // GL_EXT_gpu_shader4
namespace gl::ext::gpu_shader4 {
constexpr auto name = "GL_EXT_gpu_shader4";
}
