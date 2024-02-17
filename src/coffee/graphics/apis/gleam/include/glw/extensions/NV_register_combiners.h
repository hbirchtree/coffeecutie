#pragma once

#ifdef GL_NV_register_combiners
#include "../enums/BlendingFactor.h"
#include "../enums/CombinerBiasNV.h"
#include "../enums/CombinerMappingNV.h"
#include "../enums/CombinerParameterNV.h"
#include "../enums/CombinerRegisterNV.h"
#include "../enums/CombinerScaleNV.h"
#include "../enums/CombinerStageNV.h"
#include "../enums/CombinerVariableNV.h"
#include "../enums/EnableCap.h"
#include "../enums/FragmentShaderGenericSourceATI.h"
#include "../enums/GetPName.h"
#include "../enums/PathColor.h"
#include "../enums/SpecialNumbers.h"
#include "../enums/StencilOp.h"
#include "../enums/TextureSwizzle.h"

namespace gl::nv::register_combiners {
using gl::group::blending_factor;
using gl::group::combiner_bias_nv;
using gl::group::combiner_mapping_nv;
using gl::group::combiner_parameter_nv;
using gl::group::combiner_register_nv;
using gl::group::combiner_scale_nv;
using gl::group::combiner_stage_nv;
using gl::group::combiner_variable_nv;
using gl::group::enable_cap;
using gl::group::fragment_shader_generic_source_ati;
using gl::group::get_prop;
using gl::group::path_color;
using gl::group::special_numbers;
using gl::group::stencil_op;
using gl::group::texture_swizzle;

namespace values {
constexpr u32 register_combiners          = 0x8522;
constexpr u32 constant_color0             = 0x852A;
constexpr u32 constant_color1             = 0x852B;
constexpr u32 e_times_f                   = 0x8531;
constexpr u32 spare0_plus_secondary_color = 0x8532;
constexpr u32 combiner_ab_dot_product     = 0x8545;
constexpr u32 combiner_cd_dot_product     = 0x8546;
constexpr u32 combiner_mux_sum            = 0x8547;
constexpr u32 combiner_scale              = 0x8548;
constexpr u32 combiner_bias               = 0x8549;
constexpr u32 combiner_ab_output          = 0x854A;
constexpr u32 combiner_cd_output          = 0x854B;
constexpr u32 combiner_sum_output         = 0x854C;
constexpr u32 max_general_combiners       = 0x854D;
constexpr u32 num_general_combiners       = 0x854E;
constexpr u32 color_sum_clamp             = 0x854F;
} // namespace values

/*!
 * \brief Part of GL_NV_register_combiners
 * \param stage GLenum
 * \param portion GLenum
 * \param variable GLenum
 * \param input GLenum
 * \param mapping GLenum
 * \param componentUsage GLenum
 * \return void
 */
STATICINLINE void combiner_input(
    group::combiner_stage_nv           stage,
    group::combiner_portion_nv         portion,
    group::combiner_variable_nv        variable,
    group::combiner_register_nv        input,
    group::combiner_mapping_nv         mapping,
    group::combiner_component_usage_nv componentUsage)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CombinerInputNV)
    }
    glCombinerInputNV(
        static_cast<GLenum>(stage),
        static_cast<GLenum>(portion),
        static_cast<GLenum>(variable),
        static_cast<GLenum>(input),
        static_cast<GLenum>(mapping),
        static_cast<GLenum>(componentUsage));
    detail::error_check("CombinerInputNV"sv);
}

/*!
 * \brief Part of GL_NV_register_combiners
 * \param stage GLenum
 * \param portion GLenum
 * \param abOutput GLenum
 * \param cdOutput GLenum
 * \param sumOutput GLenum
 * \param scale GLenum
 * \param bias GLenum
 * \param abDotProduct GLboolean
 * \param cdDotProduct GLboolean
 * \param muxSum GLboolean
 * \return void
 */
STATICINLINE void combiner_output(
    group::combiner_stage_nv    stage,
    group::combiner_portion_nv  portion,
    group::combiner_register_nv abOutput,
    group::combiner_register_nv cdOutput,
    group::combiner_register_nv sumOutput,
    group::combiner_scale_nv    scale,
    group::combiner_bias_nv     bias,
    bool                        abDotProduct,
    bool                        cdDotProduct,
    bool                        muxSum)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CombinerOutputNV)
    }
    glCombinerOutputNV(
        static_cast<GLenum>(stage),
        static_cast<GLenum>(portion),
        static_cast<GLenum>(abOutput),
        static_cast<GLenum>(cdOutput),
        static_cast<GLenum>(sumOutput),
        static_cast<GLenum>(scale),
        static_cast<GLenum>(bias),
        abDotProduct,
        cdDotProduct,
        muxSum);
    detail::error_check("CombinerOutputNV"sv);
}

/*!
 * \brief Part of GL_NV_register_combiners
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void combiner_parameter(
    group::combiner_parameter_nv pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CombinerParameterfNV)
    }
    glCombinerParameterfNV(static_cast<GLenum>(pname), param);
    detail::error_check("CombinerParameterfNV"sv);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_register_combiners
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void combiner_parameter(
    group::combiner_parameter_nv pname, span_const_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CombinerParameterfvNV)
    }
    glCombinerParameterfvNV(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("CombinerParameterfvNV"sv);
}

/*!
 * \brief Part of GL_NV_register_combiners
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void combiner_parameter(
    group::combiner_parameter_nv pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CombinerParameteriNV)
    }
    glCombinerParameteriNV(static_cast<GLenum>(pname), param);
    detail::error_check("CombinerParameteriNV"sv);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_register_combiners
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void combiner_parameter(
    group::combiner_parameter_nv pname, span_const_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CombinerParameterivNV)
    }
    glCombinerParameterivNV(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("CombinerParameterivNV"sv);
}

/*!
 * \brief Part of GL_NV_register_combiners
 * \param variable GLenum
 * \param input GLenum
 * \param mapping GLenum
 * \param componentUsage GLenum
 * \return void
 */
STATICINLINE void final_combiner_input(
    group::combiner_variable_nv        variable,
    group::combiner_register_nv        input,
    group::combiner_mapping_nv         mapping,
    group::combiner_component_usage_nv componentUsage)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FinalCombinerInputNV)
    }
    glFinalCombinerInputNV(
        static_cast<GLenum>(variable),
        static_cast<GLenum>(input),
        static_cast<GLenum>(mapping),
        static_cast<GLenum>(componentUsage));
    detail::error_check("FinalCombinerInputNV"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_register_combiners
 * \param stage GLenum
 * \param portion GLenum
 * \param variable GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_combiner_input_parameter(
    group::combiner_stage_nv     stage,
    group::combiner_portion_nv   portion,
    group::combiner_variable_nv  variable,
    group::combiner_parameter_nv pname,
    span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCombinerInputParameterfvNV)
    }
    glGetCombinerInputParameterfvNV(
        static_cast<GLenum>(stage),
        static_cast<GLenum>(portion),
        static_cast<GLenum>(variable),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetCombinerInputParameterfvNV"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_register_combiners
 * \param stage GLenum
 * \param portion GLenum
 * \param variable GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_combiner_input_parameter(
    group::combiner_stage_nv     stage,
    group::combiner_portion_nv   portion,
    group::combiner_variable_nv  variable,
    group::combiner_parameter_nv pname,
    span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCombinerInputParameterivNV)
    }
    glGetCombinerInputParameterivNV(
        static_cast<GLenum>(stage),
        static_cast<GLenum>(portion),
        static_cast<GLenum>(variable),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetCombinerInputParameterivNV"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_register_combiners
 * \param stage GLenum
 * \param portion GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_combiner_output_parameter(
    group::combiner_stage_nv     stage,
    group::combiner_portion_nv   portion,
    group::combiner_parameter_nv pname,
    span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCombinerOutputParameterfvNV)
    }
    glGetCombinerOutputParameterfvNV(
        static_cast<GLenum>(stage),
        static_cast<GLenum>(portion),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetCombinerOutputParameterfvNV"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_register_combiners
 * \param stage GLenum
 * \param portion GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_combiner_output_parameter(
    group::combiner_stage_nv     stage,
    group::combiner_portion_nv   portion,
    group::combiner_parameter_nv pname,
    span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCombinerOutputParameterivNV)
    }
    glGetCombinerOutputParameterivNV(
        static_cast<GLenum>(stage),
        static_cast<GLenum>(portion),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetCombinerOutputParameterivNV"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_register_combiners
 * \param variable GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_final_combiner_input_parameter(
    group::combiner_variable_nv  variable,
    group::combiner_parameter_nv pname,
    span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFinalCombinerInputParameterfvNV)
    }
    glGetFinalCombinerInputParameterfvNV(
        static_cast<GLenum>(variable),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetFinalCombinerInputParameterfvNV"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_register_combiners
 * \param variable GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_final_combiner_input_parameter(
    group::combiner_variable_nv  variable,
    group::combiner_parameter_nv pname,
    span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFinalCombinerInputParameterivNV)
    }
    glGetFinalCombinerInputParameterivNV(
        static_cast<GLenum>(variable),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFinalCombinerInputParameterivNV"sv);
}

} // namespace gl::nv::register_combiners
#endif // GL_NV_register_combiners
namespace gl::nv::register_combiners {
constexpr auto name = "GL_NV_register_combiners";
}
