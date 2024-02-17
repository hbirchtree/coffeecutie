#pragma once

#ifdef GL_EXT_shader_framebuffer_fetch_non_coherent
namespace gl::ext::shader_framebuffer_fetch_non_coherent {
namespace values {
constexpr u32 fragment_shader_discards_samples = 0x8A52;
} // namespace values

/*!
 * \brief Part of GL_EXT_shader_framebuffer_fetch_non_coherent

 * \return void
 */
STATICINLINE void framebuffer_fetch_barrier()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferFetchBarrierEXT)
    }
    glFramebufferFetchBarrierEXT();
    detail::error_check("FramebufferFetchBarrierEXT"sv);
}

} // namespace gl::ext::shader_framebuffer_fetch_non_coherent
#endif // GL_EXT_shader_framebuffer_fetch_non_coherent
namespace gl::ext::shader_framebuffer_fetch_non_coherent {
constexpr auto name = "GL_EXT_shader_framebuffer_fetch_non_coherent";
}
