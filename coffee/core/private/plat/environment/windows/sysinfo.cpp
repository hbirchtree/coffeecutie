#include <coffee/core/plat/environment/windows/sysinfo.h>
#include <coffee/core/plat/plat_environment.h>

extern bool WMI_Query(const char* query, const wchar_t* property, std::string& target);

namespace Coffee{
namespace Environment{
namespace Windows{

uint32 WindowsSysInfo::CountThreads(ULONG_PTR bitm)
{
    uint32 bitc = 0;
    DWORD lshift = sizeof(ULONG_PTR) * 8 - 1;
    ULONG_PTR bitt = (ULONG_PTR)1 << lshift;

    for (uint32 i = 0;i <= lshift;++i)
    {
        bitc += ((bitm&bitt) ? 1 : 0);
        bitt /= 2;
    }
    return bitc;
}

WindowsSysInfo::proc_info WindowsSysInfo::GetProcInfo()
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION st = nullptr;
    DWORD rsize = 0;
    BOOL done = false;

    while (!done)
    {
        BOOL rc = GetLogicalProcessorInformation(st, &rsize);
        if (!rc)
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (st)
                    CFree(st);
                else
                    st = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)Alloc(rsize);
            }
            else
                return {};
        }
        else
            break;
    }

    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = st;
    DWORD byte_offset = 0;
    PCACHE_DESCRIPTOR cache;

    proc_info info;
    info.processors.push_back({});
    proc_info::proc* proc = &info.processors.back();

    uint32 procc = 0;

    while (byte_offset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= rsize)
    {
        switch (ptr->Relationship)
        {
        case RelationNumaNode:
            break;
        case RelationProcessorPackage:
            if (++procc == 1)
                break;
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
    }

    CFree(st);

    return info;
}

HWDeviceInfo WindowsSysInfo::Processor()
{
#if defined(COFFEE_ARCH_AMD64) || defined(COFFEE_ARCH_X86)
    char CPUString[0x20];
    char CPUBrandString[0x40];

    int CPUInfo[4];

    __cpuid(CPUInfo,0);

    MemClear(CPUString, 0x20);
    MemClear(CPUBrandString, 0x40);

    MemCpy(CPUString + sizeof(int)*0, &CPUInfo[1], sizeof(int));
    MemCpy(CPUString + sizeof(int)*1, &CPUInfo[3], sizeof(int));
    MemCpy(CPUString + sizeof(int)*2, &CPUInfo[2], sizeof(int));

    __cpuid(CPUInfo, Extended_Start);
    int nExIds = CPUInfo[0];

    for (int i = Extended_Start;i < nExIds;++i)
    {
        __cpuid(CPUInfo,i);

        if (i > CPUBrand_3)
            break;

        if (i == CPUBrand_1)
        {
            MemCpy(CPUBrandString + 0, CPUInfo, sizeof(CPUInfo));
        }
        else if (i == CPUBrand_2)
        {
            MemCpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        }
        else if (i == CPUBrand_3)
        {
            MemCpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
    }

    CString brand = CPUBrandString;
    StrUtil::trim(brand);
    cstring brand_find = Search::ChrFind(brand.c_str(), 0);
    if(brand_find)
        brand.resize(brand_find - brand.c_str());

    return HWDeviceInfo(CPUString,brand,Env::GetVar("PROCESSOR_REVISION"));
#else
	return HWDeviceInfo("Generic", COFFEE_ARCH, "0x0");
#endif
}

bool WindowsSysInfo::HasHyperThreading()
{
#if defined(COFFEE_ARCH_AMD64) || defined(COFFEE_ARCH_X86)
    bool dht = false;

    int CPUInfo[4];

    __cpuid(CPUInfo, 0);
    int nIds = CPUInfo[0];


    for (int i = 0;i < nIds;++i)
    {
        if (i > 1)
            break;

        if (i == 1)
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
    /* Checking for Wine, being a cheeky cunt */
    /* Source: https://www.winehq.org/pipermail/wine-devel/2008-September/069387.html */
#if !defined(COFFEE_WINDOWS_UWP)
	typedef const char * (CDECL *WINE_GET_VERSION_FPTR)(void);
    do{
        static WINE_GET_VERSION_FPTR pwine_get_version;

        HMODULE hntdll = GetModuleHandle("ntdll.dll");
        if(!hntdll)
            break;
        pwine_get_version = (WINE_GET_VERSION_FPTR)GetProcAddress(hntdll,"wine_get_version");
        if(!pwine_get_version)
            break;
        CString out = CString("Wine ") + pwine_get_version();
        return out;
    }while(false);
#endif
    /* Dear Microsoft, I only want a string. */
    OSVERSIONINFO a;

    ZeroMemory(&a,sizeof(a));

    a.dwOSVersionInfoSize = sizeof(a);

#ifdef COFFEE_WINDOWS_UWP
	a.dwMajorVersion = 10;
	a.dwMinorVersion = 0;
#else
	GetVersionEx(&a);
#endif

    CString out;
    out += Convert::uinttostring(a.dwMajorVersion);
    out += ".";
    out += Convert::uinttostring(a.dwMinorVersion);
    return out;
}

HWDeviceInfo WindowsSysInfo::DeviceName()
{
	CString dev_manuf;
	CString dev_desc;
	CString dev_fw;
	WMI_Query("SELECT * FROM CIM_Product", L"Vendor", dev_manuf);
	WMI_Query("SELECT * FROM CIM_Product", L"Version", dev_desc);
	WMI_Query("SELECT * FROM CIM_Product", L"SKUNumber", dev_fw);
	return HWDeviceInfo(dev_manuf, dev_desc, dev_fw);
}

HWDeviceInfo WindowsSysInfo::Motherboard()
{
	return HWDeviceInfo();
}

HWDeviceInfo WindowsSysInfo::Chassis()
{
	return HWDeviceInfo();
}



}
}
}
