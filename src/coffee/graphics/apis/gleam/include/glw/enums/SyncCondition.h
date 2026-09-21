#pragma once

#include "common.h"

namespace gl::group {

// SyncCondition
enum class sync_condition : u32
{
    sync_gpu_commands_complete = 0x9117, // GL_SYNC_GPU_COMMANDS_COMPLETE
}; // enum class sync_condition

} // namespace gl::group
