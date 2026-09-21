#pragma once

#include "common.h"

namespace gl::group {

// CommandOpcodesNV
enum class command_opcodes_nv : u32
{
    alpha_ref_command_nv         = 0x000F, // GL_ALPHA_REF_COMMAND_NV
    attribute_address_command_nv = 0x0009, // GL_ATTRIBUTE_ADDRESS_COMMAND_NV
    blend_color_command_nv       = 0x000B, // GL_BLEND_COLOR_COMMAND_NV
    draw_arrays_command_nv       = 0x0003, // GL_DRAW_ARRAYS_COMMAND_NV
    draw_arrays_instanced_command_nv =
        0x0007, // GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV
    draw_arrays_strip_command_nv = 0x0005, // GL_DRAW_ARRAYS_STRIP_COMMAND_NV
    draw_elements_command_nv     = 0x0002, // GL_DRAW_ELEMENTS_COMMAND_NV
    draw_elements_instanced_command_nv =
        0x0006, // GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV
    draw_elements_strip_command_nv =
        0x0004,                             // GL_DRAW_ELEMENTS_STRIP_COMMAND_NV
    element_address_command_nv    = 0x0008, // GL_ELEMENT_ADDRESS_COMMAND_NV
    front_face_command_nv         = 0x0012, // GL_FRONT_FACE_COMMAND_NV
    line_width_command_nv         = 0x000D, // GL_LINE_WIDTH_COMMAND_NV
    nop_command_nv                = 0x0001, // GL_NOP_COMMAND_NV
    polygon_offset_command_nv     = 0x000E, // GL_POLYGON_OFFSET_COMMAND_NV
    scissor_command_nv            = 0x0011, // GL_SCISSOR_COMMAND_NV
    stencil_ref_command_nv        = 0x000C, // GL_STENCIL_REF_COMMAND_NV
    terminate_sequence_command_nv = 0x0000, // GL_TERMINATE_SEQUENCE_COMMAND_NV
    uniform_address_command_nv    = 0x000A, // GL_UNIFORM_ADDRESS_COMMAND_NV
    viewport_command_nv           = 0x0010, // GL_VIEWPORT_COMMAND_NV
}; // enum class command_opcodes_nv

} // namespace gl::group
