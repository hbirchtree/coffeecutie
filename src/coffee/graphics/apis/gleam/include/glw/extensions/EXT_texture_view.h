#pragma once

#ifdef GL_EXT_texture_view
namespace gl::ext::texture_view {
namespace values {
constexpr libc_types::u32 texture_view_min_level   = 0x82DB;
constexpr libc_types::u32 texture_view_num_levels  = 0x82DC;
constexpr libc_types::u32 texture_view_min_layer   = 0x82DD;
constexpr libc_types::u32 texture_view_num_layers  = 0x82DE;
constexpr libc_types::u32 texture_immutable_levels = 0x82DF;
} // namespace values
STATICINLINE void texture_view(
    u32                          texture,
    group::texture_target        target,
    u32                          origtexture,
    group::sized_internal_format internalformat,
    u32                          minlevel,
    u32                          numlevels,
    u32                          minlayer,
    u32                          numlayers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureViewEXT)
        glIsTexture(texture);
    }
    glTextureViewEXT(
        texture,
        static_cast<GLenum>(target),
        origtexture,
        static_cast<GLenum>(internalformat),
        minlevel,
        numlevels,
        minlayer,
        numlayers);
    detail::error_check("TextureViewEXT"sv);
}

} // namespace gl::ext::texture_view
#endif // GL_EXT_texture_view
namespace gl::ext::texture_view {
constexpr auto name = "GL_EXT_texture_view";
}