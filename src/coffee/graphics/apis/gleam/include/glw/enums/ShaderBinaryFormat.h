#pragma once

#include "common.h"

namespace gl::group {

// ShaderBinaryFormat
enum class shader_binary_format : u32
{
    spir_v                 = 0x9551, // GL_SHADER_BINARY_FORMAT_SPIR_V
    shader_binary_huawei   = 0x9770, // GL_SHADER_BINARY_HUAWEI
    gccso_shader_binary_fj = 0x9260, // GL_GCCSO_SHADER_BINARY_FJ
    mali_shader_binary_arm = 0x8F60, // GL_MALI_SHADER_BINARY_ARM
    sgx_binary_img         = 0x8C0A, // GL_SGX_BINARY_IMG
    shader_binary_dmp      = 0x9250, // GL_SHADER_BINARY_DMP
    shader_binary_viv      = 0x8FC4, // GL_SHADER_BINARY_VIV
}; // enum class shader_binary_format

} // namespace gl::group
