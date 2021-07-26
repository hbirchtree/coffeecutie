#pragma once

#include "common.h"

namespace gl::group {

// ShaderBinaryFormat
enum class shader_binary_format : ::libc_types::u32
{
#ifdef GL_GCCSO_SHADER_BINARY_FJ
    gccso_shader_binary_fj = GL_GCCSO_SHADER_BINARY_FJ,
#endif
#ifdef GL_MALI_SHADER_BINARY_ARM
    mali_shader_binary_arm = GL_MALI_SHADER_BINARY_ARM,
#endif
#ifdef GL_SGX_BINARY_IMG
    sgx_binary_img = GL_SGX_BINARY_IMG,
#endif
#ifdef GL_SHADER_BINARY_DMP
    shader_binary_dmp = GL_SHADER_BINARY_DMP,
#endif
#ifdef GL_SHADER_BINARY_FORMAT_SPIR_V
    spir_v = GL_SHADER_BINARY_FORMAT_SPIR_V,
#endif
#ifdef GL_SHADER_BINARY_VIV
    shader_binary_viv = GL_SHADER_BINARY_VIV,
#endif
}; // enum class shader_binary_format

} // namespace gl::group
