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

inline std::vector<u16> online_cores()
{
    using namespace url::constructors;

    if(auto content = read_sysfs("/sys/devices/system/cpu/online"_sys);
       content.has_error())
        return {};
    else
    {
        using libc::str::from_string;
        using stl_types::Range;
        using stl_types::str::split::spliterator;

        auto data = content.value();
        auto view = std::string_view(data.data(), data.size() - 1);
        view      = view.substr(0, view.find('\n'));
        auto             listings = spliterator(view, ',');
        std::vector<u16> out;
        do
        {
            auto id = *listings;
            if(auto split = id.find('-'); split != std::string::npos)
            {
                auto first = from_string<u16>(id.substr(0, split).data());
                auto end   = from_string<u16>(id.substr(split + 1).data());
                for(auto i : Range<>(end - first + 1))
                    out.push_back(first + i);
            } else
                out.push_back(from_string<u16>(id.data()));
        } while(++listings != spliterator<char>());
        return out;
    }
}

inline void foreach_cpuinfo(stl_types::Function<bool(
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
        auto             it      = lines.value();
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

    std::set<std::string> die_ids;
    for(auto const& id : detail::online_cores())
    {
        if(auto die_id
           = detail::read_cpu(id, Path{"topology/physical_package_id"});
           die_id.has_value())
            die_ids.insert(die_id.value().data());
    }

    return die_ids.size();
}
inline u32 core_count(u32 cpu = 0, [[maybe_unused]] u32 /*node*/ = 0)
{
    using url::Path;

    std::string           selected_cpu = std::to_string(cpu);
    std::set<std::string> core_ids;
    for(auto const& id : detail::online_cores())
    {
        if(auto die_id
           = detail::read_cpu(id, Path{"topology/physical_package_id"});
           die_id.has_value())
        {
            auto die_id_ = die_id.value();
            if(die_id_ != selected_cpu && die_id_ != "-1")
                continue;
        } else
            continue;

        if(auto core_id = detail::read_cpu(id, Path{"topology/core_id"});
           core_id.has_value())
            core_ids.insert(core_id.value());
    }

    return core_ids.size();
}
inline u32 thread_count(u32 cpu = 0, [[maybe_unused]] u32 /*node*/ = 0)
{
    using url::Path;

    std::string             selected_cpu = std::to_string(cpu);
    std::vector<libc_types::u16> thread_ids;
    for(auto const& id : detail::online_cores())
    {
        if(auto die_id
           = detail::read_cpu(id, Path{"topology/physical_package_id"});
           die_id.has_value())
        {
            auto die_id_ = die_id.value();
            if(die_id_ != selected_cpu && die_id_ != "-1")
                continue;
        } else
            continue;

        thread_ids.push_back(id);
    }

    return thread_ids.size();
}

inline std::optional<std::pair<std::string, std::string>> model(
    u32 cpu = 0, [[maybe_unused]] u32 /*node*/ = 0)
{
    using namespace url::constructors;
    using namespace stl_types::str;

    std::string vendor, model;
    std::string implementer, variant, part;
    detail::foreach_cpuinfo([&](std::string_view const& physical,
                                std::string_view const&,
                                std::string_view const& key,
                                std::string_view const& value) {
        if(physical == std::to_string(cpu).c_str())
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

template<typename T>
using topological_map = std::map<i32, std::map<u32, T>>;

inline topological_map<u32> topo_frequency()
{
    using namespace std::string_literals;
    using url::Path;

    topological_map<u32> freqs;
    for(auto const& id : detail::online_cores())
    {
        auto die_id
            = detail::read_cpu(id, Path{"topology/physical_package_id"});
        auto core_id   = detail::read_cpu(id, Path{"topology/core_id"});
        auto frequency = detail::read_cpu(id, Path{"cpufreq/cpuinfo_max_freq"});

        if(core_id.has_error() || frequency.has_error())
            continue;

        auto die_id_ = libc::str::from_string<i32>(
            (die_id && die_id.value() != "-1" ? die_id.value() : "0"s).data());
        auto core_id_ = libc::str::from_string<u32>(core_id.value().data());
        freqs[die_id_][core_id_]
            = libc::str::from_string<u32>(frequency.value().data());
    }

    return freqs;
}

inline u32 frequency(bool current = false, u32 cpu = 0, u32 /*node*/ = 0)
{
    using url::Path;
    auto select_id = std::to_string(cpu);
    auto freq_path = current ? "scaling_cur_freq" : "cpuinfo_max_freq";
    for(auto const& id : detail::online_cores())
    {
        auto cpu_id
            = detail::read_cpu(id, Path{"topology/physical_package_id"});

        if(cpu_id.has_error()
           || (cpu_id.value() != select_id && cpu_id.value() != "-1"))
            continue;
        auto freq = detail::read_cpu(id, Path{"cpufreq"} / freq_path);
        if(freq.has_error())
            continue;
        return libc::str::from_string<u32>(freq.value().data());
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
    if(getrusage(RUSAGE_SELF, &usage) != -1)
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
        if(auto content
           = detail::read_sysfs(url::Path{dmi_root} / "product_version"))
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
    if(auto content
       = detail::read_sysfs(url::Path{dmi_root} / "chassis_vendor"))
        vendor = content.value();
    if(auto content
       = detail::read_sysfs(url::Path{dmi_root} / "chassis_version"))
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
        auto itype
            = libc::str::from_string<libc_types::u32>(type.value().data());
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
