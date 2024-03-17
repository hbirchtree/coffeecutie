#pragma once

#ifdef GL_EXT_x11_sync_object
namespace gl::ext::x11_sync_object {
namespace values {
constexpr u32 sync_x11_fence = 0x90E1;
} // namespace values

/*!
 * \brief Part of GL_EXT_x11_sync_object
 * \param external_sync_type GLenum
 * \param external_sync GLintptr
 * \param flags GLbitfield
 * \return sync
 */
STATICINLINE GLsync import_sync(
    GLenum      external_sync_type,
    GLintptr    external_sync,
    GLbitfield  flags,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ImportSyncEXT)
    }
    auto out = glImportSyncEXT(external_sync_type, external_sync, flags);
    detail::error_check("ImportSyncEXT"sv, check_errors);
    return out;
}

} // namespace gl::ext::x11_sync_object
#endif // GL_EXT_x11_sync_object
namespace gl::ext::x11_sync_object {
constexpr auto name = "GL_EXT_x11_sync_object";
}
