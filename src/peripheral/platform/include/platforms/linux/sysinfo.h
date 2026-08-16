#pragma once

#include <peripherals/identify/system.h>
#include <peripherals/stl/range.h>
#include <peripherals/stl/string/trim.h>

#include <platforms/base/device_variant.h>
#include <platforms/libc/rdwrio.h>
#include <platforms/posix/fsio.h>
#include <platforms/posix/rdwrio.h>

#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <dirent.h>

#include "sysinfo_cpumap.h"

namespace platform::info::proc::linux_::detail {
using libc_types::u32;
using std::set;
using std::string;

using libc_types::u16;
using stl_types::failure;
using stl_types::success;

stl_types::result<std::string, file::posix::posix_error> read_sysfs(
    url::Path const& file);

std::vector<std::string> read_dt_strings(url::Path const& file);

inline auto read_cpu(u32 id, url::Path const& path)
{
    const auto cpu_id = "cpu" + std::to_string(id);
    return read_sysfs(url::Path{"/sys/devices/system/cpu"} / cpu_id / path);
}

inline auto read_cpu(std::string_view const& id, url::Path const& path)
{
    const auto cpu_id = "cpu" + std::string(id.begin(), id.end());
    return read_sysfs(url::Path{"/sys/devices/system/cpu"} / cpu_id / path);
}

using cpu_hierarchy_t =
    std::map<u16, std::map<u16, std::map<u16, std::vector<u16>>>>;

cpu_hierarchy_t online_cpus();

void foreach_cpuinfo(
    stl_types::Function<bool(
        std::string_view const& physical,
        std::string_view const& cpu,
        std::string_view const& key,
        std::string_view const& value)>&& pred);

} // namespace platform::info::proc::linux_::detail

namespace platform::info::proc::linux_ {
namespace detail {
using namespace proc::linux_::detail;
}

using libc_types::i32;
using libc_types::u32;

inline u32 node_count()
{
    return 1;
}

inline u32 cpu_count()
{
    using url::Path;

    u32 cpu_count{0};
    for(auto const& [phys, clusters] : detail::online_cpus())
        cpu_count += clusters.size();
    return cpu_count;
}

inline u32 core_count(u32 cpu = 0, [[maybe_unused]] u32 /*node*/ = 0)
{
    u32 cpu_idx{0};
    for(auto const& [phys, clusters] : detail::online_cpus())
    {
        for(auto const& [cluster, cores] : clusters)
        {
            if(cpu_idx == cpu)
                return cores.size();
            cpu_idx++;
        }
    }
    return 0;
}

inline u32 thread_count(u32 cpu = 0, [[maybe_unused]] u32 /*node*/ = 0)
{
    u32 cpu_idx{0};
    for(auto const& [phys, clusters] : detail::online_cpus())
    {
        for(auto const& [cluster, cores] : clusters)
        {
            if(cpu_idx == cpu)
            {
                u32 thread_count{0};
                for(auto const& [core, cpus] : cores)
                    thread_count += cpus.size();
                return thread_count;
            }
            cpu_idx++;
        }
    }
    return 0;
}

inline std::optional<std::pair<std::string, std::string>> model(
    u32 cpu = 0, [[maybe_unused]] u32 /*node*/ = 0)
{
    using namespace url::constructors;
    using namespace stl_types::str;

    std::string vendor, model;
    std::string implementer, part;

    cpu = [&]() -> u32 {
        u32 cpu_i{0};
        for(auto const& [phys, clusters] : detail::online_cpus())
            for(auto const& [cluster, cores] : clusters)
            {
                if(cpu_i == cpu)
                    for(auto const& [core, cpus] : cores)
                        return cpus.at(0);
                cpu_i++;
            }
        return cpu;
    }();

    detail::foreach_cpuinfo([&](std::string_view const&,
                                std::string_view const& cpui,
                                std::string_view const& key,
                                std::string_view const& value) {
        if(cpui == std::to_string(cpu).c_str())
        {
            if(key == "model name")
                model = trim::both(value);
            else if(key == "vendor_id")
                vendor = trim::both(value);
            else if(key == "CPU implementer")
                implementer = trim::both(value);
            else if(key == "CPU part")
                part = trim::both(value);
        }
        return false;
    });

    if(implementer.empty() && part.empty())
    {
        // Special-case for older ARM kernels
        // Where CPU implementer/part is not listed under the given processor
        // This happens on Linux 3.0.x at least
        detail::foreach_cpuinfo([&](std::string_view const&,
                                    std::string_view const&,
                                    std::string_view const& key,
                                    std::string_view const& value) {
            if(key == "CPU implementer")
                implementer = trim::both(value);
            else if(key == "CPU part")
                part = trim::both(value);
            return false;
        });
    }

    if(auto human_implementer = detail::map_cpu_implementer(implementer))
    {
        vendor = human_implementer.value();
        if(auto human_part = detail::map_cpu_part(implementer, part))
            model = human_part.value();
    }
    vendor = vendor.empty() ? implementer : vendor;
    model  = model.empty() ? part : model;

    if(model.empty())
        return std::nullopt;
    return std::make_optional(
        std::pair<std::string, std::string>(vendor, model));
}

/* Returns total CPU time consumed by this process in nanoseconds
 * (user + system), via CLOCK_PROCESS_CPUTIME_ID. */
inline libc_types::u64 cpu_time()
{
    struct timespec ts;
    if(clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == 0)
        return static_cast<libc_types::u64>(ts.tv_sec) * 1'000'000'000ULL +
               static_cast<libc_types::u64>(ts.tv_nsec);
    return 0;
}

/* Returns a map of TID → thread name from /proc/self/task/<tid>/comm.
 * Names are at most 15 characters (TASK_COMM_LEN - 1).
 * Uses fopen/fgets directly: /proc files report size 0 in stat(), so
 * file::posix::read would allocate a zero-byte buffer and read nothing. */
std::map<u32, std::string> thread_names();

inline libc_types::u64 ticks_per_second()
{
    return static_cast<libc_types::u64>(sysconf(_SC_CLK_TCK));
}

/* Returns a map of TID → accumulated CPU ticks (utime + stime) for every
 * thread in the current process, read from /proc/self/task/<tid>/stat.
 * Fields 14+15 in the stat format: utime and stime, both in clock ticks.
 * Uses fopen/fgets directly for the same reason as thread_names(). */
std::map<u32, libc_types::u64> thread_cpu_ticks();

inline u32 frequency(bool current = false, u32 cpu = 0, u32 core = 0)
{
    using url::Path;
    auto select_id   = std::to_string(cpu);
    auto select_core = std::to_string(core);
    auto freq_path   = current ? "scaling_cur_freq" : "cpuinfo_max_freq";
    u32  cpu_i{0};
    for(auto const& [phys, clusters] : detail::online_cpus())
        for(auto const& [cluster, cores] : clusters)
        {
            if(cpu == cpu_i)
                for(auto const& [core, cpus] : cores)
                {
                    auto freq =
                        detail::read_cpu(cpus[0], Path{"cpufreq"} / freq_path);
                    if(freq.has_error())
                        continue;
                    return libc::str::from_string<u32>(freq.value().data());
                }
            cpu_i++;
        }
    return 0;
}

} // namespace platform::info::proc::linux_

namespace platform::info::memory::linux_ {
namespace detail {
using namespace proc::linux_::detail;
}

inline libc_types::u64 total()
{
    struct sysinfo info;
    if(sysinfo(&info) != -1)
        return info.totalram;
    return 0;
}

inline libc_types::u64 resident()
{
    struct rusage usage;
    if(getrusage(RUSAGE_SELF, &usage) == 0)
        return usage.ru_maxrss * 1024;
    return 0;
}

} // namespace platform::info::memory::linux_

namespace platform::info::os::linux_ {
namespace detail {
using namespace proc::linux_::detail;
}

inline std::optional<std::string> lsb_value(std::string const& key)
{
    using namespace url::constructors;

    if(auto lines = file::libc::read_lines("/etc/lsb-release"_sys);
       lines.has_value())
    {
        auto it = lines.value();
        do
        {
            auto line  = *it;
            auto split = line.find('=');
            if(line.substr(0, split) == key)
            {
                auto value = line.substr(split + 1);
                return std::string(value.begin(), value.end());
            }
        } while(!(++it).empty());
    }
    return std::nullopt;
}

inline std::optional<std::string> name()
{
    using namespace url::constructors;
    if(auto name = lsb_value("DISTRIB_ID"))
        return name;
    else if(file::posix::exists("/etc/debian_version"_sys))
        return "Debian";
    else
        return std::nullopt;
}

inline std::optional<std::string> version()
{
    using namespace url::constructors;
    if(auto version = lsb_value("DISTRIB_RELEASE"))
        return version;
    else if(auto lines = file::libc::read_lines("/etc/debian_version"_sys))
    {
        auto version = *(++lines.value());
        return std::string(version.begin(), version.end());
    } else
        return std::nullopt;
}

} // namespace platform::info::os::linux_

namespace platform::info::device::linux_ {
namespace detail {
using namespace proc::linux_::detail;
}

constexpr std::string_view dmi_root = "/sys/class/dmi/id";

std::optional<std::pair<std::string, std::string>> device();

inline std::optional<std::string> name()
{
    auto dev = device().value_or(std::pair{"<unknown>", "<unknown>"});
    return dev.first + " " + dev.second;
}

inline std::optional<std::pair<std::string, std::string>> motherboard()
{
    using namespace url::constructors;
    std::string vendor, model;
    if(auto content = detail::read_sysfs(url::Path{dmi_root} / "board_vendor"))
        vendor = content.value();
    if(auto content = detail::read_sysfs(url::Path{dmi_root} / "board_name"))
        model = content.value();
    if(auto content = detail::read_sysfs(url::Path{dmi_root} / "board_version"))
        (model += " ") += content.value();

    detail::foreach_cpuinfo([&](std::string_view const&,
                                std::string_view const&,
                                std::string_view const& key,
                                std::string_view const& value) {
        if(key == "Hardware")
        {
            model = value;
            return true;
        }
        return false;
    });

    /* Device-tree platforms (ARM/RISC-V SoCs) have no DMI, and modern arm64
     * kernels no longer emit the /proc/cpuinfo "Hardware" line, so both sources
     * above come up empty. Fall back to the device-tree root `compatible` list:
     * entries run most-specific (board) first to most-generic (SoC) last, each
     * formatted "vendor,part" — e.g. ["pine64,rockpro64", "rockchip,rk3399"].
     * The last entry is the SoC. */
    if(model.empty())
    {
        if(auto compat =
               detail::read_dt_strings("/proc/device-tree/compatible"_sys);
           !compat.empty())
        {
            auto const& soc   = compat.back();
            auto        comma = soc.find(',');
            if(comma != std::string::npos)
            {
                vendor = soc.substr(0, comma);
                model  = soc.substr(comma + 1);
            } else
                model = soc;
        }
    }

    if(model.empty())
        return std::nullopt;
    return std::pair{vendor, model};
}

inline std::optional<std::pair<std::string, std::string>> chassis()
{
    using namespace url::constructors;
    std::string vendor, model = "Chassis";
    if(auto content =
           detail::read_sysfs(url::Path{dmi_root} / "chassis_vendor"))
        vendor = content.value();
    if(auto content =
           detail::read_sysfs(url::Path{dmi_root} / "chassis_version"))
        model = content.value();
    if(model == "Not Available")
        model = {};

    if(vendor.empty())
        return std::nullopt;
    return std::pair{vendor, model};
}

inline DeviceType variant()
{
    using namespace url::constructors;
    if(auto type = detail::read_sysfs("/sys/class/dmi/id/chassis_type"_sys))
    {
        auto itype =
            libc::str::from_string<libc_types::u32>(type.value().data());
        switch(itype)
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
    } else
        return DeviceUnknown;
}

} // namespace platform::info::device::linux_
