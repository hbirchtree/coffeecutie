#include <coffee/core/plat/environment/linux/sysinfo.h>
#include <coffee/core/plat/file/linux/file.h>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/string_casting.h>
#include <coffee/core/CDebug>

#include <coffee/core/platform_data.h>

namespace Coffee{
namespace Environment{
namespace Linux{

CString LinuxSysInfo::cached_cpuinfo_string;

#define DMI_PATH "/sys/class/dmi/id/"
static const constexpr cstring invalid_info_string = "To Be Filled By O.E.M.";

using DirFun = CResources::Linux::LinuxDirFun;

using LFileFun = CResources::Linux::LinuxFileFun;

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
    CString version = LFileFun::sys_read("/etc/lsb-release");
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
#if defined(COFFEE_MAEMO)
    return PlatformData::DevicePhone;
#elif defined(COFFEE_RASPBERRYPI)
    return PlatformData::DeviceIOT;
#endif

    CString input = LFileFun::sys_read(
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

void  LinuxSysInfo::FreeCPUInfoString()
{
    cached_cpuinfo_string.resize(0);
}

CString LinuxSysInfo::CPUInfoString(bool force)
{
    /* We don't want to read it tons of times */
    if(!cached_cpuinfo_string.empty()&&!force)
        return cached_cpuinfo_string;

    //        C_PERFWARN(__FILE__,__LINE__,"Reading from /proc/cpuinfo!");

    CString data = LFileFun::sys_read("/proc/cpuinfo");

    cached_cpuinfo_string = data;
    atexit(FreeCPUInfoString);

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

    DirFun::Ls(MkUrl(net_path,
                     ResourceAccess::SpecifyStorage
                     |ResourceAccess::SystemFile),
               list);

    bool has_loopback = false;
    for(DirFun::DirItem_t const& dir : list)
    {
        if(dir.name == "lo")
            has_loopback = true;
        else {
            CString operstate_file = Env::ConcatPath(net_path, dir.name.c_str());
            operstate_file = Env::ConcatPath(operstate_file.c_str(), "operstate");
            CString state = LFileFun::sys_read(operstate_file.c_str());
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
    CString md_str = get_linux_property(cached_cpuinfo_string, md_query);
    if(!md_str.size())
        md_str = get_linux_property(cached_cpuinfo_string, mk_query_linaro);
    CString fw_str = get_linux_property(cached_cpuinfo_string, fw_query);
    if(!fw_str.size())
        fw_str = get_linux_property(cached_cpuinfo_string, fw_query_linaro);

    StrUtil::trim(mk_str);
    StrUtil::trim(md_str);
    StrUtil::trim(fw_str);

    return HWDeviceInfo(mk_str,md_str,fw_str);
}

Vector<bigscalar> LinuxSysInfo::ProcessorFrequencies()
{
    Url p = MkUrl("/sys/bus/cpu/devices", RSCA::SystemFile);

    int fd = open("/sys/bus/cpu/devices/", O_RDONLY|O_DIRECTORY);

    DirFun::DirList cpus;
    DirFun::Ls(p, cpus);

    cDebug("Listing processors: {1} {0}", cpus.size(), *p, fd);

    Vector<bigscalar> freqs;
    freqs.reserve(cpus.size());

    for(DirFun::DirItem_t const& e : cpus)
    {
        Url cpu = p
                + Path::Mk(e.name.c_str())
                + Path::Mk("cpufreq/scaling_max_freq");
        cDebug("Reading {0}", *cpu);
        CString tmp = LFileFun::sys_read((*cpu).c_str());
        if(tmp.size())
            freqs.push_back(cast_string<bigscalar>(tmp) / 1000000.0);
    }

    return freqs;
}

bigscalar LinuxSysInfo::ProcessorFrequency()
{
#if defined(__arm__)
    /* We assume that ARM platforms use Linaro-derived kernels,
     *  this applies to IoT devices and Androids. */

    auto freqs = ProcessorFrequencies();
    bigscalar maxf = 0.0;
    for(auto f : freqs)
        maxf = CMath::max(maxf, f);

    return maxf;
#else

    constexpr cstring query = "cpu MHz";

    CPUInfoString();

    CString res = get_linux_property(cached_cpuinfo_string, query);

    if(!res.size())
        return 0.0;

    StrUtil::trim(res);

    return CMath::floor(cast_string<bigscalar>(res))/1000;
#endif
}

bool LinuxSysInfo::HasFPU()
{
#if defined(COFFEE_MAEMO)
    const cstring query = "vfpv3";
#elif defined(COFFEE_ANDROID)
    const cstring query = "vfpv3";
    const cstring query2 = "vfpv4";
#else
    const cstring query = "fpu";
#endif

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);
    StrUtil::trim(result);

#if defined(COFFEE_MAEMO) || defined(COFFEE_ANDROID)
    if(result.size() == 0)
    {
        CString result = get_linux_property(cached_cpuinfo_string, query2);
        StrUtil::trim(result);
    }

    return result.size() > 0;
#else
    return result == "yes";
#endif
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
    static const cstring prod_ver = DMI_PATH "/product_version";
    static const cstring prod_name = DMI_PATH "/product_name";

    static const cstring str_gen = "Generic";
    static const cstring str_lin = "Device";

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

    CString manufac = LFileFun::sys_read(DMI_PATH "/sys_vendor");
    cstring prod_src = prod_name;
    if(manufac == "LENOVO")
        prod_src = prod_ver;
    CString product = LFileFun::sys_read(prod_ver);

    if(manufac.size() > 0 && manufac != invalid_info_string)
        manf = manufac.c_str();
    if(product.size() > 0 && manufac != invalid_info_string)
        prod = product.c_str();

    return HWDeviceInfo(manf, prod, get_kern_name() + (" " + get_kern_ver()));
#endif
}

HWDeviceInfo LinuxSysInfo::Motherboard()
{
#if defined(COFFEE_MAEMO)
    return HWDeviceInfo("Nokia", "RX-51", "0x0");
#else
    static const cstring mb_manuf = DMI_PATH "/board_vendor";
    static const cstring mb_model = DMI_PATH "/board_name";
    static const cstring mb_version = DMI_PATH "/board_version";

    CString manuf = LFileFun::sys_read(mb_manuf);
    CString model = LFileFun::sys_read(mb_model);
    CString version = LFileFun::sys_read(mb_version);

    return HWDeviceInfo(manuf, model,  version);
#endif
}

HWDeviceInfo LinuxSysInfo::Chassis()
{
    static const cstring ch_manuf = DMI_PATH "/chassis_vendor";
    static const cstring ch_model = DMI_PATH "/chassis_name";
    static const cstring ch_version = DMI_PATH "/chassis_version";

    CString manuf = LFileFun::sys_read(ch_manuf);
    CString model = LFileFun::sys_read(ch_model);
    CString version = LFileFun::sys_read(ch_version);

    static const cstring str_gen = "Generic";
    static const cstring str_lin = "Chassis";

    cstring manuf_c = str_gen;
    cstring model_c = str_lin;
    cstring versn_c = "0x0";

    if(manuf.size() && manuf != invalid_info_string)
        manuf_c = manuf.c_str();
    if(model.size() && model != invalid_info_string)
        model_c = model.c_str();
    if(version.size() && version != invalid_info_string)
        versn_c = version.c_str();

    return HWDeviceInfo(manuf_c, model_c, versn_c);
}

HWDeviceInfo LinuxSysInfo::BIOS()
{
    static const cstring bios_manuf = DMI_PATH "/bios_vendor";
    static const cstring bios_name = DMI_PATH "/bios_name";
    static const cstring bios_version = DMI_PATH "/bios_version";

    CString manuf = LFileFun::sys_read(bios_manuf);
    CString name = LFileFun::sys_read(bios_name);
    CString version = LFileFun::sys_read(bios_version);

    static const cstring str_gen = "Generic";
    static const cstring str_lin = "BIOS";

    cstring manuf_c = str_gen;
    cstring name_c = str_lin;
    cstring versn_c = "0x0";

    if(manuf.size() && manuf != invalid_info_string)
        manuf_c = manuf.c_str();
    if(name.size() && name != invalid_info_string)
        name_c = name.c_str();
    if(version.size() && version != invalid_info_string)
        versn_c = version.c_str();

    return HWDeviceInfo(manuf_c, name_c, versn_c);
}

#if !defined(COFFEE_ANDROID)
PowerInfoDef::Temp LinuxPowerInfo::CpuTemperature()
{
    static const constexpr cstring thermal_class = "/sys/class/thermal";
    Temp out = {};

    DirFun::DirList lst;
    Path tmp,tmp2;
    if(DirFun::Ls(MkUrl(thermal_class), lst))
    {
        for(auto e : lst)
            if(e.name != "." && e.name != "..")
            {
                tmp = ((tmp + thermal_class) + e.name.c_str()) + "temp";

                if(LFileFun::Exists(Url() + tmp))
                {
                    CString temp = LFileFun::sys_read(tmp.internUrl.c_str());
                    out.current = cast_string<scalar>(temp) / 1000;
                    break;
                }
            }
    }

    return out;
}
#endif

}
}
}
