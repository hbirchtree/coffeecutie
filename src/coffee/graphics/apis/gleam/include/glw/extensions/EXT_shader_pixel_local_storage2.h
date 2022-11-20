#pragma once

#ifdef GL_EXT_shader_pixel_local_storage2
namespace gl::ext::shader_pixel_local_storage2 {
namespace values {
constexpr libc_types::u32 max_shader_combined_local_storage_fast_size = 0x9650;
constexpr libc_types::u32 max_shader_combined_local_storage_size      = 0x9651;
constexpr libc_types::u32
    framebuffer_incomplete_insufficient_shader_combined_local_storage = 0x9652;
} // namespace values
template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_EXT_shader_pixel_local_storage2
     * \param offset GLsizei
     * \param n GLsizei
     * \param values const GLuint *
     * \return void
     */
    STATICINLINE
    void clear_pixel_local_storageui(i32 offset, span_const_u32 const& values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearPixelLocalStorageuiEXT)
    }
    glClearPixelLocalStorageuiEXT(
        offset,
        values.size(),
        values.size() ? reinterpret_cast<const GLuint*>(values.data())
                      : nullptr);
    detail::error_check("ClearPixelLocalStorageuiEXT"sv);
}

/*!
 * \brief Part of GL_EXT_shader_pixel_local_storage2
 * \param target GLuint
 * \param size GLsizei
 * \return void
 */
STATICINLINE void framebuffer_pixel_local_storage_size(u32 target, i32 size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferPixelLocalStorageSizeEXT)
    }
    glFramebufferPixelLocalStorageSizeEXT(target, size);
    detail::error_check("FramebufferPixelLocalStorageSizeEXT"sv);
}

/*!
 * \brief Part of GL_EXT_shader_pixel_local_storage2
 * \param target GLuint
 * \return GLsizei
 */
STATICINLINE GLsizei get_framebuffer_pixel_local_storage_size(u32 target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferPixelLocalStorageSizeEXT)
    }
    auto out = glGetFramebufferPixelLocalStorageSizeEXT(target);
    detail::error_check("GetFramebufferPixelLocalStorageSizeEXT"sv);
    return out;
}

} // namespace gl::ext::shader_pixel_local_storage2
#endif // GL_EXT_shader_pixel_local_storage2
namespace gl::ext::shader_pixel_local_storage2 {
constexpr auto name = "GL_EXT_shader_pixel_local_storage2";
}
