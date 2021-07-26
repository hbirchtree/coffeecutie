#pragma once

#ifdef GL_ARB_texture_barrier
namespace gl::arb::texture_barrier {
namespace values {
} // namespace values
STATICINLINE void texture_barrier()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBarrier)
    }
    glTextureBarrier();
    detail::error_check("TextureBarrier"sv);
}

} // namespace gl::arb::texture_barrier
#endif // GL_ARB_texture_barrier
namespace gl::arb::texture_barrier {
constexpr auto name = "GL_ARB_texture_barrier";
}
