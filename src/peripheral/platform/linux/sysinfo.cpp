#include <platforms/linux/sysinfo.h>

namespace platform::info::proc::linux_::detail {

stl_types::result<std::string, file::posix::posix_error> read_sysfs(
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

/* Reads a device-tree property (/proc/device-tree/...) as a list of strings.
 * DT string-list properties (e.g. `compatible`) are NUL-separated and carry no
 * trailing newline, so read_sysfs (which stops at '\n') is unsuitable here. */
std::vector<std::string> read_dt_strings(url::Path const& file)
{
    using namespace platform::file::posix;
    std::vector<std::string> out;
    if(auto fd = open_file(file.url()); fd.has_error())
        return out;
    else if(auto content = file::posix::read(fd.value()); content.has_error())
        return out;
    else
    {
        auto&       data = content.value();
        std::string raw(data.begin(), data.end());
        for(std::size_t start = 0; start < raw.size();)
        {
            auto nul = raw.find('\0', start);
            if(nul == std::string::npos)
                nul = raw.size();
            if(nul > start)
                out.push_back(raw.substr(start, nul - start));
            start = nul + 1;
        }
        return out;
    }
}

cpu_hierarchy_t online_cpus()
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

        /* big.LITTLE parts often report a single package with a single (or
         * absent) cluster_id, hiding the split -- Snapdragon reports all eight
         * cores as cluster 0. Peak frequency is what actually separates them,
         * so group by that before falling back to the SMT heuristic below.
         * Not on x86, where per-core turbo limits differ within one cluster. */
        if(compile_info::architecture != std::string("AMD64") &&
           first_pass.size() == 1 && first_pass[0].size() == 1)
        {
            const auto max_freq = [](u16 cpuid) -> libc_types::u64 {
                if(auto freq =
                       read_cpu(cpuid, url::Path{"cpufreq/cpuinfo_max_freq"});
                   freq.has_value() && freq.value().size())
                    return from_string<libc_types::u64>(freq.value().c_str());
                return 0;
            };
            std::map<libc_types::u64, std::map<u16, std::vector<u16>>> by_freq;
            for(auto const& [coreid, cpu_list] : first_pass[0][0])
                by_freq[max_freq(cpu_list.empty() ? coreid : cpu_list.front())]
                       [coreid] = cpu_list;
            /* Ascending frequency, so cluster 0 stays the little one */
            if(by_freq.size() > 1 && !by_freq.contains(0))
            {
                cpu_hierarchy_t clustered;
                u16             cluster{0};
                for(auto const& [_, cores] : by_freq)
                    clustered[0][cluster++] = cores;
                return clustered;
            }
        }

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

void foreach_cpuinfo(
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

}

namespace platform::info::proc::linux_ {

std::map<u32, std::string> thread_names()
{
    std::map<u32, std::string> result;

    DIR* dir = opendir("/proc/self/task");
    if(!dir)
        return result;

    char           path[64];
    char           buf[20]; /* TASK_COMM_LEN = 16, plus newline + NUL */
    struct dirent* ent;
    while((ent = readdir(dir)) != nullptr)
    {
        if(ent->d_name[0] == '.')
            continue;
        u32 tid = libc::str::from_string<u32>(ent->d_name);
        std::snprintf(
            path, sizeof(path), "/proc/self/task/%s/comm", ent->d_name);
        FILE* f = std::fopen(path, "r");
        if(!f)
        {
            result[tid] = {};
            continue;
        }
        std::string name;
        if(std::fgets(buf, sizeof(buf), f))
        {
            std::size_t len = std::strlen(buf);
            if(len > 0 && buf[len - 1] == '\n')
                buf[len - 1] = '\0';
            name = buf;
        }
        std::fclose(f);
        result[tid] = std::move(name);
    }
    closedir(dir);
    return result;
}

std::map<u32, libc_types::u64> thread_cpu_ticks()
{
    std::map<u32, libc_types::u64> result;

    DIR* dir = opendir("/proc/self/task");
    if(!dir)
        return result;

    char           path[64];
    char           line[512]; /* stat lines are ~200-300 bytes in practice */
    struct dirent* ent;
    while((ent = readdir(dir)) != nullptr)
    {
        if(ent->d_name[0] == '.')
            continue;

        u32 tid = libc::str::from_string<u32>(ent->d_name);
        std::snprintf(
            path, sizeof(path), "/proc/self/task/%s/stat", ent->d_name);
        FILE* f = std::fopen(path, "r");
        if(!f)
            continue;

        if(std::fgets(line, sizeof(line), f))
        {
            /* After ')': state ppid pgrp session tty_nr tty_pgrp flags
             *            minflt cminflt majflt cmajflt utime stime ... */
            char* rparen = std::strrchr(line, ')');
            if(rparen)
            {
                unsigned long utime{0}, stime{0};
                if(std::sscanf(
                       rparen + 1,
                       " %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu"
                       " %lu %lu",
                       &utime,
                       &stime) == 2)
                    result[tid] = static_cast<libc_types::u64>(utime) +
                                  static_cast<libc_types::u64>(stime);
            }
        }
        std::fclose(f);
    }
    closedir(dir);
    return result;
}

}

namespace platform::info::device::linux_ {

std::optional<std::pair<std::string, std::string>> device()
{
    using namespace url::constructors;
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

    /* SoC boards without DMI and without a cpuinfo "Model" line still expose a
     * human-readable board name in the device-tree root `model` property,
     * e.g. "Pine64 RockPro64" or "Raspberry Pi 4 Model B". */
    if(product.empty())
    {
        if(auto model = detail::read_dt_strings("/proc/device-tree/model"_sys);
           !model.empty())
            product = model.front();
    }

    if(product.starts_with("Raspberry"))
        vendor = "Raspberry Pi";

    if(product.empty())
        return std::nullopt;
    return std::pair{vendor, product};
}

}
