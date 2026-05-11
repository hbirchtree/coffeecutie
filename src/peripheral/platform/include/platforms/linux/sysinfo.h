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
#include <unistd.h>

#include "sysinfo_cpumap.h"

namespace platform::info::proc::linux_::detail {
using libc_types::u32;
using std::set;
using std::string;

using libc_types::u16;
using stl_types::failure;
using stl_types::success;

inline stl_types::result<std::string, file::posix::posix_error> read_sysfs(
    url::Path const& file)
{
    using namespace platform::file::posix;
    if(auto fd = open_file(file.url()); fd.has_error())
        return failure(fd.error());
    else
    {
        if(auto content = file::posix::read(fd.value()); content.has_error())
            return failure(content.error());
        else
        {
            auto& data = content.value();
            auto  end  = std::find(data.begin(), data.end(), '\n');
            return success(std::string(data.begin(), end));
        }
    }
}

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

inline cpu_hierarchy_t online_cpus()
{
    using namespace stl_types::str;
    using namespace std::literals::string_view_literals;
    using namespace url::constructors;
    using libc::str::from_string;

    if(auto lines = file::libc::read_lines("/proc/cpuinfo"_sys);
       lines.has_value())
    {
        auto it = lines.value();

        std::vector<std::tuple<u16, u16, u16>> cpus;

        std::string cpu_id  = "0";
        std::string phys_id = "0";
        do
        {
            using libc::str::from_string;

            auto comps = stl_types::str::split::str(*it, ':');
            std::vector<std::string_view> components(
                comps.begin(), comps.end());
            if(components.size() < 2)
                continue;
            auto field_key   = trim::right(components.at(0));
            auto field_value = trim::left(components.at(1));

            std::string core_id;
            if(field_key == "processor"sv)
            {
                cpu_id = field_value;
                if(auto physid = read_cpu(
                       cpu_id, url::Path{"topology/physical_package_id"});
                   physid.has_value() && physid.value().size())
                {
                    phys_id = physid.value();
                    if(phys_id == "-1")
                        phys_id = "0";
                }
                if(auto coreid =
                       read_cpu(cpu_id, url::Path{"topology/core_id"});
                   coreid.has_value() && coreid.value().size())
                {
                    core_id = coreid.value();
                }
                cpus.push_back(
                    std::make_tuple(
                        from_string<u16>(phys_id.c_str()),
                        from_string<u16>(core_id.c_str()),
                        from_string<u16>(cpu_id.c_str())));
            }
        } while(!(++it).empty());
        cpu_hierarchy_t first_pass;
        const auto      get_cluster_id = [](u16 cpuid) -> u16 {
            if(compile_info::architecture == std::string("AMD64"))
                return 0;
            if(auto clusterid_ =
                   read_cpu(cpuid, url::Path{"topology/cluster_id"});
               clusterid_.has_value() && clusterid_.value().size())
                return from_string<u16>(clusterid_.value().c_str());
            return 0;
        };
        for(auto [physid, coreid, cpuid] : cpus)
            first_pass[physid][get_cluster_id(cpuid)][coreid].push_back(cpuid);
        if(first_pass.size() == 1 && first_pass[0].size() != 1)
            return first_pass;
        cpu_hierarchy_t mapped_cpus;
        /* With the full list of CPUs, try to group into clusters based on how
         * many threads each core has.
         * This is the tell-tale sign of P/E cores in newer Intel CPUs */
        for(auto const& [physid, all] : first_pass)
            for(auto const& [_, cores] : all)
            {
                u16                counter{0};
                std::map<u16, u16> count_to_idx;
                for(auto const& [coreid, cpus] : cores)
                {
                    u16 idx{0};
                    u16 curr_count = cpus.size();
                    if(count_to_idx.contains(curr_count))
                        idx = count_to_idx[curr_count];
                    else
                        idx = count_to_idx[curr_count] = counter++;
                    mapped_cpus[physid][idx][coreid] = cpus;
                }
            }
        return mapped_cpus;
    }
    return {};
}

inline void foreach_cpuinfo(
    stl_types::Function<bool(
        std::string_view const& physical,
        std::string_view const& cpu,
        std::string_view const& key,
        std::string_view const& value)>&& pred)
{
    using namespace stl_types::str;
    using namespace std::literals::string_view_literals;
    using namespace url::constructors;

    if(auto lines = file::libc::read_lines("/proc/cpuinfo"_sys);
       lines.has_value())
    {
        auto        it      = lines.value();
        std::string proc_id = "0";
        std::string phys_id = "0";
        do
        {
            auto comps = stl_types::str::split::str(*it, ':');
            std::vector<std::string_view> components(
                comps.begin(), comps.end());
            if(components.size() < 2)
                continue;
            auto field_key   = trim::right(components.at(0));
            auto field_value = trim::left(components.at(1));

            if(field_key == "processor"sv)
            {
                proc_id = field_value;
                if(auto physid = read_cpu(
                       proc_id, url::Path{"topology/physical_package_id"});
                   physid.has_value() && physid.value().size())
                {
                    phys_id = physid.value();
                    if(phys_id == "-1")
                        phys_id = "0";
                }
            }

            if(pred(phys_id, proc_id, field_key, field_value))
                break;
        } while(!(++it).empty());
    }
}

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
    std::string implementer, variant, part;

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
            else if(key == "CPU variant")
                variant = trim::both(value);
            else if(key == "CPU part")
                part = trim::both(value);
        }
        return false;
    });

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
        return usage.ru_idrss + usage.ru_ixrss + usage.ru_isrss;
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

inline std::optional<std::pair<std::string, std::string>> device()
{
    using namespace url::constructors;
#if defined(COFFEE_BEAGLEBONEBLACK)
    return std::pair{"BeagleBone", "Black"};
#else
    std::string vendor, product;
    if(auto content = detail::read_sysfs(url::Path{dmi_root} / "sys_vendor"))
        vendor = content.value();
    if(vendor == "LENOVO")
    {
        if(auto content =
               detail::read_sysfs(url::Path{dmi_root} / "product_version"))
            product = content.value();
    } else if(
        auto content = detail::read_sysfs(url::Path{dmi_root} / "product_name"))
        product = content.value();

    detail::foreach_cpuinfo([&](std::string_view const&,
                                std::string_view const&,
                                std::string_view const& key,
                                std::string_view const& value) {
        if(key == "Model")
        {
            product = value;
            return true;
        }
        return false;
    });

    if(product.starts_with("Raspberry"))
        vendor = "Raspberry Pi";

    if(product.empty())
        return std::nullopt;
    return std::pair{vendor, product};
#endif
}

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
#if defined(COFFEE_BEAGLEBONEBLACK)
        return DeviceIOT;
#else
        return DeviceUnknown;
#endif
}

} // namespace platform::info::device::linux_
