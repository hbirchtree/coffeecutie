#pragma once

#include "common.h"

namespace gl::group {

// PathParameter
enum class path_parameter : u32
{
    path_client_length_nv       = 0x907F, // GL_PATH_CLIENT_LENGTH_NV
    path_command_count_nv       = 0x909D, // GL_PATH_COMMAND_COUNT_NV
    path_computed_length_nv     = 0x90A0, // GL_PATH_COMPUTED_LENGTH_NV
    path_coord_count_nv         = 0x909E, // GL_PATH_COORD_COUNT_NV
    path_dash_array_count_nv    = 0x909F, // GL_PATH_DASH_ARRAY_COUNT_NV
    path_dash_caps_nv           = 0x907B, // GL_PATH_DASH_CAPS_NV
    path_dash_offset_nv         = 0x907E, // GL_PATH_DASH_OFFSET_NV
    path_dash_offset_reset_nv   = 0x90B4, // GL_PATH_DASH_OFFSET_RESET_NV
    path_end_caps_nv            = 0x9076, // GL_PATH_END_CAPS_NV
    path_fill_bounding_box_nv   = 0x90A1, // GL_PATH_FILL_BOUNDING_BOX_NV
    path_fill_cover_mode_nv     = 0x9082, // GL_PATH_FILL_COVER_MODE_NV
    path_fill_mask_nv           = 0x9081, // GL_PATH_FILL_MASK_NV
    path_fill_mode_nv           = 0x9080, // GL_PATH_FILL_MODE_NV
    path_initial_dash_cap_nv    = 0x907C, // GL_PATH_INITIAL_DASH_CAP_NV
    path_initial_end_cap_nv     = 0x9077, // GL_PATH_INITIAL_END_CAP_NV
    path_join_style_nv          = 0x9079, // GL_PATH_JOIN_STYLE_NV
    path_miter_limit_nv         = 0x907A, // GL_PATH_MITER_LIMIT_NV
    path_object_bounding_box_nv = 0x908A, // GL_PATH_OBJECT_BOUNDING_BOX_NV
    path_stroke_bounding_box_nv = 0x90A2, // GL_PATH_STROKE_BOUNDING_BOX_NV
    path_stroke_cover_mode_nv   = 0x9083, // GL_PATH_STROKE_COVER_MODE_NV
    path_stroke_mask_nv         = 0x9084, // GL_PATH_STROKE_MASK_NV
    path_stroke_width_nv        = 0x9075, // GL_PATH_STROKE_WIDTH_NV
    path_terminal_dash_cap_nv   = 0x907D, // GL_PATH_TERMINAL_DASH_CAP_NV
    path_terminal_end_cap_nv    = 0x9078, // GL_PATH_TERMINAL_END_CAP_NV
}; // enum class path_parameter

} // namespace gl::group
