#pragma once

#ifdef GL_QCOM_shader_framebuffer_fetch_noncoherent
#include "../enums/FramebufferFetchNoncoherent.h"
namespace gl::qcom::shader_framebuffer_fetch_noncoherent {
using gl::group::framebuffer_fetch_noncoherent;
namespace values {
} // namespace values
STATICINLINE void framebuffer_fetch_barrier()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferFetchBarrierQCOM)
    }
    glFramebufferFetchBarrierQCOM();
    detail::error_check("FramebufferFetchBarrierQCOM"sv);
}

} // namespace gl::qcom::shader_framebuffer_fetch_noncoherent
#endif // GL_QCOM_shader_framebuffer_fetch_noncoherent
namespace gl::qcom::shader_framebuffer_fetch_noncoherent {
constexpr auto name = "GL_QCOM_shader_framebuffer_fetch_noncoherent";
}
