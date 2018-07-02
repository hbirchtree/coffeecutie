#include <coffee/core/plat/plat_windows_errors.h>

namespace Coffee {
namespace Win32 {

const char* win32_error_category::name() const noexcept
{
    return "win32_error_code";
}

std::string win32_error_category::message(int error_code) const
{
#ifdef COFFEE_WINDOWS_UWP
    LPWSTR msgBuf = nullptr;
#else
    LPSTR msgBuf = nullptr;
#endif
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
#ifdef COFFEE_WINDOWS_UWP
        (LPWSTR)&msgBuf,
#else
        (LPSTR)&msgBuf,
#endif
        0,
        nullptr);

#ifdef COFFEE_WINDOWS_UWP
    std::wstring error_w(msgBuf, size);
    std::string  error(error_w.begin(), error_w.end());
#else
    std::string error(msgBuf, size);
#endif
    LocalFree(msgBuf);
    return error;
}

} // namespace Win32
} // namespace Coffee
