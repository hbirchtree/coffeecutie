#pragma once

#include <peripherals/identify/system.h>
#include <peripherals/stl/string_ops.h>

#include <platforms/base/device_variant.h>
#include <platforms/libc/rdwrio.h>
#include <platforms/posix/fsio.h>
#include <platforms/posix/rdwrio.h>

#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "sysinfo_cpumap.h"

namespace platform::info::proc::linux_::detail {
using libc_types::u32;
using stl_types::Set;
using stl_types::String;
using stl_types::Vector;

using libc_types::u16;
using stl_types::failure;
using stl_types::success;

inline stl_types::result<stl_types::String, file::posix::posix_error>
read_sysfs(url::Url const& file)
{
    using namespace platform::file::posix;
    if(auto fd = open_file(file); fd.has_error())
        return failure(fd.error());
    else
    {
        if(auto content = file::posix::read(fd.value()); content.has_error())
            return failure(content.error());
        else
        {
            auto& data = content.value();
            auto  end  = std::find(data.begin(), data.end(), '\n');
            return success(stl_types::String(data.begin(), end));
        }
    }
}

inline auto read_sysfs(url::Path const& file)
{
    return read_sysfs(url::constructors::MkSysUrl(file));
}

inline auto read_cpu(u32 id, url::Path const& path)
{
    return read_sysfs(url::constructors::MkSysUrl(
        url::Path{"/sys/devices/system/cpu/cpu" + std::to_string(id)} / path));
}

inline auto read_cpu(std::string_view const& id, url::Path const& path)
{
    return read_sysfs(url::constructors::MkSysUrl(
        url::Path{std::string("/sys/devices/system/cpu/cpu") + id.data()} /
        path));
}

inline stl_types::Vector<u16> online_cores()
{
    using namespace url::constructors;

    if(auto content = read_sysfs("/sys/devices/system/cpu/online"_sys);
       content.has_error() || content.value().size() < 2)
        return {};
    else
    {
        using libc::str::from_string;
        using stl_types::Range;
        using stl_types::str::split::spliterator;

        auto data = content.value();
        auto view = std::string_view(data.data(), data.size() - 1);
        view      = view.substr(0, view.find('\n'));
        auto                   listings = spliterator(view, ',');
        stl_types::Vector<u16> out;
        do
        {
            auto id = *listings;
            if(auto split = id.find('-'); split != String::npos)
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
    using stl_types::Vector;

    if(auto lines = file::libc::read_lines("/proc/cpuinfo"_sys);
       lines.has_value())
    {
        auto              it      = lines.value();
        std::string_view  proc_id = "0"sv;
        stl_types::String phys_id;
        do
        {
            auto comps = stl_types::str::split::str(*it, ':');
            Vector<std::string_view> components(comps.begin(), comps.end());
            if(components.size() < 2)
                continue;
            auto field_key   = trim::right(components.at(0));
            auto field_value = trim::left(components.at(1));

            if(field_key == "processor")
            {
                proc_id = field_value;
                if(auto physid = read_cpu(
                       proc_id, url::Path{"topology/physical_package_id"}))
                    phys_id = physid.value();
            }

            if(pred(phys_id.c_str(), proc_id, field_key, field_value))
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
using stl_types::Set;
using stl_types::String;
using stl_types::Vector;

inline u32 node_count()
{
    return 1;
}
inline u32 cpu_count()
{
    using url::Path;

    Set<String> die_ids;
    for(auto const& id : detail::online_cores())
    {
        if(auto die_id =
               detail::read_cpu(id, Path{"topology/physical_package_id"});
           die_id.has_value())
            die_ids.insert(die_id.value().data());
    }

    return die_ids.size();
}
inline u32 core_count(u32 cpu = 0, u32 node = 0)
{
    using url::Path;

    String      selected_cpu = std::to_string(cpu);
    Set<String> core_ids;
    for(auto const& id : detail::online_cores())
    {
        if(auto die_id =
               detail::read_cpu(id, Path{"topology/physical_package_id"});
           die_id.has_value())
        {
            auto die_id_ = die_id.value();
            if(die_id_ != selected_cpu)
                continue;
        } else
            continue;

        if(auto core_id = detail::read_cpu(id, Path{"topology/core_id"});
           core_id.has_value())
            core_ids.insert(core_id.value());
    }

    return core_ids.size();
}
inline u32 thread_count(u32 cpu = 0, u32 node = 0)
{
    using url::Path;

    String         selected_cpu = std::to_string(cpu);
    Vector<u16> thread_ids;
    for(auto const& id : detail::online_cores())
    {
        if(auto die_id =
               detail::read_cpu(id, Path{"topology/physical_package_id"});
           die_id.has_value())
        {
            auto die_id_ = die_id.value();
            if(!std::equal(
                   die_id_.begin(),
                   die_id_.end(),
                   selected_cpu.begin(),
                   selected_cpu.end()))
                continue;
        } else
            continue;

        thread_ids.push_back(id);
    }

    return thread_ids.size();
}

inline stl_types::Optional<stl_types::Pair<String, String>> model(
    u32 cpu = 0, u32 node = 0)
{
    using namespace url::constructors;
    using namespace stl_types::str;
    using stl_types::Vector;

    stl_types::String vendor, model;
    stl_types::String implementer, variant, part;
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
    return std::make_optional(stl_types::Pair<String, String>(vendor, model));
}

template<typename T>
using topological_map = stl_types::Map<i32, stl_types::Map<u32, T>>;

inline topological_map<u32> topo_frequency()
{
    using stl_types::String;
    using url::Path;

    topological_map<u32> freqs;
    for(auto const& id : detail::online_cores())
    {
        auto die_id =
            detail::read_cpu(id, Path{"topology/physical_package_id"});
        auto core_id   = detail::read_cpu(id, Path{"topology/core_id"});
        auto frequency = detail::read_cpu(id, Path{"cpufreq/cpuinfo_max_freq"});

        if(core_id.has_error() || frequency.has_error())
            continue;

        auto die_id_ = libc::str::from_string<i32>(
            (die_id.has_value() ? die_id.value() : String("0")).data());
        auto core_id_ = libc::str::from_string<u32>(core_id.value().data());
        freqs[die_id_][core_id_] =
            libc::str::from_string<u32>(frequency.value().data());
    }

    return freqs;
}

inline u32 frequency(bool current = false, u32 cpu = 0, u32 node = 0)
{
    using url::Path;
    auto select_id = std::to_string(cpu);
    for(auto const& id : detail::online_cores())
    {
        auto cpu_id =
            detail::read_cpu(id, Path{"topology/physical_package_id"});

        if(cpu_id.has_error() || cpu_id.value() != select_id)
            break;
        auto freq = detail::read_cpu(id, Path{"cpufreq/cpuinfo_max_freq"});
        if(freq.has_error())
            break;
        return libc::str::from_string<u32>(freq.value().data());
    }
    return 0;
}

} // namespace platform::info::proc::linux_

namespace platform::info::memory::linux_ {
namespace detail {
using namespace proc::linux_::detail;
}

} // namespace platform::info::memory::linux_

namespace platform::info::os::linux_ {
namespace detail {
using namespace proc::linux_::detail;
}

inline stl_types::Optional<stl_types::String> lsb_value(
    stl_types::String const& key)
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
                return stl_types::String(value.begin(), value.end());
            }
        } while(!(++it).empty());
    }
    return std::nullopt;
}

inline stl_types::Optional<stl_types::String> name()
{
    return lsb_value("DISTRIB_ID");
}

inline stl_types::Optional<stl_types::String> version()
{
    return lsb_value("DISTRIB_RELEASE");
}

} // namespace platform::info::os::linux_

namespace platform::info::device::linux_ {
namespace detail {
using namespace proc::linux_::detail;
}

constexpr std::string_view dmi_root = "/sys/class/dmi/id";

inline stl_types::Optional<
    stl_types::Pair<stl_types::String, stl_types::String>>
device()
{
    using namespace url::constructors;
    stl_types::String vendor, product;
#if defined(COFFEE_BEAGLEBONEBLACK)
    return std::pair{"BeagleBone", "Black"};
#else
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

inline stl_types::Optional<stl_types::String> name()
{
    auto dev = device().value_or(std::pair{"<unknown>", "<unknown>"});
    return dev.first + " " + dev.second;
}

inline stl_types::Optional<
    stl_types::Pair<stl_types::String, stl_types::String>>
motherboard()
{
    using namespace url::constructors;
    stl_types::String vendor, model;
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

inline stl_types::Optional<
    stl_types::Pair<stl_types::String, stl_types::String>>
chassis()
{
    using namespace url::constructors;
    stl_types::String vendor, model = "Chassis";
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
