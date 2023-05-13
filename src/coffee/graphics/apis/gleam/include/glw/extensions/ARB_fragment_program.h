#pragma once

#ifdef GL_ARB_fragment_program
#include "../enums/ProgramFormat.h"
#include "../enums/ProgramStringProperty.h"
#include "../enums/ProgramTarget.h"
namespace gl::arb::fragment_program {
using gl::group::program_format;
using gl::group::program_string_property;
using gl::group::program_target;
namespace values {
constexpr u32 program_length                      = 0x8627;
constexpr u32 max_program_matrix_stack_depth      = 0x862E;
constexpr u32 max_program_matrices                = 0x862F;
constexpr u32 current_matrix_stack_depth          = 0x8640;
constexpr u32 current_matrix                      = 0x8641;
constexpr u32 program_error_position              = 0x864B;
constexpr u32 program_binding                     = 0x8677;
constexpr u32 program_alu_instructions            = 0x8805;
constexpr u32 program_tex_instructions            = 0x8806;
constexpr u32 program_tex_indirections            = 0x8807;
constexpr u32 program_native_alu_instructions     = 0x8808;
constexpr u32 program_native_tex_instructions     = 0x8809;
constexpr u32 program_native_tex_indirections     = 0x880A;
constexpr u32 max_program_alu_instructions        = 0x880B;
constexpr u32 max_program_tex_instructions        = 0x880C;
constexpr u32 max_program_tex_indirections        = 0x880D;
constexpr u32 max_program_native_alu_instructions = 0x880E;
constexpr u32 max_program_native_tex_instructions = 0x880F;
constexpr u32 max_program_native_tex_indirections = 0x8810;
constexpr u32 max_texture_coords                  = 0x8871;
constexpr u32 max_texture_image_units             = 0x8872;
constexpr u32 program_error_string                = 0x8874;
constexpr u32 program_format                      = 0x8876;
constexpr u32 program_instructions                = 0x88A0;
constexpr u32 max_program_instructions            = 0x88A1;
constexpr u32 program_native_instructions         = 0x88A2;
constexpr u32 max_program_native_instructions     = 0x88A3;
constexpr u32 program_temporaries                 = 0x88A4;
constexpr u32 max_program_temporaries             = 0x88A5;
constexpr u32 program_native_temporaries          = 0x88A6;
constexpr u32 max_program_native_temporaries      = 0x88A7;
constexpr u32 program_parameters                  = 0x88A8;
constexpr u32 max_program_parameters              = 0x88A9;
constexpr u32 program_native_parameters           = 0x88AA;
constexpr u32 max_program_native_parameters       = 0x88AB;
constexpr u32 program_attribs                     = 0x88AC;
constexpr u32 max_program_attribs                 = 0x88AD;
constexpr u32 program_native_attribs              = 0x88AE;
constexpr u32 max_program_native_attribs          = 0x88AF;
constexpr u32 max_program_local_parameters        = 0x88B4;
constexpr u32 max_program_env_parameters          = 0x88B5;
constexpr u32 program_under_native_limits         = 0x88B6;
constexpr u32 transpose_current_matrix            = 0x88B7;
constexpr u32 matrix0                             = 0x88C0;
constexpr u32 matrix1                             = 0x88C1;
constexpr u32 matrix2                             = 0x88C2;
constexpr u32 matrix3                             = 0x88C3;
constexpr u32 matrix4                             = 0x88C4;
constexpr u32 matrix5                             = 0x88C5;
constexpr u32 matrix6                             = 0x88C6;
constexpr u32 matrix7                             = 0x88C7;
constexpr u32 matrix8                             = 0x88C8;
constexpr u32 matrix9                             = 0x88C9;
constexpr u32 matrix10                            = 0x88CA;
constexpr u32 matrix11                            = 0x88CB;
constexpr u32 matrix12                            = 0x88CC;
constexpr u32 matrix13                            = 0x88CD;
constexpr u32 matrix14                            = 0x88CE;
constexpr u32 matrix15                            = 0x88CF;
constexpr u32 matrix16                            = 0x88D0;
constexpr u32 matrix17                            = 0x88D1;
constexpr u32 matrix18                            = 0x88D2;
constexpr u32 matrix19                            = 0x88D3;
constexpr u32 matrix20                            = 0x88D4;
constexpr u32 matrix21                            = 0x88D5;
constexpr u32 matrix22                            = 0x88D6;
constexpr u32 matrix23                            = 0x88D7;
constexpr u32 matrix24                            = 0x88D8;
constexpr u32 matrix25                            = 0x88D9;
constexpr u32 matrix26                            = 0x88DA;
constexpr u32 matrix27                            = 0x88DB;
constexpr u32 matrix28                            = 0x88DC;
constexpr u32 matrix29                            = 0x88DD;
constexpr u32 matrix30                            = 0x88DE;
constexpr u32 matrix31                            = 0x88DF;
} // namespace values
/*!
 * \brief Part of GL_ARB_fragment_program
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
requires(concepts::span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_fragment_program
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

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_fragment_program
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
requires(concepts::span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_ARB_fragment_program
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
requires(concepts::span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_fragment_program
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
requires(concepts::span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_ARB_fragment_program
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
requires(concepts::span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_fragment_program
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
     * \brief Part of GL_ARB_fragment_program
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
 * \brief Part of GL_ARB_fragment_program
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

/*!
 * \brief Part of GL_ARB_fragment_program
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
     * \brief Part of GL_ARB_fragment_program
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
    concepts::span<span_const_vec_4_f64>&&
        concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    /*!
     * \brief Part of GL_ARB_fragment_program
     * \param target GLenum
     * \param index GLuint
     * \param params const GLdouble *
     * \return void
     */
    STATICINLINE void program_env_parameter4dv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_f64 const& params)
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
     * \brief Part of GL_ARB_fragment_program
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
    concepts::span<span_const_vec_4_f32>&&
        concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    /*!
     * \brief Part of GL_ARB_fragment_program
     * \param target GLenum
     * \param index GLuint
     * \param params const GLfloat *
     * \return void
     */
    STATICINLINE void program_env_parameter4fv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_f32 const& params)
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
     * \brief Part of GL_ARB_fragment_program
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
    concepts::span<span_const_vec_4_f64>&&
        concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    /*!
     * \brief Part of GL_ARB_fragment_program
     * \param target GLenum
     * \param index GLuint
     * \param params const GLdouble *
     * \return void
     */
    STATICINLINE void program_local_parameter4dv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_f64 const& params)
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
     * \brief Part of GL_ARB_fragment_program
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
    concepts::span<span_const_vec_4_f32>&&
        concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    /*!
     * \brief Part of GL_ARB_fragment_program
     * \param target GLenum
     * \param index GLuint
     * \param params const GLfloat *
     * \return void
     */
    STATICINLINE void program_local_parameter4fv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_f32 const& params)
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
     * \brief Part of GL_ARB_fragment_program
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

} // namespace gl::arb::fragment_program
#endif // GL_ARB_fragment_program
namespace gl::arb::fragment_program {
constexpr auto name = "GL_ARB_fragment_program";
}
