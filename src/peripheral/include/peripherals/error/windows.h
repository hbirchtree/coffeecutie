#pragma once

#include <peripherals/stl/types.h>

namespace platform {
namespace file {

using namespace ::stl_types;

struct win32_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using win32_error_code = domain_error_code<u32, win32_error_category>;

} // namespace file
} // namespace platform
