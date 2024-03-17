#pragma once

#ifdef GL_ARB_texture_view
namespace gl::arb::texture_view {
namespace values {
constexpr u32 texture_view_min_level   = 0x82DB;
constexpr u32 texture_view_num_levels  = 0x82DC;
constexpr u32 texture_view_min_layer   = 0x82DD;
constexpr u32 texture_view_num_layers  = 0x82DE;
constexpr u32 texture_immutable_levels = 0x82DF;
} // namespace values

/*!
 * \brief Part of GL_ARB_texture_view
 * \param texture GLuint
 * \param target GLenum
 * \param origtexture GLuint
 * \param internalformat GLenum
 * \param minlevel GLuint
 * \param numlevels GLuint
 * \param minlayer GLuint
 * \param numlayers GLuint
 * \return void
 */
STATICINLINE void texture_view(
    u32                          texture,
    group::texture_target        target,
    u32                          origtexture,
    group::sized_internal_format internalformat,
    u32                          minlevel,
    u32                          numlevels,
    u32                          minlayer,
    u32                          numlayers,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureView)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureView(
        texture,
        static_cast<GLenum>(target),
        origtexture,
        static_cast<GLenum>(internalformat),
        minlevel,
        numlevels,
        minlayer,
        numlayers);
    detail::error_check("TextureView"sv, check_errors);
}

} // namespace gl::arb::texture_view
#endif // GL_ARB_texture_view
namespace gl::arb::texture_view {
constexpr auto name = "GL_ARB_texture_view";
}
