#pragma once

#ifdef GL_EXT_copy_texture
namespace gl::ext::copy_texture {
namespace values {
} // namespace values
template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_tex_image_1d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        vec_2_i32 const&       x,
        i32                    width,
        i32                    border)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexImage1DEXT)
    }
    glCopyTexImage1DEXT(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width,
        border);
    detail::error_check("CopyTexImage1DEXT"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_tex_image_2d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        vec_2_i32 const&       x,
        size_2_i32 const&      width,
        i32                    border)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexImage2DEXT)
    }
    glCopyTexImage2DEXT(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x.x(),
        x.y(),
        width[0],
        width[1],
        border);
    detail::error_check("CopyTexImage2DEXT"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_tex_sub_image_1d(
        group::texture_target target,
        i32                   level,
        i32                   xoffset,
        vec_2_i32 const&      x,
        i32                   width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage1DEXT)
    }
    glCopyTexSubImage1DEXT(
        static_cast<GLenum>(target), level, xoffset, x.x(), x.y(), width);
    detail::error_check("CopyTexSubImage1DEXT"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_tex_sub_image_2d(
        group::texture_target target,
        i32                   level,
        vec_2_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage2DEXT)
    }
    glCopyTexSubImage2DEXT(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage2DEXT"sv);
}

template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_tex_sub_image_3d(
        group::texture_target target,
        i32                   level,
        vec_3_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage3DEXT)
    }
    glCopyTexSubImage3DEXT(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage3DEXT"sv);
}

} // namespace gl::ext::copy_texture
#endif // GL_EXT_copy_texture
namespace gl::ext::copy_texture {
constexpr auto name = "GL_EXT_copy_texture";
}