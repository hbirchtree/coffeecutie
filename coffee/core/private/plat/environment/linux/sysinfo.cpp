#include <coffee/core/plat/environment/linux/sysinfo.h>
#include <coffee/core/plat/file/linux/file.h>
#include <coffee/core/plat/environment/linux/environment.h>
#include <coffee/core/string_casting.h>

#include <coffee/core/platform_data.h>

namespace Coffee{
namespace Environment{
namespace Linux{

thread_local CString LinuxSysInfo::cached_cpuinfo_string;

using DirFun = CResources::DirFun;

/* More paths to inspect:
 *
 * Desktop details:
 * - /sys/class/dmi/id/board_name
 * - /sys/class/dmi/id/board_vendor
 * - /sys/class/dmi/id/chassis_type
 *
 * Networking:
 * - /sys/class/net/.../operstate
 *
 */

static CString get_lsb_release()
{
    CString version = CResources::Linux::LinuxFileFun::sys_read("/etc/lsb-release");
    cstring desc = StrFind(version.c_str(), "DISTRIB_DESCRIPTION");
    if(desc && (desc = Search::ChrFind(desc, '=') + 1)
            && (desc = Search::ChrFind(desc, '"') + 1))
    {
        cstring end = Search::ChrFind(desc, '"');
        if(end || (end = Search::ChrFind(desc, 0)))
        {
            CString desc_std(desc, end-desc);
            return desc_std;
        }
    }
    return {};
}

static CString get_kern_ver()
{
    utsname d;
    if(uname(&d)!=0)
        return "?";
    else
        return d.release;
}

static CString get_linux_property(CString const& source, cstring query)
{
    auto res = source.find(query);

    if(res == source.npos)
        return {};

    res = source.find(":", res)+1;
    auto end = source.find_first_of("\n", res);

    return source.substr(res, end-res);
}

CString get_kern_name()
{
    utsname d;
    if(uname(&d)!=0)
        return "?";
    else
        return d.sysname;
}

CString get_kern_arch()
{
    utsname d;
    if(uname(&d)!=0)
        return "?";
    else
        return d.machine;
}

PlatformData::DeviceType get_device_variant()
{
    CString input = CResources::Linux::LinuxFileFun::sys_read(
                "/sys/class/dmi/id/chassis_type");

    int32 chassis_type = cast_string<int32>(input);

    switch(chassis_type)
    {
    case 1:
        return PlatformData::DeviceDesktop;
    case 10:
        return PlatformData::DeviceLaptop;
    default:
        return PlatformData::DeviceUnknown;
    }
}

CString LinuxSysInfo::GetSystemVersion()
{
    CString tmp = get_lsb_release();
    if(tmp.size() <= 0)
        tmp = get_kern_ver();
    return tmp;
}

CString LinuxSysInfo::CPUInfoString(bool force)
{
    /* We don't want to read it tons of times */
    if(!cached_cpuinfo_string.empty()&&!force)
        return cached_cpuinfo_string;

    //        C_PERFWARN(__FILE__,__LINE__,"Reading from /proc/cpuinfo!");

    CString data = CResources::Linux::LinuxFileFun::sys_read("/proc/cpuinfo");

    cached_cpuinfo_string = data;

    return data;
}

Set<CString> LinuxSysInfo::CPUFlags()
{
    const cstring query_linaro = "Features";
    const cstring query = "flags";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);

    if(!result.size())
        result = get_linux_property(cached_cpuinfo_string, query_linaro);

    if(!result.size())
        return {};

    StrUtil::trim(result);

    Set<CString> flags;
    CString::size_type ptr = 0;

    while(true)
    {
        auto sp = result.find(' ', ptr);
        flags.insert(result.substr(ptr, sp-ptr));
        if(sp == result.npos)
            break;
        ptr = sp + 1;
    }

    return flags;
}

SysInfoDef::NetStatusFlags LinuxSysInfo::NetStatus()
{
    static const constexpr cstring net_path = "/sys/class/net/";

    DirFun::DirList list;
    DirFun::Ls(net_path, list);
    bool has_loopback = false;
    for(DirFun::DirItem_t const& dir : list)
    {
        if(dir.name == "lo")
            has_loopback = true;
        else {
            CString operstate_file = Env_::ConcatPath(net_path, dir.name.c_str());
            operstate_file = Env_::ConcatPath(operstate_file.c_str(), "operstate");
            CString state = CResources::Linux::LinuxFileFun::sys_read(operstate_file.c_str());
            if(state == "up")
                return NetStatConnected;
        }
    }
    if(has_loopback)
        return NetStatLocalOnly;
    return NetStatDisconnected;
}

uint32 LinuxSysInfo::CpuCount()
{
    const cstring query = "physical id";

    CPUInfoString();

    uint32 count = 0;
    auto res = cached_cpuinfo_string.find(query);
    while(res != cached_cpuinfo_string.npos)
    {
        res = cached_cpuinfo_string.find(':', res)+1;
        auto end = cached_cpuinfo_string.find('\n', res);

        CString result = cached_cpuinfo_string.substr(res, end);
        StrUtil::trim(result);

        uint32 c = cast_string<uint32>(result)+1;
        count = CMath::max(count,c);

        res = cached_cpuinfo_string.find(query, end);
    }

    return count ? count : 1;
}

uint32 LinuxSysInfo::CoreCount()
{
    const cstring query = "cpu cores";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);

    if(!result.size())
        return ThreadCount();

    StrUtil::trim(result);

    uint32 cores = cast_string<uint32>(result);

    return cores ? cores : 1;
}

HWDeviceInfo LinuxSysInfo::Processor()
{
    const cstring mk_query = "vendor_id";
    const cstring md_query = "model name";
    const cstring fw_query = "microcode";

    const cstring mk_query_linaro = "Hardware";
    const cstring fw_query_linaro = "Revision";

    CPUInfoString();

    CString mk_str = get_linux_property(cached_cpuinfo_string, mk_query);
    if(!mk_str.size())
        mk_str = get_linux_property(cached_cpuinfo_string, mk_query_linaro);
    CString md_str = get_linux_property(cached_cpuinfo_string, md_query);
    CString fw_str = get_linux_property(cached_cpuinfo_string, fw_query);
    if(!fw_str.size())
        fw_str = get_linux_property(cached_cpuinfo_string, fw_query_linaro);

    StrUtil::trim(mk_str);
    StrUtil::trim(md_str);
    StrUtil::trim(fw_str);

    return HWDeviceInfo(mk_str,md_str,fw_str);
}

bigscalar LinuxSysInfo::ProcessorFrequency()
{
    constexpr cstring query = "cpu MHz";

    CPUInfoString();

    CString res = get_linux_property(cached_cpuinfo_string, query);

    if(!res.size())
        return 0.0;

    StrUtil::trim(res);

    return CMath::floor(cast_string<bigscalar>(res))/1000;
}

bool LinuxSysInfo::HasFPU()
{
#if defined(COFFEE_MAEMO)
    const cstring query = "vfpv3";
#else
    const cstring query = "fpu";
#endif

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);
    StrUtil::trim(result);

    return result == "yes";
}

bool LinuxSysInfo::HasFPUExceptions()
{
    const cstring query = "fpu_exception";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);
    StrUtil::trim(result);

    return StrCmp(result.c_str(),"yes");
}

uint64 LinuxSysInfo::ProcessorCacheSize()
{
    const cstring query = "cache size";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);
    StrUtil::trim(result);

    szptr e = result.find(" ");
    result.erase(e,result.size()-e);

    return cast_string<uint64>(result);
}

bool LinuxSysInfo::HasHyperThreading()
{
    Set<CString> flags = CPUFlags();

    auto it = flags.find("ht");

    if(it == flags.end())
        it = flags.find("htt");

    return it != flags.end();
}

HWDeviceInfo LinuxSysInfo::DeviceName()
{
#if defined(COFFEE_MAEMO)
    return HWDeviceInfo("Nokia", "N900", get_kern_name() + (" " + get_kern_ver()));
#else
    static const cstring prod_ver = "/sys/class/dmi/id/product_version";
    static const cstring prod_name = "/sys/class/dmi/id/product_name";

    static const cstring str_gen = "Generic";
    static const cstring str_lin = "Linux";

    /* Assumes the following format for lsb-release:
     *
     * DISTRIB_ID=Ubuntu
     * DISTRIB_RELEASE=16.04
     * DISTRIB_CODENAME=xenial
     * DISTRIB_DESCRIPTION="Ubuntu 16.04.1 LTS"
     *
     */

    cstring manf = str_gen;
    cstring prod = str_lin;

    CString manufac = CResources::Linux::LinuxFileFun::sys_read("/sys/class/dmi/id/sys_vendor");
    cstring prod_src = prod_name;
    if(manufac == "LENOVO")
        prod_src = prod_ver;
    CString product = CResources::Linux::LinuxFileFun::sys_read(prod_ver);

    if(manufac.size() > 0)
        manf = manufac.c_str();
    if(product.size() > 0)
        prod = product.c_str();

    return HWDeviceInfo(manf, prod, get_kern_name() + (" " + get_kern_ver()));
#endif
}

PowerInfoDef::Temp LinuxPowerInfo::CpuTemperature()
{
    static const constexpr cstring thermal_class = "/sys/class/thermal";
    Temp out = {};

    DirFun::DirList lst;
    CString tmp,tmp2;
    if(DirFun::Ls(thermal_class,lst))
    {
        for(auto e : lst)
            if(e.name != "." && e.name != "..")
            {
                tmp = Env_::ConcatPath(thermal_class,e.name.c_str());
                tmp2 = Env_::ConcatPath(tmp.c_str(),"temp");

                if(FileFun::Exists(tmp2.c_str()))
                {
                    CString temp = FileFun::sys_read(tmp2.c_str());
                    out.current = cast_string<scalar>(temp) / 1000;
                    break;
                }
            }
    }

    return out;
}

}
}
}
