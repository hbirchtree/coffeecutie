#pragma once

#include "blam_base_types.h"

namespace blam::scn {

// Ptr types around i16
template<typename T>
struct scenario_ptr
{
    // Within any scenario data, points to a scenario list
    i16 index;

    operator i16() const
    {
        return index;
    }
};

struct bsp_ptr
{
    // Points to a scenario BSP section
    i16 index;

    operator i16() const
    {
        return index;
    }
};

}
