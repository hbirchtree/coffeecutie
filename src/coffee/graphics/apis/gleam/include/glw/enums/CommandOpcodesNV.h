#pragma once

#include "common.h"

namespace gl::group {

// CommandOpcodesNV
enum class command_opcodes_nv : ::libc_types::u32
{
#ifdef GL_ALPHA_REF_COMMAND_NV
    alpha_ref_command_nv = GL_ALPHA_REF_COMMAND_NV,
#endif
#ifdef GL_ATTRIBUTE_ADDRESS_COMMAND_NV
    attribute_address_command_nv = GL_ATTRIBUTE_ADDRESS_COMMAND_NV,
#endif
#ifdef GL_BLEND_COLOR_COMMAND_NV
    blend_color_command_nv = GL_BLEND_COLOR_COMMAND_NV,
#endif
#ifdef GL_DRAW_ARRAYS_COMMAND_NV
    draw_arrays_command_nv = GL_DRAW_ARRAYS_COMMAND_NV,
#endif
#ifdef GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV
    draw_arrays_instanced_command_nv = GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV,
#endif
#ifdef GL_DRAW_ARRAYS_STRIP_COMMAND_NV
    draw_arrays_strip_command_nv = GL_DRAW_ARRAYS_STRIP_COMMAND_NV,
#endif
#ifdef GL_DRAW_ELEMENTS_COMMAND_NV
    draw_elements_command_nv = GL_DRAW_ELEMENTS_COMMAND_NV,
#endif
#ifdef GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV
    draw_elements_instanced_command_nv = GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV,
#endif
#ifdef GL_DRAW_ELEMENTS_STRIP_COMMAND_NV
    draw_elements_strip_command_nv = GL_DRAW_ELEMENTS_STRIP_COMMAND_NV,
#endif
#ifdef GL_ELEMENT_ADDRESS_COMMAND_NV
    element_address_command_nv = GL_ELEMENT_ADDRESS_COMMAND_NV,
#endif
#ifdef GL_FRONT_FACE_COMMAND_NV
    front_face_command_nv = GL_FRONT_FACE_COMMAND_NV,
#endif
#ifdef GL_LINE_WIDTH_COMMAND_NV
    line_width_command_nv = GL_LINE_WIDTH_COMMAND_NV,
#endif
#ifdef GL_NOP_COMMAND_NV
    nop_command_nv = GL_NOP_COMMAND_NV,
#endif
#ifdef GL_POLYGON_OFFSET_COMMAND_NV
    polygon_offset_command_nv = GL_POLYGON_OFFSET_COMMAND_NV,
#endif
#ifdef GL_SCISSOR_COMMAND_NV
    scissor_command_nv = GL_SCISSOR_COMMAND_NV,
#endif
#ifdef GL_STENCIL_REF_COMMAND_NV
    stencil_ref_command_nv = GL_STENCIL_REF_COMMAND_NV,
#endif
#ifdef GL_TERMINATE_SEQUENCE_COMMAND_NV
    terminate_sequence_command_nv = GL_TERMINATE_SEQUENCE_COMMAND_NV,
#endif
#ifdef GL_UNIFORM_ADDRESS_COMMAND_NV
    uniform_address_command_nv = GL_UNIFORM_ADDRESS_COMMAND_NV,
#endif
#ifdef GL_VIEWPORT_COMMAND_NV
    viewport_command_nv = GL_VIEWPORT_COMMAND_NV,
#endif
}; // enum class command_opcodes_nv

} // namespace gl::group
