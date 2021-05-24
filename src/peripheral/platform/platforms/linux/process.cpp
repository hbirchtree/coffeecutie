#include <platforms/linux/process.h>

#include <coffee/core/libc_types.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/file.h>
#include <url/url.h>

namespace platform {
namespace env {
namespace Linux {

using namespace ::Coffee;

struct mem_usage
{
    u64 vmsize;
    u64 vmrss;
    u64 rss;
    u64 text;
    u64 data;
};

ProcessProperty::MemUnit ProcessProperty::Mem(ProcessProperty::PID)
{
#ifndef COFFEE_LOWFAT
    using namespace url::constructors;
    if(auto result = platform::file::read_lines(MkSysUrl("/proc/self/statm"));
       result.has_error())
        return 0;
    else
    {
        auto const& mem_info = result.value().at(0);
        mem_usage   usage    = {};

        size_t spacer = 0;
        int    steps  = 0;
        while(true)
        {
            size_t start = spacer;
            spacer       = mem_info.find(' ', spacer);

            if(spacer > mem_info.size())
                break;

            u64 num = cast_string<u64>(mem_info.substr(start, spacer - start));
            if(steps == 0)
                usage.vmsize = num;
            else if(steps == 1)
                usage.vmrss = num;
            else if(steps == 2)
                usage.rss = num;
            else if(steps == 3)
                usage.text = num;
            else if(steps == 5)
                usage.data = num;
            spacer++;
            steps++;
        }

        return usage.vmrss;
    }
#else
    return 0;
#endif
}

bool MemMap::GetProcMap(ProcessProperty::PID pid, MemMap::ProcMap& target)
{
    using namespace ::semantic;
    using namespace url::constructors;

#ifndef COFFEE_LOWFAT
    CString maps_file = "/proc/" + cast_pod(pid) + "/maps";
    auto    maps_info = platform::file::read_lines(MkUrl(maps_file));

    if(maps_info.has_error())
        return false;

    szptr pos       = 0;
    bool  was_empty = false;
    for(auto const& line : maps_info.value())
    {
        target.push_back({});
        Entry& file = target.back();

        szptr space            = 0;
        u32   filename_counter = 0;
        auto  it               = str::split::spliterator<char>(line, ' ');
        while(it != str::split::spliterator<char>())
        {
            auto sec = *it;
            filename_counter++;
            switch(filename_counter)
            {
            case 1: {
                szptr mid = sec.find('-');
                auto  tmp = sec.substr(0, mid);
                file.start =
                    libc::str::from_string<u64, libc::str::convert_base_16>(
                        tmp.data());
                tmp = tmp.substr(mid + 1);
                file.end =
                    libc::str::from_string<u64, libc::str::convert_base_16>(
                        tmp.data());
                break;
            }
            case 2: {
                if(sec.find('r') != std::string_view::npos)
                    file.access |= RSCA::ReadOnly;
                if(sec.find('w') != std::string_view::npos)
                    file.access |= RSCA::WriteOnly;
                if(sec.find('x') != std::string_view::npos)
                    file.access |= RSCA::Executable;
                if(sec.find('s') != std::string_view::npos)
                    file.access |= RSCA::Shared;
                if(sec.find('p') != std::string_view::npos)
                    file.access |= RSCA::Private;
                break;
            }
            case 3: {
                auto tmp = String(sec.data(), sec.size());
                file.offset =
                    libc::str::from_string<u64, libc::str::convert_base_16>(
                        tmp.c_str());
                break;
            }
            case 4: {
                /* dev ?? */
                break;
            }
            case 5: {
                file.inode = cast_string<u64>(String(sec.data(), sec.size()));
                break;
            }
            case 6: {
                file.name = sec;
                break;
            }
            default:
                //                    fprintf(stderr, "%s - ", sec.c_str());
                break;
            }
        }
    }

    return true;
#else
    return false;
#endif
}

} // namespace Linux
} // namespace env
} // namespace platform
