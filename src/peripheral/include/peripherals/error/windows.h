#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace win32 {

using ::stl_types::domain_error_code;

struct error_category : ::stl_types::error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using error_code =
    domain_error_code<libc_types::u32, error_category>;

} // namespace win32
} // namespace platform
