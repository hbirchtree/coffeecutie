#include <platforms/linux/environment.h>

#include <peripherals/semantic/ptr_wrap.h>
#include <platforms/environment.h>
#include <platforms/file.h>

namespace platform {
namespace env {
namespace Linux {

using namespace semantic;
using url::constructors::MkSysUrl;

CString EnvironmentF::ExecutableName(cstring_w)
{
    if(auto file = platform::path::canon(MkSysUrl("/proc/self/exe")); !file)
        return {};
    else
        return file.value().internUrl;
}

Url EnvironmentF::ApplicationDir()
{
    CString execname = ExecutableName();
    if(auto path = platform::path::dir(MkSysUrl(execname));
       !path)
        return {};
    else
        return path.value();
}

} // namespace Linux

} // namespace env
} // namespace platform
