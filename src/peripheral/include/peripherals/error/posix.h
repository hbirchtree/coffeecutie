#pragma once

#include <peripherals/stl/types.h>

namespace platform {
namespace file {

using namespace ::stl_types;

struct posix_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using posix_error_code = domain_error_code<int, posix_error_category>;

namespace posix {

extern bool collect_error(posix_error_code& ec);

template<typename ErrorType>
FORCEDINLINE bool collect_error_to(ErrorType& ec)
{
    return collect_error(ec.template as<posix_error_code>());
}

} // namespace posix
} // namespace file
} // namespace platform
