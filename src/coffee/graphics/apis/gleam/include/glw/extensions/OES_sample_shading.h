#pragma once

#ifdef GL_OES_sample_shading
namespace gl::oes::sample_shading {
namespace values {
constexpr u32 sample_shading           = 0x8C36;
constexpr u32 min_sample_shading_value = 0x8C37;
} // namespace values

/*!
 * \brief Part of GL_OES_sample_shading
 * \param value GLfloat
 * \return void
 */
STATICINLINE void min_sample_shading(
    f32 value, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MinSampleShadingOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMinSampleShadingOES, value);
#endif
    glMinSampleShadingOES(value);
    detail::error_check("MinSampleShadingOES"sv, check_errors);
}

} // namespace gl::oes::sample_shading
#endif // GL_OES_sample_shading
namespace gl::oes::sample_shading {
constexpr auto name = "GL_OES_sample_shading";
}
