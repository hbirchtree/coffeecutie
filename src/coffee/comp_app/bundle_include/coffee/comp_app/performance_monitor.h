#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>

namespace rq {
class runtime_queue;
}

namespace comp_app {

struct PerformanceMonitor
    : AppService<PerformanceMonitor>
    , AppLoadableService
{
    using readable_services = detail::subsystem_list<
        PerformanceMonitor,
        CPUClockProvider,
        CPUTempProvider,
        GPUTempProvider,
        MemoryStatProvider,
        BatteryProvider,
        NetworkStatProvider,
        ScreenshotProvider,
        GPUStatProvider>;
    using proxy_type = detail::restricted::proxy_t<PerformanceMonitor>;

    time_point m_nextTime{};
    time_point m_nextScreenshot{};
    time_point m_prevFrame{};

    void start_restricted(proxy_type& p, time_point const& time);
    void end_restricted(proxy_type& p, time_point const& time);

    void load(entity_container& e, app_error& ec);
    void unload(entity_container& e, app_error& ec);

    void capture_screenshot(proxy_type& p, time_point const& time);

    rq::runtime_queue* m_worker_queue{nullptr};
};

} // namespace comp_app
