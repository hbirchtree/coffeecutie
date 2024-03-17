#pragma once

#ifdef GL_NV_blend_equation_advanced
#include "../enums/BlendingFactor.h"
#include "../enums/FragmentShaderGenericSourceATI.h"
#include "../enums/LogicOp.h"
#include "../enums/PathFillMode.h"
#include "../enums/SpecialNumbers.h"
#include "../enums/StencilOp.h"
#include "../enums/TextureSwizzle.h"

namespace gl::nv::blend_equation_advanced {
using gl::group::blending_factor;
using gl::group::fragment_shader_generic_source_ati;
using gl::group::logic_op;
using gl::group::path_fill_mode;
using gl::group::special_numbers;
using gl::group::stencil_op;
using gl::group::texture_swizzle;

namespace values {
constexpr u32 xor                     = 0x1506;
constexpr u32 red                     = 0x1903;
constexpr u32 green                   = 0x1904;
constexpr u32 blue                    = 0x1905;
constexpr u32 blend_premultiplied_src = 0x9280;
constexpr u32 blend_overlap           = 0x9281;
constexpr u32 uncorrelated            = 0x9282;
constexpr u32 disjoint                = 0x9283;
constexpr u32 conjoint                = 0x9284;
constexpr u32 src                     = 0x9286;
constexpr u32 dst                     = 0x9287;
constexpr u32 src_over                = 0x9288;
constexpr u32 dst_over                = 0x9289;
constexpr u32 src_in                  = 0x928A;
constexpr u32 dst_in                  = 0x928B;
constexpr u32 src_out                 = 0x928C;
constexpr u32 dst_out                 = 0x928D;
constexpr u32 src_atop                = 0x928E;
constexpr u32 dst_atop                = 0x928F;
constexpr u32 plus                    = 0x9291;
constexpr u32 plus_darker             = 0x9292;
constexpr u32 multiply                = 0x9294;
constexpr u32 screen                  = 0x9295;
constexpr u32 overlay                 = 0x9296;
constexpr u32 darken                  = 0x9297;
constexpr u32 lighten                 = 0x9298;
constexpr u32 colordodge              = 0x9299;
constexpr u32 colorburn               = 0x929A;
constexpr u32 hardlight               = 0x929B;
constexpr u32 softlight               = 0x929C;
constexpr u32 difference              = 0x929E;
constexpr u32 minus                   = 0x929F;
constexpr u32 exclusion               = 0x92A0;
constexpr u32 contrast                = 0x92A1;
constexpr u32 invert_rgb              = 0x92A3;
constexpr u32 lineardodge             = 0x92A4;
constexpr u32 linearburn              = 0x92A5;
constexpr u32 vividlight              = 0x92A6;
constexpr u32 linearlight             = 0x92A7;
constexpr u32 pinlight                = 0x92A8;
constexpr u32 hardmix                 = 0x92A9;
constexpr u32 hsl_hue                 = 0x92AD;
constexpr u32 hsl_saturation          = 0x92AE;
constexpr u32 hsl_color               = 0x92AF;
constexpr u32 hsl_luminosity          = 0x92B0;
constexpr u32 plus_clamped            = 0x92B1;
constexpr u32 plus_clamped_alpha      = 0x92B2;
constexpr u32 minus_clamped           = 0x92B3;
constexpr u32 invert_ovg              = 0x92B4;
} // namespace values

/*!
 * \brief Part of GL_NV_blend_equation_advanced

 * \return void
 */
STATICINLINE void blend_barrier(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendBarrierNV)
    }
    glBlendBarrierNV();
    detail::error_check("BlendBarrierNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_blend_equation_advanced
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void blend_parameter(
    GLenum pname, i32 value, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendParameteriNV)
    }
    glBlendParameteriNV(pname, value);
    detail::error_check("BlendParameteriNV"sv, check_errors);
}

} // namespace gl::nv::blend_equation_advanced
#endif // GL_NV_blend_equation_advanced
namespace gl::nv::blend_equation_advanced {
constexpr auto name = "GL_NV_blend_equation_advanced";
}
