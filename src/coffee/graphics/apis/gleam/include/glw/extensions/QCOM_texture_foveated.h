#pragma once

#ifdef GL_QCOM_texture_foveated
#include "../enums/FoveationConfigBitQCOM.h"
namespace gl::qcom::texture_foveated {
using gl::group::foveation_config_bit_qcom;
namespace values {
constexpr libc_types::u32 texture_foveated_feature_bits           = 0x8BFB;
constexpr libc_types::u32 texture_foveated_min_pixel_density      = 0x8BFC;
constexpr libc_types::u32 texture_foveated_feature_query          = 0x8BFD;
constexpr libc_types::u32 texture_foveated_num_focal_points_query = 0x8BFE;
constexpr libc_types::u32 framebuffer_incomplete_foveation        = 0x8BFF;
} // namespace values
STATICINLINE void texture_foveation_parameter(
    u32 texture,
    u32 layer,
    u32 focalPoint,
    f32 focalX,
    f32 focalY,
    f32 gainX,
    f32 gainY,
    f32 foveaArea)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureFoveationParametersQCOM)
        glIsTexture(texture);
    }
    glTextureFoveationParametersQCOM(
        texture, layer, focalPoint, focalX, focalY, gainX, gainY, foveaArea);
    detail::error_check("TextureFoveationParametersQCOM"sv);
}

} // namespace gl::qcom::texture_foveated
#endif // GL_QCOM_texture_foveated
namespace gl::qcom::texture_foveated {
constexpr auto name = "GL_QCOM_texture_foveated";
}
