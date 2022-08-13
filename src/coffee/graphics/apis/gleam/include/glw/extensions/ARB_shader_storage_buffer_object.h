#pragma once

#ifdef GL_ARB_shader_storage_buffer_object
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
#include "../enums/GetPName.h"
#include "../enums/MemoryBarrierMask.h"
namespace gl::arb::shader_storage_buffer_object {
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
using gl::group::get_prop;
using gl::group::memory_barrier_mask;
namespace values {
constexpr libc_types::u32 max_combined_image_units_and_fragment_outputs =
    0x8F39;
constexpr libc_types::u32 max_combined_shader_output_resources = 0x8F39;
constexpr libc_types::u32 max_shader_storage_block_size        = 0x90DE;
} // namespace values
STATICINLINE void shader_storage_block_binding(
    u32 program, u32 storageBlockIndex, u32 storageBlockBinding)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderStorageBlockBinding)
        glIsProgram(program);
    }
    glShaderStorageBlockBinding(
        program, storageBlockIndex, storageBlockBinding);
    detail::error_check("ShaderStorageBlockBinding"sv);
}

} // namespace gl::arb::shader_storage_buffer_object
#endif // GL_ARB_shader_storage_buffer_object
namespace gl::arb::shader_storage_buffer_object {
constexpr auto name = "GL_ARB_shader_storage_buffer_object";
}