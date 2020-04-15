#include <coffee/comp_app/subsystems.h>

namespace comp_app {

application_error_category::~application_error_category()
{
}

const char* application_error_category::name() const noexcept
{
    return "application_error_category";
}

std::string application_error_category::message(int ec) const
{
    using E = AppError;

#define ERROR_TR(err) \
    case E::err:      \
        return C_STR(err);

    switch(static_cast<AppError>(ec))
    {
        ERROR_TR(OutOfMemory)
        ERROR_TR(NoDisplay)
        ERROR_TR(SystemError)
        ERROR_TR(ServiceMissing)

        ERROR_TR(NoWindow)

        ERROR_TR(NoFramebuffer)
        ERROR_TR(InvalidFramebufferConfig)
        ERROR_TR(InvalidSwapInterval)
        ERROR_TR(SwapBuffersFailed)

        ERROR_TR(FramebufferTeardownFail)

        ERROR_TR(NoContext)
        ERROR_TR(ContextDowngrade)
        ERROR_TR(FramebufferMismatch)
        ERROR_TR(ContextAlreadyCurrent)
        ERROR_TR(ContextNotAvailable)
        ERROR_TR(ContextInvalidated)
        ERROR_TR(ContextFailedBind)

        ERROR_TR(BindingFailed)

        ERROR_TR(None)
    }
    return "Unknown error";
}

} // namespace comp_app
