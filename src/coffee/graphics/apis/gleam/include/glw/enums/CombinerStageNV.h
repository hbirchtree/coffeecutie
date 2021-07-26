#pragma once

#include "common.h"

namespace gl::group {

// CombinerStageNV
enum class combiner_stage_nv : ::libc_types::u32
{
#ifdef GL_COMBINER0_NV
    combiner0_nv = GL_COMBINER0_NV,
#endif
#ifdef GL_COMBINER1_NV
    combiner1_nv = GL_COMBINER1_NV,
#endif
#ifdef GL_COMBINER2_NV
    combiner2_nv = GL_COMBINER2_NV,
#endif
#ifdef GL_COMBINER3_NV
    combiner3_nv = GL_COMBINER3_NV,
#endif
#ifdef GL_COMBINER4_NV
    combiner4_nv = GL_COMBINER4_NV,
#endif
#ifdef GL_COMBINER5_NV
    combiner5_nv = GL_COMBINER5_NV,
#endif
#ifdef GL_COMBINER6_NV
    combiner6_nv = GL_COMBINER6_NV,
#endif
#ifdef GL_COMBINER7_NV
    combiner7_nv = GL_COMBINER7_NV,
#endif
}; // enum class combiner_stage_nv

} // namespace gl::group
