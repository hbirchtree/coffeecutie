#pragma once

#ifdef GL_ARB_copy_image
namespace gl::arb::copy_image {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_copy_image
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
    u32                               srcName,
    group::copy_image_sub_data_target srcTarget,
    i32                               srcLevel,
    i32                               srcX,
    i32                               srcY,
    i32                               srcZ,
    u32                               dstName,
    group::copy_image_sub_data_target dstTarget,
    i32                               dstLevel,
    i32                               dstX,
    i32                               dstY,
    i32                               dstZ,
    i32                               srcWidth,
    i32                               srcHeight,
    i32                               srcDepth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyImageSubData)
    }
    glCopyImageSubData(
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
    detail::error_check("CopyImageSubData"sv);
}

} // namespace gl::arb::copy_image
#endif // GL_ARB_copy_image
namespace gl::arb::copy_image {
constexpr auto name = "GL_ARB_copy_image";
}
