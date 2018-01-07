#include <coffee/core/plat/file/unix/file.h>
#include <coffee/core/CDebug>

namespace Coffee{
namespace CResources{

bool Posix::PosixFileMod_def::ErrnoCheck(cstring ref, int fd)
{
    if(errno != 0)
    {
        char path[PATH_MAX] = {};
        char fd_path[128] = {};
#if defined(COFFEE_LINUX)
        if(fd >= 0)
        {
            snprintf(fd_path, 128, "/proc/self/fd/%d", fd);
            auto size = readlink(fd_path, path, PATH_MAX);
            if(size != 0)
                ref = path;
        }
#elif defined(COFFEE_APPLE)
        if(!ref && fcntl(fd, F_GETPATH, path) != -1)
            ref = path;
#endif

//        fprintf(stderr,"ERROR:%s: %s\n",ref,strerror(errno));
        cVerbose(10, "ERROR:{0}: {1}", ref, strerror(errno));
        errno = 0;
        return true;
    }
    return false;
}

}
}
