#pragma once

#include "common.h"

namespace gl::group {

// PerfQueryDataFlags
enum class perf_query_data_flags : u32
{
#ifdef GL_PERFQUERY_DONOT_FLUSH_INTEL
    perfquery_donot_flush_intel = GL_PERFQUERY_DONOT_FLUSH_INTEL,
#endif
#ifdef GL_PERFQUERY_FLUSH_INTEL
    perfquery_flush_intel = GL_PERFQUERY_FLUSH_INTEL,
#endif
#ifdef GL_PERFQUERY_WAIT_INTEL
    perfquery_wait_intel = GL_PERFQUERY_WAIT_INTEL,
#endif
}; // enum class perf_query_data_flags

} // namespace gl::group
