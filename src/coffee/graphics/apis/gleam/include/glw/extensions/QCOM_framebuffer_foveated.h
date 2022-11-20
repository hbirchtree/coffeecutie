#pragma once

#ifdef GL_QCOM_framebuffer_foveated
#include "../enums/FoveationConfigBitQCOM.h"
namespace gl::qcom::framebuffer_foveated {
using gl::group::foveation_config_bit_qcom;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_QCOM_framebuffer_foveated
 * \param framebuffer GLuint
 * \param numLayers GLuint
 * \param focalPointsPerLayer GLuint
 * \param requestedFeatures GLuint
 * \param providedFeatures GLuint *
 * \return void
 */
STATICINLINE void framebuffer_foveation_config(
    u32  framebuffer,
    u32  numLayers,
    u32  focalPointsPerLayer,
    u32  requestedFeatures,
    u32& providedFeatures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferFoveationConfigQCOM)
        glIsFramebuffer(framebuffer);
    }
    glFramebufferFoveationConfigQCOM(
        framebuffer,
        numLayers,
        focalPointsPerLayer,
        requestedFeatures,
        &providedFeatures);
    detail::error_check("FramebufferFoveationConfigQCOM"sv);
}

/*!
 * \brief Part of GL_QCOM_framebuffer_foveated
 * \param framebuffer GLuint
 * \param layer GLuint
 * \param focalPoint GLuint
 * \param focalX GLfloat
 * \param focalY GLfloat
 * \param gainX GLfloat
 * \param gainY GLfloat
 * \param foveaArea GLfloat
 * \return void
 */
STATICINLINE void framebuffer_foveation_parameter(
    u32 framebuffer,
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
        GLW_FPTR_CHECK(FramebufferFoveationParametersQCOM)
        glIsFramebuffer(framebuffer);
    }
    glFramebufferFoveationParametersQCOM(
        framebuffer,
        layer,
        focalPoint,
        focalX,
        focalY,
        gainX,
        gainY,
        foveaArea);
    detail::error_check("FramebufferFoveationParametersQCOM"sv);
}

} // namespace gl::qcom::framebuffer_foveated
#endif // GL_QCOM_framebuffer_foveated
namespace gl::qcom::framebuffer_foveated {
constexpr auto name = "GL_QCOM_framebuffer_foveated";
}
