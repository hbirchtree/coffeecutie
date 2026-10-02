#pragma once

#include <coffee/components/types.h>

namespace comp_app {

struct FrameCounter;

struct FrameCounter : public compo::SubsystemBase
{
    using type = FrameCounter;

    using time_point = compo::time_point;

    time_point                next_print;
    std::optional<time_point> close_time;
    libc_types::u64           current{0};
    libc_types::u64           total_frames{0};

  public:
    FrameCounter();
    virtual void start_frame(compo::ContainerProxy& p, time_point const& current);
};

} // namespace comp_app
