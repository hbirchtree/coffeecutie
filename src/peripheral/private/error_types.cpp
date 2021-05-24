#include <peripherals/error/file_base.h>
#include <peripherals/error/posix.h>
#include <peripherals/error/regex.h>
#include <peripherals/error/windows.h>

#include <peripherals/error/posix.h>
#include <peripherals/stl/string_ops.h>

#include <cstring>
#include <errno.h>

namespace platform {
namespace file {

const char* file_error_category::name() const noexcept
{
    return "file_error_code";
}

std::string file_error_category::message(int error_code) const
{
    FileError ec = C_CAST<FileError>(error_code);

    switch(ec)
    {
    case FileError::NotFound:
        return "No such file or directory";
    case FileError::InvalidAccess:
        return "Invalid access flags";
    case FileError::InvalidHandle:
        return "Invalid file handle";
    case FileError::PermissionError:
        return "Permission denied";
    case FileError::MappingFailed:
        return "File mapping failed";
    case FileError::ReadFailed:
        return "File read failure";
    case FileError::WriteFailed:
        return "File write failure";
    case FileError::SystemError:
        return "System error";
    }

    Throw(implementation_error("unimplemented error message"));
}
} // namespace file
} // namespace platform

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

#if defined(COFFEE_WINDOWS)

#include <peripherals/libc/string_ops.h>
#include <peripherals/platform/windows.h>

namespace platform {
namespace win32 {

const char* error_category::name() const noexcept
{
    return "win32_error_code";
}

std::string error_category::message(int error_code) const
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
    return stl_types::str::encode::to<char>(std::wstring(msgBuf, size));
}

} // namespace win32
} // namespace platform
#endif
