#pragma once

#include "common.h"

namespace gl::group {

// ConditionalRenderMode
enum class conditional_render_mode : u32
{
    query_by_region_no_wait = 0x8E16, // GL_QUERY_BY_REGION_NO_WAIT
    query_by_region_no_wait_inverted =
        0x8E1A, // GL_QUERY_BY_REGION_NO_WAIT_INVERTED
    query_by_region_wait          = 0x8E15, // GL_QUERY_BY_REGION_WAIT
    query_by_region_wait_inverted = 0x8E19, // GL_QUERY_BY_REGION_WAIT_INVERTED
    query_no_wait                 = 0x8E14, // GL_QUERY_NO_WAIT
    query_no_wait_inverted        = 0x8E18, // GL_QUERY_NO_WAIT_INVERTED
    query_wait                    = 0x8E13, // GL_QUERY_WAIT
    query_wait_inverted           = 0x8E17, // GL_QUERY_WAIT_INVERTED
}; // enum class conditional_render_mode

} // namespace gl::group
