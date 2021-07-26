#pragma once

#include "common.h"

namespace gl::group {

// PerformanceQueryCapsMaskINTEL
enum class performance_query_caps_mask_intel : ::libc_types::u32
{
#ifdef GL_PERFQUERY_GLOBAL_CONTEXT_INTEL
    perfquery_global_context_intel = GL_PERFQUERY_GLOBAL_CONTEXT_INTEL,
#endif
#ifdef GL_PERFQUERY_SINGLE_CONTEXT_INTEL
    perfquery_single_context_intel = GL_PERFQUERY_SINGLE_CONTEXT_INTEL,
#endif
}; // enum class performance_query_caps_mask_intel
C_FLAGS(performance_query_caps_mask_intel, ::libc_types::u32);

} // namespace gl::group
