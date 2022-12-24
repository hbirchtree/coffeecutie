#pragma once

#ifdef GL_NV_explicit_multisample
namespace gl::nv::explicit_multisample {
namespace values {
constexpr libc_types::u32 sample_position                         = 0x8E50;
constexpr libc_types::u32 sample_mask                             = 0x8E51;
constexpr libc_types::u32 sample_mask_value                       = 0x8E52;
constexpr libc_types::u32 texture_binding_renderbuffer            = 0x8E53;
constexpr libc_types::u32 texture_renderbuffer_data_store_binding = 0x8E54;
constexpr libc_types::u32 texture_renderbuffer                    = 0x8E55;
constexpr libc_types::u32 sampler_renderbuffer                    = 0x8E56;
constexpr libc_types::u32 int_sampler_renderbuffer                = 0x8E57;
constexpr libc_types::u32 unsigned_int_sampler_renderbuffer       = 0x8E58;
constexpr libc_types::u32 max_sample_mask_words                   = 0x8E59;
} // namespace values
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_explicit_multisample
     * \param pname GLenum
     * \param index GLuint
     * \param val GLfloat *
     * \return void
     */
    STATICINLINE void get_multisamplefv(
        group::get_multisample_prop_nv pname, u32 index, span_f32 val)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultisamplefvNV)
    }
    glGetMultisamplefvNV(
        static_cast<GLenum>(pname),
        index,
        val.size() ? reinterpret_cast<GLfloat*>(val.data()) : nullptr);
    detail::error_check("GetMultisamplefvNV"sv);
}

/*!
 * \brief Part of GL_NV_explicit_multisample
 * \param index GLuint
 * \param mask GLbitfield
 * \return void
 */
STATICINLINE void sample_mask_indexed(u32 index, group::sample_mask_nv mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleMaskIndexedNV)
    }
    glSampleMaskIndexedNV(index, static_cast<GLenum>(mask));
    detail::error_check("SampleMaskIndexedNV"sv);
}

/*!
 * \brief Part of GL_NV_explicit_multisample
 * \param target GLenum
 * \param renderbuffer GLuint
 * \return void
 */
STATICINLINE void tex_renderbuffer(
    group::texture_target target, u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexRenderbufferNV)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glTexRenderbufferNV(static_cast<GLenum>(target), renderbuffer);
    detail::error_check("TexRenderbufferNV"sv);
}

} // namespace gl::nv::explicit_multisample
#endif // GL_NV_explicit_multisample
namespace gl::nv::explicit_multisample {
constexpr auto name = "GL_NV_explicit_multisample";
}
