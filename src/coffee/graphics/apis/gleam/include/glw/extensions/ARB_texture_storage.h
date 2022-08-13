#pragma once

#ifdef GL_ARB_texture_storage
namespace gl::arb::texture_storage {
namespace values {
constexpr libc_types::u32 texture_immutable_format = 0x912F;
} // namespace values
STATICINLINE void tex_storage_1d(
    group::texture_target        target,
    i32                          levels,
    group::sized_internal_format internalformat,
    i32                          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage1D)
    }
    glTexStorage1D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width);
    detail::error_check("TexStorage1D"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void tex_storage_2d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage2D)
    }
    glTexStorage2D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TexStorage2D"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void tex_storage_3d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage3D)
    }
    glTexStorage3D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TexStorage3D"sv);
}

} // namespace gl::arb::texture_storage
#endif // GL_ARB_texture_storage
namespace gl::arb::texture_storage {
constexpr auto name = "GL_ARB_texture_storage";
}