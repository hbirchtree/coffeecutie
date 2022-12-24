#pragma once

#ifdef GL_EXT_fragment_shading_rate
#include "../enums/FramebufferAttachment.h"
#include "../enums/GetPName.h"
#include "../enums/ShadingRate.h"
#include "../enums/ShadingRateCombinerOp.h"
namespace gl::ext::fragment_shading_rate {
using gl::group::framebuffer_attachment;
using gl::group::get_prop;
using gl::group::shading_rate;
using gl::group::shading_rate_combiner_op;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_EXT_fragment_shading_rate
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param baseLayer GLint
 * \param numLayers GLsizei
 * \param texelWidth GLsizei
 * \param texelHeight GLsizei
 * \return void
 */
STATICINLINE void framebuffer_shading_rate(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           baseLayer,
    i32                           numLayers,
    i32                           texelWidth,
    i32                           texelHeight)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferShadingRateEXT)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferShadingRateEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        baseLayer,
        numLayers,
        texelWidth,
        texelHeight);
    detail::error_check("FramebufferShadingRateEXT"sv);
}

template<class span_shading_rate>
requires(semantic::concepts::Span<span_shading_rate>&& std::is_same_v<
         std::decay_t<typename span_shading_rate::value_type>,
         std::decay_t<group::shading_rate>>)
    /*!
     * \brief Part of GL_EXT_fragment_shading_rate
     * \param samples GLsizei
     * \param maxCount GLsizei
     * \param count GLsizei *
     * \param shadingRates GLenum *
     * \return void
     */
    STATICINLINE void get_fragment_shading_rates(
        i32 samples, i32& count, span_shading_rate shadingRates)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragmentShadingRatesEXT)
    }
    glGetFragmentShadingRatesEXT(
        samples,
        shadingRates.size(),
        &count,
        shadingRates.size() ? reinterpret_cast<GLenum*>(shadingRates.data())
                            : nullptr);
    detail::error_check("GetFragmentShadingRatesEXT"sv);
}

/*!
 * \brief Part of GL_EXT_fragment_shading_rate
 * \param rate GLenum
 * \return void
 */
STATICINLINE void shading_rate(group::shading_rate rate)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShadingRateEXT)
    }
    glShadingRateEXT(static_cast<GLenum>(rate));
    detail::error_check("ShadingRateEXT"sv);
}

/*!
 * \brief Part of GL_EXT_fragment_shading_rate
 * \param combinerOp0 GLenum
 * \param combinerOp1 GLenum
 * \return void
 */
STATICINLINE void shading_rate_combiner_ops(
    group::shading_rate_combiner_op combinerOp0,
    group::shading_rate_combiner_op combinerOp1)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShadingRateCombinerOpsEXT)
    }
    glShadingRateCombinerOpsEXT(
        static_cast<GLenum>(combinerOp0), static_cast<GLenum>(combinerOp1));
    detail::error_check("ShadingRateCombinerOpsEXT"sv);
}

} // namespace gl::ext::fragment_shading_rate
#endif // GL_EXT_fragment_shading_rate
namespace gl::ext::fragment_shading_rate {
constexpr auto name = "GL_EXT_fragment_shading_rate";
}
