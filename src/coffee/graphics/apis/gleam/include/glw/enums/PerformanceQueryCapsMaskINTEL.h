#pragma once

#include "common.h"

namespace gl::group {

// PerformanceQueryCapsMaskINTEL
enum class performance_query_caps_mask_intel : u32
{
    perfquery_global_context_intel =
        0x00000001, // GL_PERFQUERY_GLOBAL_CONTEXT_INTEL
    perfquery_single_context_intel =
        0x00000000, // GL_PERFQUERY_SINGLE_CONTEXT_INTEL
}; // enum class performance_query_caps_mask_intel
C_FLAGS(performance_query_caps_mask_intel, u32);

} // namespace gl::group
