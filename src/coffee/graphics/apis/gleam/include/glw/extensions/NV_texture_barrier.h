#pragma once

#ifdef GL_NV_texture_barrier
namespace gl::nv::texture_barrier {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_NV_texture_barrier

 * \return void
 */
STATICINLINE void texture_barrier(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBarrierNV)
    }
    glTextureBarrierNV();
    detail::error_check("TextureBarrierNV"sv, check_errors);
}

} // namespace gl::nv::texture_barrier
#endif // GL_NV_texture_barrier
namespace gl::nv::texture_barrier {
constexpr auto name = "GL_NV_texture_barrier";
}
