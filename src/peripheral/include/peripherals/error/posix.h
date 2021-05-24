#pragma once

#include <peripherals/stl/types.h>

namespace platform::common::posix {

using ::stl_types::error_category;
using ::stl_types::domain_error_code;

struct posix_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
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
        errno = 0;
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
