#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

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
    ServiceMissing,

    NoWindow,

    NoFramebuffer,
    InvalidFramebufferConfig,
    InvalidSwapInterval,
    SwapBuffersFailed,

    FramebufferTeardownFail,

    /* Graphics context errors */
    NoContext,
    ContextDowngrade,
    FramebufferMismatch,   /*!< Desired framebuffer config not available */
    ContextAlreadyCurrent, /*!< Semantic error by user */
    ContextNotAvailable,   /*!< Temporarily unavailable */
    ContextInvalidated,    /*!< Resources were freed externally */
    ContextFailedBind,

    /* Binding errors (eg. GL binding) */
    BindingFailed,
};

struct application_error_category : std::error_category
{
    virtual ~application_error_category();

    virtual const char* name() const noexcept;
    virtual std::string message(int ec) const;
};

using app_error =
    stl_types::domain_error_code<AppError, application_error_category>;

} // namespace comp_app
