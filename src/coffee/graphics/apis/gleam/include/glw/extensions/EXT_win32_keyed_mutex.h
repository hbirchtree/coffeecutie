#pragma once

#ifdef GL_EXT_win32_keyed_mutex
namespace gl::ext::win32_keyed_mutex {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_EXT_win32_keyed_mutex
 * \param memory GLuint
 * \param key GLuint64
 * \param timeout GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean acquire_keyed_mutex_win32(
    u32         memory,
    u64         key,
    u32         timeout,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(AcquireKeyedMutexWin32EXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glAcquireKeyedMutexWin32EXT, memory, key, timeout);
#endif
    auto out = glAcquireKeyedMutexWin32EXT(memory, key, timeout);
    detail::error_check("AcquireKeyedMutexWin32EXT"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_EXT_win32_keyed_mutex
 * \param memory GLuint
 * \param key GLuint64
 * \return GLboolean
 */
STATICINLINE GLboolean release_keyed_mutex_win32(
    u32 memory, u64 key, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ReleaseKeyedMutexWin32EXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glReleaseKeyedMutexWin32EXT, memory, key);
#endif
    auto out = glReleaseKeyedMutexWin32EXT(memory, key);
    detail::error_check("ReleaseKeyedMutexWin32EXT"sv, check_errors);
    return out;
}

} // namespace gl::ext::win32_keyed_mutex
#endif // GL_EXT_win32_keyed_mutex
namespace gl::ext::win32_keyed_mutex {
constexpr auto name = "GL_EXT_win32_keyed_mutex";
}
