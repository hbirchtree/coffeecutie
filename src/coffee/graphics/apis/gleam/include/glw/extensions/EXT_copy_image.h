#pragma once

#ifdef GL_EXT_copy_image
namespace gl::ext::copy_image {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_EXT_copy_image
 * \param srcName GLuint
 * \param srcTarget GLenum
 * \param srcLevel GLint
 * \param srcX GLint
 * \param srcY GLint
 * \param srcZ GLint
 * \param dstName GLuint
 * \param dstTarget GLenum
 * \param dstLevel GLint
 * \param dstX GLint
 * \param dstY GLint
 * \param dstZ GLint
 * \param srcWidth GLsizei
 * \param srcHeight GLsizei
 * \param srcDepth GLsizei
 * \return void
 */
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
    i32                                srcDepth,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyImageSubDataEXT)
    }
    glCopyImageSubDataEXT(
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
    detail::error_check("CopyImageSubDataEXT"sv, check_errors);
}

} // namespace gl::ext::copy_image
#endif // GL_EXT_copy_image
namespace gl::ext::copy_image {
constexpr auto name = "GL_EXT_copy_image";
}
