#pragma once

#include "common.h"

namespace gl::group {

// SyncStatus
enum class sync_status : u32
{
    already_signaled    = 0x911A, // GL_ALREADY_SIGNALED
    condition_satisfied = 0x911C, // GL_CONDITION_SATISFIED
    timeout_expired     = 0x911B, // GL_TIMEOUT_EXPIRED
    wait_failed         = 0x911D, // GL_WAIT_FAILED
}; // enum class sync_status

} // namespace gl::group
