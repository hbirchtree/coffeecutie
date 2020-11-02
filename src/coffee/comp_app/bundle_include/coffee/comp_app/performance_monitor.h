#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>

namespace comp_app {

struct PerformanceMonitor : AppService<
                                PerformanceMonitor,
                                detail::TypeList<
                                    PerformanceMonitor,
                                    CPUClockProvider,
                                    CPUTempProvider,
                                    GPUTempProvider,
                                    MemoryStatProvider,
                                    BatteryProvider,
                                    NetworkStatProvider,
                                    ScreenshotProvider>>,
                            AppLoadableService
{
    time_point m_nextTime;
    time_point m_nextScreenshot;
    time_point m_prevFrame;

    virtual void start_restricted(Proxy& p, time_point const& time) final;
    virtual void end_restricted(Proxy& p, time_point const& time) final;

    void load(entity_container &e, app_error &ec);
    void unload(entity_container &e, app_error &ec);
};

} // namespace comp_app
