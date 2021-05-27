#pragma once

#include "common.h"

namespace gl::groups {

// PathMetricMask
enum class path_metric_mask : ::libc_types::u32 {
#ifdef GL_FONT_ASCENDER_BIT_NV
    font_ascender_bit_nv = GL_FONT_ASCENDER_BIT_NV,
#endif
#ifdef GL_FONT_DESCENDER_BIT_NV
    font_descender_bit_nv = GL_FONT_DESCENDER_BIT_NV,
#endif
#ifdef GL_FONT_HAS_KERNING_BIT_NV
    font_has_kerning_bit_nv = GL_FONT_HAS_KERNING_BIT_NV,
#endif
#ifdef GL_FONT_HEIGHT_BIT_NV
    font_height_bit_nv = GL_FONT_HEIGHT_BIT_NV,
#endif
#ifdef GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV
    font_max_advance_height_bit_nv = GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV,
#endif
#ifdef GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV
    font_max_advance_width_bit_nv = GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV,
#endif
#ifdef GL_FONT_NUM_GLYPH_INDICES_BIT_NV
    font_num_glyph_indices_bit_nv = GL_FONT_NUM_GLYPH_INDICES_BIT_NV,
#endif
#ifdef GL_FONT_UNDERLINE_POSITION_BIT_NV
    font_underline_position_bit_nv = GL_FONT_UNDERLINE_POSITION_BIT_NV,
#endif
#ifdef GL_FONT_UNDERLINE_THICKNESS_BIT_NV
    font_underline_thickness_bit_nv = GL_FONT_UNDERLINE_THICKNESS_BIT_NV,
#endif
#ifdef GL_FONT_UNITS_PER_EM_BIT_NV
    font_units_per_em_bit_nv = GL_FONT_UNITS_PER_EM_BIT_NV,
#endif
#ifdef GL_FONT_X_MAX_BOUNDS_BIT_NV
    font_x_max_bounds_bit_nv = GL_FONT_X_MAX_BOUNDS_BIT_NV,
#endif
#ifdef GL_FONT_X_MIN_BOUNDS_BIT_NV
    font_x_min_bounds_bit_nv = GL_FONT_X_MIN_BOUNDS_BIT_NV,
#endif
#ifdef GL_FONT_Y_MAX_BOUNDS_BIT_NV
    font_y_max_bounds_bit_nv = GL_FONT_Y_MAX_BOUNDS_BIT_NV,
#endif
#ifdef GL_FONT_Y_MIN_BOUNDS_BIT_NV
    font_y_min_bounds_bit_nv = GL_FONT_Y_MIN_BOUNDS_BIT_NV,
#endif
#ifdef GL_GLYPH_HAS_KERNING_BIT_NV
    glyph_has_kerning_bit_nv = GL_GLYPH_HAS_KERNING_BIT_NV,
#endif
#ifdef GL_GLYPH_HEIGHT_BIT_NV
    glyph_height_bit_nv = GL_GLYPH_HEIGHT_BIT_NV,
#endif
#ifdef GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV
    glyph_horizontal_bearing_advance_bit_nv = GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV,
#endif
#ifdef GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV
    glyph_horizontal_bearing_x_bit_nv = GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV,
#endif
#ifdef GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV
    glyph_horizontal_bearing_y_bit_nv = GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV,
#endif
#ifdef GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV
    glyph_vertical_bearing_advance_bit_nv = GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV,
#endif
#ifdef GL_GLYPH_VERTICAL_BEARING_X_BIT_NV
    glyph_vertical_bearing_x_bit_nv = GL_GLYPH_VERTICAL_BEARING_X_BIT_NV,
#endif
#ifdef GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV
    glyph_vertical_bearing_y_bit_nv = GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV,
#endif
#ifdef GL_GLYPH_WIDTH_BIT_NV
    glyph_width_bit_nv = GL_GLYPH_WIDTH_BIT_NV,
#endif
}; // enum class path_metric_mask
C_FLAGS(path_metric_mask, ::libc_types::u32);

} // namespace gl::groups
