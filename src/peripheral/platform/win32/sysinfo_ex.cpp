#include <platforms/win32/sysinfo.h>

#include <windows.h>

namespace {
std::optional<std::string> wine_version()
{
    /* To test the other branch, allow selecting whether Wine should be unmasked
     */
    if(!::getenv("COFFEE_UNMASK_WINE"))
        return std::nullopt;

    /* Checking for Wine, being a cheeky cunt */
    /* Source:
     * https://www.winehq.org/pipermail/wine-devel/2008-September/069387.html */
    typedef const char*(CDECL * WINE_GET_VERSION_FPTR)(void);

    static WINE_GET_VERSION_FPTR pwine_get_version = nullptr;

    if(!pwine_get_version)
    {
        HMODULE hntdll = GetModuleHandle("ntdll.dll");
        if(!hntdll)
            return {};
        pwine_get_version =
            (WINE_GET_VERSION_FPTR)GetProcAddress(hntdll, "wine_get_version");
        if(!pwine_get_version)
            return {};
    }

    std::string out = pwine_get_version();
    return out;
}

std::optional<std::string> registry_string(
    HKEY                   key,
    const char*            subKey,
    const char*            valueKey,
    std::string::size_type size)
{
    std::string value;
    value.resize(size);
    DWORD valueLen = value.size();

    auto stat = RegGetValueA(
        key, subKey, valueKey, RRF_RT_REG_SZ, nullptr, &value[0], &valueLen);

    if(stat != 0)
        return std::nullopt;

    if(valueLen == 1)
        return std::nullopt;

    value.resize(valueLen - 1);
    return value;
}
} // namespace

namespace platform::info {
namespace os::win32 {

std::string architecture()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    if(info.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_AMD64)
        return "amd64";
    if(info.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_INTEL)
        return "x86";
    if(info.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_ARM)
        return "arm";
    if(info.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_ARM64)
        return "arm64";
    if(info.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_IA32_ON_ARM64)
        return "x86/arm64";
    if(info.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_IA32_ON_WIN64)
        return "x86/amd64";
    return "unknown";
}

std::string kernel()
{
    if(wine_version().has_value())
        return "Wine";
    else
        return "Windows NT";
}

std::string kernel_version()
{
    if(auto wine_ver = wine_version())
        return wine_ver.value();

    OSVERSIONINFO ver;

    ZeroMemory(&ver, sizeof(ver));
    ver.dwOSVersionInfoSize = sizeof(ver);
    if(GetVersionEx(&ver) == FALSE)
        return "?.?.?";
    return fmt::format(
        "{}.{}.{}", ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber);
}

std::optional<std::string> version()
{
    if(auto release_id = registry_string(
           HKEY_LOCAL_MACHINE,
           "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
           "ReleaseId",
           128))
        return release_id;
    if(auto ver_ = registry_string(
           HKEY_LOCAL_MACHINE,
           "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
           "ProductName",
           128))
    {
        using namespace std::string_literals;
        auto ver = *ver_;
        if(ver.starts_with("Microsoft Windows "))
            return ver.substr("Microsoft Windows "s.size());
        return ver;
    }
    return std::nullopt;
}

bool is_wow64()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_IA32_ON_WIN64;
}

std::optional<std::string> wine_version()
{
    return ::wine_version();
}

} // namespace os::win32
} // namespace platform::info
