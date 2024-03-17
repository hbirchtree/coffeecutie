#pragma once

#ifdef GL_NV_copy_image
namespace gl::nv::copy_image {
namespace values {
} // namespace values

template<class size_3_i32>
requires(concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Part of GL_NV_copy_image
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
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
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
    size_3_i32 const&                  width,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyImageSubDataNV)
    }
    glCopyImageSubDataNV(
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
        width[0],
        width[1],
        width[2]);
    detail::error_check("CopyImageSubDataNV"sv, check_errors);
}

} // namespace gl::nv::copy_image
#endif // GL_NV_copy_image
namespace gl::nv::copy_image {
constexpr auto name = "GL_NV_copy_image";
}
