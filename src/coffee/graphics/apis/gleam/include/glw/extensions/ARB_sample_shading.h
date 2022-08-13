#pragma once

#ifdef GL_ARB_sample_shading
namespace gl::arb::sample_shading {
namespace values {
constexpr libc_types::u32 sample_shading           = 0x8C36;
constexpr libc_types::u32 min_sample_shading_value = 0x8C37;
} // namespace values
STATICINLINE void min_sample_shading(f32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MinSampleShadingARB)
    }
    glMinSampleShadingARB(value);
    detail::error_check("MinSampleShadingARB"sv);
}

} // namespace gl::arb::sample_shading
#endif // GL_ARB_sample_shading
namespace gl::arb::sample_shading {
constexpr auto name = "GL_ARB_sample_shading";
}