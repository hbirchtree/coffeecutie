#pragma once

#ifdef GL_EXT_vertex_weighting
#include "../enums/GetPName.h"
#include "../enums/MatrixMode.h"
namespace gl::ext::vertex_weighting {
using gl::group::get_prop;
using gl::group::matrix_mode;
namespace values {
constexpr u32 modelview1_stack_depth      = 0x8502;
constexpr u32 modelview1_matrix           = 0x8506;
constexpr u32 vertex_weighting            = 0x8509;
constexpr u32 modelview1                  = 0x850A;
constexpr u32 current_vertex_weight       = 0x850B;
constexpr u32 vertex_weight_array         = 0x850C;
constexpr u32 vertex_weight_array_size    = 0x850D;
constexpr u32 vertex_weight_array_type    = 0x850E;
constexpr u32 vertex_weight_array_stride  = 0x850F;
constexpr u32 vertex_weight_array_pointer = 0x8510;
} // namespace values
template<class span_const_void>
requires(concepts::span<span_const_void>)
    /*!
     * \brief Part of GL_EXT_vertex_weighting
     * \param size GLint
     * \param type GLenum
     * \param stride GLsizei
     * \param pointer const void *
     * \return void
     */
    STATICINLINE void vertex_weight_pointer(
        i32                                   size,
        group::vertex_weight_pointer_type_ext type,
        i32                                   stride,
        span_const_void const&                pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexWeightPointerEXT)
    }
    glVertexWeightPointerEXT(
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexWeightPointerEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_weighting
 * \param weight GLfloat
 * \return void
 */
STATICINLINE void vertex_weightf(f32 weight)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexWeightfEXT)
    }
    glVertexWeightfEXT(weight);
    detail::error_check("VertexWeightfEXT"sv);
}

template<class span_const_f32>
requires(concepts::span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_EXT_vertex_weighting
     * \param weight const GLfloat *
     * \return void
     */
    STATICINLINE void vertex_weightfv(span_const_f32 const& weight)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexWeightfvEXT)
    }
    glVertexWeightfvEXT(
        weight.size() ? reinterpret_cast<const GLfloat*>(weight.data())
                      : nullptr);
    detail::error_check("VertexWeightfvEXT"sv);
}

} // namespace gl::ext::vertex_weighting
#endif // GL_EXT_vertex_weighting
namespace gl::ext::vertex_weighting {
constexpr auto name = "GL_EXT_vertex_weighting";
}
