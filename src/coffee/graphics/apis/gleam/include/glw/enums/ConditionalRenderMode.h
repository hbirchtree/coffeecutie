#pragma once

#include "common.h"

namespace gl::groups {

// ConditionalRenderMode
enum class conditional_render_mode : ::libc_types::u32 {
#ifdef GL_QUERY_BY_REGION_NO_WAIT
    query_by_region_no_wait = GL_QUERY_BY_REGION_NO_WAIT,
#endif
#ifdef GL_QUERY_BY_REGION_NO_WAIT_INVERTED
    query_by_region_no_wait_inverted = GL_QUERY_BY_REGION_NO_WAIT_INVERTED,
#endif
#ifdef GL_QUERY_BY_REGION_WAIT
    query_by_region_wait = GL_QUERY_BY_REGION_WAIT,
#endif
#ifdef GL_QUERY_BY_REGION_WAIT_INVERTED
    query_by_region_wait_inverted = GL_QUERY_BY_REGION_WAIT_INVERTED,
#endif
#ifdef GL_QUERY_NO_WAIT
    query_no_wait = GL_QUERY_NO_WAIT,
#endif
#ifdef GL_QUERY_NO_WAIT_INVERTED
    query_no_wait_inverted = GL_QUERY_NO_WAIT_INVERTED,
#endif
#ifdef GL_QUERY_WAIT
    query_wait = GL_QUERY_WAIT,
#endif
#ifdef GL_QUERY_WAIT_INVERTED
    query_wait_inverted = GL_QUERY_WAIT_INVERTED,
#endif
}; // enum class conditional_render_mode

} // namespace gl::groups
