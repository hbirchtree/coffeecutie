#pragma once

#include <coffee/core/plat/plat_windows.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee {
namespace Win32 {

struct win32_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using win32_error_code = domain_error_code<DWORD, win32_error_category>;

} // namespace Win32
} // namespace Coffee
