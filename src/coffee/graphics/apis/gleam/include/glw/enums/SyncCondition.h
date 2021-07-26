#pragma once

#include "common.h"

namespace gl::group {

// SyncCondition
enum class sync_condition : ::libc_types::u32
{
#ifdef GL_SYNC_GPU_COMMANDS_COMPLETE
    sync_gpu_commands_complete = GL_SYNC_GPU_COMMANDS_COMPLETE,
#endif
}; // enum class sync_condition

} // namespace gl::group
