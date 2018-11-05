#include <peripherals/stl/string_casting.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <coffee/graphics/common/query/gpu_query.h>
#include <nvml.h>

namespace Coffee {
namespace GpuInfo {

/* Helper functions */
STATICINLINE void error_print(nvmlReturn_t code)
{
    if(code != NVML_SUCCESS)
        fprintf(stderr, "NVIDIA ML error: %s\n", nvmlErrorString(code));
}

nvmlDevice_t device(gpucount_t i)
{
    nvmlDevice_t dev;
    auto         r = nvmlDeviceGetHandleByIndex_v2(i, &dev);
    error_print(r);
    return dev;
}

/* Implementation functions */
SWVersionInfo GetDriver()
{
    CString driver_ver;
    driver_ver.resize(NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);

    nvmlSystemGetDriverVersion(
        &driver_ver[0], NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);

    driver_ver.resize(driver_ver.find('\0'));

    CString major = driver_ver.substr(0, driver_ver.find('.'));
    CString minor =
        driver_ver.substr(driver_ver.find('.') + 1, driver_ver.length());

    return SWVersionInfo(
        "NVIDIA",
        str::from_string<u32>(major.c_str()),
        str::from_string<u32>(minor.c_str()));
}

gpucount_t GetNumGpus()
{
    uint32 c = 0;
    nvmlDeviceGetCount(&c);
    return c;
}

HWDeviceInfo GetModel(gpucount_t i)
{
    auto dev = device(i);

    CString name;
    name.resize(NVML_DEVICE_NAME_BUFFER_SIZE);
    auto r = nvmlDeviceGetName(dev, &name[0], C_CAST<uint32>(name.size()));
    error_print(r);

    CString bios;
    bios.resize(NVML_DEVICE_VBIOS_VERSION_BUFFER_SIZE);
    r = nvmlDeviceGetVbiosVersion(dev, &bios[0], C_CAST<uint32>(bios.size()));
    error_print(r);

    return HWDeviceInfo("NVIDIA", &name[0], &bios[0]);
}

MemStatus GetMemInfo(gpucount_t i)
{
    auto dev = device(i);

    nvmlMemory_t mem = {};
    auto         r   = nvmlDeviceGetMemoryInfo(dev, &mem);
    error_print(r);

    return {mem.total, mem.used, mem.free};
}

uint64 GetProcMemInfo(gpucount_t i, proc_t pid)
{
#if NVML_API_VERSION >= 7
    auto dev = device(i);

    uint32 count = 0;
    nvmlDeviceGetGraphicsRunningProcesses(dev, &count, nullptr);

    Vector<nvmlProcessInfo_t> procs;
    procs.resize(count);
    auto r = nvmlDeviceGetGraphicsRunningProcesses(dev, &count, procs.data());
    error_print(r);

    for(nvmlProcessInfo_t const& p : procs)
        if(p.pid == pid)
            return p.usedGpuMemory;
#endif
    return 0;
}

TempRange GetTemp(gpucount_t i)
{
    auto dev = device(i);

    uint32 current = 0;
    uint32 max     = 0;

    auto r = nvmlDeviceGetTemperature(dev, NVML_TEMPERATURE_GPU, &current);
    error_print(r);

    r = nvmlDeviceGetTemperatureThreshold(
        dev, NVML_TEMPERATURE_THRESHOLD_SLOWDOWN, &max);
    error_print(r);

    return {C_CAST<scalar>(current), C_CAST<scalar>(max)};
}

ClockRange GetClock(gpucount_t i, Clock clk)
{
    auto dev = device(i);

    nvmlClockType_t type = NVML_CLOCK_COUNT;

    switch(clk)
    {
    case Clock::Graphics:
        type = NVML_CLOCK_GRAPHICS;
        break;
    case Clock::Memory:
        type = NVML_CLOCK_MEM;
        break;
    default:
        break;
    }

    if(type == NVML_CLOCK_COUNT)
        return {0, 0, 0};

    uint32 curr = 0, min = 0, max = 0;
    auto   r = nvmlDeviceGetApplicationsClock(dev, type, &curr);
    error_print(r);
    r = nvmlDeviceGetClockInfo(dev, type, &min);
    error_print(r);
    r = nvmlDeviceGetMaxClockInfo(dev, type, &max);
    error_print(r);

    return {curr, min, max};
}

PMode GetPowerMode(gpucount_t i)
{
    auto dev = device(i);

    nvmlPstates_t pstate = {};
    auto          r      = nvmlDeviceGetPerformanceState(dev, &pstate);
    error_print(r);

    return (pstate > NVML_PSTATE_1) ? PMode::Performance : PMode::Powersave;
}

UsageMeter GetUsage(gpucount_t i)
{
    auto dev = device(i);

    nvmlUtilization_t util = {};
    auto              r    = nvmlDeviceGetUtilizationRates(dev, &util);
    error_print(r);
    uint32 decoder = 0, encoder = 0, sampling = 0;
    r = nvmlDeviceGetDecoderUtilization(dev, &decoder, &sampling);
    error_print(r);
    r = nvmlDeviceGetDecoderUtilization(dev, &encoder, &sampling);
    error_print(r);

    return {C_CAST<uint8>(util.memory),
            C_CAST<uint8>(util.gpu),
            C_CAST<uint8>(decoder),
            C_CAST<uint8>(encoder)};
}

TransferStatus GetPcieTransfer(gpucount_t i)
{
#if NVML_API_VERSION >= 7
    auto dev = device(i);

    TransferStatus stat = {};
    auto           r =
        nvmlDeviceGetPcieThroughput(dev, NVML_PCIE_UTIL_RX_BYTES, &stat.rx);
    r = nvmlDeviceGetPcieThroughput(dev, NVML_PCIE_UTIL_TX_BYTES, &stat.tx);

    return stat;
#else
    return {};
#endif
}

GpuQueryInterface GetNvidia()
{
    return {
        GetDriver,
        GetNumGpus,
        GetModel,
        GetMemInfo,
        GetProcMemInfo,
        GetTemp,
        GetClock,
        GetPowerMode,
        GetUsage,
        GetPcieTransfer,
    };
}

} // namespace GpuInfo
} // namespace Coffee

extern "C" GpuQueryFunction* GetGpuQuery();

GpuQueryFunction* GetGpuQuery()
{
    if(nvmlInit() == NVML_SUCCESS && Coffee::GpuInfo::GetNumGpus())
    {
        return new GpuQueryFunction{Coffee::GpuInfo::GetNvidia};
    }
    //        fprintf(stderr, "NVML could not be initialized\n");
    return nullptr;
}
