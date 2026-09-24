#include <peripherals/error/posix.h>
#include <peripherals/error/regex.h>
#include <peripherals/error/windows.h>

#include <peripherals/error/posix.h>
#include <peripherals/stl/string_ops.h>

#include <cstring>
#include <errno.h>

namespace stl_types {
namespace regex {

const char* regex_error_category::name() const noexcept
{
    return "regex_error";
}

std::string regex_error_category::message(int) const
{
    return {};
}

} // namespace regex
} // namespace stl_types

#if defined(COFFEE_POSIX_ERRNO)

namespace platform::common::posix {

bool collect_error(posix_error_code& ec)
{
    if(errno != 0)
    {
        ec    = errno;
        errno = 0;
        return true;
    }

    return false;
}

const char* posix_error_category::name() const noexcept
{
    return "posix_error_code";
}

std::string posix_error_category::message(int error_code) const
{
    char* posix_error_msg = ::strerror(error_code);

    if(!posix_error_msg)
        return "No error";

    return posix_error_msg;
}

} // namespace platform::common::posix

#endif

/*
 *
 * Win32 error codes
 *
 */

#if defined(COFFEE_WINDOWS) || defined(COFFEE_WIN32)

#include <peripherals/libc/string_ops.h>
#include <peripherals/platform/windows.h>

namespace platform {
namespace win32 {

std::string error_to_string(libc_types::u32 err)
{
    LPSTR  msgBuf = nullptr;
    size_t size   = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&msgBuf,
        0,
        nullptr);

    auto out = std::string(msgBuf, size);
    LocalFree(msgBuf);
    return out;
}

std::string last_error()
{
    return error_to_string(GetLastError());
}

} // namespace win32
} // namespace platform
#endif
