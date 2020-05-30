#include <platforms/win32/sysinfo.h>

#include <peripherals/stl/string_casting.h>
#include <platforms/environment.h>

#if !defined(COFFEE_MINGW64)
extern bool WMI_Query(
    const char* query, const wchar_t* property, std::string& target);
#else
static inline bool WMI_Query(const char*, const wchar_t*, std::string&)
{
}
#endif

namespace platform {
namespace env {
namespace win32 {

static u32 CountThreads(ULONG_PTR bitm)
{
    uint32    bitc   = 0;
    DWORD     lshift = sizeof(ULONG_PTR) * 8 - 1;
    ULONG_PTR bitt   = (ULONG_PTR)1 << lshift;

    for(uint32 i = 0; i <= lshift; ++i)
    {
        bitc += ((bitm & bitt) ? 1 : 0);
        bitt /= 2;
    }
    return bitc;
}

WindowsSysInfo::proc_info WindowsSysInfo::GetProcInfo()
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION st    = nullptr;
    DWORD                                 rsize = 0;
    BOOL                                  done  = false;

    while(!done)
    {
        BOOL rc = GetLogicalProcessorInformation(st, &rsize);
        if(!rc)
        {
            if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if(st)
                    ::free(st);
                else
                    st = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(rsize);
            } else
                return {};
        } else
            break;
    }

    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr         = st;
    DWORD                                 byte_offset = 0;
    PCACHE_DESCRIPTOR                     cache;

    proc_info info;
    info.processors.push_back({});
    proc_info::proc* proc = &info.processors.back();

    /*while (byte_offset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <=
    rsize)
    {
        switch (ptr->Relationship)
        {
        case RelationNumaNode:
                        break;
        case RelationProcessorPackage:
            info.processors.push_back({});
            proc = &info.processors.back();
            break;
        case RelationProcessorCore:
            proc->cores++;
            proc->threads += CountThreads(ptr->ProcessorMask);
            break;
        case RelationCache:
            cache = &ptr->Cache;
            if (cache->Level == 1)
            {
                if(cache->Type==CacheData)
                    proc->cache.l1_d_size += cache->Size;
                else if (cache->Type==CacheInstruction)
                    proc->cache.l1_i_size += cache->Size;
                proc->cache.l1++;
            }
            else if (cache->Level == 2)
            {
                if (cache->Type == CacheData)
                    proc->cache.l2_d_size += cache->Size;
                else if (cache->Type == CacheInstruction)
                    proc->cache.l2_i_size += cache->Size;
                proc->cache.l2++;
            }
            else if (cache->Level == 3)
            {
                if (cache->Type == CacheData)
                    proc->cache.l3_d_size += cache->Size;
                else if (cache->Type == CacheInstruction)
                    proc->cache.l3_i_size += cache->Size;
                proc->cache.l3++;
            }
            break;
        }
        byte_offset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }*/

    ::free(st);

    return info;
}

stl_types::Optional<CString> GetWineVersion()
{
    if constexpr(compile_info::platform::is_windows_uwp)
        return {};

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

    CString out = pwine_get_version();
    return out;
}

stl_types::Optional<CString> GetRegistryString(
    HKEY key, libc_types::cstring subKey, libc_types::cstring valueKey, CString::size_type size)
{
    CString value;
    value.resize(size);
    DWORD valueLen = value.size();

    auto  stat    = RegGetValueA(
        key,
        subKey,
        valueKey,
        RRF_RT_REG_SZ,
        nullptr,
        &value[0],
        &valueLen);

    if(stat != 0)
        return {};

    if(valueLen == 1)
        return {};

    value.resize(valueLen - 1);
    return value;
}

uint64 WindowsSysInfo::MemTotal()
{
    MEMORYSTATUSEX st;
    st.dwLength = sizeof(st);

    GlobalMemoryStatusEx(&st);

    return st.ullTotalPhys;
}

uint64 WindowsSysInfo::MemAvailable()
{
    MEMORYSTATUSEX st;
    st.dwLength = sizeof(st);

    GlobalMemoryStatusEx(&st);

    return st.ullAvailPhys;
}

uint64 WindowsSysInfo::SwapTotal()
{
    MEMORYSTATUSEX st;
    st.dwLength = sizeof(st);

    GlobalMemoryStatusEx(&st);

    return st.ullTotalPageFile;
}

uint64 WindowsSysInfo::SwapAvailable()
{
    MEMORYSTATUSEX st;
    st.dwLength = sizeof(st);

    GlobalMemoryStatusEx(&st);

    return st.ullAvailPageFile;
}

platform::info::HardwareDevice WindowsSysInfo::Processor()
{
#if defined(COFFEE_ARCH_AMD64) || defined(COFFEE_ARCH_X86)
    char CPUString[0x20];
    char CPUBrandString[0x40];

    int CPUInfo[4];

    __cpuid(CPUInfo, 0);

    memset(CPUString, 0, 0x20);
    memset(CPUBrandString, 0, 0x40);

    memcpy(CPUString + sizeof(int) * 0, &CPUInfo[1], sizeof(int));
    memcpy(CPUString + sizeof(int) * 1, &CPUInfo[3], sizeof(int));
    memcpy(CPUString + sizeof(int) * 2, &CPUInfo[2], sizeof(int));

    __cpuid(CPUInfo, Extended_Start);
    int nExIds = CPUInfo[0];

    for(int i = Extended_Start; i < nExIds; ++i)
    {
        __cpuid(CPUInfo, i);

        if(i > CPUBrand_3)
            break;

        if(i == CPUBrand_1)
        {
            memcpy(CPUBrandString + 0, CPUInfo, sizeof(CPUInfo));
        } else if(i == CPUBrand_2)
        {
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        } else if(i == CPUBrand_3)
        {
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
    }

    CString brand = CPUBrandString;
    str::trim::both(brand);
    cstring brand_find = libc::str::find(brand.c_str(), '\0');
    if(brand_find)
        brand.resize(brand_find - brand.c_str());

    return platform::info::HardwareDevice(
        CPUString, brand, Env::GetVar("PROCESSOR_REVISION"));
#else
    return HWDeviceInfo("Generic", COFFEE_ARCH, "0x0");
#endif
}

bigscalar WindowsSysInfo::ProcessorFrequency()
{
    LARGE_INTEGER e;
    QueryPerformanceFrequency(&e);
    return e.QuadPart / 1000000.0;
}

bool WindowsSysInfo::HasFPU()
{
    return IsProcessorFeaturePresent(PF_FLOATING_POINT_EMULATED) == 0;
}

bool WindowsSysInfo::HasPAE()
{
    return IsProcessorFeaturePresent(PF_PAE_ENABLED);
}

bool WindowsSysInfo::HasHyperThreading()
{
#if defined(COFFEE_ARCH_AMD64) || defined(COFFEE_ARCH_X86)
    bool dht = false;

    int CPUInfo[4];

    __cpuid(CPUInfo, 0);
    int nIds = CPUInfo[0];

    for(int i = 0; i < nIds; ++i)
    {
        if(i > 1)
            break;

        if(i == 1)
        {
            dht = CPUInfo[3] & Hyperthreading;
        }
    }

    return dht;
#else
    return false;
#endif
}

CString WindowsSysInfo::GetSystemVersion()
{
    using namespace stl_types;

    if(auto wineVer = GetWineVersion(); wineVer)
        return *wineVer;

    /* Dear Microsoft, I only want a string. */
    OSVERSIONINFO a;

    ZeroMemory(&a, sizeof(a));

    a.dwOSVersionInfoSize = sizeof(a);

#ifdef COFFEE_WINDOWS_UWP
    a.dwMajorVersion = 10;
    a.dwMinorVersion = 0;
#else
    GetVersionEx(&a);
#endif

    CString out = cast_pod(a.dwMajorVersion) + "." + cast_pod(a.dwMinorVersion);

    if(auto version = env::win32::GetRegistryString(
           HKEY_LOCAL_MACHINE,
           "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
           "ReleaseId",
           128);
       version)
        return out + "." + *version;
    else
        return out;
}

platform::info::HardwareDevice WindowsSysInfo::DeviceName()
{
    CString dev_manuf;
    CString dev_desc;
    CString dev_fw;
    WMI_Query("SELECT * FROM CIM_Product", L"Vendor", dev_manuf);
    WMI_Query("SELECT * FROM CIM_Product", L"Version", dev_desc);
    WMI_Query("SELECT * FROM CIM_Product", L"SKUNumber", dev_fw);
    return platform::info::HardwareDevice(dev_manuf, dev_desc, dev_fw);
}

platform::info::HardwareDevice WindowsSysInfo::Motherboard()
{
    return platform::info::HardwareDevice();
}

platform::info::HardwareDevice WindowsSysInfo::Chassis()
{
    return platform::info::HardwareDevice();
}

} // namespace win32
} // namespace env
} // namespace platform
