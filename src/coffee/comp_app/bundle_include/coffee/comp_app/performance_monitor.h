#pragma once

#include <coffee/comp_app/subsystems.h>
#include <coffee/comp_app/services.h>

namespace comp_app {

struct PerformanceMonitor : AppService<PerformanceMonitor,
                                       detail::TypeList<
                                           PerformanceMonitor,
                                           CPUClockProvider,
                                           CPUTempProvider,
                                           GPUTempProvider,
                                           MemoryStatProvider,
                                           BatteryProvider,
                                           NetworkStatProvider
                                           >
                                       >
{
    time_point m_nextTime = {};

    virtual void start_restricted(Proxy& p, time_point const& time) final;
    virtual void end_restricted(Proxy&, time_point const& time) final;
};

} // namespace comp_app
