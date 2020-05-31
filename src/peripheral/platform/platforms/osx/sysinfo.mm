#include <platforms/osx/sysinfo.h>

#include <sys/sysctl.h>
#include <sys/types.h>

#include <peripherals/stl/stlstring_ops.h>

#if defined(COFFEE_IOS)
#import <UIKit/UIDevice.h>
#endif

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

static u64 _GetSysctlInt(const cstring mod_string)
{
    u64 temp = 0;
    size_t len  = sizeof(temp);
    sysctlbyname(mod_string, &temp, &len, nullptr, 0);
    return temp;
}

CString get_kern_name()
{
#if defined(COFFEE_IOS)
    return "Darwin";
#else
    return _GetSysctlString("kern.ostype");
#endif
}

CString get_kern_ver()
{
#if defined(COFFEE_IOS)
    return [UIDevice currentDevice].systemVersion.UTF8String;
#else
    return _GetSysctlString("kern.osrelease");
#endif
}

CString SysInfo::GetSystemVersion()
{
#if defined(COFFEE_IOS)
    if(auto ver = [UIDevice currentDevice].systemVersion.UTF8String; ver)
        return ver;
#else
    if(auto ver = _GetSysctlString("kern.osproductversion"); !ver.empty())
        return ver;
#endif

    return "0.0";
}

info::HardwareDevice SysInfo::DeviceName()
{
#if defined(COFFEE_IOS)
    UIDevice* device = [UIDevice currentDevice];
    
    return info::HardwareDevice(
                "Apple", device.name.UTF8String, GetSystemVersion());
#else
    static const cstring mod_string = "hw.model";
    static const cstring typ_string = "kern.ostype";
    static const cstring rel_string = "kern.osrelease";

    CString target = _GetSysctlString(mod_string);
    CString kern   = _GetSysctlString(typ_string);
    CString osrel  = _GetSysctlString(rel_string);

    return info::HardwareDevice("Apple", target, kern + " " + osrel);
#endif
}

info::HardwareDevice SysInfo::Processor()
{
#if defined(COFFEE_IOS)
    return info::HardwareDevice("Apple Axx", "0x0");
#else
    static const cstring ven_string = "machdep.cpu.vendor";
    static const cstring brd_string = "machdep.cpu.brand_string";
    static const cstring mcc_string = "machdep.cpu.microcode_version";

    CString vendor    = _GetSysctlString(ven_string);
    CString brand     = _GetSysctlString(brd_string);
    u64  microcode = _GetSysctlInt(mcc_string);

    return info::HardwareDevice(
        vendor, brand, str::convert::hexify(microcode & 0xFFFF, true));
#endif
}

Vector<bigscalar> SysInfo::ProcessorFrequencies(u32)
{
    static const cstring frq_string = "machdep.tsc.frequency";
    //            "hw.cpufrequency"

    u64 freq_i = _GetSysctlInt(frq_string);

    return {freq_i / (1000. * 1000. * 1000.)};
}

CoreCnt SysInfo::CpuCount()
{
    static const cstring cpu_string = "hw.packages";

    u64 c = _GetSysctlInt(cpu_string);

    return C_FCAST<CoreCnt>(c);
}

CoreCnt SysInfo::CoreCount()
{
#if defined(COFFEE_IOS)
    return C_FCAST<CoreCnt>([NSProcessInfo processInfo].activeProcessorCount);
#else
    static const cstring cre_string = "machdep.cpu.core_count";

    u64 c = _GetSysctlInt(cre_string);

    return C_FCAST<CoreCnt>(c);
#endif
}

MemUnit SysInfo::MemTotal()
{
#if defined(COFFEE_IOS)
    return [NSProcessInfo processInfo].physicalMemory;
#else
    static const cstring mtl_string = "hw.memsize";

    u64 c = _GetSysctlInt(mtl_string);

    return C_FCAST<MemUnit>(c);
#endif
}

MemUnit SysInfo::MemAvailable()
{
    static const cstring mav_string = "hw.usermem";

    u64 c = _GetSysctlInt(mav_string);

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

    u64 c = _GetSysctlInt(fpu_string);

    return c;
}

bool SysInfo::HasHyperThreading()
{
    static const cstring thd_string = "machdep.cpu.thread_count";
    u64               thr_count  = _GetSysctlInt(thd_string);

    return thr_count != CoreCount();
}

PowerInfoDef::Temp PowerInfo::CpuTemperature()
{
#if defined(COFFEE_IOS)
    auto thermal = [NSProcessInfo processInfo].thermalState;
    f32 temp = 0.f;
    
    switch(thermal)
    {
    case NSProcessInfoThermalStateNominal:
        temp = 30.f;
        break;
    case NSProcessInfoThermalStateFair:
        temp = 50.f;
        break;
    case NSProcessInfoThermalStateSerious:
        temp = 60.f;
        break;
    case NSProcessInfoThermalStateCritical:
        temp = 80.f;
        break;
    default:
        break;
    }
    
    return {temp, 0.f};
#else
    return {scalar(_GetSysctlInt("machdep.xcpm.cpu_thermal_level")), 0.f};
#endif
}

PowerInfoDef::Temp PowerInfo::GpuTemperature()
{
#if defined(COFFEE_IOS)
    return {0.f, 0.f};
#else
    return {scalar(_GetSysctlInt("machdep.xcpm.gpu_thermal_level")), 0.f};
#endif
}

bool PowerInfo::IsPowered()
{
#if defined(COFFEE_IOS)
    return false;
#else
    return true;
#endif
}

bool PowerInfo::IsCharging()
{
#if defined(COFFEE_IOS)
    return
        [UIDevice currentDevice].batteryState == UIDeviceBatteryStateCharging;
#else
    return false;
#endif
}

bool PowerInfo::HasBattery()
{
#if defined(COFFEE_IOS)
    return true;
#else
    return false;
#endif
}

u16 PowerInfo::BatteryPercentage()
{
#if defined(COFFEE_IOS)
    return static_cast<u16>([UIDevice currentDevice].batteryLevel);
#else
    return 0;
#endif
}

} // namespace mac
} // namespace env
} // namespace platform
