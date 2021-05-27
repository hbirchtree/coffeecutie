#pragma once

#include "common.h"

namespace gl::groups {

// BufferBitQCOM
enum class buffer_bit_qcom : ::libc_types::u32 {
#ifdef GL_COLOR_BUFFER_BIT0_QCOM
    color_buffer_bit0_qcom = GL_COLOR_BUFFER_BIT0_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT1_QCOM
    color_buffer_bit1_qcom = GL_COLOR_BUFFER_BIT1_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT2_QCOM
    color_buffer_bit2_qcom = GL_COLOR_BUFFER_BIT2_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT3_QCOM
    color_buffer_bit3_qcom = GL_COLOR_BUFFER_BIT3_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT4_QCOM
    color_buffer_bit4_qcom = GL_COLOR_BUFFER_BIT4_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT5_QCOM
    color_buffer_bit5_qcom = GL_COLOR_BUFFER_BIT5_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT6_QCOM
    color_buffer_bit6_qcom = GL_COLOR_BUFFER_BIT6_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT7_QCOM
    color_buffer_bit7_qcom = GL_COLOR_BUFFER_BIT7_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT0_QCOM
    depth_buffer_bit0_qcom = GL_DEPTH_BUFFER_BIT0_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT1_QCOM
    depth_buffer_bit1_qcom = GL_DEPTH_BUFFER_BIT1_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT2_QCOM
    depth_buffer_bit2_qcom = GL_DEPTH_BUFFER_BIT2_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT3_QCOM
    depth_buffer_bit3_qcom = GL_DEPTH_BUFFER_BIT3_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT4_QCOM
    depth_buffer_bit4_qcom = GL_DEPTH_BUFFER_BIT4_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT5_QCOM
    depth_buffer_bit5_qcom = GL_DEPTH_BUFFER_BIT5_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT6_QCOM
    depth_buffer_bit6_qcom = GL_DEPTH_BUFFER_BIT6_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT7_QCOM
    depth_buffer_bit7_qcom = GL_DEPTH_BUFFER_BIT7_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT0_QCOM
    multisample_buffer_bit0_qcom = GL_MULTISAMPLE_BUFFER_BIT0_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT1_QCOM
    multisample_buffer_bit1_qcom = GL_MULTISAMPLE_BUFFER_BIT1_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT2_QCOM
    multisample_buffer_bit2_qcom = GL_MULTISAMPLE_BUFFER_BIT2_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT3_QCOM
    multisample_buffer_bit3_qcom = GL_MULTISAMPLE_BUFFER_BIT3_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT4_QCOM
    multisample_buffer_bit4_qcom = GL_MULTISAMPLE_BUFFER_BIT4_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT5_QCOM
    multisample_buffer_bit5_qcom = GL_MULTISAMPLE_BUFFER_BIT5_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT6_QCOM
    multisample_buffer_bit6_qcom = GL_MULTISAMPLE_BUFFER_BIT6_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT7_QCOM
    multisample_buffer_bit7_qcom = GL_MULTISAMPLE_BUFFER_BIT7_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT0_QCOM
    stencil_buffer_bit0_qcom = GL_STENCIL_BUFFER_BIT0_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT1_QCOM
    stencil_buffer_bit1_qcom = GL_STENCIL_BUFFER_BIT1_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT2_QCOM
    stencil_buffer_bit2_qcom = GL_STENCIL_BUFFER_BIT2_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT3_QCOM
    stencil_buffer_bit3_qcom = GL_STENCIL_BUFFER_BIT3_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT4_QCOM
    stencil_buffer_bit4_qcom = GL_STENCIL_BUFFER_BIT4_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT5_QCOM
    stencil_buffer_bit5_qcom = GL_STENCIL_BUFFER_BIT5_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT6_QCOM
    stencil_buffer_bit6_qcom = GL_STENCIL_BUFFER_BIT6_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT7_QCOM
    stencil_buffer_bit7_qcom = GL_STENCIL_BUFFER_BIT7_QCOM,
#endif
}; // enum class buffer_bit_qcom
C_FLAGS(buffer_bit_qcom, ::libc_types::u32);

} // namespace gl::groups
