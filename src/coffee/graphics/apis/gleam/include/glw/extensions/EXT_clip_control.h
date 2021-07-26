#pragma once

#ifdef GL_EXT_clip_control
namespace gl::ext::clip_control {
namespace values {
constexpr libc_types::u32 lower_left          = 0x8CA1;
constexpr libc_types::u32 upper_left          = 0x8CA2;
constexpr libc_types::u32 clip_origin         = 0x935C;
constexpr libc_types::u32 clip_depth_mode     = 0x935D;
constexpr libc_types::u32 negative_one_to_one = 0x935E;
constexpr libc_types::u32 zero_to_one         = 0x935F;
} // namespace values
STATICINLINE void clip_control(GLenum origin, GLenum depth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClipControlEXT)
    }
    glClipControlEXT(origin, depth);
    detail::error_check("ClipControlEXT"sv);
}

} // namespace gl::ext::clip_control
#endif // GL_EXT_clip_control
namespace gl::ext::clip_control {
constexpr auto name = "GL_EXT_clip_control";
}
