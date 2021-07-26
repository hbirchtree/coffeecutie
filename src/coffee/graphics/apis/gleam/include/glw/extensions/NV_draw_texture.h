#pragma once

#ifdef GL_NV_draw_texture
namespace gl::nv::draw_texture {
namespace values {
} // namespace values
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
        glIsTexture(texture);
        glIsSampler(sampler);
    }
    glDrawTextureNV(texture, sampler, x0, y0, x1, y1, z, s0, t0, s1, t1);
    detail::error_check("DrawTextureNV"sv);
}

} // namespace gl::nv::draw_texture
#endif // GL_NV_draw_texture
namespace gl::nv::draw_texture {
constexpr auto name = "GL_NV_draw_texture";
}
