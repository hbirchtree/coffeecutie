#pragma once

#ifdef GL_EXT_tessellation_shader
#include "../enums/AlphaFunction.h"
#include "../enums/DepthFunction.h"
#include "../enums/FrontFaceDirection.h"
#include "../enums/IndexFunctionEXT.h"
#include "../enums/PrimitiveType.h"
#include "../enums/StencilFunction.h"
#include "../enums/UseProgramStageMask.h"
namespace gl::ext::tessellation_shader {
using gl::group::alpha_function;
using gl::group::depth_function;
using gl::group::front_face_direction;
using gl::group::index_function_ext;
using gl::group::primitive_type;
using gl::group::stencil_function;
using gl::group::use_program_stage_mask;
namespace values {
constexpr libc_types::u32 primitive_restart_for_patches_supported      = 0x8221;
constexpr libc_types::u32 max_tess_control_input_components            = 0x886C;
constexpr libc_types::u32 max_tess_evaluation_input_components         = 0x886D;
constexpr libc_types::u32 max_combined_tess_control_uniform_components = 0x8E1E;
constexpr libc_types::u32 max_combined_tess_evaluation_uniform_components =
    0x8E1F;
constexpr libc_types::u32 patch_vertices                             = 0x8E72;
constexpr libc_types::u32 tess_control_output_vertices               = 0x8E75;
constexpr libc_types::u32 tess_gen_mode                              = 0x8E76;
constexpr libc_types::u32 tess_gen_spacing                           = 0x8E77;
constexpr libc_types::u32 tess_gen_vertex_order                      = 0x8E78;
constexpr libc_types::u32 tess_gen_point_mode                        = 0x8E79;
constexpr libc_types::u32 isolines                                   = 0x8E7A;
constexpr libc_types::u32 fractional_odd                             = 0x8E7B;
constexpr libc_types::u32 fractional_even                            = 0x8E7C;
constexpr libc_types::u32 max_patch_vertices                         = 0x8E7D;
constexpr libc_types::u32 max_tess_gen_level                         = 0x8E7E;
constexpr libc_types::u32 max_tess_control_uniform_components        = 0x8E7F;
constexpr libc_types::u32 max_tess_evaluation_uniform_components     = 0x8E80;
constexpr libc_types::u32 max_tess_control_texture_image_units       = 0x8E81;
constexpr libc_types::u32 max_tess_evaluation_texture_image_units    = 0x8E82;
constexpr libc_types::u32 max_tess_control_output_components         = 0x8E83;
constexpr libc_types::u32 max_tess_patch_components                  = 0x8E84;
constexpr libc_types::u32 max_tess_control_total_output_components   = 0x8E85;
constexpr libc_types::u32 max_tess_evaluation_output_components      = 0x8E86;
constexpr libc_types::u32 tess_evaluation_shader                     = 0x8E87;
constexpr libc_types::u32 tess_control_shader                        = 0x8E88;
constexpr libc_types::u32 max_tess_control_uniform_blocks            = 0x8E89;
constexpr libc_types::u32 max_tess_evaluation_uniform_blocks         = 0x8E8A;
constexpr libc_types::u32 max_tess_control_image_uniforms            = 0x90CB;
constexpr libc_types::u32 max_tess_evaluation_image_uniforms         = 0x90CC;
constexpr libc_types::u32 max_tess_control_shader_storage_blocks     = 0x90D8;
constexpr libc_types::u32 max_tess_evaluation_shader_storage_blocks  = 0x90D9;
constexpr libc_types::u32 max_tess_control_atomic_counter_buffers    = 0x92CD;
constexpr libc_types::u32 max_tess_evaluation_atomic_counter_buffers = 0x92CE;
constexpr libc_types::u32 max_tess_control_atomic_counters           = 0x92D3;
constexpr libc_types::u32 max_tess_evaluation_atomic_counters        = 0x92D4;
constexpr libc_types::u32 is_per_patch                               = 0x92E7;
constexpr libc_types::u32 referenced_by_tess_control_shader          = 0x9307;
constexpr libc_types::u32 referenced_by_tess_evaluation_shader       = 0x9308;
} // namespace values
STATICINLINE void patch_parameter(group::patch_parameter_name pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PatchParameteriEXT)
    }
    glPatchParameteriEXT(static_cast<GLenum>(pname), value);
    detail::error_check("PatchParameteriEXT"sv);
}

} // namespace gl::ext::tessellation_shader
#endif // GL_EXT_tessellation_shader
namespace gl::ext::tessellation_shader {
constexpr auto name = "GL_EXT_tessellation_shader";
}