#pragma once

#ifdef GL_NV_video_capture
namespace gl::nv::video_capture {
namespace values {
constexpr u32 video_buffer                          = 0x9020;
constexpr u32 video_buffer_binding                  = 0x9021;
constexpr u32 field_upper                           = 0x9022;
constexpr u32 field_lower                           = 0x9023;
constexpr u32 num_video_capture_streams             = 0x9024;
constexpr u32 next_video_capture_buffer_status      = 0x9025;
constexpr u32 video_capture_to_422_supported        = 0x9026;
constexpr u32 last_video_capture_status             = 0x9027;
constexpr u32 video_buffer_pitch                    = 0x9028;
constexpr u32 video_color_conversion_matrix         = 0x9029;
constexpr u32 video_color_conversion_max            = 0x902A;
constexpr u32 video_color_conversion_min            = 0x902B;
constexpr u32 video_color_conversion_offset         = 0x902C;
constexpr u32 video_buffer_internal_format          = 0x902D;
constexpr u32 partial_success                       = 0x902E;
constexpr u32 success                               = 0x902F;
constexpr u32 failure                               = 0x9030;
constexpr u32 ycbycr8_422                           = 0x9031;
constexpr u32 ycbaycr8a_4224                        = 0x9032;
constexpr u32 z6y10z6cb10z6y10z6cr10_422            = 0x9033;
constexpr u32 z6y10z6cb10z6a10z6y10z6cr10z6a10_4224 = 0x9034;
constexpr u32 z4y12z4cb12z4y12z4cr12_422            = 0x9035;
constexpr u32 z4y12z4cb12z4a12z4y12z4cr12z4a12_4224 = 0x9036;
constexpr u32 z4y12z4cb12z4cr12_444                 = 0x9037;
constexpr u32 video_capture_frame_width             = 0x9038;
constexpr u32 video_capture_frame_height            = 0x9039;
constexpr u32 video_capture_field_upper_height      = 0x903A;
constexpr u32 video_capture_field_lower_height      = 0x903B;
constexpr u32 video_capture_surface_origin          = 0x903C;
} // namespace values

/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \return void
 */
STATICINLINE void begin_video_capture(u32 video_capture_slot)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginVideoCaptureNV)
    }
    glBeginVideoCaptureNV(video_capture_slot);
    detail::error_check("BeginVideoCaptureNV"sv);
}

/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param stream GLuint
 * \param frame_region GLenum
 * \param offset GLintptrARB
 * \return void
 */
STATICINLINE void bind_video_capture_stream_buffer(
    u32 video_capture_slot, u32 stream, GLenum frame_region, GLintptrARB offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVideoCaptureStreamBufferNV)
    }
    glBindVideoCaptureStreamBufferNV(
        video_capture_slot, stream, frame_region, offset);
    detail::error_check("BindVideoCaptureStreamBufferNV"sv);
}

/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param stream GLuint
 * \param frame_region GLenum
 * \param target GLenum
 * \param texture GLuint
 * \return void
 */
STATICINLINE void bind_video_capture_stream_texture(
    u32    video_capture_slot,
    u32    stream,
    GLenum frame_region,
    GLenum target,
    u32    texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVideoCaptureStreamTextureNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindVideoCaptureStreamTextureNV(
        video_capture_slot, stream, frame_region, target, texture);
    detail::error_check("BindVideoCaptureStreamTextureNV"sv);
}

/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \return void
 */
STATICINLINE void end_video_capture(u32 video_capture_slot)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndVideoCaptureNV)
    }
    glEndVideoCaptureNV(video_capture_slot);
    detail::error_check("EndVideoCaptureNV"sv);
}

template<class span_f64>
requires(
    concepts::span<span_f64> && std::is_same_v<
                                    std::decay_t<typename span_f64::value_type>,
                                    std::decay_t<f64>>)
/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param stream GLuint
 * \param pname GLenum
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_video_capture_streamdv(
    u32 video_capture_slot, u32 stream, GLenum pname, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVideoCaptureStreamdvNV)
    }
    glGetVideoCaptureStreamdvNV(
        video_capture_slot,
        stream,
        pname,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetVideoCaptureStreamdvNV"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param stream GLuint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_video_capture_streamfv(
    u32 video_capture_slot, u32 stream, GLenum pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVideoCaptureStreamfvNV)
    }
    glGetVideoCaptureStreamfvNV(
        video_capture_slot,
        stream,
        pname,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetVideoCaptureStreamfvNV"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param stream GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_video_capture_streamiv(
    u32 video_capture_slot, u32 stream, GLenum pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVideoCaptureStreamivNV)
    }
    glGetVideoCaptureStreamivNV(
        video_capture_slot,
        stream,
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetVideoCaptureStreamivNV"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_video_captureiv(
    u32 video_capture_slot, GLenum pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVideoCaptureivNV)
    }
    glGetVideoCaptureivNV(
        video_capture_slot,
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetVideoCaptureivNV"sv);
}

template<class span_GLuint64EXT, class span_u32>
requires(
    concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param sequence_num GLuint *
 * \param capture_time GLuint64EXT *
 * \return GLenum
 */
STATICINLINE GLenum video_capture(
    u32              video_capture_slot,
    span_u32         sequence_num,
    span_GLuint64EXT capture_time)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VideoCaptureNV)
    }
    auto out = glVideoCaptureNV(
        video_capture_slot,
        sequence_num.size() ? reinterpret_cast<GLuint*>(sequence_num.data())
                            : nullptr,
        capture_time.size()
            ? reinterpret_cast<GLuint64EXT*>(capture_time.data())
            : nullptr);
    detail::error_check("VideoCaptureNV"sv);
    return out;
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param stream GLuint
 * \param pname GLenum
 * \param params const GLdouble *
 * \return void
 */
STATICINLINE void video_capture_stream_parameter(
    u32                   video_capture_slot,
    u32                   stream,
    GLenum                pname,
    span_const_f64 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VideoCaptureStreamParameterdvNV)
    }
    glVideoCaptureStreamParameterdvNV(
        video_capture_slot,
        stream,
        pname,
        params.size() ? reinterpret_cast<const GLdouble*>(params.data())
                      : nullptr);
    detail::error_check("VideoCaptureStreamParameterdvNV"sv);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param stream GLuint
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void video_capture_stream_parameter(
    u32                   video_capture_slot,
    u32                   stream,
    GLenum                pname,
    span_const_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VideoCaptureStreamParameterfvNV)
    }
    glVideoCaptureStreamParameterfvNV(
        video_capture_slot,
        stream,
        pname,
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("VideoCaptureStreamParameterfvNV"sv);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_NV_video_capture
 * \param video_capture_slot GLuint
 * \param stream GLuint
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void video_capture_stream_parameter(
    u32                   video_capture_slot,
    u32                   stream,
    GLenum                pname,
    span_const_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VideoCaptureStreamParameterivNV)
    }
    glVideoCaptureStreamParameterivNV(
        video_capture_slot,
        stream,
        pname,
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("VideoCaptureStreamParameterivNV"sv);
}

} // namespace gl::nv::video_capture
#endif // GL_NV_video_capture
namespace gl::nv::video_capture {
constexpr auto name = "GL_NV_video_capture";
}
