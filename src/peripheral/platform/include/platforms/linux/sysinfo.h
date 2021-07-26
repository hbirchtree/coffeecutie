#pragma once

#include <peripherals/identify/system.h>

#include <platforms/posix/fsio.h>
#include <platforms/posix/rdwrio.h>

#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace platform::info::proc::linux_ {
namespace detail {

inline stl_types::result<semantic::mem_chunk<char>, file::posix::posix_error>
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
            return success(content.value());
    }
}

inline auto read_cpu(u32 id, url::Path const& path)
{
    return read_sysfs(url::constructors::MkSysUrl(
        url::Path{"/sys/devices/system/cpu/cpu" + std::to_string(id)} / path));
}

inline stl_types::Vector<u16> online_cores()
{
    using namespace url::constructors;

    if(auto content = read_sysfs("/sys/devices/system/cpu/online"_sys);
       content.has_error() || content.value().view.size() < 2)
        return {};
    else
    {
        using libc::str::from_string;

        auto data     = content.value().view;
        auto view     = std::string_view(data.data(), data.size() - 1);
        auto listings = str::split::spliterator(view, ',');
        stl_types::Vector<u16> out;
        while(listings != str::split::spliterator<char>())
        {
            auto id = *listings;
            if(auto split = id.find('-'); split != String::npos)
            {
                auto first = from_string<u16>(id.substr(0, split).data());
                auto end   = from_string<u16>(id.substr(split + 1).data());
                for(auto i : Range<>(end - first))
                    out.push_back(first + i);
            } else
                out.push_back(from_string<u16>(id.data()));
        }
        return out;
    }
}

} // namespace detail

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
        if(auto die_id = detail::read_cpu(id, Path{"topology/die_id"});
           die_id.has_value())
            die_ids.insert(die_id.value().view.data());
    }

    return die_ids.size();
}
inline u32 core_count(u32 cpu = 0, u32 node = 0)
{
    using url::Path;

    String      selected_cpu = std::to_string(cpu) + "\n";
    Set<String> core_ids;
    for(auto const& id : detail::online_cores())
    {
        if(auto die_id = detail::read_cpu(id, Path{"topology/die_id"});
           die_id.has_value())
        {
            auto die_id_ = die_id.value().view;
            if(!std::equal(
                   die_id_.begin(),
                   die_id_.end(),
                   selected_cpu.begin(),
                   selected_cpu.end()))
                break;
        } else
            break;

        if(auto core_id = detail::read_cpu(id, Path{"topology/core_id"});
           core_id.has_value())
            core_ids.insert(core_id.value().view.data());
    }

    return core_ids.size();
}
inline u32 thread_count(u32 cpu = 0, u32 node = 0)
{
    return std::thread::hardware_concurrency();
}

} // namespace platform::info::proc::linux_
