#include <coffee/core/CFiles>
#include <coffee/core/plat/environment/linux/process.h>
#include <peripherals/stl/string_casting.h>

namespace Coffee {
namespace Environment {
namespace Linux {

struct mem_usage
{
    u64 vmsize;
    u64 vmrss;
    u64 rss;
    u64 text;
    u64 data;
};

LinuxProcessProperty::MemUnit LinuxProcessProperty::Mem(
    LinuxProcessProperty::PID)
{
#ifndef COFFEE_LOWFAT
    file_error ec;

    CString mem_info =
        CResources::Linux::LinuxFileFun::sys_read("/proc/self/statm", ec);

    mem_usage usage = {};

    size_t spacer = 0;
    int    steps  = 0;
    while(true)
    {
        size_t start = spacer;
        spacer       = mem_info.find(' ', spacer);

        if(spacer > mem_info.size())
            break;

        u64 num = cast_string<u64>(
            str::encapsulate(&mem_info[start], C_CAST<u32>(spacer - start)));
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
#else
    return 0;
#endif
}

bool MemMap::GetProcMap(LinuxProcessProperty::PID pid, MemMap::ProcMap& target)
{
#ifndef COFFEE_LOWFAT
    file_error ec;

    CString maps_file = "/proc/" + cast_pod(pid) + "/maps";
    CString maps_info =
        CResources::Linux::LinuxFileFun::sys_read(maps_file.c_str(), ec);

    szptr end       = maps_info.find('\n');
    szptr pos       = 0;
    bool  was_empty = false;
    while(end < maps_info.size())
    {
        target.push_back({});
        Entry& file = target.back();

        szptr space            = 0;
        u32   filename_counter = 0;
        while(space < end)
        {
            space     = maps_info.find(' ', pos);
            szptr len = space - pos;
            if(space > end)
                len = end - pos;
            auto sec  = str::encapsulate(&maps_info[pos], len);
            was_empty = sec.size() == 0;
            if(!was_empty)
            {
                filename_counter++;
                switch(filename_counter)
                {
                case 1:
                {
                    szptr   mid = sec.find('-');
                    CString tmp = str::encapsulate(sec.data(), mid);
                    file.start =
                        str::from_string<u64, str::convert_base_16>(tmp.data());
                    tmp = &sec[mid + 1];
                    file.end =
                        str::from_string<u64, str::convert_base_16>(tmp.data());
                    break;
                }
                case 2:
                {
                    if(sec.find('r') < end)
                        file.access |= RSCA::ReadOnly;
                    if(sec.find('w') < end)
                        file.access |= RSCA::WriteOnly;
                    if(sec.find('x') < end)
                        file.access |= RSCA::Executable;
                    if(sec.find('s') < end)
                        file.access |= RSCA::Shared;
                    if(sec.find('p') < end)
                        file.access |= RSCA::Private;
                    break;
                }
                case 3:
                {
                    file.offset = str::from_string<u64, str::convert_base_16>(
                        sec.c_str());
                    break;
                }
                case 4:
                {
                    /* dev ?? */
                    break;
                }
                case 5:
                {
                    file.inode = cast_string<u64>(sec);
                    break;
                }
                case 6:
                {
                    file.name = sec;
                    break;
                }
                default:
                    //                    fprintf(stderr, "%s - ", sec.c_str());
                    break;
                }
            }
            pos = space + 1;
            if(space > end)
            {
                pos = end + 1;
                break;
            }
        }
        end = maps_info.find('\n', pos);
    }

    return true;
#else
    return false;
#endif
}

} // namespace Linux
} // namespace Environment
} // namespace Coffee
