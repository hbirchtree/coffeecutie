#pragma once

#ifdef GL_QCOM_frame_extrapolation
namespace gl::qcom::frame_extrapolation {
namespace values {
} // namespace values
STATICINLINE void extrapolate_tex_2d(
    u32 src1, u32 src2, u32 output, f32 scaleFactor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtrapolateTex2DQCOM)
    }
    glExtrapolateTex2DQCOM(src1, src2, output, scaleFactor);
    detail::error_check("ExtrapolateTex2DQCOM"sv);
}

} // namespace gl::qcom::frame_extrapolation
#endif // GL_QCOM_frame_extrapolation
namespace gl::qcom::frame_extrapolation {
constexpr auto name = "GL_QCOM_frame_extrapolation";
}
