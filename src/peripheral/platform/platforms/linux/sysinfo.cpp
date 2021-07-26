#include <platforms/linux/sysinfo.h>

#include <coffee/core/platform_data.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/types.h>
#include <platforms/environment.h>
#include <platforms/file.h>

namespace platform {
namespace env {
namespace Linux {

Vector<String> SysInfo::cached_cpuinfo_string;

#define DMI_PATH "/sys/class/dmi/id/"
static const constexpr cstring invalid_info_string = "To Be Filled By O.E.M.";

STATICINLINE auto sys_read(String const& file)
{
    using url::constructors::MkUrl;

    if(auto res = platform::file::read_lines(MkUrl(file)); res.has_error())
        return Vector<String>();
    else
        return res.value();
}

STATICINLINE Optional<String> sys_read_str(String const& file)
{
    if(auto lines = sys_read(file); lines.empty())
        return std::nullopt;
    else
        return lines.at(0);
}

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

static void lsb_find_val(cstring& value, CString::size_type& len)
{
    if(!value)
        return;

    value = libc::str::find(value, '=');

    if(!value)
        return;

    value++;

    auto terminator = libc::str::find(value, '\n');
    len = terminator ? C_FCAST<CString::size_type>(terminator - value) : 0;
}

lsb_data get_lsb_release()
{
#ifndef COFFEE_LOWFAT
    auto const& lsb_strings = sys_read("/etc/lsb-release");

    auto dist =
        std::find_if(lsb_strings.begin(), lsb_strings.end(), [](auto const& s) {
            return stl_types::str::find::starts_with<char>(s, "DISTRIB_ID");
        });
    auto rel =
        std::find_if(lsb_strings.begin(), lsb_strings.end(), [](auto const& s) {
            return stl_types::str::find::starts_with<char>(
                s, "DISTRIB_RELEASE");
        });

    lsb_data out = {
        .distribution = dist != lsb_strings.end() ? *dist : String(),
        .release      = rel != lsb_strings.end() ? *rel : String(),
    };
    if(!out.distribution.empty())
        out.distribution =
            out.distribution.substr(out.distribution.find("=") + 1);
    if(!out.release.empty())
        out.release = out.release.substr(out.release.find("=") + 1);

    if(!out.distribution.empty() && !out.release.empty())
        return out;
#endif

    return {};
}

CString get_kern_ver()
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

static String get_linux_property(Vector<String> const& source, cstring query)
{
#ifndef COFFEE_LOWFAT
    auto property =
        std::find_if(source.begin(), source.end(), [query](auto const& s) {
            return stl_types::str::find::starts_with<char>(s, query);
        });

    if(property == source.end())
        return {};

    auto out = property->substr(libc::str::len(query) + 2);
    return str::trim::both(out);
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

info::DeviceType get_device_variant()
{
    using namespace platform::info;

    auto input = sys_read("/sys/class/dmi/id/chassis_type");

    if(input.empty())
        return info::DeviceUnknown;

    i32 chassis_type = cast_string<i32>(input[0]);

    switch(chassis_type)
    {
    case 0x1:
    case 0x2:
    case 0x19:
        return DeviceUnknown;
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
        return DeviceDesktop;
    case 0x8:
    case 0x9:
    case 0xa:
    case 0xc:
    case 0xe:
    case 0xf:
        return DeviceLaptop;
    case 0xb:
        return DeviceTablet;
    case 0xd:
        return DeviceAllInOne;
    case 0x11:
    case 0x17:
    case 0x1c:
    case 0x1d:
        return DeviceServer;
    case 0x10:
    case 0x18:
        return DeviceIOT;
    default:
        return DeviceUnknown;
    }
}

CString SysInfo::GetSystemVersion()
{
    CString tmp = get_lsb_release().release;

    if(tmp.empty())
        tmp = get_kern_ver();

    return tmp;
}

Vector<String> SysInfo::CPUInfoString(bool force)
{
    /* We don't want to read it tons of times */
    if(!cached_cpuinfo_string.empty() && !force)
        return cached_cpuinfo_string;

    auto data = sys_read("/proc/cpuinfo");

    cached_cpuinfo_string = data;

    return data;
}

Set<CString> SysInfo::CPUFlags()
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

SysInfoDef::NetStatusFlags SysInfo::NetStatus()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
    using semantic::RSCA;
    using url::Path;
    using url::constructors::MkUrl;

    static const constexpr cstring net_path = "/sys/class/net/";

    bool has_loopback = false;
    if(auto listing = platform::file::list(MkUrl(net_path, RSCA::SystemFile));
       listing.has_error())
        return SysInfo::NetStatLocalOnly;
    else
        for(auto const& dir : listing.value())
        {
            if(dir.name == "lo")
                has_loopback = true;
            else
            {
                Path operstate_file = Path(net_path) / dir.name / "operstate";
                auto state          = sys_read(operstate_file.internUrl)[0];
                if(state == "up")
                    return NetStatConnected;
            }
        }
    if(has_loopback)
        return NetStatLocalOnly;
#endif
    return NetStatDisconnected;
}

u32 SysInfo::CpuCount()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
    CPUInfoString();

    auto res = get_linux_property(cached_cpuinfo_string, "physical id");
    if(res.empty())
        return 1;

    return cast_string<u32>(res);
#else
    return ThreadCount();
#endif
}

u32 SysInfo::CoreCount()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
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

u64 SysInfo::CachedMemory()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
    auto data = sys_read("/proc/meminfo");
    auto it   = std::find_if(data.begin(), data.end(), [](auto const& s) {
        return str::find::starts_with<char>(s, "Cached:");
      });

    if(it == data.end())
        return 0;
    auto mem = (*it).substr(it->find(':'));
    str::trim::both(mem);
    return cast_string<u64>(mem.substr(0, mem.size() - 2)) * 1000;
#else
    return 0;
#endif
}

info::HardwareDevice SysInfo::Processor()
{
#if !defined(COFFEE_LOWFAT) && \
    (defined(COFFEE_LINUX) || defined(COFFEE_ANDROID))
    const cstring mk_query = "vendor_id";
    const cstring md_query = "model name";
    const cstring fw_query = "microcode";

    const cstring md_query_linaro = "Hardware";
    const cstring fw_query_linaro = "Revision";

    const cstring mk_query_arm = "CPU implementer";
    const cstring fw_query_arm = "CPU revision";

    CPUInfoString();

    CString mk_str = get_linux_property(cached_cpuinfo_string, mk_query);
    if(!mk_str.size())
        mk_str = get_linux_property(cached_cpuinfo_string, mk_query_arm);
    CString md_str = get_linux_property(cached_cpuinfo_string, md_query);
    if(!md_str.size())
        md_str = get_linux_property(cached_cpuinfo_string, md_query_linaro);
    CString fw_str = get_linux_property(cached_cpuinfo_string, fw_query);
    if(!fw_str.size())
        fw_str = get_linux_property(cached_cpuinfo_string, fw_query_linaro);
    if(!fw_str.size())
        fw_str = get_linux_property(cached_cpuinfo_string, fw_query_arm);

    str::trim::both(mk_str);
    str::trim::both(md_str);
    str::trim::both(fw_str);

    return info::HardwareDevice(mk_str, md_str, fw_str);
#else
    return {};
#endif
}

Vector<f64> SysInfo::ProcessorFrequencies(bool current)
{
#if !defined(COFFEE_LOWFAT) && \
    (defined(COFFEE_LINUX) || defined(COFFEE_ANDROID))
    using namespace url::constructors;
    using semantic::RSCA;
    using url::Path;
    using url::Url;

    static const struct
    {
        cstring prefix;
        cstring suffix;
    } root_paths[3] = {
        {"/sys/bus/cpu/devices",
         current ? "cpufreq/scaling_cur_freq" : "cpufreq/scaling_max_freq"},
        {"/sys/devices/system/cpu",
         current ? "cpufreq/scaling_cur_freq" : "cpufreq/scaling_max_freq"},
        {"/sys/devices/system/cpu",
         current ? "cpufreq/scaling_cur_freq" : "cpufreq/scaling_max_freq"},
    };

    for(size_t i = 0; i < 3; i++)
    {
        Path p           = Path(root_paths[i].prefix);
        auto cpu_listing = platform::file::list(MkSysUrl(p));

        if(cpu_listing.has_error())
            continue;
        auto cpus = std::move(cpu_listing.value());

        if(cpus.size() == 0)
            continue;

        Vector<f64> freqs;
        freqs.reserve(cpus.size());

        for(auto const& e : cpus)
        {
            if(e.name.substr(0, 3) != "cpu")
                continue;

            Url  cpu = MkSysUrl(p / e.name / root_paths[i].suffix);
            auto tmp = sys_read_str((*cpu));
            if(tmp)
                freqs.push_back(cast_string<f64>(*tmp) / 1000000.0);
        }

        if(freqs.size() == 0)
            continue;

        return freqs;
    }
#endif

    return {};
}

f64 SysInfo::ProcessorFrequency()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
#if defined(__arm__)
    /* We assume that ARM platforms use Linaro-derived kernels,
     *  this applies to IoT devices and Androids. */

    auto      freqs = ProcessorFrequencies();
    bigscalar maxf  = 0.0;
    for(auto f : freqs)
        maxf = math::max(maxf, f);

    return maxf;
#else

    constexpr cstring query = "cpu MHz";

    CPUInfoString();

    CString res = get_linux_property(cached_cpuinfo_string, query);

    if(!res.size())
        return 0.0;

    str::trim::both(res);

    return math::floor(cast_string<f64>(res)) / 1000;
#endif
#else
    return 0.0;
#endif
}

bool SysInfo::HasFPU()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
#if defined(COFFEE_MAEMO)
    constexpr cstring query = "vfpv3";
#elif defined(COFFEE_ANDROID)
    constexpr cstring query = "vfpv3";
    constexpr cstring query2 = "vfpv4";
#else
    constexpr cstring query = "fpu";
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

bool SysInfo::HasFPUExceptions()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
    constexpr cstring query = "fpu_exception";

    CPUInfoString();

    CString result = get_linux_property(cached_cpuinfo_string, query);
    str::trim::both(result);

    return libc::str::cmp(result.c_str(), "yes");
#else
    return false;
#endif
}

u64 SysInfo::ProcessorCacheSize()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
    constexpr cstring query = "cache size";

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

bool SysInfo::HasHyperThreading()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
    Set<CString> flags = CPUFlags();

    auto it = flags.find("ht");

    if(it == flags.end())
        it = flags.find("htt");

    return it != flags.end();
#else
    return false;
#endif
}

info::HardwareDevice SysInfo::DeviceName()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
#if defined(COFFEE_MAEMO)
    return info::HardwareDevice(
        "Nokia", "N900", get_kern_name() + (" " + get_kern_ver()));
#else
    constexpr cstring prod_ver = DMI_PATH "/product_version";
    constexpr cstring prod_name = DMI_PATH "/product_name";
    constexpr cstring manufacturer = DMI_PATH "/sys_vendor";

    auto manufac = sys_read_str(manufacturer).value_or("Generic");
    auto product = sys_read_str(manufac == "LENOVO" ? prod_ver : prod_name)
                       .value_or("Device");

    return info::HardwareDevice(
        manufac, product, get_kern_name() + (" " + get_kern_ver()));
#endif
#else
    return {};
#endif
}

info::HardwareDevice SysInfo::Motherboard()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
#if defined(COFFEE_MAEMO)
    return info::HardwareDevice("Nokia", "RX-51", "0x0");
#else
    constexpr cstring mb_manuf = DMI_PATH "/board_vendor";
    constexpr cstring mb_model = DMI_PATH "/board_name";
    constexpr cstring mb_version = DMI_PATH "/bios_version";

    auto manuf = sys_read_str(mb_manuf).value_or("Generic");
    auto model = sys_read_str(mb_model).value_or("Motherboard");
    auto version = sys_read_str(mb_version).value_or("0x0");

    return info::HardwareDevice(manuf, model, version);
#endif
#else
    return {};
#endif
}

info::HardwareDevice SysInfo::Chassis()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
    constexpr cstring ch_manuf   = DMI_PATH "/chassis_vendor";
    constexpr cstring ch_model   = DMI_PATH "/chassis_name";
    constexpr cstring ch_version = DMI_PATH "/chassis_version";

    auto manuf   = sys_read_str(ch_manuf).value_or("Generic");
    auto model   = sys_read_str(ch_model).value_or("Chassis");
    auto version = sys_read_str(ch_version).value_or("0x0");

    return info::HardwareDevice(manuf, model, version);
#else
    return {};
#endif
}

info::HardwareDevice SysInfo::BIOS()
{
#if !defined(COFFEE_LOWFAT) && defined(COFFEE_LINUX)
    constexpr cstring bios_manuf   = DMI_PATH "/bios_vendor";
    constexpr cstring bios_name    = DMI_PATH "/board_version";
    constexpr cstring bios_version = DMI_PATH "/bios_version";

    auto manuf   = sys_read_str(bios_manuf).value_or("Generic");
    auto name    = sys_read_str(bios_name).value_or("BIOS");
    auto version = sys_read_str(bios_version).value_or("0x0");

    return info::HardwareDevice(manuf, name, version);
#else
    return {};
#endif
}

#if !defined(COFFEE_ANDROID)
PowerInfoDef::Temp PowerInfo::CpuTemperature()
{
#ifndef COFFEE_LOWFAT
    using namespace url::constructors;
    using semantic::RSCA;
    using url::Path;
    using url::Url;

    constexpr cstring thermal_class = "/sys/class/thermal";
    constexpr cstring hwmon_class   = "/sys/class/hwmon";
    Temp                           out           = {};

    /* First, look for thermal_zone* units */
    if(auto listing = platform::file::list(MkSysUrl(thermal_class));
       listing.has_error())
        return {};
    else
    {
        for(auto e : listing.value())
            if(e.name != "." && e.name != "..")
            {
                Path tmp = Path(thermal_class) / e.name / "temp";

                if(platform::file::exists(MkSysUrl(tmp)))
                {
                    auto temp   = sys_read_str(tmp.internUrl).value_or("0");
                    out.current = cast_string<scalar>(temp) / 1000;
                    break;
                }
            }
    }

    /* hwmon* units */
    if(auto listing = platform::file::list(MkSysUrl(hwmon_class));
       listing.has_error())
        return {};
    else
        for(auto e : listing.value())
        {
            Url path = MkSysUrl(Path(e.name));
            if(auto listing = platform::file::list(path); listing.has_error())
                continue;
            {
                for(auto e2 : listing.value())
                {
                    auto prefix = e2.name.substr(0, 4);
                    auto suffix = e2.name.substr(e2.name.size() - 5, 5);
                    if(prefix != "temp" && suffix != "input")
                        continue;

                    Url path2 = path / Path{e2.name};

                    auto temp = sys_read_str(path2.internUrl).value_or("0");

                    out.current = cast_string<scalar>(temp);
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
} // namespace env
} // namespace platform
