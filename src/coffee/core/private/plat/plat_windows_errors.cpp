#include <coffee/core/plat/plat_windows_errors.h>

#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/plat/plat_windows.h>

namespace Coffee {
namespace Win32 {

const char* win32_error_category::name() const noexcept
{
    return "win32_error_code";
}

std::string win32_error_category::message(int error_code) const
{
    LPWSTR msgBuf = nullptr;
    size_t size   = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msgBuf,
        0,
        nullptr);

    LocalFree(msgBuf);
    return str::encode::to<char>(std::wstring(msgBuf, size));
}

} // namespace Win32
} // namespace Coffee
