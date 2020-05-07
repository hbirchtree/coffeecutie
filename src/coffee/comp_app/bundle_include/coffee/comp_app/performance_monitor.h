#pragma once

#include <coffee/comp_app/subsystems.h>

namespace comp_app {

struct PerformanceMonitor : AppService<PerformanceMonitor>
{
    time_point m_nextTime = {};

    virtual void start_restricted(Proxy&, time_point const& time) final;
    virtual void end_restricted(Proxy&, time_point const& time) final;
};

} // namespace comp_app
