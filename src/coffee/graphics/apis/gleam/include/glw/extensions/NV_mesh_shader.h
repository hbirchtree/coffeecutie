#pragma once

#ifdef GL_NV_mesh_shader
#include "../enums/UseProgramStageMask.h"
namespace gl::nv::mesh_shader {
using gl::group::use_program_stage_mask;
namespace values {
constexpr libc_types::u32 max_mesh_uniform_blocks                 = 0x8E60;
constexpr libc_types::u32 max_mesh_texture_image_units            = 0x8E61;
constexpr libc_types::u32 max_mesh_image_uniforms                 = 0x8E62;
constexpr libc_types::u32 max_mesh_uniform_components             = 0x8E63;
constexpr libc_types::u32 max_mesh_atomic_counter_buffers         = 0x8E64;
constexpr libc_types::u32 max_mesh_atomic_counters                = 0x8E65;
constexpr libc_types::u32 max_mesh_shader_storage_blocks          = 0x8E66;
constexpr libc_types::u32 max_combined_mesh_uniform_components    = 0x8E67;
constexpr libc_types::u32 max_task_uniform_blocks                 = 0x8E68;
constexpr libc_types::u32 max_task_texture_image_units            = 0x8E69;
constexpr libc_types::u32 max_task_image_uniforms                 = 0x8E6A;
constexpr libc_types::u32 max_task_uniform_components             = 0x8E6B;
constexpr libc_types::u32 max_task_atomic_counter_buffers         = 0x8E6C;
constexpr libc_types::u32 max_task_atomic_counters                = 0x8E6D;
constexpr libc_types::u32 max_task_shader_storage_blocks          = 0x8E6E;
constexpr libc_types::u32 max_combined_task_uniform_components    = 0x8E6F;
constexpr libc_types::u32 mesh_output_per_vertex_granularity      = 0x92DF;
constexpr libc_types::u32 max_mesh_total_memory_size              = 0x9536;
constexpr libc_types::u32 max_task_total_memory_size              = 0x9537;
constexpr libc_types::u32 max_mesh_output_vertices                = 0x9538;
constexpr libc_types::u32 max_mesh_output_primitives              = 0x9539;
constexpr libc_types::u32 max_task_output_count                   = 0x953A;
constexpr libc_types::u32 max_mesh_work_group_size                = 0x953B;
constexpr libc_types::u32 max_task_work_group_size                = 0x953C;
constexpr libc_types::u32 max_draw_mesh_tasks_count               = 0x953D;
constexpr libc_types::u32 mesh_work_group_size                    = 0x953E;
constexpr libc_types::u32 task_work_group_size                    = 0x953F;
constexpr libc_types::u32 mesh_output_per_primitive_granularity   = 0x9543;
constexpr libc_types::u32 max_mesh_views                          = 0x9557;
constexpr libc_types::u32 mesh_shader                             = 0x9559;
constexpr libc_types::u32 task_shader                             = 0x955A;
constexpr libc_types::u32 mesh_vertices_out                       = 0x9579;
constexpr libc_types::u32 mesh_primitives_out                     = 0x957A;
constexpr libc_types::u32 mesh_output_type                        = 0x957B;
constexpr libc_types::u32 mesh_subroutine                         = 0x957C;
constexpr libc_types::u32 task_subroutine                         = 0x957D;
constexpr libc_types::u32 mesh_subroutine_uniform                 = 0x957E;
constexpr libc_types::u32 task_subroutine_uniform                 = 0x957F;
constexpr libc_types::u32 uniform_block_referenced_by_mesh_shader = 0x959C;
constexpr libc_types::u32 uniform_block_referenced_by_task_shader = 0x959D;
constexpr libc_types::u32 atomic_counter_buffer_referenced_by_mesh_shader =
    0x959E;
constexpr libc_types::u32 atomic_counter_buffer_referenced_by_task_shader =
    0x959F;
constexpr libc_types::u32 referenced_by_mesh_shader       = 0x95A0;
constexpr libc_types::u32 referenced_by_task_shader       = 0x95A1;
constexpr libc_types::u32 max_mesh_work_group_invocations = 0x95A2;
constexpr libc_types::u32 max_task_work_group_invocations = 0x95A3;
} // namespace values
STATICINLINE void draw_mesh_tasks(u32 first, u32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawMeshTasksNV)
    }
    glDrawMeshTasksNV(first, count);
    detail::error_check("DrawMeshTasksNV"sv);
}

STATICINLINE void draw_mesh_tasks_indirect(GLintptr indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawMeshTasksIndirectNV)
    }
    glDrawMeshTasksIndirectNV(indirect);
    detail::error_check("DrawMeshTasksIndirectNV"sv);
}

STATICINLINE void multi_draw_mesh_tasks_indirect(
    GLintptr indirect, i32 drawcount, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawMeshTasksIndirectNV)
    }
    glMultiDrawMeshTasksIndirectNV(indirect, drawcount, stride);
    detail::error_check("MultiDrawMeshTasksIndirectNV"sv);
}

STATICINLINE void multi_draw_mesh_tasks_indirect_count(
    GLintptr indirect, GLintptr drawcount, i32 maxdrawcount, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawMeshTasksIndirectCountNV)
    }
    glMultiDrawMeshTasksIndirectCountNV(
        indirect, drawcount, maxdrawcount, stride);
    detail::error_check("MultiDrawMeshTasksIndirectCountNV"sv);
}

} // namespace gl::nv::mesh_shader
#endif // GL_NV_mesh_shader
namespace gl::nv::mesh_shader {
constexpr auto name = "GL_NV_mesh_shader";
}
