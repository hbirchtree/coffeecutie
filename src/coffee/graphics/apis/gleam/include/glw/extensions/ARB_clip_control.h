#pragma once

#ifdef GL_ARB_clip_control
#include "../enums/ClipControlDepth.h"
#include "../enums/ClipControlOrigin.h"
namespace gl::arb::clip_control {
using gl::group::clip_control_depth;
using gl::group::clip_control_origin;
namespace values {
constexpr u32 clip_origin     = 0x935C;
constexpr u32 clip_depth_mode = 0x935D;
} // namespace values
/*!
 * \brief Part of GL_ARB_clip_control
 * \param origin GLenum
 * \param depth GLenum
 * \return void
 */
STATICINLINE void clip_control(
    group::clip_control_origin origin, group::clip_control_depth depth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClipControl)
    }
    glClipControl(static_cast<GLenum>(origin), static_cast<GLenum>(depth));
    detail::error_check("ClipControl"sv);
}

} // namespace gl::arb::clip_control
#endif // GL_ARB_clip_control
namespace gl::arb::clip_control {
constexpr auto name = "GL_ARB_clip_control";
}
