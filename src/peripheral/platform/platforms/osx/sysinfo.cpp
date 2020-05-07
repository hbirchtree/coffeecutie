#include <platforms/osx/sysinfo.h>

#include <sys/sysctl.h>
#include <sys/types.h>

#include <peripherals/stl/stlstring_ops.h>

#include <IOKit/IOKitLib.h>

namespace platform {
namespace env {
namespace mac {

/*
 * OS X has a sexy API for retrieving hardware information. Big thumbs up!
 */

static CString _GetSysctlString(const cstring mod_string)
{
    CString target;

    size_t len = 0;
    sysctlbyname(mod_string, nullptr, &len, nullptr, 0);

    if(len > 0)
    {
        target.resize(len + 1);
        sysctlbyname(mod_string, &target[0], &len, nullptr, 0);
        target.resize(libc::str::find(&target[0], '\0') - &target[0]);
    }

    return target;
}

static uint64 _GetSysctlInt(const cstring mod_string)
{
    uint64 temp = 0;
    size_t len  = sizeof(temp);
    sysctlbyname(mod_string, &temp, &len, nullptr, 0);
    return temp;
}

CString get_kern_name()
{
    return _GetSysctlString("kern.ostype");
}

CString get_kern_ver()
{
    return _GetSysctlString("kern.osrelease");
}

CString SysInfo::GetSystemVersion()
{
    CString version = _GetSysctlString("kern.osproductversion");

    if(!version.empty())
        return version;

    return "0.0";
}

info::HardwareDevice SysInfo::DeviceName()
{
    static const cstring mod_string = "hw.model";
    static const cstring typ_string = "kern.ostype";
    static const cstring rel_string = "kern.osrelease";

    CString target = _GetSysctlString(mod_string);
    CString kern   = _GetSysctlString(typ_string);
    CString osrel  = _GetSysctlString(rel_string);

    return info::HardwareDevice("Apple", target, kern + " " + osrel);
}

info::HardwareDevice SysInfo::Processor()
{
    static const cstring ven_string = "machdep.cpu.vendor";
    static const cstring brd_string = "machdep.cpu.brand_string";
    static const cstring mcc_string = "machdep.cpu.microcode_version";

    CString vendor    = _GetSysctlString(ven_string);
    CString brand     = _GetSysctlString(brd_string);
    uint64  microcode = _GetSysctlInt(mcc_string);

    return info::HardwareDevice(
        vendor, brand, str::convert::hexify(microcode & 0xFFFF, true));
}

Vector<bigscalar> SysInfo::ProcessorFrequencies(u32)
{
    static const cstring frq_string = "machdep.tsc.frequency";
    //            "hw.cpufrequency"

    uint64 freq_i = _GetSysctlInt(frq_string);

    return {freq_i / (1000. * 1000. * 1000.)};
}

CoreCnt SysInfo::CpuCount()
{
    static const cstring cpu_string = "hw.packages";

    uint64 c = _GetSysctlInt(cpu_string);

    return C_FCAST<CoreCnt>(c);
}

CoreCnt SysInfo::CoreCount()
{
    static const cstring cre_string = "machdep.cpu.core_count";

    uint64 c = _GetSysctlInt(cre_string);

    return C_FCAST<CoreCnt>(c);
}

MemUnit SysInfo::MemTotal()
{
    static const cstring mtl_string = "hw.memsize";

    uint64 c = _GetSysctlInt(mtl_string);

    return C_FCAST<MemUnit>(c);
}

MemUnit SysInfo::MemAvailable()
{
    static const cstring mav_string = "hw.usermem";

    uint64 c = _GetSysctlInt(mav_string);

    return MemTotal() - c;
}

MemUnit SysInfo::MemResident()
{
    if(rusage usage; getrusage(RUSAGE_SELF, &usage) == 0)
        return C_FCAST<MemUnit>(usage.ru_maxrss) / 1024;
    return 0;
}

bool SysInfo::HasFPU()
{
    static const cstring fpu_string = "hw.optional.floatingpoint";

    uint64 c = _GetSysctlInt(fpu_string);

    return c;
}

bool SysInfo::HasHyperThreading()
{
    static const cstring thd_string = "machdep.cpu.thread_count";
    uint64               thr_count  = _GetSysctlInt(thd_string);

    return thr_count != CoreCount();
}

PowerInfoDef::Temp PowerInfo::CpuTemperature()
{
    return {scalar(_GetSysctlInt("machdep.xcpm.cpu_thermal_level")), 0.f};
}

PowerInfoDef::Temp PowerInfo::GpuTemperature()
{
    return {scalar(_GetSysctlInt("machdep.xcpm.gpu_thermal_level")), 0.f};
}

} // namespace mac
} // namespace env
} // namespace platform
