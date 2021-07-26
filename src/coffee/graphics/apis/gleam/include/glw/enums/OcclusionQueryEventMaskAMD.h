#pragma once

#include "common.h"

namespace gl::group {

// OcclusionQueryEventMaskAMD
enum class occlusion_query_event_mask_amd : ::libc_types::u32
{
#ifdef GL_QUERY_ALL_EVENT_BITS_AMD
    query_all_event_bits_amd = GL_QUERY_ALL_EVENT_BITS_AMD,
#endif
#ifdef GL_QUERY_DEPTH_BOUNDS_FAIL_EVENT_BIT_AMD
    query_depth_bounds_fail_event_bit_amd =
        GL_QUERY_DEPTH_BOUNDS_FAIL_EVENT_BIT_AMD,
#endif
#ifdef GL_QUERY_DEPTH_FAIL_EVENT_BIT_AMD
    query_depth_fail_event_bit_amd = GL_QUERY_DEPTH_FAIL_EVENT_BIT_AMD,
#endif
#ifdef GL_QUERY_DEPTH_PASS_EVENT_BIT_AMD
    query_depth_pass_event_bit_amd = GL_QUERY_DEPTH_PASS_EVENT_BIT_AMD,
#endif
#ifdef GL_QUERY_STENCIL_FAIL_EVENT_BIT_AMD
    query_stencil_fail_event_bit_amd = GL_QUERY_STENCIL_FAIL_EVENT_BIT_AMD,
#endif
}; // enum class occlusion_query_event_mask_amd
C_FLAGS(occlusion_query_event_mask_amd, ::libc_types::u32);

} // namespace gl::group
