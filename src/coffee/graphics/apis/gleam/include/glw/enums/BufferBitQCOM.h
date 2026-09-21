#pragma once

#include "common.h"

namespace gl::group {

// BufferBitQCOM
enum class buffer_bit_qcom : u32
{
    color_buffer_bit0_qcom = 0x00000001, // GL_COLOR_BUFFER_BIT0_QCOM
    color_buffer_bit1_qcom = 0x00000002, // GL_COLOR_BUFFER_BIT1_QCOM
    color_buffer_bit2_qcom = 0x00000004, // GL_COLOR_BUFFER_BIT2_QCOM
    color_buffer_bit3_qcom = 0x00000008, // GL_COLOR_BUFFER_BIT3_QCOM
    color_buffer_bit4_qcom = 0x00000010, // GL_COLOR_BUFFER_BIT4_QCOM
    color_buffer_bit5_qcom = 0x00000020, // GL_COLOR_BUFFER_BIT5_QCOM
    color_buffer_bit6_qcom = 0x00000040, // GL_COLOR_BUFFER_BIT6_QCOM
    color_buffer_bit7_qcom = 0x00000080, // GL_COLOR_BUFFER_BIT7_QCOM
    depth_buffer_bit0_qcom = 0x00000100, // GL_DEPTH_BUFFER_BIT0_QCOM
    depth_buffer_bit1_qcom = 0x00000200, // GL_DEPTH_BUFFER_BIT1_QCOM
    depth_buffer_bit2_qcom = 0x00000400, // GL_DEPTH_BUFFER_BIT2_QCOM
    depth_buffer_bit3_qcom = 0x00000800, // GL_DEPTH_BUFFER_BIT3_QCOM
    depth_buffer_bit4_qcom = 0x00001000, // GL_DEPTH_BUFFER_BIT4_QCOM
    depth_buffer_bit5_qcom = 0x00002000, // GL_DEPTH_BUFFER_BIT5_QCOM
    depth_buffer_bit6_qcom = 0x00004000, // GL_DEPTH_BUFFER_BIT6_QCOM
    depth_buffer_bit7_qcom = 0x00008000, // GL_DEPTH_BUFFER_BIT7_QCOM
    multisample_buffer_bit0_qcom =
        0x01000000, // GL_MULTISAMPLE_BUFFER_BIT0_QCOM
    multisample_buffer_bit1_qcom =
        0x02000000, // GL_MULTISAMPLE_BUFFER_BIT1_QCOM
    multisample_buffer_bit2_qcom =
        0x04000000, // GL_MULTISAMPLE_BUFFER_BIT2_QCOM
    multisample_buffer_bit3_qcom =
        0x08000000, // GL_MULTISAMPLE_BUFFER_BIT3_QCOM
    multisample_buffer_bit4_qcom =
        0x10000000, // GL_MULTISAMPLE_BUFFER_BIT4_QCOM
    multisample_buffer_bit5_qcom =
        0x20000000, // GL_MULTISAMPLE_BUFFER_BIT5_QCOM
    multisample_buffer_bit6_qcom =
        0x40000000, // GL_MULTISAMPLE_BUFFER_BIT6_QCOM
    multisample_buffer_bit7_qcom =
        0x80000000,                        // GL_MULTISAMPLE_BUFFER_BIT7_QCOM
    stencil_buffer_bit0_qcom = 0x00010000, // GL_STENCIL_BUFFER_BIT0_QCOM
    stencil_buffer_bit1_qcom = 0x00020000, // GL_STENCIL_BUFFER_BIT1_QCOM
    stencil_buffer_bit2_qcom = 0x00040000, // GL_STENCIL_BUFFER_BIT2_QCOM
    stencil_buffer_bit3_qcom = 0x00080000, // GL_STENCIL_BUFFER_BIT3_QCOM
    stencil_buffer_bit4_qcom = 0x00100000, // GL_STENCIL_BUFFER_BIT4_QCOM
    stencil_buffer_bit5_qcom = 0x00200000, // GL_STENCIL_BUFFER_BIT5_QCOM
    stencil_buffer_bit6_qcom = 0x00400000, // GL_STENCIL_BUFFER_BIT6_QCOM
    stencil_buffer_bit7_qcom = 0x00800000, // GL_STENCIL_BUFFER_BIT7_QCOM
}; // enum class buffer_bit_qcom
C_FLAGS(buffer_bit_qcom, u32);

} // namespace gl::group
