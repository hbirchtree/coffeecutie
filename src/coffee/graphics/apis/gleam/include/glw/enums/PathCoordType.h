#pragma once

#include "common.h"

namespace gl::group {

// PathCoordType
enum class path_coord_type : u32
{
    arc_to_nv                      = 0xFE, // GL_ARC_TO_NV
    circular_ccw_arc_to_nv         = 0xF8, // GL_CIRCULAR_CCW_ARC_TO_NV
    circular_cw_arc_to_nv          = 0xFA, // GL_CIRCULAR_CW_ARC_TO_NV
    circular_tangent_arc_to_nv     = 0xFC, // GL_CIRCULAR_TANGENT_ARC_TO_NV
    close_path_nv                  = 0x00, // GL_CLOSE_PATH_NV
    conic_curve_to_nv              = 0x1A, // GL_CONIC_CURVE_TO_NV
    cubic_curve_to_nv              = 0x0C, // GL_CUBIC_CURVE_TO_NV
    dup_first_cubic_curve_to_nv    = 0xF2, // GL_DUP_FIRST_CUBIC_CURVE_TO_NV
    dup_last_cubic_curve_to_nv     = 0xF4, // GL_DUP_LAST_CUBIC_CURVE_TO_NV
    horizontal_line_to_nv          = 0x06, // GL_HORIZONTAL_LINE_TO_NV
    large_ccw_arc_to_nv            = 0x16, // GL_LARGE_CCW_ARC_TO_NV
    large_cw_arc_to_nv             = 0x18, // GL_LARGE_CW_ARC_TO_NV
    line_to_nv                     = 0x04, // GL_LINE_TO_NV
    move_to_nv                     = 0x02, // GL_MOVE_TO_NV
    quadratic_curve_to_nv          = 0x0A, // GL_QUADRATIC_CURVE_TO_NV
    rect_nv                        = 0xF6, // GL_RECT_NV
    relative_arc_to_nv             = 0xFF, // GL_RELATIVE_ARC_TO_NV
    relative_conic_curve_to_nv     = 0x1B, // GL_RELATIVE_CONIC_CURVE_TO_NV
    relative_cubic_curve_to_nv     = 0x0D, // GL_RELATIVE_CUBIC_CURVE_TO_NV
    relative_horizontal_line_to_nv = 0x07, // GL_RELATIVE_HORIZONTAL_LINE_TO_NV
    relative_large_ccw_arc_to_nv   = 0x17, // GL_RELATIVE_LARGE_CCW_ARC_TO_NV
    relative_large_cw_arc_to_nv    = 0x19, // GL_RELATIVE_LARGE_CW_ARC_TO_NV
    relative_line_to_nv            = 0x05, // GL_RELATIVE_LINE_TO_NV
    relative_move_to_nv            = 0x03, // GL_RELATIVE_MOVE_TO_NV
    relative_quadratic_curve_to_nv = 0x0B, // GL_RELATIVE_QUADRATIC_CURVE_TO_NV
    relative_rect_nv               = 0xF7, // GL_RELATIVE_RECT_NV
    relative_rounded_rect2_nv      = 0xEB, // GL_RELATIVE_ROUNDED_RECT2_NV
    relative_rounded_rect4_nv      = 0xED, // GL_RELATIVE_ROUNDED_RECT4_NV
    relative_rounded_rect8_nv      = 0xEF, // GL_RELATIVE_ROUNDED_RECT8_NV
    relative_rounded_rect_nv       = 0xE9, // GL_RELATIVE_ROUNDED_RECT_NV
    relative_small_ccw_arc_to_nv   = 0x13, // GL_RELATIVE_SMALL_CCW_ARC_TO_NV
    relative_small_cw_arc_to_nv    = 0x15, // GL_RELATIVE_SMALL_CW_ARC_TO_NV
    relative_smooth_cubic_curve_to_nv =
        0x11, // GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV
    relative_smooth_quadratic_curve_to_nv =
        0x0F, // GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV
    relative_vertical_line_to_nv = 0x09, // GL_RELATIVE_VERTICAL_LINE_TO_NV
    restart_path_nv              = 0xF0, // GL_RESTART_PATH_NV
    rounded_rect2_nv             = 0xEA, // GL_ROUNDED_RECT2_NV
    rounded_rect4_nv             = 0xEC, // GL_ROUNDED_RECT4_NV
    rounded_rect8_nv             = 0xEE, // GL_ROUNDED_RECT8_NV
    rounded_rect_nv              = 0xE8, // GL_ROUNDED_RECT_NV
    small_ccw_arc_to_nv          = 0x12, // GL_SMALL_CCW_ARC_TO_NV
    small_cw_arc_to_nv           = 0x14, // GL_SMALL_CW_ARC_TO_NV
    smooth_cubic_curve_to_nv     = 0x10, // GL_SMOOTH_CUBIC_CURVE_TO_NV
    smooth_quadratic_curve_to_nv = 0x0E, // GL_SMOOTH_QUADRATIC_CURVE_TO_NV
    vertical_line_to_nv          = 0x08, // GL_VERTICAL_LINE_TO_NV
}; // enum class path_coord_type

} // namespace gl::group
