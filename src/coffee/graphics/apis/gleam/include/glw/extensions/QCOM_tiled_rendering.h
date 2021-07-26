#pragma once

#ifdef GL_QCOM_tiled_rendering
#include "../enums/BufferBitQCOM.h"
namespace gl::qcom::tiled_rendering {
using gl::group::buffer_bit_qcom;
namespace values {
} // namespace values
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
requires(semantic::concepts::Vector<vec_2_u32, u32, 2>&&
             semantic::concepts::Size2D<size_2_u32, u32>) STATICINLINE
    void start_tiling(
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
        x.x(), x.y(), width[0], width[1], static_cast<GLenum>(preserveMask));
    detail::error_check("StartTilingQCOM"sv);
}

} // namespace gl::qcom::tiled_rendering
#endif // GL_QCOM_tiled_rendering
namespace gl::qcom::tiled_rendering {
constexpr auto name = "GL_QCOM_tiled_rendering";
}
