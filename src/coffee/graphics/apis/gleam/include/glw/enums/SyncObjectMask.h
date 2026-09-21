#pragma once

#include "common.h"

namespace gl::group {

// SyncObjectMask
enum class sync_object_mask : u32
{
    sync_flush_commands_bit = 0x00000001, // GL_SYNC_FLUSH_COMMANDS_BIT
    sync_flush_commands_bit_apple =
        0x00000001, // GL_SYNC_FLUSH_COMMANDS_BIT_APPLE
}; // enum class sync_object_mask
C_FLAGS(sync_object_mask, u32);

} // namespace gl::group
