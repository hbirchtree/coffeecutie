#include <platforms/linux/environment.h>

#include <peripherals/semantic/ptr_wrap.h>
#include <platforms/environment.h>
#include <platforms/file.h>

namespace platform {
namespace env {
namespace Linux {

using namespace semantic;

CString EnvironmentF::ExecutableName(cstring_w)
{
    Ptr<char, ptr_opts::managed_ptr> p = realpath("/proc/self/exe", nullptr);

    CString v = p.get();
    return v;
}

Url EnvironmentF::ApplicationDir()
{
    file::file_error ec;

    CString execname = ExecutableName();
    return file::DirFun::Dirname(execname.c_str(), ec);
}

} // namespace Linux

} // namespace env
} // namespace platform
