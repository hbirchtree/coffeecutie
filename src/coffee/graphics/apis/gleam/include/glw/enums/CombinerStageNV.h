#pragma once

#include "common.h"

namespace gl::group {

// CombinerStageNV
enum class combiner_stage_nv : u32
{
    combiner0_nv = 0x8550, // GL_COMBINER0_NV
    combiner1_nv = 0x8551, // GL_COMBINER1_NV
    combiner2_nv = 0x8552, // GL_COMBINER2_NV
    combiner3_nv = 0x8553, // GL_COMBINER3_NV
    combiner4_nv = 0x8554, // GL_COMBINER4_NV
    combiner5_nv = 0x8555, // GL_COMBINER5_NV
    combiner6_nv = 0x8556, // GL_COMBINER6_NV
    combiner7_nv = 0x8557, // GL_COMBINER7_NV
}; // enum class combiner_stage_nv

} // namespace gl::group
