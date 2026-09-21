#pragma once

#include "common.h"

namespace gl::group {

// OcclusionQueryEventMaskAMD
enum class occlusion_query_event_mask_amd : u32
{
    query_all_event_bits_amd = 0xFFFFFFFF, // GL_QUERY_ALL_EVENT_BITS_AMD
    query_depth_bounds_fail_event_bit_amd =
        0x00000008, // GL_QUERY_DEPTH_BOUNDS_FAIL_EVENT_BIT_AMD
    query_depth_fail_event_bit_amd =
        0x00000002, // GL_QUERY_DEPTH_FAIL_EVENT_BIT_AMD
    query_depth_pass_event_bit_amd =
        0x00000001, // GL_QUERY_DEPTH_PASS_EVENT_BIT_AMD
    query_stencil_fail_event_bit_amd =
        0x00000004, // GL_QUERY_STENCIL_FAIL_EVENT_BIT_AMD
}; // enum class occlusion_query_event_mask_amd
C_FLAGS(occlusion_query_event_mask_amd, u32);

} // namespace gl::group
