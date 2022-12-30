#pragma once

#ifdef GL_NV_shading_rate_image
namespace gl::nv::shading_rate_image {
namespace values {
constexpr libc_types::u32 shading_rate_image_binding               = 0x955B;
constexpr libc_types::u32 shading_rate_image_texel_width           = 0x955C;
constexpr libc_types::u32 shading_rate_image_texel_height          = 0x955D;
constexpr libc_types::u32 shading_rate_image_palette_size          = 0x955E;
constexpr libc_types::u32 max_coarse_fragment_samples              = 0x955F;
constexpr libc_types::u32 shading_rate_image                       = 0x9563;
constexpr libc_types::u32 shading_rate_no_invocations              = 0x9564;
constexpr libc_types::u32 shading_rate_1_invocation_per_pixel      = 0x9565;
constexpr libc_types::u32 shading_rate_1_invocation_per_1x2_pixels = 0x9566;
constexpr libc_types::u32 shading_rate_1_invocation_per_2x1_pixels = 0x9567;
constexpr libc_types::u32 shading_rate_1_invocation_per_2x2_pixels = 0x9568;
constexpr libc_types::u32 shading_rate_1_invocation_per_2x4_pixels = 0x9569;
constexpr libc_types::u32 shading_rate_1_invocation_per_4x2_pixels = 0x956A;
constexpr libc_types::u32 shading_rate_1_invocation_per_4x4_pixels = 0x956B;
constexpr libc_types::u32 shading_rate_2_invocations_per_pixel     = 0x956C;
constexpr libc_types::u32 shading_rate_4_invocations_per_pixel     = 0x956D;
constexpr libc_types::u32 shading_rate_8_invocations_per_pixel     = 0x956E;
constexpr libc_types::u32 shading_rate_16_invocations_per_pixel    = 0x956F;
constexpr libc_types::u32 shading_rate_sample_order_default        = 0x95AE;
constexpr libc_types::u32 shading_rate_sample_order_pixel_major    = 0x95AF;
constexpr libc_types::u32 shading_rate_sample_order_sample_major   = 0x95B0;
} // namespace values
/*!
 * \brief Part of GL_NV_shading_rate_image
 * \param texture GLuint
 * \return void
 */
STATICINLINE void bind_shading_rate_image(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindShadingRateImageNV)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindShadingRateImageNV(texture);
    detail::error_check("BindShadingRateImageNV"sv);
}

/*!
 * \brief Part of GL_NV_shading_rate_image
 * \param viewport GLuint
 * \param entry GLuint
 * \param rate GLenum *
 * \return void
 */
STATICINLINE void get_shading_rate_image_palette(
    u32 viewport, u32 entry, GLenum& rate)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShadingRateImagePaletteNV)
    }
    glGetShadingRateImagePaletteNV(viewport, entry, &rate);
    detail::error_check("GetShadingRateImagePaletteNV"sv);
}

template<class span_i32>
requires(
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_shading_rate_image
 * \param rate GLenum
 * \param samples GLuint
 * \param index GLuint
 * \param location GLint *
 * \return void
 */
STATICINLINE void get_shading_rate_sample_locationiv(
    GLenum rate, u32 samples, u32 index, span_i32 location)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShadingRateSampleLocationivNV)
    }
    glGetShadingRateSampleLocationivNV(
        rate,
        samples,
        index,
        location.size() ? reinterpret_cast<GLint*>(location.data()) : nullptr);
    detail::error_check("GetShadingRateSampleLocationivNV"sv);
}

/*!
 * \brief Part of GL_NV_shading_rate_image
 * \param synchronize GLboolean
 * \return void
 */
STATICINLINE void shading_rate_image_barrier(bool synchronize)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShadingRateImageBarrierNV)
    }
    glShadingRateImageBarrierNV(synchronize);
    detail::error_check("ShadingRateImageBarrierNV"sv);
}

template<class span_const_GLenum>
requires(
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLenum::value_type>,
        std::decay_t<GLenum>>)
/*!
 * \brief Part of GL_NV_shading_rate_image
 * \param viewport GLuint
 * \param first GLuint
 * \param count GLsizei
 * \param rates const GLenum *
 * \return void
 */
STATICINLINE void shading_rate_image_palette(
    u32 viewport, u32 first, span_const_GLenum const& rates)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShadingRateImagePaletteNV)
    }
    glShadingRateImagePaletteNV(
        viewport,
        first,
        rates.size(),
        rates.size() ? reinterpret_cast<const GLenum*>(rates.data()) : nullptr);
    detail::error_check("ShadingRateImagePaletteNV"sv);
}

/*!
 * \brief Part of GL_NV_shading_rate_image
 * \param order GLenum
 * \return void
 */
STATICINLINE void shading_rate_sample_order(GLenum order)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShadingRateSampleOrderNV)
    }
    glShadingRateSampleOrderNV(order);
    detail::error_check("ShadingRateSampleOrderNV"sv);
}

template<class span_const_i32>
requires(
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_shading_rate_image
 * \param rate GLenum
 * \param samples GLuint
 * \param locations const GLint *
 * \return void
 */
STATICINLINE void shading_rate_sample_order_custom(
    GLenum rate, u32 samples, span_const_i32 const& locations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShadingRateSampleOrderCustomNV)
    }
    glShadingRateSampleOrderCustomNV(
        rate,
        samples,
        locations.size() ? reinterpret_cast<const GLint*>(locations.data())
                         : nullptr);
    detail::error_check("ShadingRateSampleOrderCustomNV"sv);
}

} // namespace gl::nv::shading_rate_image
#endif // GL_NV_shading_rate_image
namespace gl::nv::shading_rate_image {
constexpr auto name = "GL_NV_shading_rate_image";
}
