#include <coffee/core/platform_data.h>

#include <peripherals/base.h>
#include <platforms/environment.h>
#include <platforms/sysinfo.h>

#if defined(COFFEE_ANDROID)
#include <coffee/android/android_main.h>
#endif

#if defined(COFFEE_APPLE_MOBILE)
#include <coffee/foreign/foreign.h>
#endif

#if defined(COFFEE_WINDOWS)
#include <lm.h>
#include <peripherals/stl/string_casting.h>
#endif

#if defined(__GLIBC__)
#include <gnu/libc-version.h>
#endif

using namespace ::stl_types;

namespace platform {

//using namespace Coffee;

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
//namespace env {
//namespace Linux {
//extern CString          get_kern_name();
//extern CString          get_kern_arch();
//extern CString          get_kern_ver();
//extern lsb_data         get_lsb_release();
//extern info::DeviceType get_device_variant();
//} // namespace Linux
//} // namespace env
#endif

#if defined(COFFEE_WINDOWS)
namespace env {
namespace win32 {

extern stl_types::Optional<CString> GetWineVersion();
stl_types::Optional<CString>        GetRegistryString(
           HKEY                key,
           libc_types::cstring subKey,
           libc_types::cstring valueKey,
           CString::size_type  size);

}
} // namespace env
#endif

namespace info {

CString system_name()
{
#ifndef COFFEE_LOWFAT
    //    const constexpr cstring _fmt = "%s %s %u-bit (%s ";
//    CString sys_ver   = SysInfo::GetSystemVersion();
    CString sys_ver = {};
    CString sys_name  = C_SYSTEM_STRING;
    CString curr_arch = COFFEE_ARCH;
#if defined(COFFEE_ANDROID)
    sys_name = "Android"; // Override this for clarity
#elif defined(COFFEE_LINUX)
//    sys_name = env::Linux::get_lsb_release().distribution;

//    if(sys_name.empty())
//        sys_name = env::Linux::get_kern_name();
#endif
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
//    curr_arch = env::Linux::get_kern_arch();
#endif

    CString base = sys_name + " " + sys_ver + " (" + curr_arch + ")";

#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP)
    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    static LPFN_ISWOW64PROCESS fnIsWow64Process;

    /* TODO: Add Wine check */

    do
    {
        if(!fnIsWow64Process)
        {
            fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
                GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
        }

        if(!fnIsWow64Process)
            break;

        BOOL is_wow64 = FALSE;

        if(fnIsWow64Process(GetCurrentProcess(), &is_wow64) && is_wow64 == TRUE)
            base.append(" (WoW64)");
    } while(false);
#endif

    return base;
#else
    return {};
#endif
}

} // namespace info
} // namespace platform
