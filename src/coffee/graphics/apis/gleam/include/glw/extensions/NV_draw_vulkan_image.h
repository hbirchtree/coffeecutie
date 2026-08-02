#pragma once

#ifdef GL_NV_draw_vulkan_image
namespace gl::nv::draw_vulkan_image {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_NV_draw_vulkan_image
 * \param vkImage GLuint64
 * \param sampler GLuint
 * \param x0 GLfloat
 * \param y0 GLfloat
 * \param x1 GLfloat
 * \param y1 GLfloat
 * \param z GLfloat
 * \param s0 GLfloat
 * \param t0 GLfloat
 * \param s1 GLfloat
 * \param t1 GLfloat
 * \return void
 */
STATICINLINE void draw_vk_image(
    u64         vkImage,
    u32         sampler,
    f32         x0,
    f32         y0,
    f32         x1,
    f32         y1,
    f32         z,
    f32         s0,
    f32         t0,
    f32         s1,
    f32         t1,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DrawVkImageNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawVkImageNV, vkImage, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1);
#endif
    glDrawVkImageNV(vkImage, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1);
    detail::error_check("DrawVkImageNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_draw_vulkan_image
 * \param name const GLchar *
 * \return GLVULKANPROCNV
 */
STATICINLINE GLVULKANPROCNV get_vk_proc_addr(
    std::string_view const& name, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetVkProcAddrNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetVkProcAddrNV, name);
#endif
    auto out = glGetVkProcAddrNV(name.data());
    detail::error_check("GetVkProcAddrNV"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_NV_draw_vulkan_image
 * \param vkSemaphore GLuint64
 * \return void
 */
STATICINLINE void wait_vk_semaphore(
    u64 vkSemaphore, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(WaitVkSemaphoreNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glWaitVkSemaphoreNV, vkSemaphore);
#endif
    glWaitVkSemaphoreNV(vkSemaphore);
    detail::error_check("WaitVkSemaphoreNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_draw_vulkan_image
 * \param vkSemaphore GLuint64
 * \return void
 */
STATICINLINE void signal_vk_semaphore(
    u64 vkSemaphore, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(SignalVkSemaphoreNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glSignalVkSemaphoreNV, vkSemaphore);
#endif
    glSignalVkSemaphoreNV(vkSemaphore);
    detail::error_check("SignalVkSemaphoreNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_draw_vulkan_image
 * \param vkFence GLuint64
 * \return void
 */
STATICINLINE void signal_vk_fence(
    u64 vkFence, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(SignalVkFenceNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glSignalVkFenceNV, vkFence);
#endif
    glSignalVkFenceNV(vkFence);
    detail::error_check("SignalVkFenceNV"sv, check_errors);
}

} // namespace gl::nv::draw_vulkan_image
#endif // GL_NV_draw_vulkan_image
namespace gl::nv::draw_vulkan_image {
constexpr auto name = "GL_NV_draw_vulkan_image";
}
