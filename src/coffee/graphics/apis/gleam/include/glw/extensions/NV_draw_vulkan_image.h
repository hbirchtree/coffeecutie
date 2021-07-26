#pragma once

#ifdef GL_NV_draw_vulkan_image
namespace gl::nv::draw_vulkan_image {
namespace values {
} // namespace values
STATICINLINE void draw_vk_image(
    u64 vkImage,
    u32 sampler,
    f32 x0,
    f32 y0,
    f32 x1,
    f32 y1,
    f32 z,
    f32 s0,
    f32 t0,
    f32 s1,
    f32 t1)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawVkImageNV)
        glIsSampler(sampler);
    }
    glDrawVkImageNV(vkImage, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1);
    detail::error_check("DrawVkImageNV"sv);
}

STATICINLINE GLVULKANPROCNV get_vk_proc_addr(std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVkProcAddrNV)
    }
    auto out = glGetVkProcAddrNV(name.data());
    detail::error_check("GetVkProcAddrNV"sv);
    return out;
}

STATICINLINE void wait_vk_semaphore(u64 vkSemaphore)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WaitVkSemaphoreNV)
    }
    glWaitVkSemaphoreNV(vkSemaphore);
    detail::error_check("WaitVkSemaphoreNV"sv);
}

STATICINLINE void signal_vk_semaphore(u64 vkSemaphore)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SignalVkSemaphoreNV)
    }
    glSignalVkSemaphoreNV(vkSemaphore);
    detail::error_check("SignalVkSemaphoreNV"sv);
}

STATICINLINE void signal_vk_fence(u64 vkFence)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SignalVkFenceNV)
    }
    glSignalVkFenceNV(vkFence);
    detail::error_check("SignalVkFenceNV"sv);
}

} // namespace gl::nv::draw_vulkan_image
#endif // GL_NV_draw_vulkan_image
namespace gl::nv::draw_vulkan_image {
constexpr auto name = "GL_NV_draw_vulkan_image";
}
