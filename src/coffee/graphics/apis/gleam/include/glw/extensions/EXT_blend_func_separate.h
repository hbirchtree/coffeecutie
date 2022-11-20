#pragma once

#ifdef GL_EXT_blend_func_separate
namespace gl::ext::blend_func_separate {
namespace values {
constexpr libc_types::u32 blend_dst_rgb   = 0x80C8;
constexpr libc_types::u32 blend_src_rgb   = 0x80C9;
constexpr libc_types::u32 blend_dst_alpha = 0x80CA;
constexpr libc_types::u32 blend_src_alpha = 0x80CB;
} // namespace values
/*!
 * \brief Part of GL_EXT_blend_func_separate
 * \param sfactorRGB GLenum
 * \param dfactorRGB GLenum
 * \param sfactorAlpha GLenum
 * \param dfactorAlpha GLenum
 * \return void
 */
STATICINLINE void blend_func_separate(
    group::blending_factor sfactorRGB,
    group::blending_factor dfactorRGB,
    group::blending_factor sfactorAlpha,
    group::blending_factor dfactorAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncSeparateEXT)
    }
    glBlendFuncSeparateEXT(
        static_cast<GLenum>(sfactorRGB),
        static_cast<GLenum>(dfactorRGB),
        static_cast<GLenum>(sfactorAlpha),
        static_cast<GLenum>(dfactorAlpha));
    detail::error_check("BlendFuncSeparateEXT"sv);
}

} // namespace gl::ext::blend_func_separate
#endif // GL_EXT_blend_func_separate
namespace gl::ext::blend_func_separate {
constexpr auto name = "GL_EXT_blend_func_separate";
}
