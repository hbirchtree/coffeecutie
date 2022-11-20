#pragma once

#ifdef GL_ARB_color_buffer_float
#include "../enums/ClampColorModeARB.h"
#include "../enums/ClampColorTargetARB.h"
namespace gl::arb::color_buffer_float {
using gl::group::clamp_color_mode_arb;
using gl::group::clamp_color_target_arb;
namespace values {
constexpr libc_types::u32 rgba_float_mode = 0x8820;
} // namespace values
/*!
 * \brief Part of GL_ARB_color_buffer_float
 * \param target GLenum
 * \param clamp GLenum
 * \return void
 */
STATICINLINE void clamp_color(
    group::clamp_color_target_arb target, group::clamp_color_mode_arb clamp)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClampColorARB)
    }
    glClampColorARB(static_cast<GLenum>(target), static_cast<GLenum>(clamp));
    detail::error_check("ClampColorARB"sv);
}

} // namespace gl::arb::color_buffer_float
#endif // GL_ARB_color_buffer_float
namespace gl::arb::color_buffer_float {
constexpr auto name = "GL_ARB_color_buffer_float";
}
