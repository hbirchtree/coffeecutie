#pragma once

#ifdef GL_EXT_stencil_clear_tag
namespace gl::ext::stencil_clear_tag {
namespace values {
constexpr u32 stencil_tag_bits        = 0x88F2;
constexpr u32 stencil_clear_tag_value = 0x88F3;
} // namespace values

/*!
 * \brief Part of GL_EXT_stencil_clear_tag
 * \param stencilTagBits GLsizei
 * \param stencilClearTag GLuint
 * \return void
 */
STATICINLINE void stencil_clear_tag(i32 stencilTagBits, u32 stencilClearTag)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilClearTagEXT)
    }
    glStencilClearTagEXT(stencilTagBits, stencilClearTag);
    detail::error_check("StencilClearTagEXT"sv);
}

} // namespace gl::ext::stencil_clear_tag
#endif // GL_EXT_stencil_clear_tag
namespace gl::ext::stencil_clear_tag {
constexpr auto name = "GL_EXT_stencil_clear_tag";
}
