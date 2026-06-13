#include <nvml_comp/nvml_comp.h>

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/core/debug/formatting.h>
#include <nvidia/gdk/nvml.h>
#include <peripherals/stl/magic_enum.hpp>
#include <platforms/profiling/jsonprofile.h>

namespace nvml_comp {

struct NVMLGPUStats
    : comp_app::interfaces::GPUStatProvider
    , comp_app::AppService<NVMLGPUStats, comp_app::GPUStatProvider>
    , comp_app::AppLoadableService
{
    std::optional<libc_types::u64> mem_resident() final
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

    std::optional<libc_types::u64> mem_total() final
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

    std::map<std::string_view, std::string> stats_strings() final
    {
        using namespace std::string_view_literals;
        nvmlGpuOperationMode_t operating_mode = {}, pending_mode = {};
        nvmlDeviceGetGpuOperationMode(m_device, &operating_mode, &pending_mode);
        auto           op_mode_ = magic_enum::enum_name(operating_mode);
        constexpr auto prefix   = "NVML_GOM_"sv;
        if(op_mode_.size() > prefix.size())
            op_mode_ = op_mode_.substr(prefix.size());
        return {
            {"GPU operating mode",
             std::string(op_mode_.begin(), op_mode_.end())},
        };
    }

    std::multimap<std::string_view, reading_t> stats_numeric() final
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
        nvmlDeviceGetPcieThroughput(
            m_device, NVML_PCIE_UTIL_RX_BYTES, &pcie_rx);
        nvmlDeviceGetPcieThroughput(
            m_device, NVML_PCIE_UTIL_TX_BYTES, &pcie_tx);
        unsigned int temp = 0, temp_slowdown = 0, temp_critical = 0;
        nvmlDeviceGetTemperature(m_device, NVML_TEMPERATURE_GPU, &temp);
        nvmlDeviceGetTemperatureThreshold(
            m_device, NVML_TEMPERATURE_THRESHOLD_SLOWDOWN, &temp_slowdown);
        nvmlDeviceGetTemperatureThreshold(
            m_device, NVML_TEMPERATURE_THRESHOLD_SHUTDOWN, &temp_critical);
        unsigned int power_usage = 0, power_enforced_limit = 0, power_limit = 0;
        nvmlDeviceGetPowerUsage(m_device, &power_usage);
        nvmlDeviceGetPowerManagementLimit(m_device, &power_limit);
        nvmlDeviceGetEnforcedPowerLimit(m_device, &power_enforced_limit);
        return {
            {"GPU BAR1",
             {static_cast<libc_types::i64>(bar1Mem.bar1Used), 0, "Usage"}},
            {"GPU BAR1",
             {static_cast<libc_types::i64>(bar1Mem.bar1Total), 1, "Total"}},
            {"GPU memory bus usage", {usage.memory}},
            {"GPU clock", {graphics, 0, "Graphics clock"}},
            {"GPU clock", {sm, 1, "SM clock"}},
            {"GPU clock", {memory, 2, "Memory clock"}},
            {"GPU PCIe", {pcie_rx, 0, "RX"}},
            {"GPU PCIe", {pcie_tx, 1, "TX"}},
            {"GPU temperature", {temp, 0, "Current"}},
            {"GPU temperature", {temp_slowdown, 1, "Slowdown"}},
            {"GPU temperature", {temp_critical, 2, "Critical"}},
            {"GPU power", {power_usage, 0, "Current draw"}},
            {"GPU power", {power_enforced_limit, 1, "Limit (enforced)"}},
            {"GPU power", {power_limit, 2, "Limit (max)"}},
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

    if(numDevices > 1)
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

    auto& appInfo = *e.service<comp_app::AppInfo>();

    nvmlBrandType_t brand;
    if(nvmlDeviceGetBrand(device, &brand) == NVML_SUCCESS)
    {
        using namespace std::string_view_literals;
        auto           brand_ = magic_enum::enum_name(brand);
        constexpr auto prefix = "NVML_BRAND_"sv;
        if(brand_.size() > prefix.size())
            brand_ = brand_.substr(prefix.size());
        appInfo.add("nvidia:brand", std::string(brand_.begin(), brand_.end()));
    }
    appInfo.add("nvidia:model", name);
    nvmlMemory_t mem;
    if(nvmlDeviceGetMemoryInfo(device, &mem) == NVML_SUCCESS)
        appInfo.add("nvidia:totalMemory", std::to_string(mem.total));
    unsigned int gpu_clock = 0, mem_clock = 0, sm_clock = 0;
    nvmlDeviceGetMaxClockInfo(device, NVML_CLOCK_GRAPHICS, &gpu_clock);
    nvmlDeviceGetMaxClockInfo(device, NVML_CLOCK_MEM, &mem_clock);
    nvmlDeviceGetMaxClockInfo(device, NVML_CLOCK_SM, &sm_clock);
    if(gpu_clock)
        appInfo.add("nvidia:maxGpuClock", std::to_string(gpu_clock));
    if(mem_clock)
        appInfo.add("nvidia:maxMemClock", std::to_string(mem_clock));
    if(sm_clock)
        appInfo.add("nvidia:maxSmClock", std::to_string(sm_clock));
    unsigned int thresh_slowdown = 0, thresh_critical = 0;
    nvmlDeviceGetTemperatureThreshold(
        device, NVML_TEMPERATURE_THRESHOLD_SLOWDOWN, &thresh_slowdown);
    nvmlDeviceGetTemperatureThreshold(
        device, NVML_TEMPERATURE_THRESHOLD_SHUTDOWN, &thresh_critical);
    if(thresh_slowdown)
        appInfo.add("nvidia:slowdownLimit", std::to_string(thresh_slowdown));
    if(thresh_critical)
        appInfo.add("nvidia:criticalLimit", std::to_string(thresh_critical));

    using namespace platform::profiling;
    // Available in later versions, lets us see the amount of SMs in total
    // nvmlDeviceAttributes_t attrs;

    return true;
}

} // namespace nvml_comp
