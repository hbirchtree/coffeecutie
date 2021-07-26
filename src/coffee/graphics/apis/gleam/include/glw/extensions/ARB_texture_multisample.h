#pragma once

#ifdef GL_ARB_texture_multisample
#include "../enums/AttributeType.h"
#include "../enums/CopyImageSubDataTarget.h"
#include "../enums/EnableCap.h"
#include "../enums/GetMultisamplePNameNV.h"
#include "../enums/GetPName.h"
#include "../enums/GlslTypeToken.h"
#include "../enums/TextureTarget.h"
#include "../enums/UniformType.h"
namespace gl::arb::texture_multisample {
using gl::group::attribute_type;
using gl::group::copy_image_sub_data_target;
using gl::group::enable_cap;
using gl::group::get_multisample_prop_nv;
using gl::group::get_prop;
using gl::group::glsl_type_token;
using gl::group::texture_target;
using gl::group::uniform_type;
namespace values {
constexpr libc_types::u32 sample_mask_value              = 0x8E52;
constexpr libc_types::u32 texture_samples                = 0x9106;
constexpr libc_types::u32 texture_fixed_sample_locations = 0x9107;
} // namespace values
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_multisamplefv(
        group::get_multisample_prop_nv pname, u32 index, span_f32 val)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultisamplefv)
    }
    glGetMultisamplefv(
        static_cast<GLenum>(pname),
        index,
        val.size() ? reinterpret_cast<GLfloat*>(val.data()) : nullptr);
    detail::error_check("GetMultisamplefv"sv);
}

STATICINLINE void sample_maski(u32 maskNumber, GLbitfield mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleMaski)
    }
    glSampleMaski(maskNumber, mask);
    detail::error_check("SampleMaski"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void tex_image_2d_multisample(
        group::texture_target  target,
        i32                    samples,
        group::internal_format internalformat,
        size_2_i32 const&      width,
        bool                   fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage2DMultisample)
    }
    glTexImage2DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        fixedsamplelocations);
    detail::error_check("TexImage2DMultisample"sv);
}

template<class size_3_i32>
requires(semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void tex_image_3d_multisample(
        group::texture_target  target,
        i32                    samples,
        group::internal_format internalformat,
        size_3_i32 const&      width,
        bool                   fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage3DMultisample)
    }
    glTexImage3DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        fixedsamplelocations);
    detail::error_check("TexImage3DMultisample"sv);
}

} // namespace gl::arb::texture_multisample
#endif // GL_ARB_texture_multisample
namespace gl::arb::texture_multisample {
constexpr auto name = "GL_ARB_texture_multisample";
}
