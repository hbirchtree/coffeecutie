#pragma once

#ifdef GL_EXT_vertex_attrib_64bit
#include "../enums/AttributeType.h"
#include "../enums/BinormalPointerTypeEXT.h"
#include "../enums/FogCoordinatePointerType.h"
#include "../enums/FogPointerTypeEXT.h"
#include "../enums/FogPointerTypeIBM.h"
#include "../enums/IndexPointerType.h"
#include "../enums/MapTypeNV.h"
#include "../enums/NormalPointerType.h"
#include "../enums/SecondaryColorPointerTypeIBM.h"
#include "../enums/TangentPointerTypeEXT.h"
#include "../enums/TexCoordPointerType.h"
#include "../enums/UniformType.h"
#include "../enums/VertexAttribLType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
#include "../enums/VertexPointerType.h"
#include "../enums/WeightPointerTypeARB.h"
namespace gl::ext::vertex_attrib_64bit {
using gl::group::attribute_type;
using gl::group::binormal_pointer_type_ext;
using gl::group::fog_coordinate_pointer_type;
using gl::group::fog_pointer_type_ext;
using gl::group::fog_pointer_type_ibm;
using gl::group::index_pointer_type;
using gl::group::map_type_nv;
using gl::group::normal_pointer_type;
using gl::group::secondary_color_pointer_type_ibm;
using gl::group::tangent_pointer_type_ext;
using gl::group::tex_coord_pointer_type;
using gl::group::uniform_type;
using gl::group::vertex_attrib_long;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
using gl::group::vertex_pointer_type;
using gl::group::weight_pointer_type_arb;
namespace values {
constexpr libc_types::u32 double_mat2   = 0x8F46;
constexpr libc_types::u32 double_mat3   = 0x8F47;
constexpr libc_types::u32 double_mat4   = 0x8F48;
constexpr libc_types::u32 double_mat2x3 = 0x8F49;
constexpr libc_types::u32 double_mat2x4 = 0x8F4A;
constexpr libc_types::u32 double_mat3x2 = 0x8F4B;
constexpr libc_types::u32 double_mat3x4 = 0x8F4C;
constexpr libc_types::u32 double_mat4x2 = 0x8F4D;
constexpr libc_types::u32 double_mat4x3 = 0x8F4E;
constexpr libc_types::u32 double_vec2   = 0x8FFC;
constexpr libc_types::u32 double_vec3   = 0x8FFD;
constexpr libc_types::u32 double_vec4   = 0x8FFE;
} // namespace values
template<class span_f64>
requires(semantic::concepts::Span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param pname GLenum
     * \param params GLdouble *
     * \return void
     */
    STATICINLINE void get_vertex_attrib_ldv(
        u32 index, group::vertex_attrib_enum pname, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribLdvEXT)
    }
    glGetVertexAttribLdvEXT(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribLdvEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_attrib_64bit
 * \param index GLuint
 * \param x GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib_l1d(u32 index, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1dEXT)
    }
    glVertexAttribL1dEXT(index, x);
    detail::error_check("VertexAttribL1dEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param v const GLdouble *
     * \return void
     */
    STATICINLINE void vertex_attrib_l1dv(u32 index, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1dvEXT)
    }
    glVertexAttribL1dvEXT(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL1dvEXT"sv);
}

template<class vec_2_f64>
requires(semantic::concepts::Vector<vec_2_f64, f64, 2>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param x GLdouble
     * \param y GLdouble
     * \return void
     */
    STATICINLINE void vertex_attrib_l2d(u32 index, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2dEXT)
    }
    glVertexAttribL2dEXT(index, x.x(), x.y());
    detail::error_check("VertexAttribL2dEXT"sv);
}

template<class span_const_vec_2_f64>
requires(semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
             Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param v const GLdouble *
     * \return void
     */
    STATICINLINE
    void vertex_attrib_l2dv(u32 index, span_const_vec_2_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2dvEXT)
    }
    glVertexAttribL2dvEXT(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL2dvEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param x GLdouble
     * \param y GLdouble
     * \param z GLdouble
     * \return void
     */
    STATICINLINE void vertex_attrib_l3d(u32 index, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3dEXT)
    }
    glVertexAttribL3dEXT(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttribL3dEXT"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param v const GLdouble *
     * \return void
     */
    STATICINLINE
    void vertex_attrib_l3dv(u32 index, span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3dvEXT)
    }
    glVertexAttribL3dvEXT(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL3dvEXT"sv);
}

template<class vec_4_f64>
requires(semantic::concepts::Vector<vec_4_f64, f64, 4>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param x GLdouble
     * \param y GLdouble
     * \param z GLdouble
     * \param w GLdouble
     * \return void
     */
    STATICINLINE void vertex_attrib_l4d(u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4dEXT)
    }
    glVertexAttribL4dEXT(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribL4dEXT"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param v const GLdouble *
     * \return void
     */
    STATICINLINE
    void vertex_attrib_l4dv(u32 index, span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4dvEXT)
    }
    glVertexAttribL4dvEXT(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL4dvEXT"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_EXT_vertex_attrib_64bit
     * \param index GLuint
     * \param size GLint
     * \param type GLenum
     * \param stride GLsizei
     * \param pointer const void *
     * \return void
     */
    STATICINLINE void vertex_attrib_l_pointer(
        u32                       index,
        group::vertex_attrib_long type,
        i32                       stride,
        span_const_void const&    pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribLPointerEXT)
    }
    glVertexAttribLPointerEXT(
        index,
        pointer.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribLPointerEXT"sv);
}

} // namespace gl::ext::vertex_attrib_64bit
#endif // GL_EXT_vertex_attrib_64bit
namespace gl::ext::vertex_attrib_64bit {
constexpr auto name = "GL_EXT_vertex_attrib_64bit";
}
