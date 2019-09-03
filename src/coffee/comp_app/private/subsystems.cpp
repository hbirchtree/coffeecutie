#include <coffee/comp_app/subsystems.h>

namespace comp_app {

application_error_category::~application_error_category()
{
}

const char *application_error_category::name() const noexcept
{
    return "application_error_category";
}

std::string application_error_category::message(int ec) const
{
    return {};
}

}
