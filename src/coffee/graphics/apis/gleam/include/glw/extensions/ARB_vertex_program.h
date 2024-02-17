#pragma once

#ifdef GL_ARB_vertex_program
#include "../enums/ProgramFormat.h"
#include "../enums/ProgramStringProperty.h"
#include "../enums/ProgramTarget.h"
#include "../enums/VertexAttribPointerPropertyARB.h"

namespace gl::arb::vertex_program {
using gl::group::program_format;
using gl::group::program_string_property;
using gl::group::program_target;
using gl::group::vertex_attrib_pointer_property_arb;

namespace values {
constexpr u32 color_sum                            = 0x8458;
constexpr u32 vertex_attrib_array_enabled          = 0x8622;
constexpr u32 vertex_attrib_array_size             = 0x8623;
constexpr u32 vertex_attrib_array_stride           = 0x8624;
constexpr u32 vertex_attrib_array_type             = 0x8625;
constexpr u32 current_vertex_attrib                = 0x8626;
constexpr u32 program_length                       = 0x8627;
constexpr u32 max_program_matrix_stack_depth       = 0x862E;
constexpr u32 max_program_matrices                 = 0x862F;
constexpr u32 current_matrix_stack_depth           = 0x8640;
constexpr u32 current_matrix                       = 0x8641;
constexpr u32 vertex_program_point_size            = 0x8642;
constexpr u32 vertex_program_two_side              = 0x8643;
constexpr u32 program_error_position               = 0x864B;
constexpr u32 program_binding                      = 0x8677;
constexpr u32 max_vertex_attribs                   = 0x8869;
constexpr u32 vertex_attrib_array_normalized       = 0x886A;
constexpr u32 program_error_string                 = 0x8874;
constexpr u32 program_format                       = 0x8876;
constexpr u32 program_instructions                 = 0x88A0;
constexpr u32 max_program_instructions             = 0x88A1;
constexpr u32 program_native_instructions          = 0x88A2;
constexpr u32 max_program_native_instructions      = 0x88A3;
constexpr u32 program_temporaries                  = 0x88A4;
constexpr u32 max_program_temporaries              = 0x88A5;
constexpr u32 program_native_temporaries           = 0x88A6;
constexpr u32 max_program_native_temporaries       = 0x88A7;
constexpr u32 program_parameters                   = 0x88A8;
constexpr u32 max_program_parameters               = 0x88A9;
constexpr u32 program_native_parameters            = 0x88AA;
constexpr u32 max_program_native_parameters        = 0x88AB;
constexpr u32 program_attribs                      = 0x88AC;
constexpr u32 max_program_attribs                  = 0x88AD;
constexpr u32 program_native_attribs               = 0x88AE;
constexpr u32 max_program_native_attribs           = 0x88AF;
constexpr u32 program_address_registers            = 0x88B0;
constexpr u32 max_program_address_registers        = 0x88B1;
constexpr u32 program_native_address_registers     = 0x88B2;
constexpr u32 max_program_native_address_registers = 0x88B3;
constexpr u32 max_program_local_parameters         = 0x88B4;
constexpr u32 max_program_env_parameters           = 0x88B5;
constexpr u32 program_under_native_limits          = 0x88B6;
constexpr u32 transpose_current_matrix             = 0x88B7;
constexpr u32 matrix0                              = 0x88C0;
constexpr u32 matrix1                              = 0x88C1;
constexpr u32 matrix2                              = 0x88C2;
constexpr u32 matrix3                              = 0x88C3;
constexpr u32 matrix4                              = 0x88C4;
constexpr u32 matrix5                              = 0x88C5;
constexpr u32 matrix6                              = 0x88C6;
constexpr u32 matrix7                              = 0x88C7;
constexpr u32 matrix8                              = 0x88C8;
constexpr u32 matrix9                              = 0x88C9;
constexpr u32 matrix10                             = 0x88CA;
constexpr u32 matrix11                             = 0x88CB;
constexpr u32 matrix12                             = 0x88CC;
constexpr u32 matrix13                             = 0x88CD;
constexpr u32 matrix14                             = 0x88CE;
constexpr u32 matrix15                             = 0x88CF;
constexpr u32 matrix16                             = 0x88D0;
constexpr u32 matrix17                             = 0x88D1;
constexpr u32 matrix18                             = 0x88D2;
constexpr u32 matrix19                             = 0x88D3;
constexpr u32 matrix20                             = 0x88D4;
constexpr u32 matrix21                             = 0x88D5;
constexpr u32 matrix22                             = 0x88D6;
constexpr u32 matrix23                             = 0x88D7;
constexpr u32 matrix24                             = 0x88D8;
constexpr u32 matrix25                             = 0x88D9;
constexpr u32 matrix26                             = 0x88DA;
constexpr u32 matrix27                             = 0x88DB;
constexpr u32 matrix28                             = 0x88DC;
constexpr u32 matrix29                             = 0x88DD;
constexpr u32 matrix30                             = 0x88DE;
constexpr u32 matrix31                             = 0x88DF;
} // namespace values

/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param program GLuint
 * \return void
 */
STATICINLINE void bind_program(group::program_target target, u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindProgramARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glBindProgramARB(static_cast<GLenum>(target), program);
    detail::error_check("BindProgramARB"sv);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param n GLsizei
 * \param programs const GLuint *
 * \return void
 */
STATICINLINE void delete_programs(span_const_u32 const& programs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteProgramsARB)
    }
    glDeleteProgramsARB(
        programs.size(),
        programs.size() ? reinterpret_cast<const GLuint*>(programs.data())
                        : nullptr);
    detail::error_check("DeleteProgramsARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \return void
 */
STATICINLINE void disable_vertex_attrib_array(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexAttribArrayARB)
    }
    glDisableVertexAttribArrayARB(index);
    detail::error_check("DisableVertexAttribArrayARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \return void
 */
STATICINLINE void enable_vertex_attrib_array(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexAttribArrayARB)
    }
    glEnableVertexAttribArrayARB(index);
    detail::error_check("EnableVertexAttribArrayARB"sv);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param n GLsizei
 * \param programs GLuint *
 * \return void
 */
STATICINLINE void gen_programs(span_u32 programs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenProgramsARB)
    }
    glGenProgramsARB(
        programs.size(),
        programs.size() ? reinterpret_cast<GLuint*>(programs.data()) : nullptr);
    detail::error_check("GenProgramsARB"sv);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_program_env_parameter(
    group::program_target target, u32 index, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramEnvParameterdvARB)
    }
    glGetProgramEnvParameterdvARB(
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetProgramEnvParameterdvARB"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_program_env_parameter(
    group::program_target target, u32 index, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramEnvParameterfvARB)
    }
    glGetProgramEnvParameterfvARB(
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetProgramEnvParameterfvARB"sv);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_program_local_parameter(
    group::program_target target, u32 index, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramLocalParameterdvARB)
    }
    glGetProgramLocalParameterdvARB(
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetProgramLocalParameterdvARB"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_program_local_parameter(
    group::program_target target, u32 index, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramLocalParameterfvARB)
    }
    glGetProgramLocalParameterfvARB(
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetProgramLocalParameterfvARB"sv);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param pname GLenum
 * \param string void *
 * \return void
 */
STATICINLINE void get_program_string(
    group::program_target          target,
    group::program_string_property pname,
    span_void                      string)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramStringARB)
    }
    glGetProgramStringARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        string.size() ? reinterpret_cast<void*>(string.data()) : nullptr);
    detail::error_check("GetProgramStringARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_programiv(
    group::program_target       target,
    group::program_property_arb pname,
    i32&                        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramivARB)
    }
    glGetProgramivARB(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), &params);
    detail::error_check("GetProgramivARB"sv);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param pname GLenum
 * \param pointer void **
 * \return void
 */
STATICINLINE void get_vertex_attrib_pointerv(
    u32                                       index,
    group::vertex_attrib_pointer_property_arb pname,
    span_void                                 pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribPointervARB)
    }
    glGetVertexAttribPointervARB(
        index,
        static_cast<GLenum>(pname),
        pointer.size() ? reinterpret_cast<void**>(pointer.data()) : nullptr);
    detail::error_check("GetVertexAttribPointervARB"sv);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param pname GLenum
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_vertex_attribdv(
    u32 index, group::vertex_attrib_property_arb pname, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribdvARB)
    }
    glGetVertexAttribdvARB(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribdvARB"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_vertex_attribfv(
    u32 index, group::vertex_attrib_property_arb pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribfvARB)
    }
    glGetVertexAttribfvARB(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribfvARB"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_vertex_attribiv(
    u32 index, group::vertex_attrib_property_arb pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribivARB)
    }
    glGetVertexAttribivARB(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribivARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_program
 * \param program GLuint
 * \return Boolean
 */
STATICINLINE bool is_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsProgramARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glIsProgramARB(program);
    detail::error_check("IsProgramARB"sv);
    return out == GL_TRUE ? true : false;
}

template<class vec_4_f64>
requires(concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void program_env_parameter4d(
    group::program_target target, u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameter4dARB)
    }
    glProgramEnvParameter4dARB(
        static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramEnvParameter4dARB"sv);
}

template<class span_const_vec_4_f64>
requires(
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param params const GLdouble *
 * \return void
 */
STATICINLINE void program_env_parameter4dv(
    group::program_target target, u32 index, span_const_vec_4_f64 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameter4dvARB)
    }
    glProgramEnvParameter4dvARB(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLdouble*>(params.data()));
    detail::error_check("ProgramEnvParameter4dvARB"sv);
}

template<class vec_4_f32>
requires(concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \param w GLfloat
 * \return void
 */
STATICINLINE void program_env_parameter4f(
    group::program_target target, u32 index, vec_4_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameter4fARB)
    }
    glProgramEnvParameter4fARB(
        static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramEnvParameter4fARB"sv);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void program_env_parameter4fv(
    group::program_target target, u32 index, span_const_vec_4_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameter4fvARB)
    }
    glProgramEnvParameter4fvARB(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLfloat*>(params.data()));
    detail::error_check("ProgramEnvParameter4fvARB"sv);
}

template<class vec_4_f64>
requires(concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void program_local_parameter4d(
    group::program_target target, u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameter4dARB)
    }
    glProgramLocalParameter4dARB(
        static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramLocalParameter4dARB"sv);
}

template<class span_const_vec_4_f64>
requires(
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param params const GLdouble *
 * \return void
 */
STATICINLINE void program_local_parameter4dv(
    group::program_target target, u32 index, span_const_vec_4_f64 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameter4dvARB)
    }
    glProgramLocalParameter4dvARB(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLdouble*>(params.data()));
    detail::error_check("ProgramLocalParameter4dvARB"sv);
}

template<class vec_4_f32>
requires(concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \param w GLfloat
 * \return void
 */
STATICINLINE void program_local_parameter4f(
    group::program_target target, u32 index, vec_4_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameter4fARB)
    }
    glProgramLocalParameter4fARB(
        static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramLocalParameter4fARB"sv);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param index GLuint
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void program_local_parameter4fv(
    group::program_target target, u32 index, span_const_vec_4_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameter4fvARB)
    }
    glProgramLocalParameter4fvARB(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLfloat*>(params.data()));
    detail::error_check("ProgramLocalParameter4fvARB"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param target GLenum
 * \param format GLenum
 * \param len GLsizei
 * \param string const void *
 * \return void
 */
STATICINLINE void program_string(
    group::program_target  target,
    group::program_format  format,
    span_const_void const& string)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramStringARB)
    }
    glProgramStringARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        string.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        string.size() ? reinterpret_cast<const void*>(string.data()) : nullptr);
    detail::error_check("ProgramStringARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib1d(u32 index, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1dARB)
    }
    glVertexAttrib1dARB(index, x);
    detail::error_check("VertexAttrib1dARB"sv);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void vertex_attrib1dv(u32 index, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1dvARB)
    }
    glVertexAttrib1dvARB(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib1dvARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLfloat
 * \return void
 */
STATICINLINE void vertex_attrib1f(u32 index, f32 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1fARB)
    }
    glVertexAttrib1fARB(index, x);
    detail::error_check("VertexAttrib1fARB"sv);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void vertex_attrib1fv(u32 index, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1fvARB)
    }
    glVertexAttrib1fvARB(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib1fvARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLshort
 * \return void
 */
STATICINLINE void vertex_attrib1s(u32 index, i16 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1sARB)
    }
    glVertexAttrib1sARB(index, x);
    detail::error_check("VertexAttrib1sARB"sv);
}

template<class span_const_i16>
requires(
    concepts::span<span_const_i16> &&
    std::is_same_v<
        std::decay_t<typename span_const_i16::value_type>,
        std::decay_t<i16>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void vertex_attrib1sv(u32 index, span_const_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1svARB)
    }
    glVertexAttrib1svARB(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib1svARB"sv);
}

template<class vec_2_f64>
requires(concepts::vector<vec_2_f64, f64, 2>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib2d(u32 index, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2dARB)
    }
    glVertexAttrib2dARB(index, x[0], x[1]);
    detail::error_check("VertexAttrib2dARB"sv);
}

template<class span_const_vec_2_f64>
requires(
    concepts::span<span_const_vec_2_f64> &&
    concepts::vector<typename span_const_vec_2_f64::value_type, f64, 2>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void vertex_attrib2dv(u32 index, span_const_vec_2_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2dvARB)
    }
    glVertexAttrib2dvARB(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib2dvARB"sv);
}

template<class vec_2_f32>
requires(concepts::vector<vec_2_f32, f32, 2>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \return void
 */
STATICINLINE void vertex_attrib2f(u32 index, vec_2_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2fARB)
    }
    glVertexAttrib2fARB(index, x[0], x[1]);
    detail::error_check("VertexAttrib2fARB"sv);
}

template<class span_const_vec_2_f32>
requires(
    concepts::span<span_const_vec_2_f32> &&
    concepts::vector<typename span_const_vec_2_f32::value_type, f32, 2>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void vertex_attrib2fv(u32 index, span_const_vec_2_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2fvARB)
    }
    glVertexAttrib2fvARB(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib2fvARB"sv);
}

template<class vec_2_i16>
requires(concepts::vector<vec_2_i16, i16, 2>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLshort
 * \param y GLshort
 * \return void
 */
STATICINLINE void vertex_attrib2s(u32 index, vec_2_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2sARB)
    }
    glVertexAttrib2sARB(index, x[0], x[1]);
    detail::error_check("VertexAttrib2sARB"sv);
}

template<class span_const_vec_2_i16>
requires(
    concepts::span<span_const_vec_2_i16> &&
    concepts::vector<typename span_const_vec_2_i16::value_type, i16, 2>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void vertex_attrib2sv(u32 index, span_const_vec_2_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2svARB)
    }
    glVertexAttrib2svARB(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib2svARB"sv);
}

template<class vec_3_f64>
requires(concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib3d(u32 index, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3dARB)
    }
    glVertexAttrib3dARB(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttrib3dARB"sv);
}

template<class span_const_vec_3_f64>
requires(
    concepts::span<span_const_vec_3_f64> &&
    concepts::vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void vertex_attrib3dv(u32 index, span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3dvARB)
    }
    glVertexAttrib3dvARB(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib3dvARB"sv);
}

template<class vec_3_f32>
requires(concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \return void
 */
STATICINLINE void vertex_attrib3f(u32 index, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3fARB)
    }
    glVertexAttrib3fARB(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttrib3fARB"sv);
}

template<class span_const_vec_3_f32>
requires(
    concepts::span<span_const_vec_3_f32> &&
    concepts::vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void vertex_attrib3fv(u32 index, span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3fvARB)
    }
    glVertexAttrib3fvARB(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib3fvARB"sv);
}

template<class vec_3_i16>
requires(concepts::vector<vec_3_i16, i16, 3>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLshort
 * \param y GLshort
 * \param z GLshort
 * \return void
 */
STATICINLINE void vertex_attrib3s(u32 index, vec_3_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3sARB)
    }
    glVertexAttrib3sARB(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttrib3sARB"sv);
}

template<class span_const_vec_3_i16>
requires(
    concepts::span<span_const_vec_3_i16> &&
    concepts::vector<typename span_const_vec_3_i16::value_type, i16, 3>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void vertex_attrib3sv(u32 index, span_const_vec_3_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3svARB)
    }
    glVertexAttrib3svARB(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib3svARB"sv);
}

template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLbyte *
 * \return void
 */
STATICINLINE void vertex_attrib4_nbv(u32 index, span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4NbvARB)
    }
    glVertexAttrib4NbvARB(
        index, v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4NbvARB"sv);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void vertex_attrib4_niv(u32 index, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4NivARB)
    }
    glVertexAttrib4NivARB(
        index, v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4NivARB"sv);
}

template<class span_const_i16>
requires(
    concepts::span<span_const_i16> &&
    std::is_same_v<
        std::decay_t<typename span_const_i16::value_type>,
        std::decay_t<i16>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void vertex_attrib4_nsv(u32 index, span_const_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4NsvARB)
    }
    glVertexAttrib4NsvARB(
        index, v.size() ? reinterpret_cast<const GLshort*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4NsvARB"sv);
}

template<class vec_4_u8>
requires(concepts::vector<vec_4_u8, u8, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLubyte
 * \param y GLubyte
 * \param z GLubyte
 * \param w GLubyte
 * \return void
 */
STATICINLINE void vertex_attrib4_nub(u32 index, vec_4_u8 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4NubARB)
    }
    glVertexAttrib4NubARB(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttrib4NubARB"sv);
}

template<class span_const_u8>
requires(
    concepts::span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLubyte *
 * \return void
 */
STATICINLINE void vertex_attrib4_nubv(u32 index, span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4NubvARB)
    }
    glVertexAttrib4NubvARB(
        index, v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4NubvARB"sv);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib4_nuiv(u32 index, span_const_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4NuivARB)
    }
    glVertexAttrib4NuivARB(
        index, v.size() ? reinterpret_cast<const GLuint*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4NuivARB"sv);
}

template<class span_const_u16>
requires(
    concepts::span<span_const_u16> &&
    std::is_same_v<
        std::decay_t<typename span_const_u16::value_type>,
        std::decay_t<u16>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLushort *
 * \return void
 */
STATICINLINE void vertex_attrib4_nusv(u32 index, span_const_u16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4NusvARB)
    }
    glVertexAttrib4NusvARB(
        index,
        v.size() ? reinterpret_cast<const GLushort*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4NusvARB"sv);
}

template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLbyte *
 * \return void
 */
STATICINLINE void vertex_attrib4bv(u32 index, span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4bvARB)
    }
    glVertexAttrib4bvARB(
        index, v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4bvARB"sv);
}

template<class vec_4_f64>
requires(concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib4d(u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4dARB)
    }
    glVertexAttrib4dARB(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttrib4dARB"sv);
}

template<class span_const_vec_4_f64>
requires(
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void vertex_attrib4dv(u32 index, span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4dvARB)
    }
    glVertexAttrib4dvARB(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib4dvARB"sv);
}

template<class vec_4_f32>
requires(concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \param w GLfloat
 * \return void
 */
STATICINLINE void vertex_attrib4f(u32 index, vec_4_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4fARB)
    }
    glVertexAttrib4fARB(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttrib4fARB"sv);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void vertex_attrib4fv(u32 index, span_const_vec_4_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4fvARB)
    }
    glVertexAttrib4fvARB(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib4fvARB"sv);
}

template<class span_const_vec_4_i32>
requires(
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void vertex_attrib4iv(u32 index, span_const_vec_4_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4ivARB)
    }
    glVertexAttrib4ivARB(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttrib4ivARB"sv);
}

template<class vec_4_i16>
requires(concepts::vector<vec_4_i16, i16, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param x GLshort
 * \param y GLshort
 * \param z GLshort
 * \param w GLshort
 * \return void
 */
STATICINLINE void vertex_attrib4s(u32 index, vec_4_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4sARB)
    }
    glVertexAttrib4sARB(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttrib4sARB"sv);
}

template<class span_const_vec_4_i16>
requires(
    concepts::span<span_const_vec_4_i16> &&
    concepts::vector<typename span_const_vec_4_i16::value_type, i16, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void vertex_attrib4sv(u32 index, span_const_vec_4_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4svARB)
    }
    glVertexAttrib4svARB(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib4svARB"sv);
}

template<class span_const_u8>
requires(
    concepts::span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLubyte *
 * \return void
 */
STATICINLINE void vertex_attrib4ubv(u32 index, span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4ubvARB)
    }
    glVertexAttrib4ubvARB(
        index, v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4ubvARB"sv);
}

template<class span_const_vec_4_u32>
requires(
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib4uiv(u32 index, span_const_vec_4_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4uivARB)
    }
    glVertexAttrib4uivARB(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttrib4uivARB"sv);
}

template<class span_const_vec_4_u16>
requires(
    concepts::span<span_const_vec_4_u16> &&
    concepts::vector<typename span_const_vec_4_u16::value_type, u16, 4>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param v const GLushort *
 * \return void
 */
STATICINLINE void vertex_attrib4usv(u32 index, span_const_vec_4_u16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4usvARB)
    }
    glVertexAttrib4usvARB(index, reinterpret_cast<const GLushort*>(v.data()));
    detail::error_check("VertexAttrib4usvARB"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_vertex_program
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param normalized GLboolean
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void vertex_attrib_pointer(
    u32                               index,
    i32                               size,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    i32                               stride,
    span_const_void const&            pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribPointerARB)
    }
    glVertexAttribPointerARB(
        index,
        size,
        static_cast<GLenum>(type),
        normalized,
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribPointerARB"sv);
}

} // namespace gl::arb::vertex_program
#endif // GL_ARB_vertex_program
namespace gl::arb::vertex_program {
constexpr auto name = "GL_ARB_vertex_program";
}
