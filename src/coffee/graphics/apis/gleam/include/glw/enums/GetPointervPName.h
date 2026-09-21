#pragma once

#include "common.h"

namespace gl::group {

// GetPointervPName
enum class get_pointerv_prop : u32
{
    color_array_pointer         = 0x8090, // GL_COLOR_ARRAY_POINTER
    debug_callback_function     = 0x8244, // GL_DEBUG_CALLBACK_FUNCTION
    debug_callback_user_param   = 0x8245, // GL_DEBUG_CALLBACK_USER_PARAM
    edge_flag_array_pointer     = 0x8093, // GL_EDGE_FLAG_ARRAY_POINTER
    feedback_buffer_pointer     = 0x0DF0, // GL_FEEDBACK_BUFFER_POINTER
    index_array_pointer         = 0x8091, // GL_INDEX_ARRAY_POINTER
    normal_array_pointer        = 0x808F, // GL_NORMAL_ARRAY_POINTER
    selection_buffer_pointer    = 0x0DF3, // GL_SELECTION_BUFFER_POINTER
    texture_coord_array_pointer = 0x8092, // GL_TEXTURE_COORD_ARRAY_POINTER
    vertex_array_pointer        = 0x808E, // GL_VERTEX_ARRAY_POINTER
    color_array_pointer_ext     = 0x8090, // GL_COLOR_ARRAY_POINTER_EXT
    edge_flag_array_pointer_ext = 0x8093, // GL_EDGE_FLAG_ARRAY_POINTER_EXT
    index_array_pointer_ext     = 0x8091, // GL_INDEX_ARRAY_POINTER_EXT
    instrument_buffer_pointer_sgix =
        0x8180,                        // GL_INSTRUMENT_BUFFER_POINTER_SGIX
    normal_array_pointer_ext = 0x808F, // GL_NORMAL_ARRAY_POINTER_EXT
    texture_coord_array_pointer_ext =
        0x8092,                        // GL_TEXTURE_COORD_ARRAY_POINTER_EXT
    vertex_array_pointer_ext = 0x808E, // GL_VERTEX_ARRAY_POINTER_EXT
}; // enum class get_pointerv_prop

} // namespace gl::group
