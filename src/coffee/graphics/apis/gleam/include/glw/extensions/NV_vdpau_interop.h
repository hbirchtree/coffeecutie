#pragma once

#ifdef GL_NV_vdpau_interop
namespace gl::nv::vdpau_interop {
namespace values {
constexpr u32 surface_state      = 0x86EB;
constexpr u32 surface_registered = 0x86FD;
constexpr u32 surface_mapped     = 0x8700;
constexpr u32 write_discard      = 0x88BE;
} // namespace values

/*!
 * \brief Part of GL_NV_vdpau_interop

 * \return void
 */
STATICINLINE void vdpau_fini(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAUFiniNV)
    }
    glVDPAUFiniNV();
    detail::error_check("VDPAUFiniNV"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param surface GLvdpauSurfaceNV
 * \param pname GLenum
 * \param count GLsizei
 * \param length GLsizei *
 * \param values GLint *
 * \return void
 */
STATICINLINE void vdpau_get_surfaceiv(
    GLvdpauSurfaceNV surface,
    GLenum           pname,
    span_i32         length,
    span_i32         values,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAUGetSurfaceivNV)
    }
    glVDPAUGetSurfaceivNV(
        surface,
        pname,
        values.size(),
        length.size() ? reinterpret_cast<GLsizei*>(length.data()) : nullptr,
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("VDPAUGetSurfaceivNV"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param vdpDevice const void *
 * \param getProcAddress const void *
 * \return void
 */
STATICINLINE void vdpau_init(
    span_const_void const& vdpDevice,
    span_const_void const& getProcAddress,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAUInitNV)
    }
    glVDPAUInitNV(
        vdpDevice.size() ? reinterpret_cast<const void*>(vdpDevice.data())
                         : nullptr,
        getProcAddress.size()
            ? reinterpret_cast<const void*>(getProcAddress.data())
            : nullptr);
    detail::error_check("VDPAUInitNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param surface GLvdpauSurfaceNV
 * \return Boolean
 */
STATICINLINE bool vdpau_is_surface(
    GLvdpauSurfaceNV surface, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAUIsSurfaceNV)
    }
    auto out = glVDPAUIsSurfaceNV(surface);
    detail::error_check("VDPAUIsSurfaceNV"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<class span_const_GLvdpauSurfaceNV>
requires(
    concepts::span<span_const_GLvdpauSurfaceNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLvdpauSurfaceNV::value_type>,
        std::decay_t<GLvdpauSurfaceNV>>)
/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param numSurfaces GLsizei
 * \param surfaces const GLvdpauSurfaceNV *
 * \return void
 */
STATICINLINE void vdpau_map_surfaces(
    span_const_GLvdpauSurfaceNV const& surfaces,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAUMapSurfacesNV)
    }
    glVDPAUMapSurfacesNV(
        surfaces.size(),
        surfaces.size()
            ? reinterpret_cast<const GLvdpauSurfaceNV*>(surfaces.data())
            : nullptr);
    detail::error_check("VDPAUMapSurfacesNV"sv, check_errors);
}

template<class span_const_u32, class span_const_void>
requires(
    concepts::span<span_const_void> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param vdpSurface const void *
 * \param target GLenum
 * \param numTextureNames GLsizei
 * \param textureNames const GLuint *
 * \return vdpauSurfaceNV
 */
STATICINLINE GLvdpauSurfaceNV vdpau_register_output_surface(
    span_const_void const& vdpSurface,
    GLenum                 target,
    span_const_u32 const&  textureNames,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAURegisterOutputSurfaceNV)
    }
    auto out = glVDPAURegisterOutputSurfaceNV(
        vdpSurface.size() ? reinterpret_cast<const void*>(vdpSurface.data())
                          : nullptr,
        target,
        textureNames.size(),
        textureNames.size()
            ? reinterpret_cast<const GLuint*>(textureNames.data())
            : nullptr);
    detail::error_check("VDPAURegisterOutputSurfaceNV"sv, check_errors);
    return out;
}

template<class span_const_u32, class span_const_void>
requires(
    concepts::span<span_const_void> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param vdpSurface const void *
 * \param target GLenum
 * \param numTextureNames GLsizei
 * \param textureNames const GLuint *
 * \return vdpauSurfaceNV
 */
STATICINLINE GLvdpauSurfaceNV vdpau_register_video_surface(
    span_const_void const& vdpSurface,
    GLenum                 target,
    span_const_u32 const&  textureNames,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAURegisterVideoSurfaceNV)
    }
    auto out = glVDPAURegisterVideoSurfaceNV(
        vdpSurface.size() ? reinterpret_cast<const void*>(vdpSurface.data())
                          : nullptr,
        target,
        textureNames.size(),
        textureNames.size()
            ? reinterpret_cast<const GLuint*>(textureNames.data())
            : nullptr);
    detail::error_check("VDPAURegisterVideoSurfaceNV"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param surface GLvdpauSurfaceNV
 * \param access GLenum
 * \return void
 */
STATICINLINE void vdpau_surface_access(
    GLvdpauSurfaceNV surface,
    GLenum           access,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAUSurfaceAccessNV)
    }
    glVDPAUSurfaceAccessNV(surface, access);
    detail::error_check("VDPAUSurfaceAccessNV"sv, check_errors);
}

template<class span_const_GLvdpauSurfaceNV>
requires(
    concepts::span<span_const_GLvdpauSurfaceNV> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLvdpauSurfaceNV::value_type>,
        std::decay_t<GLvdpauSurfaceNV>>)
/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param numSurface GLsizei
 * \param surfaces const GLvdpauSurfaceNV *
 * \return void
 */
STATICINLINE void vdpau_unmap_surfaces(
    span_const_GLvdpauSurfaceNV const& surfaces,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAUUnmapSurfacesNV)
    }
    glVDPAUUnmapSurfacesNV(
        surfaces.size(),
        surfaces.size()
            ? reinterpret_cast<const GLvdpauSurfaceNV*>(surfaces.data())
            : nullptr);
    detail::error_check("VDPAUUnmapSurfacesNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_vdpau_interop
 * \param surface GLvdpauSurfaceNV
 * \return void
 */
STATICINLINE void vdpau_unregister_surface(
    GLvdpauSurfaceNV surface, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VDPAUUnregisterSurfaceNV)
    }
    glVDPAUUnregisterSurfaceNV(surface);
    detail::error_check("VDPAUUnregisterSurfaceNV"sv, check_errors);
}

} // namespace gl::nv::vdpau_interop
#endif // GL_NV_vdpau_interop
namespace gl::nv::vdpau_interop {
constexpr auto name = "GL_NV_vdpau_interop";
}
