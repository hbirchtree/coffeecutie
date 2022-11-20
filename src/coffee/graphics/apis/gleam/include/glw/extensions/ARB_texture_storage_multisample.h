#pragma once

#ifdef GL_ARB_texture_storage_multisample
namespace gl::arb::texture_storage_multisample {
namespace values {
} // namespace values
template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Part of GL_ARB_texture_storage_multisample
     * \param target GLenum
     * \param samples GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \param fixedsamplelocations GLboolean
     * \return void
     */
    STATICINLINE void tex_storage_2d_multisample(
        group::texture_target        target,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage2DMultisample)
    }
    glTexStorage2DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        fixedsamplelocations);
    detail::error_check("TexStorage2DMultisample"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>)
    /*!
     * \brief Part of GL_ARB_texture_storage_multisample
     * \param target GLenum
     * \param samples GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param fixedsamplelocations GLboolean
     * \return void
     */
    STATICINLINE void tex_storage_3d_multisample(
        group::texture_target        target,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage3DMultisample)
    }
    glTexStorage3DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        fixedsamplelocations);
    detail::error_check("TexStorage3DMultisample"sv);
}

} // namespace gl::arb::texture_storage_multisample
#endif // GL_ARB_texture_storage_multisample
namespace gl::arb::texture_storage_multisample {
constexpr auto name = "GL_ARB_texture_storage_multisample";
}
