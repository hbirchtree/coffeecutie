#pragma once

#ifdef GL_NV_draw_texture
namespace gl::nv::draw_texture {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_NV_draw_texture
 * \param texture GLuint
 * \param sampler GLuint
 * \param x0 GLfloat
 * \param y0 GLfloat
 * \param x1 GLfloat
 * \param y1 GLfloat
 * \param z GLfloat
 * \param s0 GLfloat
 * \param t0 GLfloat
 * \param s1 GLfloat
 * \param t1 GLfloat
 * \return void
 */
STATICINLINE void draw_texture(
    u32 texture,
    u32 sampler,
    f32 x0,
    f32 y0,
    f32 x1,
    f32 y1,
    f32 z,
    f32 s0,
    f32 t0,
    f32 s1,
    f32 t1)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTextureNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glDrawTextureNV(texture, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1);
    detail::error_check("DrawTextureNV"sv);
}

} // namespace gl::nv::draw_texture
#endif // GL_NV_draw_texture
namespace gl::nv::draw_texture {
constexpr auto name = "GL_NV_draw_texture";
}
