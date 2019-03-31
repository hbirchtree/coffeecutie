#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/stl/type_safety.h>

namespace typing {
namespace graphics {

namespace query_composition {

enum QueryType_t
{
    Samples    = 1 << 0,
    Primitives = 1 << 1,
    Timing     = 1 << 2,

    /* Modifiers */
    Feedback     = 1 << (8 + 0),
    Conservative = 1 << (8 + 1),
    Any          = 1 << (8 + 2),
};

using query_flag = unsigned int;

template<query_flag Base, query_flag Mod>
struct query_basetype
{
    enum ex
    {
        value = Base | Mod
    };

    FORCEDINLINE operator query_flag()
    {
        return value;
    }
};

using query_any_samples = query_basetype<Samples, Any>;
using query_any_samples_conservative =
    query_basetype<Samples, Any | Conservative>;
using query_samples = query_basetype<Samples, 0>;

using query_primitives          = query_basetype<Primitives, 0>;
using query_feedback_primitives = query_basetype<Primitives, Feedback>;

using query_timing = query_basetype<Timing, 0>;

} // namespace query_composition

namespace query {

using flags = query_composition::query_flag;

using any_samples = query_composition::query_any_samples;
using any_samples_conservative =
    query_composition::query_any_samples_conservative;
using samples             = query_composition::query_samples;
using primitives          = query_composition::query_primitives;
using feedback_primitives = query_composition::query_feedback_primitives;
using timing              = query_composition::query_timing;

} // namespace query
} // namespace graphics
} // namespace typing
