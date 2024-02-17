#pragma once

#ifdef GL_EXT_vertex_shader
#include "../enums/DataTypeEXT.h"
#include "../enums/GetVariantValueEXT.h"
#include "../enums/ParameterRangeEXT.h"
#include "../enums/VariantCapEXT.h"
#include "../enums/VertexShaderCoordOutEXT.h"
#include "../enums/VertexShaderOpEXT.h"
#include "../enums/VertexShaderParameterEXT.h"
#include "../enums/VertexShaderStorageTypeEXT.h"

namespace gl::ext::vertex_shader {
using gl::group::data_type_ext;
using gl::group::get_variant_value_ext;
using gl::group::parameter_range_ext;
using gl::group::variant_cap_ext;
using gl::group::vertex_shader_coord_out_ext;
using gl::group::vertex_shader_op_ext;
using gl::group::vertex_shader_parameter_ext;
using gl::group::vertex_shader_storage_type_ext;

namespace values {
constexpr u32 vertex_shader                               = 0x8780;
constexpr u32 vertex_shader_binding                       = 0x8781;
constexpr u32 output_vertex                               = 0x879A;
constexpr u32 output_color0                               = 0x879B;
constexpr u32 output_color1                               = 0x879C;
constexpr u32 output_texture_coord0                       = 0x879D;
constexpr u32 output_texture_coord1                       = 0x879E;
constexpr u32 output_texture_coord2                       = 0x879F;
constexpr u32 output_texture_coord3                       = 0x87A0;
constexpr u32 output_texture_coord4                       = 0x87A1;
constexpr u32 output_texture_coord5                       = 0x87A2;
constexpr u32 output_texture_coord6                       = 0x87A3;
constexpr u32 output_texture_coord7                       = 0x87A4;
constexpr u32 output_texture_coord8                       = 0x87A5;
constexpr u32 output_texture_coord9                       = 0x87A6;
constexpr u32 output_texture_coord10                      = 0x87A7;
constexpr u32 output_texture_coord11                      = 0x87A8;
constexpr u32 output_texture_coord12                      = 0x87A9;
constexpr u32 output_texture_coord13                      = 0x87AA;
constexpr u32 output_texture_coord14                      = 0x87AB;
constexpr u32 output_texture_coord15                      = 0x87AC;
constexpr u32 output_texture_coord16                      = 0x87AD;
constexpr u32 output_texture_coord17                      = 0x87AE;
constexpr u32 output_texture_coord18                      = 0x87AF;
constexpr u32 output_texture_coord19                      = 0x87B0;
constexpr u32 output_texture_coord20                      = 0x87B1;
constexpr u32 output_texture_coord21                      = 0x87B2;
constexpr u32 output_texture_coord22                      = 0x87B3;
constexpr u32 output_texture_coord23                      = 0x87B4;
constexpr u32 output_texture_coord24                      = 0x87B5;
constexpr u32 output_texture_coord25                      = 0x87B6;
constexpr u32 output_texture_coord26                      = 0x87B7;
constexpr u32 output_texture_coord27                      = 0x87B8;
constexpr u32 output_texture_coord28                      = 0x87B9;
constexpr u32 output_texture_coord29                      = 0x87BA;
constexpr u32 output_texture_coord30                      = 0x87BB;
constexpr u32 output_texture_coord31                      = 0x87BC;
constexpr u32 output_fog                                  = 0x87BD;
constexpr u32 max_vertex_shader_instructions              = 0x87C5;
constexpr u32 max_vertex_shader_variants                  = 0x87C6;
constexpr u32 max_vertex_shader_invariants                = 0x87C7;
constexpr u32 max_vertex_shader_local_constants           = 0x87C8;
constexpr u32 max_vertex_shader_locals                    = 0x87C9;
constexpr u32 max_optimized_vertex_shader_instructions    = 0x87CA;
constexpr u32 max_optimized_vertex_shader_variants        = 0x87CB;
constexpr u32 max_optimized_vertex_shader_local_constants = 0x87CC;
constexpr u32 max_optimized_vertex_shader_invariants      = 0x87CD;
constexpr u32 max_optimized_vertex_shader_locals          = 0x87CE;
constexpr u32 vertex_shader_instructions                  = 0x87CF;
constexpr u32 vertex_shader_variants                      = 0x87D0;
constexpr u32 vertex_shader_invariants                    = 0x87D1;
constexpr u32 vertex_shader_local_constants               = 0x87D2;
constexpr u32 vertex_shader_locals                        = 0x87D3;
constexpr u32 vertex_shader_optimized                     = 0x87D4;
constexpr u32 variant_array_pointer                       = 0x87E9;
constexpr u32 invariant_value                             = 0x87EA;
constexpr u32 invariant_datatype                          = 0x87EB;
constexpr u32 local_constant_value                        = 0x87EC;
constexpr u32 local_constant_datatype                     = 0x87ED;
} // namespace values

/*!
 * \brief Part of GL_EXT_vertex_shader

 * \return void
 */
STATICINLINE void begin_vertex_shader()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginVertexShaderEXT)
    }
    glBeginVertexShaderEXT();
    detail::error_check("BeginVertexShaderEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param light GLenum
 * \param value GLenum
 * \return GLuint
 */
STATICINLINE GLuint
bind_light_parameter(group::light_name light, group::light_parameter value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindLightParameterEXT)
    }
    auto out = glBindLightParameterEXT(
        static_cast<GLenum>(light), static_cast<GLenum>(value));
    detail::error_check("BindLightParameterEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param face GLenum
 * \param value GLenum
 * \return GLuint
 */
STATICINLINE GLuint bind_material_parameter(
    group::triangle_face face, group::material_parameter value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindMaterialParameterEXT)
    }
    auto out = glBindMaterialParameterEXT(
        static_cast<GLenum>(face), static_cast<GLenum>(value));
    detail::error_check("BindMaterialParameterEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param value GLenum
 * \return GLuint
 */
STATICINLINE GLuint bind_parameter(group::vertex_shader_parameter_ext value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindParameterEXT)
    }
    auto out = glBindParameterEXT(static_cast<GLenum>(value));
    detail::error_check("BindParameterEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param unit GLenum
 * \param coord GLenum
 * \param value GLenum
 * \return GLuint
 */
STATICINLINE GLuint bind_tex_gen_parameter(
    group::texture_unit          unit,
    group::texture_coord_name    coord,
    group::texture_gen_parameter value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTexGenParameterEXT)
    }
    auto out = glBindTexGenParameterEXT(
        static_cast<GLenum>(unit),
        static_cast<GLenum>(coord),
        static_cast<GLenum>(value));
    detail::error_check("BindTexGenParameterEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param unit GLenum
 * \param value GLenum
 * \return GLuint
 */
STATICINLINE GLuint bind_texture_unit_parameter(
    group::texture_unit unit, group::vertex_shader_texture_unit_parameter value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTextureUnitParameterEXT)
    }
    auto out = glBindTextureUnitParameterEXT(
        static_cast<GLenum>(unit), static_cast<GLenum>(value));
    detail::error_check("BindTextureUnitParameterEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \return void
 */
STATICINLINE void bind_vertex_shader(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexShaderEXT)
    }
    glBindVertexShaderEXT(id);
    detail::error_check("BindVertexShaderEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \return void
 */
STATICINLINE void delete_vertex_shader(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteVertexShaderEXT)
    }
    glDeleteVertexShaderEXT(id);
    detail::error_check("DeleteVertexShaderEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \return void
 */
STATICINLINE void disable_variant_client_state(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVariantClientStateEXT)
    }
    glDisableVariantClientStateEXT(id);
    detail::error_check("DisableVariantClientStateEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \return void
 */
STATICINLINE void enable_variant_client_state(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVariantClientStateEXT)
    }
    glEnableVariantClientStateEXT(id);
    detail::error_check("EnableVariantClientStateEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader

 * \return void
 */
STATICINLINE void end_vertex_shader()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndVertexShaderEXT)
    }
    glEndVertexShaderEXT();
    detail::error_check("EndVertexShaderEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param res GLuint
 * \param src GLuint
 * \param num GLuint
 * \return void
 */
STATICINLINE void extract_component(u32 res, u32 src, u32 num)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtractComponentEXT)
    }
    glExtractComponentEXT(res, src, num);
    detail::error_check("ExtractComponentEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param datatype GLenum
 * \param storagetype GLenum
 * \param range GLenum
 * \param components GLuint
 * \return GLuint
 */
STATICINLINE GLuint gen_symbols(
    group::data_type_ext                  datatype,
    group::vertex_shader_storage_type_ext storagetype,
    group::parameter_range_ext            range,
    u32                                   components)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenSymbolsEXT)
    }
    auto out = glGenSymbolsEXT(
        static_cast<GLenum>(datatype),
        static_cast<GLenum>(storagetype),
        static_cast<GLenum>(range),
        components);
    detail::error_check("GenSymbolsEXT"sv);
    return out;
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param range GLuint
 * \return GLuint
 */
STATICINLINE GLuint gen_vertex_shaders(u32 range)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenVertexShadersEXT)
    }
    auto out = glGenVertexShadersEXT(range);
    detail::error_check("GenVertexShadersEXT"sv);
    return out;
}

template<class span_bool>
requires(
    concepts::span<span_bool> &&
    std::is_same_v<
        std::decay_t<typename span_bool::value_type>,
        std::decay_t<bool>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLboolean *
 * \return void
 */
STATICINLINE void get_invariant_booleanv(
    u32 id, group::get_variant_value_ext value, span_bool data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInvariantBooleanvEXT)
    }
    glGetInvariantBooleanvEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLboolean*>(data.data()) : nullptr);
    detail::error_check("GetInvariantBooleanvEXT"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLfloat *
 * \return void
 */
STATICINLINE void get_invariant_floatv(
    u32 id, group::get_variant_value_ext value, span_f32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInvariantFloatvEXT)
    }
    glGetInvariantFloatvEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetInvariantFloatvEXT"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLint *
 * \return void
 */
STATICINLINE void get_invariant_integerv(
    u32 id, group::get_variant_value_ext value, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInvariantIntegervEXT)
    }
    glGetInvariantIntegervEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetInvariantIntegervEXT"sv);
}

template<class span_bool>
requires(
    concepts::span<span_bool> &&
    std::is_same_v<
        std::decay_t<typename span_bool::value_type>,
        std::decay_t<bool>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLboolean *
 * \return void
 */
STATICINLINE void get_local_constant_booleanv(
    u32 id, group::get_variant_value_ext value, span_bool data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetLocalConstantBooleanvEXT)
    }
    glGetLocalConstantBooleanvEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLboolean*>(data.data()) : nullptr);
    detail::error_check("GetLocalConstantBooleanvEXT"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLfloat *
 * \return void
 */
STATICINLINE void get_local_constant_floatv(
    u32 id, group::get_variant_value_ext value, span_f32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetLocalConstantFloatvEXT)
    }
    glGetLocalConstantFloatvEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetLocalConstantFloatvEXT"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLint *
 * \return void
 */
STATICINLINE void get_local_constant_integerv(
    u32 id, group::get_variant_value_ext value, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetLocalConstantIntegervEXT)
    }
    glGetLocalConstantIntegervEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetLocalConstantIntegervEXT"sv);
}

template<class span_bool>
requires(
    concepts::span<span_bool> &&
    std::is_same_v<
        std::decay_t<typename span_bool::value_type>,
        std::decay_t<bool>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLboolean *
 * \return void
 */
STATICINLINE void get_variant_booleanv(
    u32 id, group::get_variant_value_ext value, span_bool data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVariantBooleanvEXT)
    }
    glGetVariantBooleanvEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLboolean*>(data.data()) : nullptr);
    detail::error_check("GetVariantBooleanvEXT"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLfloat *
 * \return void
 */
STATICINLINE void get_variant_floatv(
    u32 id, group::get_variant_value_ext value, span_f32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVariantFloatvEXT)
    }
    glGetVariantFloatvEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetVariantFloatvEXT"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data GLint *
 * \return void
 */
STATICINLINE void get_variant_integerv(
    u32 id, group::get_variant_value_ext value, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVariantIntegervEXT)
    }
    glGetVariantIntegervEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetVariantIntegervEXT"sv);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param value GLenum
 * \param data void **
 * \return void
 */
STATICINLINE void get_variant_pointerv(
    u32 id, group::get_variant_value_ext value, span_void data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVariantPointervEXT)
    }
    glGetVariantPointervEXT(
        id,
        static_cast<GLenum>(value),
        data.size() ? reinterpret_cast<void**>(data.data()) : nullptr);
    detail::error_check("GetVariantPointervEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param res GLuint
 * \param src GLuint
 * \param num GLuint
 * \return void
 */
STATICINLINE void insert_component(u32 res, u32 src, u32 num)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InsertComponentEXT)
    }
    glInsertComponentEXT(res, src, num);
    detail::error_check("InsertComponentEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param cap GLenum
 * \return Boolean
 */
STATICINLINE bool is_variant_enabled(u32 id, group::variant_cap_ext cap)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsVariantEnabledEXT)
    }
    auto out = glIsVariantEnabledEXT(id, static_cast<GLenum>(cap));
    detail::error_check("IsVariantEnabledEXT"sv);
    return out == GL_TRUE ? true : false;
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param type GLenum
 * \param addr const void *
 * \return void
 */
STATICINLINE void set_invariant(
    u32 id, group::scalar_type type, span_const_void const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SetInvariantEXT)
    }
    glSetInvariantEXT(
        id,
        static_cast<GLenum>(type),
        addr.size() ? reinterpret_cast<const void*>(addr.data()) : nullptr);
    detail::error_check("SetInvariantEXT"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param type GLenum
 * \param addr const void *
 * \return void
 */
STATICINLINE void set_local_constant(
    u32 id, group::scalar_type type, span_const_void const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SetLocalConstantEXT)
    }
    glSetLocalConstantEXT(
        id,
        static_cast<GLenum>(type),
        addr.size() ? reinterpret_cast<const void*>(addr.data()) : nullptr);
    detail::error_check("SetLocalConstantEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param op GLenum
 * \param res GLuint
 * \param arg1 GLuint
 * \return void
 */
STATICINLINE void shader_op1(group::vertex_shader_op_ext op, u32 res, u32 arg1)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderOp1EXT)
    }
    glShaderOp1EXT(static_cast<GLenum>(op), res, arg1);
    detail::error_check("ShaderOp1EXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param op GLenum
 * \param res GLuint
 * \param arg1 GLuint
 * \param arg2 GLuint
 * \return void
 */
STATICINLINE void shader_op2(
    group::vertex_shader_op_ext op, u32 res, u32 arg1, u32 arg2)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderOp2EXT)
    }
    glShaderOp2EXT(static_cast<GLenum>(op), res, arg1, arg2);
    detail::error_check("ShaderOp2EXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param op GLenum
 * \param res GLuint
 * \param arg1 GLuint
 * \param arg2 GLuint
 * \param arg3 GLuint
 * \return void
 */
STATICINLINE void shader_op3(
    group::vertex_shader_op_ext op, u32 res, u32 arg1, u32 arg2, u32 arg3)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderOp3EXT)
    }
    glShaderOp3EXT(static_cast<GLenum>(op), res, arg1, arg2, arg3);
    detail::error_check("ShaderOp3EXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param res GLuint
 * \param in GLuint
 * \param outX GLenum
 * \param outY GLenum
 * \param outZ GLenum
 * \param outW GLenum
 * \return void
 */
STATICINLINE void swizzle(
    u32                                res,
    u32                                in,
    group::vertex_shader_coord_out_ext outX,
    group::vertex_shader_coord_out_ext outY,
    group::vertex_shader_coord_out_ext outZ,
    group::vertex_shader_coord_out_ext outW)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SwizzleEXT)
    }
    glSwizzleEXT(
        res,
        in,
        static_cast<GLenum>(outX),
        static_cast<GLenum>(outY),
        static_cast<GLenum>(outZ),
        static_cast<GLenum>(outW));
    detail::error_check("SwizzleEXT"sv);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param type GLenum
 * \param stride GLuint
 * \param addr const void *
 * \return void
 */
STATICINLINE void variant_pointer(
    u32 id, group::scalar_type type, u32 stride, span_const_void const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantPointerEXT)
    }
    glVariantPointerEXT(
        id,
        static_cast<GLenum>(type),
        stride,
        addr.size() ? reinterpret_cast<const void*>(addr.data()) : nullptr);
    detail::error_check("VariantPointerEXT"sv);
}

template<class span_const_i8>
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param addr const GLbyte *
 * \return void
 */
STATICINLINE void variantbv(u32 id, span_const_i8 const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantbvEXT)
    }
    glVariantbvEXT(
        id,
        addr.size() ? reinterpret_cast<const GLbyte*>(addr.data()) : nullptr);
    detail::error_check("VariantbvEXT"sv);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param addr const GLdouble *
 * \return void
 */
STATICINLINE void variantdv(u32 id, span_const_f64 const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantdvEXT)
    }
    glVariantdvEXT(
        id,
        addr.size() ? reinterpret_cast<const GLdouble*>(addr.data()) : nullptr);
    detail::error_check("VariantdvEXT"sv);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param addr const GLfloat *
 * \return void
 */
STATICINLINE void variantfv(u32 id, span_const_f32 const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantfvEXT)
    }
    glVariantfvEXT(
        id,
        addr.size() ? reinterpret_cast<const GLfloat*>(addr.data()) : nullptr);
    detail::error_check("VariantfvEXT"sv);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param addr const GLint *
 * \return void
 */
STATICINLINE void variantiv(u32 id, span_const_i32 const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantivEXT)
    }
    glVariantivEXT(
        id,
        addr.size() ? reinterpret_cast<const GLint*>(addr.data()) : nullptr);
    detail::error_check("VariantivEXT"sv);
}

template<class span_const_i16>
requires(
    concepts::span<span_const_i16> &&
    std::is_same_v<
        std::decay_t<typename span_const_i16::value_type>,
        std::decay_t<i16>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param addr const GLshort *
 * \return void
 */
STATICINLINE void variantsv(u32 id, span_const_i16 const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantsvEXT)
    }
    glVariantsvEXT(
        id,
        addr.size() ? reinterpret_cast<const GLshort*>(addr.data()) : nullptr);
    detail::error_check("VariantsvEXT"sv);
}

template<class span_const_u8>
requires(
    concepts::span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param addr const GLubyte *
 * \return void
 */
STATICINLINE void variantubv(u32 id, span_const_u8 const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantubvEXT)
    }
    glVariantubvEXT(
        id,
        addr.size() ? reinterpret_cast<const GLubyte*>(addr.data()) : nullptr);
    detail::error_check("VariantubvEXT"sv);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param addr const GLuint *
 * \return void
 */
STATICINLINE void variantuiv(u32 id, span_const_u32 const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantuivEXT)
    }
    glVariantuivEXT(
        id,
        addr.size() ? reinterpret_cast<const GLuint*>(addr.data()) : nullptr);
    detail::error_check("VariantuivEXT"sv);
}

template<class span_const_u16>
requires(
    concepts::span<span_const_u16> &&
    std::is_same_v<
        std::decay_t<typename span_const_u16::value_type>,
        std::decay_t<u16>>)
/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param id GLuint
 * \param addr const GLushort *
 * \return void
 */
STATICINLINE void variantusv(u32 id, span_const_u16 const& addr)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VariantusvEXT)
    }
    glVariantusvEXT(
        id,
        addr.size() ? reinterpret_cast<const GLushort*>(addr.data()) : nullptr);
    detail::error_check("VariantusvEXT"sv);
}

/*!
 * \brief Part of GL_EXT_vertex_shader
 * \param res GLuint
 * \param in GLuint
 * \param outX GLenum
 * \param outY GLenum
 * \param outZ GLenum
 * \param outW GLenum
 * \return void
 */
STATICINLINE void write_mask(
    u32                                 res,
    u32                                 in,
    group::vertex_shader_write_mask_ext outX,
    group::vertex_shader_write_mask_ext outY,
    group::vertex_shader_write_mask_ext outZ,
    group::vertex_shader_write_mask_ext outW)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WriteMaskEXT)
    }
    glWriteMaskEXT(
        res,
        in,
        static_cast<GLenum>(outX),
        static_cast<GLenum>(outY),
        static_cast<GLenum>(outZ),
        static_cast<GLenum>(outW));
    detail::error_check("WriteMaskEXT"sv);
}

} // namespace gl::ext::vertex_shader
#endif // GL_EXT_vertex_shader
namespace gl::ext::vertex_shader {
constexpr auto name = "GL_EXT_vertex_shader";
}
