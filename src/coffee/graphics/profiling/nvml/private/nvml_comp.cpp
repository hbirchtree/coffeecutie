#include <nvml_comp/nvml_comp.h>

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/core/debug/formatting.h>
#include <magic_enum.hpp>
#include <nvidia/gdk/nvml.h>
#include <platforms/profiling/jsonprofile.h>

namespace nvml_comp {

struct NVMLGPUStats
    : comp_app::interfaces::GPUStatProvider
    , comp_app::AppService<NVMLGPUStats, comp_app::GPUStatProvider>
    , comp_app::AppLoadableService
{
    std::optional<libc_types::u32> mem_resident() final
    {
        unsigned int num_procs{0};
        if(nvmlDeviceGetGraphicsRunningProcesses(
               m_device, &num_procs, nullptr) != NVML_SUCCESS)
            return std::nullopt;
        std::vector<nvmlProcessInfo_t> procs(num_procs);
        if(nvmlDeviceGetGraphicsRunningProcesses(
               m_device, &num_procs, procs.data()) != NVML_SUCCESS)
            return std::nullopt;
        auto it = std::find_if(
            procs.begin(),
            procs.end(),
            [pid = getpid()](nvmlProcessInfo_t const& info) {
                return pid == info.pid;
            });
        if(it == procs.end())
            return std::nullopt;
        return it->usedGpuMemory;
    }

    std::optional<libc_types::u32> mem_total() final
    {
        nvmlMemory_t mem;
        nvmlDeviceGetMemoryInfo(m_device, &mem);
        return mem.total;
    }

    std::optional<libc_types::u8> usage() final
    {
        nvmlUtilization_t usage;
        nvmlDeviceGetUtilizationRates(m_device, &usage);
        return usage.gpu;
    }

    std::map<std::string_view, libc_types::f32> stats_numeric() final
    {
        nvmlBAR1Memory_t bar1Mem;
        nvmlDeviceGetBAR1MemoryInfo(m_device, &bar1Mem);
        nvmlUtilization_t usage;
        nvmlDeviceGetUtilizationRates(m_device, &usage);
        unsigned int graphics = 0, sm = 0, memory = 0, video = 0;
        nvmlDeviceGetClockInfo(m_device, NVML_CLOCK_GRAPHICS, &graphics);
        nvmlDeviceGetClockInfo(m_device, NVML_CLOCK_SM, &sm);
        nvmlDeviceGetClockInfo(m_device, NVML_CLOCK_MEM, &memory);
        // Available in later versions
        // nvmlDeviceGetClockInfo(m_device, NVML_CLOCK_VIDEO, &video);
        unsigned int pcie_rx = 0, pcie_tx = 0;
        nvmlDeviceGetPcieThroughput(m_device, NVML_PCIE_UTIL_RX_BYTES, &pcie_rx);
        nvmlDeviceGetPcieThroughput(m_device, NVML_PCIE_UTIL_TX_BYTES, &pcie_tx);
        return {
            {"GPU BAR1 usage", bar1Mem.bar1Used},
            {"GPU BAR1 total", bar1Mem.bar1Total},
            {"GPU memory bus usage", usage.memory},
            {"GPU clock", graphics},
            {"GPU SM clock", sm},
            {"GPU memory clock", memory},
            {"GPU PCIe RX", pcie_rx},
            {"GPU PCIe TX", pcie_tx},
        };
    }

    std::map<std::string_view, stats_desc_t> stats_description() final
    {
        return {};
    }

    nvmlDevice_t m_device{};
};

bool try_load_nvml(compo::EntityContainer& e)
{
    using Coffee::cDebug;
    using compo::type_list_t;

    if(auto res = nvmlInit(); res != NVML_SUCCESS)
    {
        cDebug("Failed to init NVML: {}", magic_enum::enum_name(res));
        return false;
    }

    unsigned int numDevices{};
    nvmlDeviceGetCount(&numDevices);

    if(numDevices < 1)
    {
        cDebug("NVML detected, but no devices available");
        return false;
    }

    nvmlDevice_t      device;
    nvmlComputeMode_t mode;
    if(auto ret = nvmlDeviceGetHandleByIndex(0, &device); ret != NVML_SUCCESS)
    {
        cDebug("Failed to get device handle: {}", magic_enum::enum_name(ret));
        return false;
    }

    std::string name(NVML_DEVICE_NAME_BUFFER_SIZE, '\0');
    nvmlDeviceGetName(device, name.data(), name.size() - 1);
    name.resize(name.find('\0'));

    if(numDevices > 2)
        cDebug("Multiple devices detected, selecting {}", name);
    else
        cDebug("Selected {}", name);

    comp_app::AppLoader loader;
    comp_app::app_error ec;
    loader.registerAll<type_list_t<NVMLGPUStats>>(e, ec);
    C_ERROR_CHECK(ec);

    auto& nvml    = e.subsystem_cast<NVMLGPUStats>();
    nvml.m_device = device;

    auto ts = std::chrono::duration_cast<std::chrono::microseconds>(
        compo::clock::now().time_since_epoch());

    using namespace platform::profiling;
    // Available in later versions, lets us see the amount of SMs in total
    // nvmlDeviceAttributes_t attrs;

    return true;
}

} // namespace nvml_comp
