#pragma once

#ifdef GL_QCOM_shader_framebuffer_fetch_noncoherent
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"

namespace gl::qcom::shader_framebuffer_fetch_noncoherent {
using gl::group::enable_cap;
using gl::group::get_prop;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_QCOM_shader_framebuffer_fetch_noncoherent

 * \return void
 */
STATICINLINE void framebuffer_fetch_barrier(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferFetchBarrierQCOM)
    }
    glFramebufferFetchBarrierQCOM();
    detail::error_check("FramebufferFetchBarrierQCOM"sv, check_errors);
}

} // namespace gl::qcom::shader_framebuffer_fetch_noncoherent
#endif // GL_QCOM_shader_framebuffer_fetch_noncoherent
namespace gl::qcom::shader_framebuffer_fetch_noncoherent {
constexpr auto name = "GL_QCOM_shader_framebuffer_fetch_noncoherent";
}
