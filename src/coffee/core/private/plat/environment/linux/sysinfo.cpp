#include <coffee/core/plat/environment/linux/sysinfo.h>

#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/plat/plat_file.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/string_casting.h>
#include <coffee/core/types/tdef/stltypes.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace Environment {
namespace Linux {

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
#ifndef COFFEE_LOWFAT
    CString version = LFileFun::sys_read("/etc/lsb-release");
    cstring desc    = str::find(version.c_str(), "DISTRIB_DESCRIPTION");
    if(desc && (desc = str::find(desc, '=') + 1) &&
       (desc = str::find(desc, '"') + 1))
    {
        cstring end = str::find(desc, '"');
        if(end || (end = str::find(desc, '\0')))
        {
            CString desc_std(desc, end - desc);
            return desc_std;
        }
    }
#endif
    return {};
}

static CString get_kern_ver()
{
#ifndef COFFEE_LOWFAT
    utsname d;
    if(uname(&d) != 0)
        return "?";
    else
        return d.release;
#else
    return {};
#endif
}

static CString get_linux_property(CString const& source, cstring query)
{
#ifndef COFFEE_LOWFAT
    auto res = source.find(query);

    if(res == source.npos)
        return {};

    res      = source.find(":", res) + 1;
    auto end = source.find_first_of("\n", res);

    return source.substr(res, end - res);
#else
    return {};
#endif
}

CString get_kern_name()
{
    utsname d;
    if(uname(&d) != 0)
        return "?";
    else
        return d.sysname;
}

CString get_kern_arch()
{
    utsname d;
    if(uname(&d) != 0)
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
    CString input = LFileFun::sys_read("/sys/class/dmi/id/chassis_type");

    i32 chassis_type = cast_string<i32>(input);

    switch(chassis_type)
    {
    case 0x1:
    case 0x2:
    case 0x19:
        return PlatformData::DeviceUnknown;
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
        return PlatformData::DeviceDesktop;
    case 0x8:
    case 0x9:
    case 0xa:
    case 0xc:
    case 0xe:
    case 0xf:
        return PlatformData::DeviceLaptop;
    case 0xb:
        return PlatformData::DeviceTablet;
    case 0xd:
        return PlatformData::DeviceAllInOne;
    case 0x11:
    case 0x17:
    case 0x1c:
    case 0x1d:
        return PlatformData::DeviceServer;
    case 0x10:
    case 0x18:
        return PlatformData::DeviceIOT;
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

void LinuxSysInfo::FreeCPUInfoString()
{
    cached_cpuinfo_string.resize(0);
}

CString LinuxSysInfo::CPUInfoString(bool force)
{
    /* We don't want to read it tons of times */
    if(!cached_cpuinfo_string.empty() && !force)
        return cached_cpuinfo_string;

    //        C_PERFWARN(__FILE__,__LINE__,"Reading from /proc/cpuinfo!");

    CString data = LFileFun::sys_read("/proc/cpuinfo");

    cached_cpuinfo_string = data;
    atexit(FreeCPUInfoString);

    return data;
}

Set<CString> LinuxSysInfo::CPUFlags()
{
#ifndef COFFEE_LOWFAT
    const cstring query_linaro = "Features";
    const cstring query        = "flags";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);

    if(!result.size())
        result = get_linux_property(cached_cpuinfo_string, query_linaro);

    if(!result.size())
        return Set<CString>();

    str::trim::both(result);

    Set<CString>       flags;
    CString::size_type ptr = 0;

    while(true)
    {
        auto sp = result.find(' ', ptr);
        if(sp == CString::npos)
            return flags;
        flags.insert(result.substr(ptr, sp - ptr));
        if(sp == result.npos)
            break;
        ptr = sp + 1;
    }

    return flags;
#else
    return {};
#endif
}

SysInfoDef::NetStatusFlags LinuxSysInfo::NetStatus()
{
#ifndef COFFEE_LOWFAT
    static const constexpr cstring net_path = "/sys/class/net/";

    DirFun::DirList list;

    file_error ec;

    DirFun::Ls(MkUrl(net_path, RSCA::SystemFile), list, ec);

    bool has_loopback = false;
    for(DirFun::DirItem_t const& dir : list)
    {
        if(dir.name == "lo")
            has_loopback = true;
        else
        {
            CString operstate_file =
                Env::ConcatPath(net_path, dir.name.c_str());
            operstate_file =
                Env::ConcatPath(operstate_file.c_str(), "operstate");
            CString state = LFileFun::sys_read(operstate_file.c_str());
            if(state == "up")
                return NetStatConnected;
        }
    }
    if(has_loopback)
        return NetStatLocalOnly;
#endif
    return NetStatDisconnected;
}

u32 LinuxSysInfo::CpuCount()
{
#ifndef COFFEE_LOWFAT
    const cstring query = "physical id";

    CPUInfoString();

    u32  count = 0;
    auto res   = cached_cpuinfo_string.find(query);
    while(res != cached_cpuinfo_string.npos)
    {
        res      = cached_cpuinfo_string.find(':', res) + 1;
        auto end = cached_cpuinfo_string.find('\n', res);

        CString result = cached_cpuinfo_string.substr(res, end - res);
        str::trim::both(result);

        u32 c = cast_string<u32>(result) + 1;
        count = CMath::max(count, c);

        res = cached_cpuinfo_string.find(query, end);
    }

    return count ? count : 1;
#else
    return ThreadCount();
#endif
}

u32 LinuxSysInfo::CoreCount()
{
#ifndef COFFEE_LOWFAT
    const cstring query = "cpu cores";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);

    if(!result.size())
    {
    /* Some Android devices are bad at reporting CPU cores
     *  using the hardware_concurrency API. For this reason,
     *  we default to NPROC and hope that phones won't have
     *  multi-socket setups anytime soon. */
#if defined(COFFEE_ANDROID)
        auto procs = sysconf(_SC_NPROCESSORS_ONLN);
#else
        return C_FCAST<u32>(ThreadCount());
#endif
    }

    str::trim::both(result);

    u32 cores = cast_string<u32>(result);

    return cores ? cores : 1;
#else
    return ThreadCount();
#endif
}

u64 LinuxSysInfo::CachedMemory()
{
#ifndef COFFEE_LOWFAT
    CString data = LFileFun::sys_read("/proc/meminfo");

    auto idx = data.find("\nCached:");

    if(idx != data.npos)
    {
        idx      = data.find(":", idx) + 1;
        auto end = data.find("\n", idx);

        auto mem = data.substr(idx, end - idx - 2);

        u64 count = cast_string<u64>(str::trim::both(mem));

        return count * 1000;
    }
#endif

    return 0;
}

HWDeviceInfo LinuxSysInfo::Processor()
{
#ifndef COFFEE_LOWFAT
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

    str::trim::both(mk_str);
    str::trim::both(md_str);
    str::trim::both(fw_str);

    return HWDeviceInfo(mk_str, md_str, fw_str);
#else
    return {};
#endif
}

Vector<bigscalar> LinuxSysInfo::ProcessorFrequencies()
{
#ifndef COFFEE_LOWFAT
    static const constexpr struct
    {
        cstring prefix;
        cstring suffix;
    } root_paths[3] = {
        {"/sys/bus/cpu/devices", "cpufreq/scaling_max_freq"},
        {"/sys/devices/system/cpu", "cpufreq/scaling_max_freq"},
        {"/sys/devices/system/cpu", "cpufreq/cpuinfo_max_freq"},
    };

    for(size_t i = 0; i < 3; i++)
    {
        file_error ec;
        Url        p = MkUrl(root_paths[i].prefix, RSCA::SystemFile);

        DirFun::DirList cpus;
        DirFun::Ls(p, cpus, ec);

        if(cpus.size() == 0)
            continue;

        Vector<bigscalar> freqs;
        freqs.reserve(cpus.size());

        for(DirFun::DirItem_t const& e : cpus)
        {
            if(e.name.substr(0, 3) != "cpu")
                continue;

            Url cpu =
                p + Path::Mk(e.name.c_str()) + Path::Mk(root_paths[i].suffix);
            CString tmp = LFileFun::sys_read((*cpu).c_str());
            tmp         = tmp.substr(0, tmp.find('\n'));
            if(tmp.size())
                freqs.push_back(cast_string<bigscalar>(tmp) / 1000000.0);
        }

        if(freqs.size() == 0)
            continue;

        return freqs;
    }
#endif

    return {};
}

bigscalar LinuxSysInfo::ProcessorFrequency()
{
#ifndef COFFEE_LOWFAT
#if defined(__arm__)
    /* We assume that ARM platforms use Linaro-derived kernels,
     *  this applies to IoT devices and Androids. */

    auto      freqs = ProcessorFrequencies();
    bigscalar maxf  = 0.0;
    for(auto f : freqs)
        maxf = CMath::max(maxf, f);

    return maxf;
#else

    constexpr cstring query = "cpu MHz";

    CPUInfoString();

    CString res = get_linux_property(cached_cpuinfo_string, query);

    if(!res.size())
        return 0.0;

    str::trim::both(res);

    return CMath::floor(cast_string<bigscalar>(res)) / 1000;
#endif
#else
    return 0.0;
#endif
}

bool LinuxSysInfo::HasFPU()
{
#ifndef COFFEE_LOWFAT
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
    str::trim::both(result);

#if defined(COFFEE_MAEMO) || defined(COFFEE_ANDROID)
    if(result.size() == 0)
    {
#if defined(COFFEE_MAEMO)
        CString result = get_linux_property(cached_cpuinfo_string, query);
#else
        CString result = get_linux_property(cached_cpuinfo_string, query2);
#endif
        str::trim::both(result);
    }

    return result.size() > 0;
#else
    return result == "yes";
#endif
#else
    return false;
#endif
}

bool LinuxSysInfo::HasFPUExceptions()
{
#ifndef COFFEE_LOWFAT
    const cstring query = "fpu_exception";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);
    str::trim::both(result);

    return str::cmp(result.c_str(), "yes");
#else
    return false;
#endif
}

u64 LinuxSysInfo::ProcessorCacheSize()
{
#ifndef COFFEE_LOWFAT
    const cstring query = "cache size";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);
    str::trim::both(result);

    szptr e = result.find(" ");
    if(e == CString::npos)
        return 0;
    result.erase(e, result.size() - e);

    return cast_string<u64>(result);
#else
    return 0;
#endif
}

bool LinuxSysInfo::HasHyperThreading()
{
#ifndef COFFEE_LOWFAT
    Set<CString> flags = CPUFlags();

    auto it = flags.find("ht");

    if(it == flags.end())
        it = flags.find("htt");

    return it != flags.end();
#else
    return false;
#endif
}

HWDeviceInfo LinuxSysInfo::DeviceName()
{
#ifndef COFFEE_LOWFAT
#if defined(COFFEE_MAEMO)
    return HWDeviceInfo(
        "Nokia", "N900", get_kern_name() + (" " + get_kern_ver()));
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
#else
    return {};
#endif
}

HWDeviceInfo LinuxSysInfo::Motherboard()
{
#ifndef COFFEE_LOWFAT
#if defined(COFFEE_MAEMO)
    return HWDeviceInfo("Nokia", "RX-51", "0x0");
#else
    static const cstring mb_manuf = DMI_PATH "/board_vendor";
    static const cstring mb_model = DMI_PATH "/board_name";
    static const cstring mb_version = DMI_PATH "/bios_version";

    CString manuf = LFileFun::sys_read(mb_manuf);
    CString model = LFileFun::sys_read(mb_model);
    CString version = LFileFun::sys_read(mb_version);

    return HWDeviceInfo(manuf, model, version);
#endif
#else
    return {};
#endif
}

HWDeviceInfo LinuxSysInfo::Chassis()
{
#ifndef COFFEE_LOWFAT
    static const cstring ch_manuf   = DMI_PATH "/chassis_vendor";
    static const cstring ch_model   = DMI_PATH "/chassis_name";
    static const cstring ch_version = DMI_PATH "/chassis_version";

    CString manuf   = LFileFun::sys_read(ch_manuf);
    CString model   = LFileFun::sys_read(ch_model);
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
#else
    return {};
#endif
}

HWDeviceInfo LinuxSysInfo::BIOS()
{
#ifndef COFFEE_LOWFAT
    static const cstring bios_manuf   = DMI_PATH "/bios_vendor";
    static const cstring bios_name    = DMI_PATH "/board_version";
    static const cstring bios_version = DMI_PATH "/bios_version";

    CString manuf   = LFileFun::sys_read(bios_manuf);
    CString name    = LFileFun::sys_read(bios_name);
    CString version = LFileFun::sys_read(bios_version);

    static const cstring str_gen = "Generic";
    static const cstring str_lin = "BIOS";

    cstring manuf_c = str_gen;
    cstring name_c  = str_lin;
    cstring versn_c = "0x0";

    if(manuf.size() && manuf != invalid_info_string)
        manuf_c = manuf.c_str();
    if(name.size() && name != invalid_info_string)
        name_c = name.c_str();
    if(version.size() && version != invalid_info_string)
        versn_c = version.c_str();

    return HWDeviceInfo(manuf_c, name_c, versn_c);
#else
    return {};
#endif
}

#if !defined(COFFEE_ANDROID)
PowerInfoDef::Temp LinuxPowerInfo::CpuTemperature()
{
#ifndef COFFEE_LOWFAT
    static const constexpr cstring thermal_class = "/sys/class/thermal";
    static const constexpr cstring hwmon_class   = "/sys/class/hwmon";
    Temp                           out           = {};

    /* First, look for thermal_zone* units */
    DirFun::DirList lst;
    Path            tmp, tmp2;
    Url             base = MkUrl("", RSCA::SystemFile);

    file_error ec;

    if(DirFun::Ls(MkUrl(thermal_class, RSCA::SystemFile), lst, ec))
    {
        for(auto e : lst)
            if(e.name != "." && e.name != "..")
            {
                tmp = ((Path{} + thermal_class) + e.name.c_str()) + "temp";

                if(LFileFun::Exists(base + tmp, ec))
                {
                    CString temp =
                        LFileFun::sys_read(tmp.internUrl.c_str(), ec);
                    out.current = cast_string<scalar>(temp) / 1000;
                    break;
                }
            }
    }

    lst  = {};
    tmp  = Path::Mk("");
    tmp2 = Path::Mk("");

    /* hwmon* units */
    if(DirFun::Ls(MkUrl(hwmon_class, RSCA::SystemFile), lst, ec))
    {
        DirFun::DirList lst2;
        Url             path = MkUrl("", RSCA::SystemFile);
        for(auto e : lst)
        {
            tmp       = Path{hwmon_class};
            Url path2 = path + (tmp + e.name.c_str());
            if(DirFun::Ls(path2, lst2, ec))
            {
                for(auto e2 : lst2)
                {
                    auto prefix = e2.name.substr(0, 4);
                    auto suffix = e2.name.substr(e2.name.size() - 5, 5);
                    if(prefix != "temp" && suffix != "input")
                        continue;

                    Url path2 = path + Path{e2.name.c_str()};

                    CString temp_s =
                        LFileFun::sys_read(path2.internUrl.c_str(), ec);

                    out.current = cast_string<scalar>(temp_s);
                }
            }
        }
    }

    return out;
#else
    return {};
#endif
}
#endif

} // namespace Linux
} // namespace Environment
} // namespace Coffee
