#pragma once

#ifdef GL_NV_internalformat_sample_query
#include "../enums/CopyImageSubDataTarget.h"
#include "../enums/ObjectIdentifier.h"
#include "../enums/RenderbufferTarget.h"
#include "../enums/TextureTarget.h"
namespace gl::nv::internalformat_sample_query {
using gl::group::copy_image_sub_data_target;
using gl::group::object_identifier;
using gl::group::renderbuffer_target;
using gl::group::texture_target;
namespace values {
constexpr u32 multisamples        = 0x9371;
constexpr u32 supersample_scale_x = 0x9372;
constexpr u32 supersample_scale_y = 0x9373;
constexpr u32 conformant          = 0x9374;
} // namespace values
template<class span_i32>
requires(concepts::span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_internalformat_sample_query
     * \param target GLenum
     * \param internalformat GLenum
     * \param samples GLsizei
     * \param pname GLenum
     * \param count GLsizei
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_internalformat_sampleiv(
        group::texture_target       target,
        group::internal_format      internalformat,
        i32                         samples,
        group::internal_format_prop pname,
        span_i32                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInternalformatSampleivNV)
    }
    glGetInternalformatSampleivNV(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        samples,
        static_cast<GLenum>(pname),
        params.size(),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetInternalformatSampleivNV"sv);
}

} // namespace gl::nv::internalformat_sample_query
#endif // GL_NV_internalformat_sample_query
namespace gl::nv::internalformat_sample_query {
constexpr auto name = "GL_NV_internalformat_sample_query";
}
