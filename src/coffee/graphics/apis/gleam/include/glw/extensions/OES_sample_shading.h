#pragma once

#ifdef GL_OES_sample_shading
namespace gl::oes::sample_shading {
namespace values {
constexpr libc_types::u32 sample_shading           = 0x8C36;
constexpr libc_types::u32 min_sample_shading_value = 0x8C37;
} // namespace values
/*!
 * \brief Part of GL_OES_sample_shading
 * \param value GLfloat
 * \return void
 */
STATICINLINE void min_sample_shading(f32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MinSampleShadingOES)
    }
    glMinSampleShadingOES(value);
    detail::error_check("MinSampleShadingOES"sv);
}

} // namespace gl::oes::sample_shading
#endif // GL_OES_sample_shading
namespace gl::oes::sample_shading {
constexpr auto name = "GL_OES_sample_shading";
}
