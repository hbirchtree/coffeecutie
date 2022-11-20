#pragma once

#ifdef GL_NV_sample_locations
namespace gl::nv::sample_locations {
namespace values {
constexpr libc_types::u32 sample_location                           = 0x8E50;
constexpr libc_types::u32 sample_location_subpixel_bits             = 0x933D;
constexpr libc_types::u32 sample_location_pixel_grid_width          = 0x933E;
constexpr libc_types::u32 sample_location_pixel_grid_height         = 0x933F;
constexpr libc_types::u32 programmable_sample_location_table_size   = 0x9340;
constexpr libc_types::u32 programmable_sample_location              = 0x9341;
constexpr libc_types::u32 framebuffer_programmable_sample_locations = 0x9342;
constexpr libc_types::u32 framebuffer_sample_location_pixel_grid    = 0x9343;
} // namespace values
template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_sample_locations
     * \param target GLenum
     * \param start GLuint
     * \param count GLsizei
     * \param v const GLfloat *
     * \return void
     */
    STATICINLINE void framebuffer_sample_locationsfv(
        group::framebuffer_target target,
        u32                       start,
        i32                       count,
        span_const_f32 const&     v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferSampleLocationsfvNV)
    }
    glFramebufferSampleLocationsfvNV(
        static_cast<GLenum>(target),
        start,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("FramebufferSampleLocationsfvNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_sample_locations
     * \param framebuffer GLuint
     * \param start GLuint
     * \param count GLsizei
     * \param v const GLfloat *
     * \return void
     */
    STATICINLINE void named_framebuffer_sample_locationsfv(
        u32 framebuffer, u32 start, i32 count, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferSampleLocationsfvNV)
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferSampleLocationsfvNV(
        framebuffer,
        start,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("NamedFramebufferSampleLocationsfvNV"sv);
}

/*!
 * \brief Part of GL_NV_sample_locations

 * \return void
 */
STATICINLINE void resolve_depth_values()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResolveDepthValuesNV)
    }
    glResolveDepthValuesNV();
    detail::error_check("ResolveDepthValuesNV"sv);
}

} // namespace gl::nv::sample_locations
#endif // GL_NV_sample_locations
namespace gl::nv::sample_locations {
constexpr auto name = "GL_NV_sample_locations";
}
