#pragma once

#ifdef GL_QCOM_tiled_rendering
#include "../enums/BufferBitQCOM.h"

namespace gl::qcom::tiled_rendering {
using gl::group::buffer_bit_qcom;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_QCOM_tiled_rendering
 * \param preserveMask GLbitfield
 * \return void
 */
STATICINLINE void end_tiling(group::buffer_bit_qcom preserveMask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndTilingQCOM)
    }
    glEndTilingQCOM(static_cast<GLenum>(preserveMask));
    detail::error_check("EndTilingQCOM"sv);
}

template<class size_2_u32, class vec_2_u32>
requires(
    concepts::vector<vec_2_u32, u32, 2> && concepts::size_2d<size_2_u32, u32>)
/*!
 * \brief Part of GL_QCOM_tiled_rendering
 * \param x GLuint
 * \param y GLuint
 * \param width GLuint
 * \param height GLuint
 * \param preserveMask GLbitfield
 * \return void
 */
STATICINLINE void start_tiling(
    vec_2_u32 const&       x,
    size_2_u32 const&      width,
    group::buffer_bit_qcom preserveMask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StartTilingQCOM)
    }
    glStartTilingQCOM(
        x[0], x[1], width[0], width[1], static_cast<GLenum>(preserveMask));
    detail::error_check("StartTilingQCOM"sv);
}

} // namespace gl::qcom::tiled_rendering
#endif // GL_QCOM_tiled_rendering
namespace gl::qcom::tiled_rendering {
constexpr auto name = "GL_QCOM_tiled_rendering";
}
