#include <coffee/core/plat/environment/linux/process.h>
#include <coffee/core/CFiles>
#include <coffee/core/string_casting.h>
#include <coffee/core/plat/memory/stlstring_ops.h>

namespace Coffee{
namespace Environment{
namespace Linux{

struct mem_usage
{
    uint64 vmsize;
    uint64 vmrss;
    uint64 rss;
    uint64 text;
    uint64 data;
};

LinuxProcessProperty::MemUnit LinuxProcessProperty::Mem(LinuxProcessProperty::PID)
{
    CString mem_info = CResources::Linux::LinuxFileFun::sys_read("/proc/self/statm");

    mem_usage usage = {};

    size_t spacer = 0;
    int steps = 0;
    while(true)
    {
        size_t start = spacer;
        spacer = mem_info.find(' ', spacer);

        if(spacer > mem_info.size())
            break;

        uint64 num = cast_string<uint64>(StrUtil::encapsulate(&mem_info[start],
                                                              C_CAST<uint32>(spacer - start)));
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

    return usage.vmsize - usage.vmrss;
}



}
}
}
