#pragma once

#include <peripherals/stl/types.h>

namespace platform::common::posix {

using ::stl_types::domain_error_code;
using ::stl_types::error_category;

struct posix_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

inline std::string_view error_to_string(int err)
{
    using namespace std::string_view_literals;
#define ERROR_TO_STR(code) \
    case code:             \
        return #code##sv;

    switch(err)
    {
        ERROR_TO_STR(EPERM)
        ERROR_TO_STR(ENOENT)
        ERROR_TO_STR(ESRCH)
        ERROR_TO_STR(EINTR)
        ERROR_TO_STR(EIO)
        ERROR_TO_STR(ENXIO)
        ERROR_TO_STR(EBADF)
        ERROR_TO_STR(EAGAIN)
        ERROR_TO_STR(ENOMEM)
        ERROR_TO_STR(EACCES)
        ERROR_TO_STR(EFAULT)
        ERROR_TO_STR(EBUSY)
        ERROR_TO_STR(EEXIST)
        ERROR_TO_STR(ENODEV)
        ERROR_TO_STR(ENOTDIR)
        ERROR_TO_STR(EISDIR)
        ERROR_TO_STR(EINVAL)
        ERROR_TO_STR(EMFILE)
        ERROR_TO_STR(ENOTTY)
        ERROR_TO_STR(ETXTBSY)
        ERROR_TO_STR(EFBIG)
        ERROR_TO_STR(ENOSPC)
        ERROR_TO_STR(ESPIPE)
        ERROR_TO_STR(EROFS)
        ERROR_TO_STR(EMLINK)
        ERROR_TO_STR(EPIPE)
    default:
        return "unknown error code"sv;
    }
#undef ERROR_TO_STR
}

struct posix_runtime_error : std::runtime_error
{
    posix_runtime_error(int error) :
        std::runtime_error([error]() {
            auto err_msg = error_to_string(error);
            return std::string(err_msg.begin(), err_msg.end());
        }())
    {
    }
};

using posix_error      = int;
using posix_error_code = domain_error_code<int, posix_error_category>;

extern bool collect_error(posix_error_code& ec);

FORCEDINLINE bool collect_error(posix_error& ec)
{
    if(errno != 0)
    {
        ec    = errno;
        errno = 0;
        return true;
    }
    return false;
}

FORCEDINLINE posix_error get_error()
{
    if(errno != 0)
    {
        auto out = errno;
        errno    = 0;
        return out;
    }
    return 0;
}

template<typename ErrorType>
FORCEDINLINE bool collect_error_to(ErrorType& ec)
{
    return collect_error(ec.template as<posix_error_code>());
}

} // namespace platform::common::posix
