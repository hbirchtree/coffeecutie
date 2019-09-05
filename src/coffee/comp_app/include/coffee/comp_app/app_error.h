#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>

#define C_EXPECT_ZERO(v)                             \
    {                                                \
        if(v != 0)                                   \
            Throw(undefined_behavior(#v " failed")); \
    }

namespace comp_app {

enum class AppError
{
    None = 0,

    OutOfMemory,
    NoDisplay,
    SystemError,

    NoWindow,

    /* Graphics context errors */
    NoContext,
    FramebufferMismatch,   /*!< Desired framebuffer config not available */
    ContextAlreadyCurrent, /*!< Semantic error by user */
    ContextNotAvailable,   /*!< Temporarily unavailable */
    ContextInvalidated,    /*!< Resources were freed externally */
};

struct application_error_category : stl_types::error_category
{
    virtual ~application_error_category();

    virtual const char* name() const noexcept;
    virtual std::string message(int ec) const;
};

using app_error =
    stl_types::domain_error_code<AppError, application_error_category>;

} // namespace comp_app
