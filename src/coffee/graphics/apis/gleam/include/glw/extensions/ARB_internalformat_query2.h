#pragma once

#ifdef GL_ARB_internalformat_query2
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
#include "../enums/CopyImageSubDataTarget.h"
#include "../enums/EnableCap.h"
#include "../enums/GetFramebufferParameter.h"
#include "../enums/GetPName.h"
#include "../enums/InternalFormatPName.h"
#include "../enums/ObjectIdentifier.h"
#include "../enums/RenderbufferTarget.h"
#include "../enums/TextureTarget.h"
namespace gl::arb::internalformat_query2 {
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
using gl::group::copy_image_sub_data_target;
using gl::group::enable_cap;
using gl::group::get_framebuffer_parameter;
using gl::group::get_prop;
using gl::group::internal_format_prop;
using gl::group::object_identifier;
using gl::group::renderbuffer_target;
using gl::group::texture_target;
namespace values {
constexpr libc_types::u32 max_combined_dimensions    = 0x8282;
constexpr libc_types::u32 depth_components           = 0x8284;
constexpr libc_types::u32 stencil_components         = 0x8285;
constexpr libc_types::u32 manual_generate_mipmap     = 0x8294;
constexpr libc_types::u32 srgb_decode                = 0x8299;
constexpr libc_types::u32 full_support               = 0x82B7;
constexpr libc_types::u32 caveat_support             = 0x82B8;
constexpr libc_types::u32 image_class_4_x_32         = 0x82B9;
constexpr libc_types::u32 image_class_2_x_32         = 0x82BA;
constexpr libc_types::u32 image_class_1_x_32         = 0x82BB;
constexpr libc_types::u32 image_class_4_x_16         = 0x82BC;
constexpr libc_types::u32 image_class_2_x_16         = 0x82BD;
constexpr libc_types::u32 image_class_1_x_16         = 0x82BE;
constexpr libc_types::u32 image_class_4_x_8          = 0x82BF;
constexpr libc_types::u32 image_class_2_x_8          = 0x82C0;
constexpr libc_types::u32 image_class_1_x_8          = 0x82C1;
constexpr libc_types::u32 image_class_11_11_10       = 0x82C2;
constexpr libc_types::u32 image_class_10_10_10_2     = 0x82C3;
constexpr libc_types::u32 view_class_128_bits        = 0x82C4;
constexpr libc_types::u32 view_class_96_bits         = 0x82C5;
constexpr libc_types::u32 view_class_64_bits         = 0x82C6;
constexpr libc_types::u32 view_class_48_bits         = 0x82C7;
constexpr libc_types::u32 view_class_32_bits         = 0x82C8;
constexpr libc_types::u32 view_class_24_bits         = 0x82C9;
constexpr libc_types::u32 view_class_16_bits         = 0x82CA;
constexpr libc_types::u32 view_class_8_bits          = 0x82CB;
constexpr libc_types::u32 view_class_s3tc_dxt1_rgb   = 0x82CC;
constexpr libc_types::u32 view_class_s3tc_dxt1_rgba  = 0x82CD;
constexpr libc_types::u32 view_class_s3tc_dxt3_rgba  = 0x82CE;
constexpr libc_types::u32 view_class_s3tc_dxt5_rgba  = 0x82CF;
constexpr libc_types::u32 view_class_rgtc1_red       = 0x82D0;
constexpr libc_types::u32 view_class_rgtc2_rg        = 0x82D1;
constexpr libc_types::u32 view_class_bptc_unorm      = 0x82D2;
constexpr libc_types::u32 view_class_bptc_float      = 0x82D3;
constexpr libc_types::u32 view_class_eac_r11         = 0x9383;
constexpr libc_types::u32 view_class_eac_rg11        = 0x9384;
constexpr libc_types::u32 view_class_etc2_rgb        = 0x9385;
constexpr libc_types::u32 view_class_etc2_rgba       = 0x9386;
constexpr libc_types::u32 view_class_etc2_eac_rgba   = 0x9387;
constexpr libc_types::u32 view_class_astc_4x4_rgba   = 0x9388;
constexpr libc_types::u32 view_class_astc_5x4_rgba   = 0x9389;
constexpr libc_types::u32 view_class_astc_5x5_rgba   = 0x938A;
constexpr libc_types::u32 view_class_astc_6x5_rgba   = 0x938B;
constexpr libc_types::u32 view_class_astc_6x6_rgba   = 0x938C;
constexpr libc_types::u32 view_class_astc_8x5_rgba   = 0x938D;
constexpr libc_types::u32 view_class_astc_8x6_rgba   = 0x938E;
constexpr libc_types::u32 view_class_astc_8x8_rgba   = 0x938F;
constexpr libc_types::u32 view_class_astc_10x5_rgba  = 0x9390;
constexpr libc_types::u32 view_class_astc_10x6_rgba  = 0x9391;
constexpr libc_types::u32 view_class_astc_10x8_rgba  = 0x9392;
constexpr libc_types::u32 view_class_astc_10x10_rgba = 0x9393;
constexpr libc_types::u32 view_class_astc_12x10_rgba = 0x9394;
constexpr libc_types::u32 view_class_astc_12x12_rgba = 0x9395;
} // namespace values
template<class span_i64>
requires(semantic::concepts::Span<span_i64>&& std::is_same_v<
         std::decay_t<typename span_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void get_internalformati64v(
        group::texture_target       target,
        group::internal_format      internalformat,
        group::internal_format_prop pname,
        span_i64                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInternalformati64v)
    }
    glGetInternalformati64v(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(pname),
        params.size(),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetInternalformati64v"sv);
}

} // namespace gl::arb::internalformat_query2
#endif // GL_ARB_internalformat_query2
namespace gl::arb::internalformat_query2 {
constexpr auto name = "GL_ARB_internalformat_query2";
}
