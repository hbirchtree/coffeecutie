#pragma once

#include "common.h"

namespace gl::group {

// SyncStatus
enum class sync_status : ::libc_types::u32
{
#ifdef GL_ALREADY_SIGNALED
    already_signaled = GL_ALREADY_SIGNALED,
#endif
#ifdef GL_CONDITION_SATISFIED
    condition_satisfied = GL_CONDITION_SATISFIED,
#endif
#ifdef GL_TIMEOUT_EXPIRED
    timeout_expired = GL_TIMEOUT_EXPIRED,
#endif
#ifdef GL_WAIT_FAILED
    wait_failed = GL_WAIT_FAILED,
#endif
}; // enum class sync_status

} // namespace gl::group
