#pragma once

#include "common.h"

namespace gl::group {

// PathCoordType
enum class path_coord_type : u32
{
#ifdef GL_ARC_TO_NV
    arc_to_nv = GL_ARC_TO_NV,
#endif
#ifdef GL_CIRCULAR_CCW_ARC_TO_NV
    circular_ccw_arc_to_nv = GL_CIRCULAR_CCW_ARC_TO_NV,
#endif
#ifdef GL_CIRCULAR_CW_ARC_TO_NV
    circular_cw_arc_to_nv = GL_CIRCULAR_CW_ARC_TO_NV,
#endif
#ifdef GL_CIRCULAR_TANGENT_ARC_TO_NV
    circular_tangent_arc_to_nv = GL_CIRCULAR_TANGENT_ARC_TO_NV,
#endif
#ifdef GL_CLOSE_PATH_NV
    close_path_nv = GL_CLOSE_PATH_NV,
#endif
#ifdef GL_CONIC_CURVE_TO_NV
    conic_curve_to_nv = GL_CONIC_CURVE_TO_NV,
#endif
#ifdef GL_CUBIC_CURVE_TO_NV
    cubic_curve_to_nv = GL_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_DUP_FIRST_CUBIC_CURVE_TO_NV
    dup_first_cubic_curve_to_nv = GL_DUP_FIRST_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_DUP_LAST_CUBIC_CURVE_TO_NV
    dup_last_cubic_curve_to_nv = GL_DUP_LAST_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_HORIZONTAL_LINE_TO_NV
    horizontal_line_to_nv = GL_HORIZONTAL_LINE_TO_NV,
#endif
#ifdef GL_LARGE_CCW_ARC_TO_NV
    large_ccw_arc_to_nv = GL_LARGE_CCW_ARC_TO_NV,
#endif
#ifdef GL_LARGE_CW_ARC_TO_NV
    large_cw_arc_to_nv = GL_LARGE_CW_ARC_TO_NV,
#endif
#ifdef GL_LINE_TO_NV
    line_to_nv = GL_LINE_TO_NV,
#endif
#ifdef GL_MOVE_TO_NV
    move_to_nv = GL_MOVE_TO_NV,
#endif
#ifdef GL_QUADRATIC_CURVE_TO_NV
    quadratic_curve_to_nv = GL_QUADRATIC_CURVE_TO_NV,
#endif
#ifdef GL_RECT_NV
    rect_nv = GL_RECT_NV,
#endif
#ifdef GL_RELATIVE_ARC_TO_NV
    relative_arc_to_nv = GL_RELATIVE_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_CONIC_CURVE_TO_NV
    relative_conic_curve_to_nv = GL_RELATIVE_CONIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_CUBIC_CURVE_TO_NV
    relative_cubic_curve_to_nv = GL_RELATIVE_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_HORIZONTAL_LINE_TO_NV
    relative_horizontal_line_to_nv = GL_RELATIVE_HORIZONTAL_LINE_TO_NV,
#endif
#ifdef GL_RELATIVE_LARGE_CCW_ARC_TO_NV
    relative_large_ccw_arc_to_nv = GL_RELATIVE_LARGE_CCW_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_LARGE_CW_ARC_TO_NV
    relative_large_cw_arc_to_nv = GL_RELATIVE_LARGE_CW_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_LINE_TO_NV
    relative_line_to_nv = GL_RELATIVE_LINE_TO_NV,
#endif
#ifdef GL_RELATIVE_MOVE_TO_NV
    relative_move_to_nv = GL_RELATIVE_MOVE_TO_NV,
#endif
#ifdef GL_RELATIVE_QUADRATIC_CURVE_TO_NV
    relative_quadratic_curve_to_nv = GL_RELATIVE_QUADRATIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_RECT_NV
    relative_rect_nv = GL_RELATIVE_RECT_NV,
#endif
#ifdef GL_RELATIVE_ROUNDED_RECT2_NV
    relative_rounded_rect2_nv = GL_RELATIVE_ROUNDED_RECT2_NV,
#endif
#ifdef GL_RELATIVE_ROUNDED_RECT4_NV
    relative_rounded_rect4_nv = GL_RELATIVE_ROUNDED_RECT4_NV,
#endif
#ifdef GL_RELATIVE_ROUNDED_RECT8_NV
    relative_rounded_rect8_nv = GL_RELATIVE_ROUNDED_RECT8_NV,
#endif
#ifdef GL_RELATIVE_ROUNDED_RECT_NV
    relative_rounded_rect_nv = GL_RELATIVE_ROUNDED_RECT_NV,
#endif
#ifdef GL_RELATIVE_SMALL_CCW_ARC_TO_NV
    relative_small_ccw_arc_to_nv = GL_RELATIVE_SMALL_CCW_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_SMALL_CW_ARC_TO_NV
    relative_small_cw_arc_to_nv = GL_RELATIVE_SMALL_CW_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV
    relative_smooth_cubic_curve_to_nv = GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV
    relative_smooth_quadratic_curve_to_nv =
        GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_VERTICAL_LINE_TO_NV
    relative_vertical_line_to_nv = GL_RELATIVE_VERTICAL_LINE_TO_NV,
#endif
#ifdef GL_RESTART_PATH_NV
    restart_path_nv = GL_RESTART_PATH_NV,
#endif
#ifdef GL_ROUNDED_RECT2_NV
    rounded_rect2_nv = GL_ROUNDED_RECT2_NV,
#endif
#ifdef GL_ROUNDED_RECT4_NV
    rounded_rect4_nv = GL_ROUNDED_RECT4_NV,
#endif
#ifdef GL_ROUNDED_RECT8_NV
    rounded_rect8_nv = GL_ROUNDED_RECT8_NV,
#endif
#ifdef GL_ROUNDED_RECT_NV
    rounded_rect_nv = GL_ROUNDED_RECT_NV,
#endif
#ifdef GL_SMALL_CCW_ARC_TO_NV
    small_ccw_arc_to_nv = GL_SMALL_CCW_ARC_TO_NV,
#endif
#ifdef GL_SMALL_CW_ARC_TO_NV
    small_cw_arc_to_nv = GL_SMALL_CW_ARC_TO_NV,
#endif
#ifdef GL_SMOOTH_CUBIC_CURVE_TO_NV
    smooth_cubic_curve_to_nv = GL_SMOOTH_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_SMOOTH_QUADRATIC_CURVE_TO_NV
    smooth_quadratic_curve_to_nv = GL_SMOOTH_QUADRATIC_CURVE_TO_NV,
#endif
#ifdef GL_VERTICAL_LINE_TO_NV
    vertical_line_to_nv = GL_VERTICAL_LINE_TO_NV,
#endif
}; // enum class path_coord_type

} // namespace gl::group
