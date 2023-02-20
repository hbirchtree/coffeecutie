#pragma once

#include <coffee/components/types.h>

namespace comp_app {

struct FrameCounter;

using FrameTag  = compo::ValueTag<FrameCounter, libc_types::u32>;

struct FrameCounter : public compo::globals::ValueSubsystem<FrameTag>
{
    time_point next_print;

  public:
    virtual void start_frame(ContainerProxy&, time_point const& current);
};

} // namespace comp_app
