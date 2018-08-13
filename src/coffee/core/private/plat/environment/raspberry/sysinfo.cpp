#include <coffee/core/plat/environment/raspberry/sysinfo.h>
#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/plat/memory/string_ops.h>
#include <coffee/core/plat/plat_file.h>

namespace Coffee {
namespace Environment {
namespace Raspberry {

static const constexpr cstring model_names[] = {
    "A",
    "A+",
    "B",
    "B+",
    "2 B",
    "3 B",

    "Compute",
    "Zero",
};

HWDeviceInfo RaspberrySysInfo::DeviceName()
{
    static const constexpr cstring raspberry_string = "Raspberry Pi";
    static const constexpr cstring boardrev_string  = "boardrev";

    cstring model_string = model_names[0];
    CString firmware_string;
    CString cmdline = FileFun::sys_read("/proc/cmdline");

    do
    {
        if(cmdline.size() <= 0)
            break;
        cstring cmdline_s = cmdline.c_str();
        cstring match     = str::find(cmdline_s, boardrev_string);
        if(!match)
            break;
        cstring val = str::find(match, '=');
        if(!val)
            break;
        val += 1;
        cstring end = str::find(val, ' ');
        if(!end || (end - val) <= 0)
            break;
        firmware_string.insert(0, val, end - val);

        /* Pi Zero */
        if(firmware_string == "0x900092" || firmware_string == "0x900093")
            model_string = model_names[7];
        /* Pi Compute */
        else if(firmware_string == "0x11" || firmware_string == "0x14")
            model_string = model_names[6];
        /* Pi A */
        else if(
            firmware_string == "0x7" || firmware_string == "0x8" ||
            firmware_string == "0x9")
            model_string = model_names[0];
        /* Pi B */
        else if(
            firmware_string == "0x2" || firmware_string == "0x3" ||
            firmware_string == "0x4" || firmware_string == "0x5" ||
            firmware_string == "0x6")
            model_string = model_names[2];
        /* Pi A+ */
        else if(firmware_string == "0x12" || firmware_string == "0x15")
            model_string = model_names[1];
        /* Pi B+ */
        else if(firmware_string == "0x10" || firmware_string == "0x13")
            model_string = model_names[3];
        /* Pi 2 B */
        else if(
            firmware_string == "0xa01040" || firmware_string == "0xa01041" ||
            firmware_string == "0xa21041")
            model_string = model_names[4];
        /* Pi 3 B */
        else if(firmware_string == "0xa02082" || firmware_string == "0xa22082")
            model_string = model_names[5];
        else
            model_string = "[???]";
    } while(false);

    return HWDeviceInfo(raspberry_string, model_string, firmware_string);
}

bigscalar RaspberrySysInfo::ProcessorFrequency()
{
    bigscalar freq   = 0.0;
    CString   freq_s = FileFun::sys_read(
        "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");

    if(freq_s.size() > 0)
        freq = str::from_string<bigscalar>(freq_s.c_str()) / 1000000;

    return freq;
}

} // namespace Raspberry
} // namespace Environment
} // namespace Coffee
