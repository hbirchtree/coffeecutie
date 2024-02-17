#pragma once

#ifdef GL_EXT_stencil_two_side
namespace gl::ext::stencil_two_side {
namespace values {
constexpr u32 stencil_test_two_side = 0x8910;
constexpr u32 active_stencil_face   = 0x8911;
} // namespace values

/*!
 * \brief Part of GL_EXT_stencil_two_side
 * \param face GLenum
 * \return void
 */
STATICINLINE void active_stencil_face(group::triangle_face face)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ActiveStencilFaceEXT)
    }
    glActiveStencilFaceEXT(static_cast<GLenum>(face));
    detail::error_check("ActiveStencilFaceEXT"sv);
}

} // namespace gl::ext::stencil_two_side
#endif // GL_EXT_stencil_two_side
namespace gl::ext::stencil_two_side {
constexpr auto name = "GL_EXT_stencil_two_side";
}
