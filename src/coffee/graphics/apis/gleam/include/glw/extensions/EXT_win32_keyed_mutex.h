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
 * \return Boolean
 */
STATICINLINE bool acquire_keyed_mutex_win32(u32 memory, u64 key, u32 timeout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AcquireKeyedMutexWin32EXT)
    }
    auto out = glAcquireKeyedMutexWin32EXT(memory, key, timeout);
    detail::error_check("AcquireKeyedMutexWin32EXT"sv);
    return out == GL_TRUE ? true : false;
}

/*!
 * \brief Part of GL_EXT_win32_keyed_mutex
 * \param memory GLuint
 * \param key GLuint64
 * \return Boolean
 */
STATICINLINE bool release_keyed_mutex_win32(u32 memory, u64 key)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReleaseKeyedMutexWin32EXT)
    }
    auto out = glReleaseKeyedMutexWin32EXT(memory, key);
    detail::error_check("ReleaseKeyedMutexWin32EXT"sv);
    return out == GL_TRUE ? true : false;
}

} // namespace gl::ext::win32_keyed_mutex
#endif // GL_EXT_win32_keyed_mutex
namespace gl::ext::win32_keyed_mutex {
constexpr auto name = "GL_EXT_win32_keyed_mutex";
}
