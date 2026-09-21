#pragma once

#include "common.h"

namespace gl::group {

// PrimitiveType
enum class primitive_type : u32
{
    lines                        = 0x0001, // GL_LINES
    lines_adjacency              = 0x000A, // GL_LINES_ADJACENCY
    line_loop                    = 0x0002, // GL_LINE_LOOP
    line_strip                   = 0x0003, // GL_LINE_STRIP
    line_strip_adjacency         = 0x000B, // GL_LINE_STRIP_ADJACENCY
    patches                      = 0x000E, // GL_PATCHES
    points                       = 0x0000, // GL_POINTS
    polygon                      = 0x0009, // GL_POLYGON
    quads                        = 0x0007, // GL_QUADS
    quad_strip                   = 0x0008, // GL_QUAD_STRIP
    triangles                    = 0x0004, // GL_TRIANGLES
    triangles_adjacency          = 0x000C, // GL_TRIANGLES_ADJACENCY
    triangle_fan                 = 0x0006, // GL_TRIANGLE_FAN
    triangle_strip               = 0x0005, // GL_TRIANGLE_STRIP
    triangle_strip_adjacency     = 0x000D, // GL_TRIANGLE_STRIP_ADJACENCY
    lines_adjacency_arb          = 0x000A, // GL_LINES_ADJACENCY_ARB
    lines_adjacency_ext          = 0x000A, // GL_LINES_ADJACENCY_EXT
    line_strip_adjacency_arb     = 0x000B, // GL_LINE_STRIP_ADJACENCY_ARB
    line_strip_adjacency_ext     = 0x000B, // GL_LINE_STRIP_ADJACENCY_EXT
    patches_ext                  = 0x000E, // GL_PATCHES_EXT
    quads_ext                    = 0x0007, // GL_QUADS_EXT
    triangles_adjacency_arb      = 0x000C, // GL_TRIANGLES_ADJACENCY_ARB
    triangles_adjacency_ext      = 0x000C, // GL_TRIANGLES_ADJACENCY_EXT
    triangle_strip_adjacency_arb = 0x000D, // GL_TRIANGLE_STRIP_ADJACENCY_ARB
    triangle_strip_adjacency_ext = 0x000D, // GL_TRIANGLE_STRIP_ADJACENCY_EXT
}; // enum class primitive_type

} // namespace gl::group
