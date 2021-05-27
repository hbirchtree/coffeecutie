#pragma once

#include "common.h"

namespace gl::groups {

// PathParameter
enum class path_parameter : ::libc_types::u32 {
#ifdef GL_PATH_CLIENT_LENGTH_NV
    path_client_length_nv = GL_PATH_CLIENT_LENGTH_NV,
#endif
#ifdef GL_PATH_COMMAND_COUNT_NV
    path_command_count_nv = GL_PATH_COMMAND_COUNT_NV,
#endif
#ifdef GL_PATH_COMPUTED_LENGTH_NV
    path_computed_length_nv = GL_PATH_COMPUTED_LENGTH_NV,
#endif
#ifdef GL_PATH_COORD_COUNT_NV
    path_coord_count_nv = GL_PATH_COORD_COUNT_NV,
#endif
#ifdef GL_PATH_DASH_ARRAY_COUNT_NV
    path_dash_array_count_nv = GL_PATH_DASH_ARRAY_COUNT_NV,
#endif
#ifdef GL_PATH_DASH_CAPS_NV
    path_dash_caps_nv = GL_PATH_DASH_CAPS_NV,
#endif
#ifdef GL_PATH_DASH_OFFSET_NV
    path_dash_offset_nv = GL_PATH_DASH_OFFSET_NV,
#endif
#ifdef GL_PATH_DASH_OFFSET_RESET_NV
    path_dash_offset_reset_nv = GL_PATH_DASH_OFFSET_RESET_NV,
#endif
#ifdef GL_PATH_END_CAPS_NV
    path_end_caps_nv = GL_PATH_END_CAPS_NV,
#endif
#ifdef GL_PATH_FILL_BOUNDING_BOX_NV
    path_fill_bounding_box_nv = GL_PATH_FILL_BOUNDING_BOX_NV,
#endif
#ifdef GL_PATH_FILL_COVER_MODE_NV
    path_fill_cover_mode_nv = GL_PATH_FILL_COVER_MODE_NV,
#endif
#ifdef GL_PATH_FILL_MASK_NV
    path_fill_mask_nv = GL_PATH_FILL_MASK_NV,
#endif
#ifdef GL_PATH_FILL_MODE_NV
    path_fill_mode_nv = GL_PATH_FILL_MODE_NV,
#endif
#ifdef GL_PATH_INITIAL_DASH_CAP_NV
    path_initial_dash_cap_nv = GL_PATH_INITIAL_DASH_CAP_NV,
#endif
#ifdef GL_PATH_INITIAL_END_CAP_NV
    path_initial_end_cap_nv = GL_PATH_INITIAL_END_CAP_NV,
#endif
#ifdef GL_PATH_JOIN_STYLE_NV
    path_join_style_nv = GL_PATH_JOIN_STYLE_NV,
#endif
#ifdef GL_PATH_MITER_LIMIT_NV
    path_miter_limit_nv = GL_PATH_MITER_LIMIT_NV,
#endif
#ifdef GL_PATH_OBJECT_BOUNDING_BOX_NV
    path_object_bounding_box_nv = GL_PATH_OBJECT_BOUNDING_BOX_NV,
#endif
#ifdef GL_PATH_STROKE_BOUNDING_BOX_NV
    path_stroke_bounding_box_nv = GL_PATH_STROKE_BOUNDING_BOX_NV,
#endif
#ifdef GL_PATH_STROKE_COVER_MODE_NV
    path_stroke_cover_mode_nv = GL_PATH_STROKE_COVER_MODE_NV,
#endif
#ifdef GL_PATH_STROKE_MASK_NV
    path_stroke_mask_nv = GL_PATH_STROKE_MASK_NV,
#endif
#ifdef GL_PATH_STROKE_WIDTH_NV
    path_stroke_width_nv = GL_PATH_STROKE_WIDTH_NV,
#endif
#ifdef GL_PATH_TERMINAL_DASH_CAP_NV
    path_terminal_dash_cap_nv = GL_PATH_TERMINAL_DASH_CAP_NV,
#endif
#ifdef GL_PATH_TERMINAL_END_CAP_NV
    path_terminal_end_cap_nv = GL_PATH_TERMINAL_END_CAP_NV,
#endif
}; // enum class path_parameter

} // namespace gl::groups
