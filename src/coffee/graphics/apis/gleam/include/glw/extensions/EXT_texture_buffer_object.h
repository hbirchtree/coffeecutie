#pragma once

#ifdef GL_EXT_texture_buffer_object
namespace gl::ext::texture_buffer_object {
namespace values {
constexpr libc_types::u32 texture_buffer                    = 0x8C2A;
constexpr libc_types::u32 max_texture_buffer_size           = 0x8C2B;
constexpr libc_types::u32 texture_binding_buffer            = 0x8C2C;
constexpr libc_types::u32 texture_buffer_data_store_binding = 0x8C2D;
constexpr libc_types::u32 texture_buffer_format             = 0x8C2E;
} // namespace values
STATICINLINE void tex_buffer(
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBufferEXT)
        glIsBuffer(buffer);
    }
    glTexBufferEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TexBufferEXT"sv);
}

} // namespace gl::ext::texture_buffer_object
#endif // GL_EXT_texture_buffer_object
namespace gl::ext::texture_buffer_object {
constexpr auto name = "GL_EXT_texture_buffer_object";
}
