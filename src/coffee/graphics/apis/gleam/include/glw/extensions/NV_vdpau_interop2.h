#pragma once

#ifdef GL_NV_vdpau_interop2
namespace gl::nv::vdpau_interop2 {
namespace values {
} // namespace values

template<class span_const_u32, class span_const_void>
requires(
    concepts::span<span_const_void> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_vdpau_interop2
 * \param vdpSurface const void *
 * \param target GLenum
 * \param numTextureNames GLsizei
 * \param textureNames const GLuint *
 * \param isFrameStructure GLboolean
 * \return GLvdpauSurfaceNV
 */
STATICINLINE GLvdpauSurfaceNV
vdpau_register_video_surface_with_picture_structure(
    span_const_void const& vdpSurface,
    GLenum                 target,
    span_const_u32 const&  textureNames,
    bool                   isFrameStructure,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VDPAURegisterVideoSurfaceWithPictureStructureNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glVDPAURegisterVideoSurfaceWithPictureStructureNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(vdpSurface.data()),
            vdpSurface.size_bytes()),
        target,
        gsl::span<const char>(
            reinterpret_cast<const char*>(textureNames.data()),
            textureNames.size_bytes()),
        isFrameStructure);
#endif
    auto out = glVDPAURegisterVideoSurfaceWithPictureStructureNV(
        vdpSurface.size() ? reinterpret_cast<const void*>(vdpSurface.data())
                          : nullptr,
        target,
        textureNames.size(),
        textureNames.size()
            ? reinterpret_cast<const GLuint*>(textureNames.data())
            : nullptr,
        isFrameStructure);
    detail::error_check(
        "VDPAURegisterVideoSurfaceWithPictureStructureNV"sv, check_errors);
    return out;
}

} // namespace gl::nv::vdpau_interop2
#endif // GL_NV_vdpau_interop2
namespace gl::nv::vdpau_interop2 {
constexpr auto name = "GL_NV_vdpau_interop2";
}
