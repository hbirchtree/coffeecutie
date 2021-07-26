#pragma once

#ifdef GL_NV_vertex_program
#include "../enums/VertexAttribEnumNV.h"
namespace gl::nv::vertex_program {
using gl::group::vertex_attrib_enum_nv;
namespace values {
constexpr libc_types::u32 vertex_program               = 0x8620;
constexpr libc_types::u32 vertex_state_program         = 0x8621;
constexpr libc_types::u32 attrib_array_size            = 0x8623;
constexpr libc_types::u32 attrib_array_stride          = 0x8624;
constexpr libc_types::u32 attrib_array_type            = 0x8625;
constexpr libc_types::u32 current_attrib               = 0x8626;
constexpr libc_types::u32 program_length               = 0x8627;
constexpr libc_types::u32 program_string               = 0x8628;
constexpr libc_types::u32 modelview_projection         = 0x8629;
constexpr libc_types::u32 identity                     = 0x862A;
constexpr libc_types::u32 inverse                      = 0x862B;
constexpr libc_types::u32 transpose                    = 0x862C;
constexpr libc_types::u32 inverse_transpose            = 0x862D;
constexpr libc_types::u32 max_track_matrix_stack_depth = 0x862E;
constexpr libc_types::u32 max_track_matrices           = 0x862F;
constexpr libc_types::u32 matrix0                      = 0x8630;
constexpr libc_types::u32 matrix1                      = 0x8631;
constexpr libc_types::u32 matrix2                      = 0x8632;
constexpr libc_types::u32 matrix3                      = 0x8633;
constexpr libc_types::u32 matrix4                      = 0x8634;
constexpr libc_types::u32 matrix5                      = 0x8635;
constexpr libc_types::u32 matrix6                      = 0x8636;
constexpr libc_types::u32 matrix7                      = 0x8637;
constexpr libc_types::u32 current_matrix_stack_depth   = 0x8640;
constexpr libc_types::u32 current_matrix               = 0x8641;
constexpr libc_types::u32 vertex_program_point_size    = 0x8642;
constexpr libc_types::u32 vertex_program_two_side      = 0x8643;
constexpr libc_types::u32 attrib_array_pointer         = 0x8645;
constexpr libc_types::u32 program_target               = 0x8646;
constexpr libc_types::u32 program_resident             = 0x8647;
constexpr libc_types::u32 track_matrix                 = 0x8648;
constexpr libc_types::u32 track_matrix_transform       = 0x8649;
constexpr libc_types::u32 vertex_program_binding       = 0x864A;
constexpr libc_types::u32 program_error_position       = 0x864B;
constexpr libc_types::u32 vertex_attrib_array0         = 0x8650;
constexpr libc_types::u32 vertex_attrib_array1         = 0x8651;
constexpr libc_types::u32 vertex_attrib_array2         = 0x8652;
constexpr libc_types::u32 vertex_attrib_array3         = 0x8653;
constexpr libc_types::u32 vertex_attrib_array4         = 0x8654;
constexpr libc_types::u32 vertex_attrib_array5         = 0x8655;
constexpr libc_types::u32 vertex_attrib_array6         = 0x8656;
constexpr libc_types::u32 vertex_attrib_array7         = 0x8657;
constexpr libc_types::u32 vertex_attrib_array8         = 0x8658;
constexpr libc_types::u32 vertex_attrib_array9         = 0x8659;
constexpr libc_types::u32 vertex_attrib_array10        = 0x865A;
constexpr libc_types::u32 vertex_attrib_array11        = 0x865B;
constexpr libc_types::u32 vertex_attrib_array12        = 0x865C;
constexpr libc_types::u32 vertex_attrib_array13        = 0x865D;
constexpr libc_types::u32 vertex_attrib_array14        = 0x865E;
constexpr libc_types::u32 vertex_attrib_array15        = 0x865F;
constexpr libc_types::u32 map1_vertex_attrib0_4        = 0x8660;
constexpr libc_types::u32 map1_vertex_attrib1_4        = 0x8661;
constexpr libc_types::u32 map1_vertex_attrib2_4        = 0x8662;
constexpr libc_types::u32 map1_vertex_attrib3_4        = 0x8663;
constexpr libc_types::u32 map1_vertex_attrib4_4        = 0x8664;
constexpr libc_types::u32 map1_vertex_attrib5_4        = 0x8665;
constexpr libc_types::u32 map1_vertex_attrib6_4        = 0x8666;
constexpr libc_types::u32 map1_vertex_attrib7_4        = 0x8667;
constexpr libc_types::u32 map1_vertex_attrib8_4        = 0x8668;
constexpr libc_types::u32 map1_vertex_attrib9_4        = 0x8669;
constexpr libc_types::u32 map1_vertex_attrib10_4       = 0x866A;
constexpr libc_types::u32 map1_vertex_attrib11_4       = 0x866B;
constexpr libc_types::u32 map1_vertex_attrib12_4       = 0x866C;
constexpr libc_types::u32 map1_vertex_attrib13_4       = 0x866D;
constexpr libc_types::u32 map1_vertex_attrib14_4       = 0x866E;
constexpr libc_types::u32 map1_vertex_attrib15_4       = 0x866F;
constexpr libc_types::u32 map2_vertex_attrib0_4        = 0x8670;
constexpr libc_types::u32 map2_vertex_attrib1_4        = 0x8671;
constexpr libc_types::u32 map2_vertex_attrib2_4        = 0x8672;
constexpr libc_types::u32 map2_vertex_attrib3_4        = 0x8673;
constexpr libc_types::u32 map2_vertex_attrib4_4        = 0x8674;
constexpr libc_types::u32 map2_vertex_attrib5_4        = 0x8675;
constexpr libc_types::u32 map2_vertex_attrib6_4        = 0x8676;
constexpr libc_types::u32 map2_vertex_attrib7_4        = 0x8677;
constexpr libc_types::u32 map2_vertex_attrib8_4        = 0x8678;
constexpr libc_types::u32 map2_vertex_attrib9_4        = 0x8679;
constexpr libc_types::u32 map2_vertex_attrib10_4       = 0x867A;
constexpr libc_types::u32 map2_vertex_attrib11_4       = 0x867B;
constexpr libc_types::u32 map2_vertex_attrib12_4       = 0x867C;
constexpr libc_types::u32 map2_vertex_attrib13_4       = 0x867D;
constexpr libc_types::u32 map2_vertex_attrib14_4       = 0x867E;
constexpr libc_types::u32 map2_vertex_attrib15_4       = 0x867F;
} // namespace values
template<class span_bool, class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_bool>&&
                              std::is_same_v<
                 std::decay_t<typename span_bool::value_type>,
                 std::decay_t<bool>>) STATICINLINE GLboolean
    are_programs_resident(span_const_u32 const& programs, span_bool residences)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AreProgramsResidentNV)
    }
    auto out = glAreProgramsResidentNV(
        programs.size(),
        programs.size() ? reinterpret_cast<const GLuint*>(programs.data())
                        : nullptr,
        residences.size() ? reinterpret_cast<GLboolean*>(residences.data())
                          : nullptr);
    detail::error_check("AreProgramsResidentNV"sv);
    return out;
}

STATICINLINE void bind_program(group::vertex_attrib_enum_nv target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindProgramNV)
    }
    glBindProgramNV(static_cast<GLenum>(target), id);
    detail::error_check("BindProgramNV"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void delete_programs(span_const_u32 const& programs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteProgramsNV)
    }
    glDeleteProgramsNV(
        programs.size(),
        programs.size() ? reinterpret_cast<const GLuint*>(programs.data())
                        : nullptr);
    detail::error_check("DeleteProgramsNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void execute_program(
        group::vertex_attrib_enum_nv target,
        u32                          id,
        span_const_f32 const&        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExecuteProgramNV)
    }
    glExecuteProgramNV(
        static_cast<GLenum>(target),
        id,
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("ExecuteProgramNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE void gen_programs(span_u32 programs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenProgramsNV)
    }
    glGenProgramsNV(
        programs.size(),
        programs.size() ? reinterpret_cast<GLuint*>(programs.data()) : nullptr);
    detail::error_check("GenProgramsNV"sv);
}

template<class span_f64>
requires(semantic::concepts::Span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void get_program_parameter(
        group::vertex_attrib_enum_nv target,
        u32                          index,
        group::vertex_attrib_enum_nv pname,
        span_f64                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramParameterdvNV)
    }
    glGetProgramParameterdvNV(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetProgramParameterdvNV"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_program_parameter(
        group::vertex_attrib_enum_nv target,
        u32                          index,
        group::vertex_attrib_enum_nv pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramParameterfvNV)
    }
    glGetProgramParameterfvNV(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetProgramParameterfvNV"sv);
}

template<class span_u8>
requires(semantic::concepts::Span<span_u8>&& std::is_same_v<
         std::decay_t<typename span_u8::value_type>,
         std::decay_t<u8>>) STATICINLINE
    void get_program_string(
        u32 id, group::vertex_attrib_enum_nv pname, span_u8 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramStringNV)
    }
    glGetProgramStringNV(
        id,
        static_cast<GLenum>(pname),
        program.size() ? reinterpret_cast<GLubyte*>(program.data()) : nullptr);
    detail::error_check("GetProgramStringNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_programiv(
        u32 id, group::vertex_attrib_enum_nv pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramivNV)
    }
    glGetProgramivNV(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramivNV"sv);
}

STATICINLINE void get_track_matrixiv(
    group::vertex_attrib_enum_nv target,
    u32                          address,
    group::vertex_attrib_enum_nv pname,
    i32&                         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTrackMatrixivNV)
    }
    glGetTrackMatrixivNV(
        static_cast<GLenum>(target),
        address,
        static_cast<GLenum>(pname),
        &params);
    detail::error_check("GetTrackMatrixivNV"sv);
}

template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_vertex_attrib_pointerv(
        u32 index, group::vertex_attrib_enum_nv pname, span_void pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribPointervNV)
    }
    glGetVertexAttribPointervNV(
        index,
        static_cast<GLenum>(pname),
        pointer.size() ? reinterpret_cast<void**>(pointer.data()) : nullptr);
    detail::error_check("GetVertexAttribPointervNV"sv);
}

STATICINLINE void get_vertex_attribdv(
    u32 index, group::vertex_attrib_enum_nv pname, f64& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribdvNV)
    }
    glGetVertexAttribdvNV(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribdvNV"sv);
}

STATICINLINE void get_vertex_attribfv(
    u32 index, group::vertex_attrib_enum_nv pname, f32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribfvNV)
    }
    glGetVertexAttribfvNV(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribfvNV"sv);
}

STATICINLINE void get_vertex_attribiv(
    u32 index, group::vertex_attrib_enum_nv pname, i32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribivNV)
    }
    glGetVertexAttribivNV(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribivNV"sv);
}

STATICINLINE GLboolean is_program(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsProgramNV)
    }
    auto out = glIsProgramNV(id);
    detail::error_check("IsProgramNV"sv);
    return out;
}

template<class span_const_u8>
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>) STATICINLINE
    void load_program(
        group::vertex_attrib_enum_nv target,
        u32                          id,
        span_const_u8 const&         program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LoadProgramNV)
    }
    glLoadProgramNV(
        static_cast<GLenum>(target),
        id,
        program.size(),
        program.size() ? reinterpret_cast<const GLubyte*>(program.data())
                       : nullptr);
    detail::error_check("LoadProgramNV"sv);
}

template<class vec_4_f64>
requires(semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void program_parameter4d(
        group::vertex_attrib_enum_nv target, u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameter4dNV)
    }
    glProgramParameter4dNV(
        static_cast<GLenum>(target), index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("ProgramParameter4dNV"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE void program_parameter4dv(
        group::vertex_attrib_enum_nv target,
        u32                          index,
        span_const_vec_4_f64 const&  v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameter4dvNV)
    }
    glProgramParameter4dvNV(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("ProgramParameter4dvNV"sv);
}

template<class vec_4_f32>
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void program_parameter4f(
        group::vertex_attrib_enum_nv target, u32 index, vec_4_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameter4fNV)
    }
    glProgramParameter4fNV(
        static_cast<GLenum>(target), index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("ProgramParameter4fNV"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void program_parameter4fv(
        group::vertex_attrib_enum_nv target,
        u32                          index,
        span_const_vec_4_f32 const&  v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameter4fvNV)
    }
    glProgramParameter4fvNV(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("ProgramParameter4fvNV"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE void program_parameters4dv(
        group::vertex_attrib_enum_nv target,
        u32                          index,
        i32                          count,
        span_const_vec_4_f64 const&  v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameters4dvNV)
    }
    glProgramParameters4dvNV(
        static_cast<GLenum>(target),
        index,
        count,
        reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("ProgramParameters4dvNV"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void program_parameters4fv(
        group::vertex_attrib_enum_nv target,
        u32                          index,
        i32                          count,
        span_const_vec_4_f32 const&  v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameters4fvNV)
    }
    glProgramParameters4fvNV(
        static_cast<GLenum>(target),
        index,
        count,
        reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("ProgramParameters4fvNV"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void request_resident_programs(span_const_u32 const& programs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RequestResidentProgramsNV)
    }
    glRequestResidentProgramsNV(
        programs.size(),
        programs.size() ? reinterpret_cast<const GLuint*>(programs.data())
                        : nullptr);
    detail::error_check("RequestResidentProgramsNV"sv);
}

STATICINLINE void track_matrix(
    group::vertex_attrib_enum_nv target,
    u32                          address,
    group::vertex_attrib_enum_nv matrix,
    group::vertex_attrib_enum_nv transform)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TrackMatrixNV)
    }
    glTrackMatrixNV(
        static_cast<GLenum>(target),
        address,
        static_cast<GLenum>(matrix),
        static_cast<GLenum>(transform));
    detail::error_check("TrackMatrixNV"sv);
}

STATICINLINE void vertex_attrib1d(u32 index, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1dNV)
    }
    glVertexAttrib1dNV(index, x);
    detail::error_check("VertexAttrib1dNV"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void vertex_attrib1dv(u32 index, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1dvNV)
    }
    glVertexAttrib1dvNV(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib1dvNV"sv);
}

STATICINLINE void vertex_attrib1f(u32 index, f32 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1fNV)
    }
    glVertexAttrib1fNV(index, x);
    detail::error_check("VertexAttrib1fNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void vertex_attrib1fv(u32 index, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1fvNV)
    }
    glVertexAttrib1fvNV(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib1fvNV"sv);
}

STATICINLINE void vertex_attrib1s(u32 index, i16 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1sNV)
    }
    glVertexAttrib1sNV(index, x);
    detail::error_check("VertexAttrib1sNV"sv);
}

template<class span_const_i16>
requires(semantic::concepts::Span<span_const_i16>&& std::is_same_v<
         std::decay_t<typename span_const_i16::value_type>,
         std::decay_t<i16>>) STATICINLINE
    void vertex_attrib1sv(u32 index, span_const_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1svNV)
    }
    glVertexAttrib1svNV(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib1svNV"sv);
}

template<class vec_2_f64>
requires(semantic::concepts::Vector<vec_2_f64, f64, 2>) STATICINLINE
    void vertex_attrib2d(u32 index, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2dNV)
    }
    glVertexAttrib2dNV(index, x.x(), x.y());
    detail::error_check("VertexAttrib2dNV"sv);
}

template<class span_const_vec_2_f64>
requires(semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
             Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    STATICINLINE void vertex_attrib2dv(u32 index, span_const_vec_2_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2dvNV)
    }
    glVertexAttrib2dvNV(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib2dvNV"sv);
}

template<class vec_2_f32>
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>) STATICINLINE
    void vertex_attrib2f(u32 index, vec_2_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2fNV)
    }
    glVertexAttrib2fNV(index, x.x(), x.y());
    detail::error_check("VertexAttrib2fNV"sv);
}

template<class span_const_vec_2_f32>
requires(semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
             Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    STATICINLINE void vertex_attrib2fv(u32 index, span_const_vec_2_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2fvNV)
    }
    glVertexAttrib2fvNV(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib2fvNV"sv);
}

template<class vec_2_i16>
requires(semantic::concepts::Vector<vec_2_i16, i16, 2>) STATICINLINE
    void vertex_attrib2s(u32 index, vec_2_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2sNV)
    }
    glVertexAttrib2sNV(index, x.x(), x.y());
    detail::error_check("VertexAttrib2sNV"sv);
}

template<class span_const_vec_2_i16>
requires(semantic::concepts::Span<span_const_vec_2_i16>&& semantic::concepts::
             Vector<typename span_const_vec_2_i16::value_type, i16, 2>)
    STATICINLINE void vertex_attrib2sv(u32 index, span_const_vec_2_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2svNV)
    }
    glVertexAttrib2svNV(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib2svNV"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void vertex_attrib3d(u32 index, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3dNV)
    }
    glVertexAttrib3dNV(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3dNV"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE void vertex_attrib3dv(u32 index, span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3dvNV)
    }
    glVertexAttrib3dvNV(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib3dvNV"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void vertex_attrib3f(u32 index, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3fNV)
    }
    glVertexAttrib3fNV(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3fNV"sv);
}

template<class span_const_vec_3_f32>
requires(semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
             Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE void vertex_attrib3fv(u32 index, span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3fvNV)
    }
    glVertexAttrib3fvNV(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib3fvNV"sv);
}

template<class vec_3_i16>
requires(semantic::concepts::Vector<vec_3_i16, i16, 3>) STATICINLINE
    void vertex_attrib3s(u32 index, vec_3_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3sNV)
    }
    glVertexAttrib3sNV(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3sNV"sv);
}

template<class span_const_vec_3_i16>
requires(semantic::concepts::Span<span_const_vec_3_i16>&& semantic::concepts::
             Vector<typename span_const_vec_3_i16::value_type, i16, 3>)
    STATICINLINE void vertex_attrib3sv(u32 index, span_const_vec_3_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3svNV)
    }
    glVertexAttrib3svNV(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib3svNV"sv);
}

template<class vec_4_f64>
requires(semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void vertex_attrib4d(u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4dNV)
    }
    glVertexAttrib4dNV(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4dNV"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE void vertex_attrib4dv(u32 index, span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4dvNV)
    }
    glVertexAttrib4dvNV(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib4dvNV"sv);
}

template<class vec_4_f32>
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void vertex_attrib4f(u32 index, vec_4_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4fNV)
    }
    glVertexAttrib4fNV(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4fNV"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void vertex_attrib4fv(u32 index, span_const_vec_4_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4fvNV)
    }
    glVertexAttrib4fvNV(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib4fvNV"sv);
}

template<class vec_4_i16>
requires(semantic::concepts::Vector<vec_4_i16, i16, 4>) STATICINLINE
    void vertex_attrib4s(u32 index, vec_4_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4sNV)
    }
    glVertexAttrib4sNV(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4sNV"sv);
}

template<class span_const_vec_4_i16>
requires(semantic::concepts::Span<span_const_vec_4_i16>&& semantic::concepts::
             Vector<typename span_const_vec_4_i16::value_type, i16, 4>)
    STATICINLINE void vertex_attrib4sv(u32 index, span_const_vec_4_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4svNV)
    }
    glVertexAttrib4svNV(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib4svNV"sv);
}

template<class vec_4_u8>
requires(semantic::concepts::Vector<vec_4_u8, u8, 4>) STATICINLINE
    void vertex_attrib4ub(u32 index, vec_4_u8 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4ubNV)
    }
    glVertexAttrib4ubNV(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4ubNV"sv);
}

template<class span_const_u8>
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>) STATICINLINE
    void vertex_attrib4ubv(u32 index, span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4ubvNV)
    }
    glVertexAttrib4ubvNV(
        index, v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4ubvNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void vertex_attrib_pointer(
        u32                          index,
        i32                          fsize,
        group::vertex_attrib_enum_nv type,
        i32                          stride,
        span_const_void const&       pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribPointerNV)
    }
    glVertexAttribPointerNV(
        index,
        fsize,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribPointerNV"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void vertex_attribs1dv(u32 index, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs1dvNV)
    }
    glVertexAttribs1dvNV(
        index, v.size(), reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribs1dvNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void vertex_attribs1fv(u32 index, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs1fvNV)
    }
    glVertexAttribs1fvNV(
        index, v.size(), reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttribs1fvNV"sv);
}

template<class span_const_i16>
requires(semantic::concepts::Span<span_const_i16>&& std::is_same_v<
         std::decay_t<typename span_const_i16::value_type>,
         std::decay_t<i16>>) STATICINLINE
    void vertex_attribs1sv(u32 index, span_const_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs1svNV)
    }
    glVertexAttribs1svNV(
        index, v.size(), reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttribs1svNV"sv);
}

template<class span_const_vec_2_f64>
requires(semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
             Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    STATICINLINE
    void vertex_attribs2dv(u32 index, i32 count, span_const_vec_2_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs2dvNV)
    }
    glVertexAttribs2dvNV(
        index, count, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribs2dvNV"sv);
}

template<class span_const_vec_2_f32>
requires(semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
             Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    STATICINLINE
    void vertex_attribs2fv(u32 index, i32 count, span_const_vec_2_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs2fvNV)
    }
    glVertexAttribs2fvNV(
        index, count, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttribs2fvNV"sv);
}

template<class span_const_vec_2_i16>
requires(semantic::concepts::Span<span_const_vec_2_i16>&& semantic::concepts::
             Vector<typename span_const_vec_2_i16::value_type, i16, 2>)
    STATICINLINE
    void vertex_attribs2sv(u32 index, i32 count, span_const_vec_2_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs2svNV)
    }
    glVertexAttribs2svNV(
        index, count, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttribs2svNV"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE
    void vertex_attribs3dv(u32 index, i32 count, span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs3dvNV)
    }
    glVertexAttribs3dvNV(
        index, count, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribs3dvNV"sv);
}

template<class span_const_vec_3_f32>
requires(semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
             Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE
    void vertex_attribs3fv(u32 index, i32 count, span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs3fvNV)
    }
    glVertexAttribs3fvNV(
        index, count, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttribs3fvNV"sv);
}

template<class span_const_vec_3_i16>
requires(semantic::concepts::Span<span_const_vec_3_i16>&& semantic::concepts::
             Vector<typename span_const_vec_3_i16::value_type, i16, 3>)
    STATICINLINE
    void vertex_attribs3sv(u32 index, i32 count, span_const_vec_3_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs3svNV)
    }
    glVertexAttribs3svNV(
        index, count, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttribs3svNV"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE
    void vertex_attribs4dv(u32 index, i32 count, span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs4dvNV)
    }
    glVertexAttribs4dvNV(
        index, count, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribs4dvNV"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE
    void vertex_attribs4fv(u32 index, i32 count, span_const_vec_4_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs4fvNV)
    }
    glVertexAttribs4fvNV(
        index, count, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttribs4fvNV"sv);
}

template<class span_const_vec_4_i16>
requires(semantic::concepts::Span<span_const_vec_4_i16>&& semantic::concepts::
             Vector<typename span_const_vec_4_i16::value_type, i16, 4>)
    STATICINLINE
    void vertex_attribs4sv(u32 index, i32 count, span_const_vec_4_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs4svNV)
    }
    glVertexAttribs4svNV(
        index, count, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttribs4svNV"sv);
}

template<class span_const_u8>
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>) STATICINLINE
    void vertex_attribs4ubv(u32 index, i32 count, span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribs4ubvNV)
    }
    glVertexAttribs4ubvNV(
        index,
        count,
        v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttribs4ubvNV"sv);
}

} // namespace gl::nv::vertex_program
#endif // GL_NV_vertex_program
namespace gl::nv::vertex_program {
constexpr auto name = "GL_NV_vertex_program";
}
