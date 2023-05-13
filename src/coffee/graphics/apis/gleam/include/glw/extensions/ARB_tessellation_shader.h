#pragma once

#ifdef GL_ARB_tessellation_shader
#include "../enums/AlphaFunction.h"
#include "../enums/DepthFunction.h"
#include "../enums/FrontFaceDirection.h"
#include "../enums/GetPName.h"
#include "../enums/IndexFunctionEXT.h"
#include "../enums/PatchParameterName.h"
#include "../enums/PipelineParameterName.h"
#include "../enums/PrimitiveType.h"
#include "../enums/ShaderType.h"
#include "../enums/StencilFunction.h"
#include "../enums/UniformBlockPName.h"
namespace gl::arb::tessellation_shader {
using gl::group::alpha_function;
using gl::group::depth_function;
using gl::group::front_face_direction;
using gl::group::get_prop;
using gl::group::index_function_ext;
using gl::group::patch_parameter_name;
using gl::group::pipeline_parameter_name;
using gl::group::primitive_type;
using gl::group::shader_type;
using gl::group::stencil_function;
using gl::group::uniform_block_prop;
namespace values {
constexpr u32 max_tess_control_input_components               = 0x886C;
constexpr u32 max_tess_evaluation_input_components            = 0x886D;
constexpr u32 max_combined_tess_control_uniform_components    = 0x8E1E;
constexpr u32 max_combined_tess_evaluation_uniform_components = 0x8E1F;
constexpr u32 tess_control_output_vertices                    = 0x8E75;
constexpr u32 tess_gen_mode                                   = 0x8E76;
constexpr u32 tess_gen_spacing                                = 0x8E77;
constexpr u32 tess_gen_vertex_order                           = 0x8E78;
constexpr u32 tess_gen_point_mode                             = 0x8E79;
constexpr u32 isolines                                        = 0x8E7A;
constexpr u32 fractional_odd                                  = 0x8E7B;
constexpr u32 fractional_even                                 = 0x8E7C;
constexpr u32 max_patch_vertices                              = 0x8E7D;
constexpr u32 max_tess_gen_level                              = 0x8E7E;
constexpr u32 max_tess_control_uniform_components             = 0x8E7F;
constexpr u32 max_tess_evaluation_uniform_components          = 0x8E80;
constexpr u32 max_tess_control_texture_image_units            = 0x8E81;
constexpr u32 max_tess_evaluation_texture_image_units         = 0x8E82;
constexpr u32 max_tess_control_output_components              = 0x8E83;
constexpr u32 max_tess_patch_components                       = 0x8E84;
constexpr u32 max_tess_control_total_output_components        = 0x8E85;
constexpr u32 max_tess_evaluation_output_components           = 0x8E86;
} // namespace values
template<class span_const_f32>
requires(concepts::span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_tessellation_shader
     * \param pname GLenum
     * \param values const GLfloat *
     * \return void
     */
    STATICINLINE void patch_parameter(
        group::patch_parameter_name pname, span_const_f32 const& values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PatchParameterfv)
    }
    glPatchParameterfv(
        static_cast<GLenum>(pname),
        values.size() ? reinterpret_cast<const GLfloat*>(values.data())
                      : nullptr);
    detail::error_check("PatchParameterfv"sv);
}

/*!
 * \brief Part of GL_ARB_tessellation_shader
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void patch_parameter(group::patch_parameter_name pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PatchParameteri)
    }
    glPatchParameteri(static_cast<GLenum>(pname), value);
    detail::error_check("PatchParameteri"sv);
}

} // namespace gl::arb::tessellation_shader
#endif // GL_ARB_tessellation_shader
namespace gl::arb::tessellation_shader {
constexpr auto name = "GL_ARB_tessellation_shader";
}
