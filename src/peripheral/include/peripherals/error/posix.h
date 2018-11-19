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

}
}
