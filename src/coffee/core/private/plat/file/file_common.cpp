#include <coffee/core/plat/file/file_def.h>

#include <coffee/core/plat/file/cfile.h>
#include <coffee/core/plat/plat_environment.h>

namespace Coffee {
namespace CResources {

const char* file_error_category::name() const noexcept
{
    return "FileFun";
}

std::string file_error_category::message(int error_code) const
{
    FileError ec = C_CAST<FileError>(error_code);

    switch(ec)
    {
    case FileError::SystemError:
        return "System error";
    default:
        break;
    }

    Throw(implementation_error("unimplemented error message"));
}

const char* FILE_error_category::name() const noexcept
{
    return "FILE_error_code";
}

std::string FILE_error_category::message(int error_code) const
{
    char* posix_error_msg = strerror(error_code);

    if(!posix_error_msg)
        return "No error";

    return posix_error_msg;
}

void FILEApi::CheckError(FILE* fd, FILE_error_code &ec)
{
    if(!ferror(fd) && fd)
        return;

    ec = errno;
    errno = 0;
}

} // namespace CResources
} // namespace Coffee
