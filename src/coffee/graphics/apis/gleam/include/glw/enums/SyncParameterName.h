#pragma once

#include "common.h"

namespace gl::group {

// SyncParameterName
enum class sync_parameter_name : u32
{
    object_type    = 0x9112, // GL_OBJECT_TYPE
    sync_condition = 0x9113, // GL_SYNC_CONDITION
    sync_flags     = 0x9115, // GL_SYNC_FLAGS
    sync_status    = 0x9114, // GL_SYNC_STATUS
}; // enum class sync_parameter_name

} // namespace gl::group
