#include <peripherals/error/file_base.h>
#include <peripherals/error/posix.h>
#include <peripherals/error/windows.h>

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

#if defined(COFFEE_POSIX_ERRNO)

#include <cstring>

namespace platform {
namespace file {

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

} // namespace file
} // namespace platform

#endif

/*
 *
 * Win32 error codes
 *
 */

#if defined(COFFEE_WINDOWS)

namespace platform {
namespace file {

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

} // namespace file
} // namespace platform
#endif
