#pragma once

#include "common.h"

namespace gl::groups {

// PrimitiveType
enum class primitive_type : ::libc_types::u32 {
#ifdef GL_LINES
    lines = GL_LINES,
#endif
#ifdef GL_LINES_ADJACENCY
    lines_adjacency = GL_LINES_ADJACENCY,
#endif
#ifdef GL_LINES_ADJACENCY_ARB
    lines_adjacency_arb = GL_LINES_ADJACENCY_ARB,
#endif
#ifdef GL_LINES_ADJACENCY_EXT
    lines_adjacency_ext = GL_LINES_ADJACENCY_EXT,
#endif
#ifdef GL_LINE_LOOP
    line_loop = GL_LINE_LOOP,
#endif
#ifdef GL_LINE_STRIP
    line_strip = GL_LINE_STRIP,
#endif
#ifdef GL_LINE_STRIP_ADJACENCY
    line_strip_adjacency = GL_LINE_STRIP_ADJACENCY,
#endif
#ifdef GL_LINE_STRIP_ADJACENCY_ARB
    line_strip_adjacency_arb = GL_LINE_STRIP_ADJACENCY_ARB,
#endif
#ifdef GL_LINE_STRIP_ADJACENCY_EXT
    line_strip_adjacency_ext = GL_LINE_STRIP_ADJACENCY_EXT,
#endif
#ifdef GL_PATCHES
    patches = GL_PATCHES,
#endif
#ifdef GL_PATCHES_EXT
    patches_ext = GL_PATCHES_EXT,
#endif
#ifdef GL_POINTS
    points = GL_POINTS,
#endif
#ifdef GL_POLYGON
    polygon = GL_POLYGON,
#endif
#ifdef GL_QUADS
    quads = GL_QUADS,
#endif
#ifdef GL_QUADS_EXT
    quads_ext = GL_QUADS_EXT,
#endif
#ifdef GL_QUAD_STRIP
    quad_strip = GL_QUAD_STRIP,
#endif
#ifdef GL_TRIANGLES
    triangles = GL_TRIANGLES,
#endif
#ifdef GL_TRIANGLES_ADJACENCY
    triangles_adjacency = GL_TRIANGLES_ADJACENCY,
#endif
#ifdef GL_TRIANGLES_ADJACENCY_ARB
    triangles_adjacency_arb = GL_TRIANGLES_ADJACENCY_ARB,
#endif
#ifdef GL_TRIANGLES_ADJACENCY_EXT
    triangles_adjacency_ext = GL_TRIANGLES_ADJACENCY_EXT,
#endif
#ifdef GL_TRIANGLE_FAN
    triangle_fan = GL_TRIANGLE_FAN,
#endif
#ifdef GL_TRIANGLE_STRIP
    triangle_strip = GL_TRIANGLE_STRIP,
#endif
#ifdef GL_TRIANGLE_STRIP_ADJACENCY
    triangle_strip_adjacency = GL_TRIANGLE_STRIP_ADJACENCY,
#endif
#ifdef GL_TRIANGLE_STRIP_ADJACENCY_ARB
    triangle_strip_adjacency_arb = GL_TRIANGLE_STRIP_ADJACENCY_ARB,
#endif
#ifdef GL_TRIANGLE_STRIP_ADJACENCY_EXT
    triangle_strip_adjacency_ext = GL_TRIANGLE_STRIP_ADJACENCY_EXT,
#endif
}; // enum class primitive_type

} // namespace gl::groups
