#include <coffee/core/plat/file/unix/file.h>

namespace Coffee {
namespace CResources {

bool Posix::PosixFileMod_def::ErrnoCheck(file_error& ec, cstring ref, int fd)
{
#ifndef COFFEE_LOWFAT
    if(errno != 0)
    {
        if(ref)
            ec.m_internalError = ref;

        ec                 = FileError::SystemError;
        ec                 = posix_error_code();
        ec.m_internalError = errno;
        errno              = 0;
        return true;
    }
#endif
    return false;
}

} // namespace CResources
} // namespace Coffee
