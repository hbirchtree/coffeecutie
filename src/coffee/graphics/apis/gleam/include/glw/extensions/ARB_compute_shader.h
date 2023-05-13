#pragma once

#ifdef GL_ARB_compute_shader
#include "../enums/AtomicCounterBufferPName.h"
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
#include "../enums/GetPName.h"
#include "../enums/ProgramPropertyARB.h"
#include "../enums/ShaderType.h"
#include "../enums/UniformBlockPName.h"
#include "../enums/UseProgramStageMask.h"
namespace gl::arb::compute_shader {
using gl::group::atomic_counter_buffer_prop;
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
using gl::group::get_prop;
using gl::group::program_property_arb;
using gl::group::shader_type;
using gl::group::uniform_block_prop;
using gl::group::use_program_stage_mask;
namespace values {
constexpr u32 max_compute_shared_memory_size = 0x8262;
constexpr u32 max_compute_image_uniforms     = 0x91BD;
} // namespace values
/*!
 * \brief Part of GL_ARB_compute_shader
 * \param num_groups_x GLuint
 * \param num_groups_y GLuint
 * \param num_groups_z GLuint
 * \return void
 */
STATICINLINE void dispatch_compute(
    u32 num_groups_x, u32 num_groups_y, u32 num_groups_z)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DispatchCompute)
    }
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    detail::error_check("DispatchCompute"sv);
}

/*!
 * \brief Part of GL_ARB_compute_shader
 * \param indirect GLintptr
 * \return void
 */
STATICINLINE void dispatch_compute_indirect(GLintptr indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DispatchComputeIndirect)
    }
    glDispatchComputeIndirect(indirect);
    detail::error_check("DispatchComputeIndirect"sv);
}

} // namespace gl::arb::compute_shader
#endif // GL_ARB_compute_shader
namespace gl::arb::compute_shader {
constexpr auto name = "GL_ARB_compute_shader";
}
