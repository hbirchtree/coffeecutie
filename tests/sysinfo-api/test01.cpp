#include <coffee/core/CEnvironment>
#include <coffee/core/CMath>
#include <coffee/core/CPlatform>
#include <coffee/core/CSysInfo>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool sysinfo_printing()
{
    cDebug("Device name: {0}", SysInfo::DeviceName());

    cDebug("System memory: {0:1}MB", SysInfo::MemTotal() / CMath::pow(1024, 2));
    cDebug(
        "Available system memory: {0:1}MB",
        SysInfo::MemAvailable() / CMath::pow(1024, 2));

    cDebug("Processor info: {0}", SysInfo::Processor());
    cDebug("Frequency: {0:2}GHz", SysInfo::ProcessorFrequency());
    cDebug("Hyper-threading: {0}", SysInfo::HasHyperThreading());
    cDebug("FPU: {0}", SysInfo::HasFPU());
    cDebug("CPUs: {0}", SysInfo::CpuCount());
    cDebug("Cores: {0}", SysInfo::CoreCount());
    cDebug("Threads: {0}", SysInfo::ThreadCount());
    cDebug("Processor cache: {0}", SysInfo::ProcessorCacheSize());

    cDebug("CPU temperature: {0}", PowerInfo::CpuTemperature().current);

    Profiler::Profile("Get system data");

    return true;
}

COFFEE_TESTS_BEGIN(1)

    {sysinfo_printing, "Basic SysInfo information"}

COFFEE_TESTS_END()
