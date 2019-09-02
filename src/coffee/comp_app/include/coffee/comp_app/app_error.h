#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>

namespace comp_app {

enum class AppError
{
    None = 0,
};

struct application_error_category : stl_types::error_category
{
    virtual ~application_error_category();

    virtual const char* name() const noexcept;
    virtual std::string message(int ec) const;
};

using app_error =
    stl_types::domain_error_code<AppError, application_error_category>;

}
