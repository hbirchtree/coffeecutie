#pragma once

#include "common.h"

namespace gl::group {

// SyncParameterName
enum class sync_parameter_name : ::libc_types::u32
{
#ifdef GL_OBJECT_TYPE
    object_type = GL_OBJECT_TYPE,
#endif
#ifdef GL_SYNC_CONDITION
    sync_condition = GL_SYNC_CONDITION,
#endif
#ifdef GL_SYNC_FLAGS
    sync_flags = GL_SYNC_FLAGS,
#endif
#ifdef GL_SYNC_STATUS
    sync_status = GL_SYNC_STATUS,
#endif
}; // enum class sync_parameter_name

} // namespace gl::group
