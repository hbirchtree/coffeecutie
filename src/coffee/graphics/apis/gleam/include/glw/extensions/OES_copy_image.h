#pragma once

#ifdef GL_OES_copy_image
namespace gl::oes::copy_image {
namespace values {
} // namespace values
STATICINLINE void copy_image_sub_data(
    u32                                srcName,
    group::copy_buffer_sub_data_target srcTarget,
    i32                                srcLevel,
    i32                                srcX,
    i32                                srcY,
    i32                                srcZ,
    u32                                dstName,
    group::copy_buffer_sub_data_target dstTarget,
    i32                                dstLevel,
    i32                                dstX,
    i32                                dstY,
    i32                                dstZ,
    i32                                srcWidth,
    i32                                srcHeight,
    i32                                srcDepth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyImageSubDataOES)
    }
    glCopyImageSubDataOES(
        srcName,
        static_cast<GLenum>(srcTarget),
        srcLevel,
        srcX,
        srcY,
        srcZ,
        dstName,
        static_cast<GLenum>(dstTarget),
        dstLevel,
        dstX,
        dstY,
        dstZ,
        srcWidth,
        srcHeight,
        srcDepth);
    detail::error_check("CopyImageSubDataOES"sv);
}

} // namespace gl::oes::copy_image
#endif // GL_OES_copy_image
namespace gl::oes::copy_image {
constexpr auto name = "GL_OES_copy_image";
}