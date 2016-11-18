#include <coffee/core/plat/environment/osx/sysinfo.h>

#include <sys/types.h>
#include <sys/sysctl.h>

namespace Coffee{
namespace Environment{
namespace Mac{

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
        target.resize(len+1);
        sysctlbyname(mod_string, &target[0], &len, nullptr, 0);
        target.resize(Mem::Search::ChrFind(&target[0], '\0') - &target[0]);
    }

    return target;
}

CString MacSysInfo::GetSystemVersion()
{
    FILE* out = popen("sw_vers -productVersion","r");
    if(out)
    {
        char buf[16];
        char* ptr = fgets(buf, sizeof(buf),out);
        pclose(out);
        CString output = ptr;
        output.resize(Mem::Search::ChrFind((cstring)ptr,'\n')-ptr);
        return output;
    }else
        return "0.0";
}

HWDeviceInfo MacSysInfo::DeviceName()
{
    static const cstring mod_string = "hw.model";
    static const cstring typ_string = "kern.ostype";
    static const cstring rel_string = "kern.osrelease";

    CString target = _GetSysctlString(mod_string);
    CString kern = _GetSysctlString(typ_string);;
    CString osrel = _GetSysctlString(rel_string);;

    return HWDeviceInfo("Apple", target, kern + " " + osrel);
}

HWDeviceInfo MacSysInfo::Processor()
{
    static const cstring ven_string = "machdep.cpu.vendor";
    static const cstring brd_string = "machdep.cpu.brand_string";
    static const cstring mcc_string = "machdep.cpu.microcode_version";

    CString vendor = _GetSysCtlString(ven_string);
    CString brand = _GetSysCtlString(brd_string);
    CString microcode = _GetSysCtlString(mcc_string);

    return HWDeviceInfo(vendor, brand, microcode);
}

bigscalar MacSysInfo::ProcessorFrequency()
{
//    static const cstring frq_string = "machdep.tsc.frequency";
    static const cstring frq_string = "hw.cpufrequency";

    CString freq_s = _GetSysCtlString(frq_string);

    return Mem::Convert::strtoscalar(freq_s.data()) / (1024. * 1024. * 1024.);
}

CoreCnt MacSysInfo::CpuCount()
{
    static const cstring cpu_string = "hw.packages";

    CString c = _GetSysCtlString(cpu_string);

    return Mem::Convert::strtouint(c.data());
}

CoreCnt MacSysInfo::CoreCount()
{
    static const cstring cre_string = "machdep.cpu.core_count";

    CString c = _GetSysCtlString(cre_string);

    return Mem::Convert::strtouint(c.data());
}

MemUnit MacSysInfo::MemTotal()
{
    static const cstring mtl_string = "hw.memsize";

    CString c = _GetSysCtlString(mtl_string);

    return Mem::Convert::strtouint(c.data());
}

MemUnit MacSysInfo::MemAvailable()
{
    return MemTotal();
}

bool MacSysInfo::HasFPU()
{
    static const cstring fpu_string = "hw.optional.floatingpoint";

    CString fpu_s = _GetSysCtlString(fpu_string);

    return fpu_s == "1";
}

bool MacSysInfo::HasHyperThreading()
{
    static const cstring thd_string = "machdep.cpu.thread_count";
    CString c = _GetSysCtlString(cre_string);
    CoreCnt thr_count = Mem::Convert::strtouint(c.data());

    return thr_count == CoreCount();
}

}
}
}
