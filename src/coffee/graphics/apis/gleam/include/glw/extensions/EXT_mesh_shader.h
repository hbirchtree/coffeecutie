#pragma once

#ifdef GL_EXT_mesh_shader
#include "../enums/GetPName.h"
#include "../enums/QueryTarget.h"
#include "../enums/UseProgramStageMask.h"

namespace gl::ext::mesh_shader {
using gl::group::get_prop;
using gl::group::query_target;
using gl::group::use_program_stage_mask;

namespace values {
constexpr u32 max_mesh_uniform_blocks                         = 0x8E60;
constexpr u32 max_mesh_texture_image_units                    = 0x8E61;
constexpr u32 max_mesh_image_uniforms                         = 0x8E62;
constexpr u32 max_mesh_uniform_components                     = 0x8E63;
constexpr u32 max_mesh_atomic_counter_buffers                 = 0x8E64;
constexpr u32 max_mesh_atomic_counters                        = 0x8E65;
constexpr u32 max_mesh_shader_storage_blocks                  = 0x8E66;
constexpr u32 max_combined_mesh_uniform_components            = 0x8E67;
constexpr u32 max_task_uniform_blocks                         = 0x8E68;
constexpr u32 max_task_texture_image_units                    = 0x8E69;
constexpr u32 max_task_image_uniforms                         = 0x8E6A;
constexpr u32 max_task_uniform_components                     = 0x8E6B;
constexpr u32 max_task_atomic_counter_buffers                 = 0x8E6C;
constexpr u32 max_task_atomic_counters                        = 0x8E6D;
constexpr u32 max_task_shader_storage_blocks                  = 0x8E6E;
constexpr u32 max_combined_task_uniform_components            = 0x8E6F;
constexpr u32 mesh_output_per_vertex_granularity              = 0x92DF;
constexpr u32 max_mesh_output_vertices                        = 0x9538;
constexpr u32 mesh_work_group_size                            = 0x953E;
constexpr u32 task_work_group_size                            = 0x953F;
constexpr u32 mesh_output_per_primitive_granularity           = 0x9543;
constexpr u32 max_mesh_multiview_view_count                   = 0x9557;
constexpr u32 mesh_shader                                     = 0x9559;
constexpr u32 task_shader                                     = 0x955A;
constexpr u32 mesh_vertices_out                               = 0x9579;
constexpr u32 mesh_primitives_out                             = 0x957A;
constexpr u32 mesh_output_type                                = 0x957B;
constexpr u32 mesh_subroutine                                 = 0x957C;
constexpr u32 task_subroutine                                 = 0x957D;
constexpr u32 mesh_subroutine_uniform                         = 0x957E;
constexpr u32 task_subroutine_uniform                         = 0x957F;
constexpr u32 uniform_block_referenced_by_mesh_shader         = 0x959C;
constexpr u32 uniform_block_referenced_by_task_shader         = 0x959D;
constexpr u32 atomic_counter_buffer_referenced_by_mesh_shader = 0x959E;
constexpr u32 atomic_counter_buffer_referenced_by_task_shader = 0x959F;
constexpr u32 referenced_by_mesh_shader                       = 0x95A0;
constexpr u32 referenced_by_task_shader                       = 0x95A1;
} // namespace values

/*!
 * \brief Part of GL_EXT_mesh_shader
 * \param num_groups_x GLuint
 * \param num_groups_y GLuint
 * \param num_groups_z GLuint
 * \return void
 */
STATICINLINE void draw_mesh_tasks(
    u32         num_groups_x,
    u32         num_groups_y,
    u32         num_groups_z,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawMeshTasksEXT)
    }
    glDrawMeshTasksEXT(num_groups_x, num_groups_y, num_groups_z);
    detail::error_check("DrawMeshTasksEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_mesh_shader
 * \param indirect GLintptr
 * \return void
 */
STATICINLINE void draw_mesh_tasks_indirect(
    GLintptr indirect, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawMeshTasksIndirectEXT)
    }
    glDrawMeshTasksIndirectEXT(indirect);
    detail::error_check("DrawMeshTasksIndirectEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_mesh_shader
 * \param indirect GLintptr
 * \param drawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_mesh_tasks_indirect(
    GLintptr    indirect,
    i32         drawcount,
    i32         stride,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawMeshTasksIndirectEXT)
    }
    glMultiDrawMeshTasksIndirectEXT(indirect, drawcount, stride);
    detail::error_check("MultiDrawMeshTasksIndirectEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_mesh_shader
 * \param indirect GLintptr
 * \param drawcount GLintptr
 * \param maxdrawcount GLsizei
 * \param stride GLsizei
 * \return void
 */
STATICINLINE void multi_draw_mesh_tasks_indirect_count(
    GLintptr    indirect,
    GLintptr    drawcount,
    i32         maxdrawcount,
    i32         stride,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawMeshTasksIndirectCountEXT)
    }
    glMultiDrawMeshTasksIndirectCountEXT(
        indirect, drawcount, maxdrawcount, stride);
    detail::error_check("MultiDrawMeshTasksIndirectCountEXT"sv, check_errors);
}

} // namespace gl::ext::mesh_shader
#endif // GL_EXT_mesh_shader
namespace gl::ext::mesh_shader {
constexpr auto name = "GL_EXT_mesh_shader";
}
