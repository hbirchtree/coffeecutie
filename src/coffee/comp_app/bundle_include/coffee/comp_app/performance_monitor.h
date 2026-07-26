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
        AppLoader,
        CPUClockProvider,
        CPUTempProvider,
        GPUTempProvider,
        MemoryStatProvider,
        BatteryProvider,
        NetworkStatProvider,
        ScreenshotProvider,
        GPUStatProvider,
        SensorStatProvider>;
    using proxy_type = detail::restricted::proxy_t<PerformanceMonitor>;

    time_point m_nextTime{};
    time_point m_nextScreenshot{};
    time_point m_prevFrame{};

    void start_restricted(proxy_type& p, time_point const& time);
    void end_restricted(proxy_type& p, time_point const& time);

    void load(entity_container& e, app_error& ec);
    void unload(entity_container& e, app_error& ec);

    void capture_screenshot(
        proxy_type& p, std::string const& name, time_point const& time);

    rq::runtime_queue* m_worker_queue{nullptr};
    int                m_screenshot_quality{30};
    /* When set (e.g. by the dummy plug), screenshot encode + file write run
     * inline on the calling thread instead of the worker queue. The dummy plug
     * exits via quick_exit/window close right after its last screenshot, which
     * would otherwise abandon the async worker before it flushes to disk. */
    bool m_synchronous_screenshots{false};
};

} // namespace comp_app
