#pragma once

#ifdef GL_NV_viewport_swizzle
namespace gl::nv::viewport_swizzle {
namespace values {
constexpr libc_types::u32 viewport_swizzle_positive_x = 0x9350;
constexpr libc_types::u32 viewport_swizzle_negative_x = 0x9351;
constexpr libc_types::u32 viewport_swizzle_positive_y = 0x9352;
constexpr libc_types::u32 viewport_swizzle_negative_y = 0x9353;
constexpr libc_types::u32 viewport_swizzle_positive_z = 0x9354;
constexpr libc_types::u32 viewport_swizzle_negative_z = 0x9355;
constexpr libc_types::u32 viewport_swizzle_positive_w = 0x9356;
constexpr libc_types::u32 viewport_swizzle_negative_w = 0x9357;
constexpr libc_types::u32 viewport_swizzle_x          = 0x9358;
constexpr libc_types::u32 viewport_swizzle_y          = 0x9359;
constexpr libc_types::u32 viewport_swizzle_z          = 0x935A;
constexpr libc_types::u32 viewport_swizzle_w          = 0x935B;
} // namespace values
/*!
 * \brief Part of GL_NV_viewport_swizzle
 * \param index GLuint
 * \param swizzlex GLenum
 * \param swizzley GLenum
 * \param swizzlez GLenum
 * \param swizzlew GLenum
 * \return void
 */
STATICINLINE void viewport_swizzle(
    u32    index,
    GLenum swizzlex,
    GLenum swizzley,
    GLenum swizzlez,
    GLenum swizzlew)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportSwizzleNV)
    }
    glViewportSwizzleNV(index, swizzlex, swizzley, swizzlez, swizzlew);
    detail::error_check("ViewportSwizzleNV"sv);
}

} // namespace gl::nv::viewport_swizzle
#endif // GL_NV_viewport_swizzle
namespace gl::nv::viewport_swizzle {
constexpr auto name = "GL_NV_viewport_swizzle";
}
