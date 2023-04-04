#pragma once

#include "common.h"

namespace gl::group {

// SyncObjectMask
enum class sync_object_mask : u32
{
#ifdef GL_SYNC_FLUSH_COMMANDS_BIT
    sync_flush_commands_bit = GL_SYNC_FLUSH_COMMANDS_BIT,
#endif
#ifdef GL_SYNC_FLUSH_COMMANDS_BIT_APPLE
    sync_flush_commands_bit_apple = GL_SYNC_FLUSH_COMMANDS_BIT_APPLE,
#endif
}; // enum class sync_object_mask
C_FLAGS(sync_object_mask, ::libc_types::u32);

} // namespace gl::group
