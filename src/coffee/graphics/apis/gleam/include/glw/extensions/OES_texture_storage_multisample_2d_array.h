#pragma once

#ifdef GL_OES_texture_storage_multisample_2d_array
namespace gl::oes::texture_storage_multisample_2d_array {
namespace values {
constexpr libc_types::u32 texture_2d_multisample_array              = 0x9102;
constexpr libc_types::u32 texture_binding_2d_multisample_array      = 0x9105;
constexpr libc_types::u32 sampler_2d_multisample_array              = 0x910B;
constexpr libc_types::u32 int_sampler_2d_multisample_array          = 0x910C;
constexpr libc_types::u32 unsigned_int_sampler_2d_multisample_array = 0x910D;
} // namespace values
template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>)
/*!
 * \brief Part of GL_OES_texture_storage_multisample_2d_array
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
        GLW_FPTR_CHECK(TexStorage3DMultisampleOES)
    }
    glTexStorage3DMultisampleOES(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        fixedsamplelocations);
    detail::error_check("TexStorage3DMultisampleOES"sv);
}

} // namespace gl::oes::texture_storage_multisample_2d_array
#endif // GL_OES_texture_storage_multisample_2d_array
namespace gl::oes::texture_storage_multisample_2d_array {
constexpr auto name = "GL_OES_texture_storage_multisample_2d_array";
}
