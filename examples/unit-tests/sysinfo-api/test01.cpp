#include <coffee/core/CUnitTesting>
#include <coffee/core/CPlatform>
#include <coffee/core/CDebug>

using namespace Coffee;

bool sysinfo_printing()
{
    cDebug("Device name: {0}",SysInfo::DeviceName());

    cDebug("System memory: {0:1}MB",
           SysInfo::MemTotal()/CMath::pow<bigscalar>(1024,2));
    cDebug("Available system memory: {0:1}MB",
           SysInfo::MemAvailable()/CMath::pow<bigscalar>(1024,2));

    cDebug("Processor info: {0}",SysInfo::Processor());
    cDebug("Frequency: {0:2}GHz",SysInfo::ProcessorFrequency());
    cDebug("Hyper-threading: {0}",SysInfo::HasHyperThreading());
    cDebug("FPU: {0}",SysInfo::HasFPU());
    cDebug("CPUs: {0}",SysInfo::CpuCount());
    cDebug("Cores: {0}",SysInfo::CoreCount());
    cDebug("Threads: {0}",SysInfo::ThreadCount());
	cDebug("Processor cache: {0}",SysInfo::ProcessorCacheSize());

    cDebug("CPU temperature: {0}",PowerInfo::CpuTemperature().current);

    Profiler::Profile("Get system data");

    return true;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {sysinfo_printing}
};

COFFEE_RUN_TESTS(_tests);
