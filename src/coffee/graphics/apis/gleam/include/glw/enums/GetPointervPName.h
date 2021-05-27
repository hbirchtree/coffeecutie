#pragma once

#include "common.h"

namespace gl::groups {

// GetPointervPName
enum class get_pointerv_prop : ::libc_types::u32 {
#ifdef GL_COLOR_ARRAY_POINTER
    color_array_pointer = GL_COLOR_ARRAY_POINTER,
#endif
#ifdef GL_COLOR_ARRAY_POINTER_EXT
    color_array_pointer_ext = GL_COLOR_ARRAY_POINTER_EXT,
#endif
#ifdef GL_DEBUG_CALLBACK_FUNCTION
    debug_callback_function = GL_DEBUG_CALLBACK_FUNCTION,
#endif
#ifdef GL_DEBUG_CALLBACK_USER_PARAM
    debug_callback_user_param = GL_DEBUG_CALLBACK_USER_PARAM,
#endif
#ifdef GL_EDGE_FLAG_ARRAY_POINTER
    edge_flag_array_pointer = GL_EDGE_FLAG_ARRAY_POINTER,
#endif
#ifdef GL_EDGE_FLAG_ARRAY_POINTER_EXT
    edge_flag_array_pointer_ext = GL_EDGE_FLAG_ARRAY_POINTER_EXT,
#endif
#ifdef GL_FEEDBACK_BUFFER_POINTER
    feedback_buffer_pointer = GL_FEEDBACK_BUFFER_POINTER,
#endif
#ifdef GL_INDEX_ARRAY_POINTER
    index_array_pointer = GL_INDEX_ARRAY_POINTER,
#endif
#ifdef GL_INDEX_ARRAY_POINTER_EXT
    index_array_pointer_ext = GL_INDEX_ARRAY_POINTER_EXT,
#endif
#ifdef GL_INSTRUMENT_BUFFER_POINTER_SGIX
    instrument_buffer_pointer_sgix = GL_INSTRUMENT_BUFFER_POINTER_SGIX,
#endif
#ifdef GL_NORMAL_ARRAY_POINTER
    normal_array_pointer = GL_NORMAL_ARRAY_POINTER,
#endif
#ifdef GL_NORMAL_ARRAY_POINTER_EXT
    normal_array_pointer_ext = GL_NORMAL_ARRAY_POINTER_EXT,
#endif
#ifdef GL_SELECTION_BUFFER_POINTER
    selection_buffer_pointer = GL_SELECTION_BUFFER_POINTER,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY_POINTER
    texture_coord_array_pointer = GL_TEXTURE_COORD_ARRAY_POINTER,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY_POINTER_EXT
    texture_coord_array_pointer_ext = GL_TEXTURE_COORD_ARRAY_POINTER_EXT,
#endif
#ifdef GL_VERTEX_ARRAY_POINTER
    vertex_array_pointer = GL_VERTEX_ARRAY_POINTER,
#endif
#ifdef GL_VERTEX_ARRAY_POINTER_EXT
    vertex_array_pointer_ext = GL_VERTEX_ARRAY_POINTER_EXT,
#endif
}; // enum class get_pointerv_prop

} // namespace gl::groups
