#pragma once

#include "common.h"

namespace gl::group {

// PerfQueryDataFlags
enum class perf_query_data_flags : u32
{
    perfquery_donot_flush_intel = 0x83F9, // GL_PERFQUERY_DONOT_FLUSH_INTEL
    perfquery_flush_intel       = 0x83FA, // GL_PERFQUERY_FLUSH_INTEL
    perfquery_wait_intel        = 0x83FB, // GL_PERFQUERY_WAIT_INTEL
}; // enum class perf_query_data_flags

} // namespace gl::group
