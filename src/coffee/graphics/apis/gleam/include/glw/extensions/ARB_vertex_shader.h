#pragma once

#ifdef GL_ARB_vertex_shader
#include "../enums/AttributeType.h"
#include "../enums/BinormalPointerTypeEXT.h"
#include "../enums/FogCoordinatePointerType.h"
#include "../enums/FogPointerTypeEXT.h"
#include "../enums/FogPointerTypeIBM.h"
#include "../enums/IndexPointerType.h"
#include "../enums/ListNameType.h"
#include "../enums/MapTypeNV.h"
#include "../enums/NormalPointerType.h"
#include "../enums/PixelType.h"
#include "../enums/SecondaryColorPointerTypeIBM.h"
#include "../enums/ShaderType.h"
#include "../enums/TangentPointerTypeEXT.h"
#include "../enums/TexCoordPointerType.h"
#include "../enums/UniformType.h"
#include "../enums/VertexAttribPointerPropertyARB.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
#include "../enums/VertexPointerType.h"
#include "../enums/VertexWeightPointerTypeEXT.h"
#include "../enums/WeightPointerTypeARB.h"
namespace gl::arb::vertex_shader {
using gl::group::attribute_type;
using gl::group::binormal_pointer_type_ext;
using gl::group::fog_coordinate_pointer_type;
using gl::group::fog_pointer_type_ext;
using gl::group::fog_pointer_type_ibm;
using gl::group::index_pointer_type;
using gl::group::list_name_type;
using gl::group::map_type_nv;
using gl::group::normal_pointer_type;
using gl::group::pixel_type;
using gl::group::secondary_color_pointer_type_ibm;
using gl::group::shader_type;
using gl::group::tangent_pointer_type_ext;
using gl::group::tex_coord_pointer_type;
using gl::group::uniform_type;
using gl::group::vertex_attrib_pointer_property_arb;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
using gl::group::vertex_pointer_type;
using gl::group::vertex_weight_pointer_type_ext;
using gl::group::weight_pointer_type_arb;
namespace values {
constexpr libc_types::u32 vertex_attrib_array_enabled        = 0x8622;
constexpr libc_types::u32 vertex_attrib_array_size           = 0x8623;
constexpr libc_types::u32 vertex_attrib_array_stride         = 0x8624;
constexpr libc_types::u32 vertex_attrib_array_type           = 0x8625;
constexpr libc_types::u32 current_vertex_attrib              = 0x8626;
constexpr libc_types::u32 vertex_program_point_size          = 0x8642;
constexpr libc_types::u32 vertex_program_two_side            = 0x8643;
constexpr libc_types::u32 max_vertex_attribs                 = 0x8869;
constexpr libc_types::u32 vertex_attrib_array_normalized     = 0x886A;
constexpr libc_types::u32 max_texture_coords                 = 0x8871;
constexpr libc_types::u32 max_texture_image_units            = 0x8872;
constexpr libc_types::u32 max_vertex_uniform_components      = 0x8B4A;
constexpr libc_types::u32 max_varying_floats                 = 0x8B4B;
constexpr libc_types::u32 max_vertex_texture_image_units     = 0x8B4C;
constexpr libc_types::u32 max_combined_texture_image_units   = 0x8B4D;
constexpr libc_types::u32 object_active_attributes           = 0x8B89;
constexpr libc_types::u32 object_active_attribute_max_length = 0x8B8A;
} // namespace values
template<class span_const_GLcharARB>
requires(semantic::concepts::Span<span_const_GLcharARB>&& std::is_same_v<
         std::decay_t<typename span_const_GLcharARB::value_type>,
         std::decay_t<GLcharARB>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
     * \param programObj GLhandleARB
     * \param index GLuint
     * \param name const GLcharARB *
     * \return void
     */
    STATICINLINE void bind_attrib_location(
        GLhandleARB programObj, u32 index, span_const_GLcharARB const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindAttribLocationARB)
    }
    glBindAttribLocationARB(
        programObj,
        index,
        name.size() ? reinterpret_cast<const GLcharARB*>(name.data())
                    : nullptr);
    detail::error_check("BindAttribLocationARB"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_shader
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
 * \brief Part of GL_ARB_vertex_shader
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

template<class span_GLcharARB>
requires(semantic::concepts::Span<span_GLcharARB>&& std::is_same_v<
         std::decay_t<typename span_GLcharARB::value_type>,
         std::decay_t<GLcharARB>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
     * \param programObj GLhandleARB
     * \param index GLuint
     * \param maxLength GLsizei
     * \param length GLsizei *
     * \param size GLint *
     * \param type GLenum *
     * \param name GLcharARB *
     * \return void
     */
    STATICINLINE void get_active_attrib(
        GLhandleARB    programObj,
        u32            index,
        i32&           length,
        i32&           size,
        GLenum&        type,
        span_GLcharARB name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveAttribARB)
    }
    glGetActiveAttribARB(
        programObj,
        index,
        name.size(),
        &length,
        &size,
        &type,
        name.size() ? reinterpret_cast<GLcharARB*>(name.data()) : nullptr);
    detail::error_check("GetActiveAttribARB"sv);
}

template<class span_const_GLcharARB>
requires(semantic::concepts::Span<span_const_GLcharARB>&& std::is_same_v<
         std::decay_t<typename span_const_GLcharARB::value_type>,
         std::decay_t<GLcharARB>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
     * \param programObj GLhandleARB
     * \param name const GLcharARB *
     * \return GLint
     */
    STATICINLINE GLint get_attrib_location(
        GLhandleARB programObj, span_const_GLcharARB const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttribLocationARB)
    }
    auto out = glGetAttribLocationARB(
        programObj,
        name.size() ? reinterpret_cast<const GLcharARB*>(name.data())
                    : nullptr);
    detail::error_check("GetAttribLocationARB"sv);
    return out;
}

template<class span_void>
requires(semantic::concepts::Span<span_void>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
 * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
 * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
 * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_i16>&& std::is_same_v<
         std::decay_t<typename span_const_i16::value_type>,
         std::decay_t<i16>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_2_f64, f64, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib2dARB(index, x.x(), x.y());
    detail::error_check("VertexAttrib2dARB"sv);
}

template<class span_const_vec_2_f64>
requires(semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
             Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib2fARB(index, x.x(), x.y());
    detail::error_check("VertexAttrib2fARB"sv);
}

template<class span_const_vec_2_f32>
requires(semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
             Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_2_i16, i16, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib2sARB(index, x.x(), x.y());
    detail::error_check("VertexAttrib2sARB"sv);
}

template<class span_const_vec_2_i16>
requires(semantic::concepts::Span<span_const_vec_2_i16>&& semantic::concepts::
             Vector<typename span_const_vec_2_i16::value_type, i16, 2>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib3dARB(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3dARB"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib3fARB(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3fARB"sv);
}

template<class span_const_vec_3_f32>
requires(semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
             Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_3_i16, i16, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib3sARB(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3sARB"sv);
}

template<class span_const_vec_3_i16>
requires(semantic::concepts::Span<span_const_vec_3_i16>&& semantic::concepts::
             Vector<typename span_const_vec_3_i16::value_type, i16, 3>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_i16>&& std::is_same_v<
         std::decay_t<typename span_const_i16::value_type>,
         std::decay_t<i16>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_4_u8, u8, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib4NubARB(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4NubARB"sv);
}

template<class span_const_u8>
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_u16>&& std::is_same_v<
         std::decay_t<typename span_const_u16::value_type>,
         std::decay_t<u16>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_4_f64, f64, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib4dARB(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4dARB"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib4fARB(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4fARB"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Vector<vec_4_i16, i16, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
    glVertexAttrib4sARB(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4sARB"sv);
}

template<class span_const_vec_4_i16>
requires(semantic::concepts::Span<span_const_vec_4_i16>&& semantic::concepts::
             Vector<typename span_const_vec_4_i16::value_type, i16, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
     * \param index GLuint
     * \param v const GLuint *
     * \return void
     */
    STATICINLINE
    void vertex_attrib4uiv(u32 index, span_const_vec_4_u32 const& v)
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
requires(semantic::concepts::Span<span_const_vec_4_u16>&& semantic::concepts::
             Vector<typename span_const_vec_4_u16::value_type, u16, 4>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
     * \param index GLuint
     * \param v const GLushort *
     * \return void
     */
    STATICINLINE
    void vertex_attrib4usv(u32 index, span_const_vec_4_u16 const& v)
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
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_ARB_vertex_shader
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

} // namespace gl::arb::vertex_shader
#endif // GL_ARB_vertex_shader
namespace gl::arb::vertex_shader {
constexpr auto name = "GL_ARB_vertex_shader";
}
