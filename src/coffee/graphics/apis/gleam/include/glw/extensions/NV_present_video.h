#pragma once

#ifdef GL_NV_present_video
namespace gl::nv::present_video {
namespace values {
constexpr u32 frame            = 0x8E26;
constexpr u32 fields           = 0x8E27;
constexpr u32 current_time     = 0x8E28;
constexpr u32 num_fill_streams = 0x8E29;
constexpr u32 present_time     = 0x8E2A;
constexpr u32 present_duration = 0x8E2B;
} // namespace values
template<class span_GLint64EXT>
requires(concepts::span<span_GLint64EXT>&& std::is_same_v<
         std::decay_t<typename span_GLint64EXT::value_type>,
         std::decay_t<GLint64EXT>>)
    /*!
     * \brief Part of GL_NV_present_video
     * \param video_slot GLuint
     * \param pname GLenum
     * \param params GLint64EXT *
     * \return void
     */
    STATICINLINE
    void get_videoi64v(u32 video_slot, GLenum pname, span_GLint64EXT params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVideoi64vNV)
    }
    glGetVideoi64vNV(
        video_slot,
        pname,
        params.size() ? reinterpret_cast<GLint64EXT*>(params.data()) : nullptr);
    detail::error_check("GetVideoi64vNV"sv);
}

template<class span_i32>
requires(concepts::span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_present_video
     * \param video_slot GLuint
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_videoiv(u32 video_slot, GLenum pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVideoivNV)
    }
    glGetVideoivNV(
        video_slot,
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetVideoivNV"sv);
}

template<class span_GLuint64EXT>
requires(concepts::span<span_GLuint64EXT>&& std::is_same_v<
         std::decay_t<typename span_GLuint64EXT::value_type>,
         std::decay_t<GLuint64EXT>>)
    /*!
     * \brief Part of GL_NV_present_video
     * \param video_slot GLuint
     * \param pname GLenum
     * \param params GLuint64EXT *
     * \return void
     */
    STATICINLINE
    void get_videoui64v(u32 video_slot, GLenum pname, span_GLuint64EXT params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVideoui64vNV)
    }
    glGetVideoui64vNV(
        video_slot,
        pname,
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetVideoui64vNV"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_present_video
     * \param video_slot GLuint
     * \param pname GLenum
     * \param params GLuint *
     * \return void
     */
    STATICINLINE
    void get_videouiv(u32 video_slot, GLenum pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVideouivNV)
    }
    glGetVideouivNV(
        video_slot,
        pname,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetVideouivNV"sv);
}

/*!
 * \brief Part of GL_NV_present_video
 * \param video_slot GLuint
 * \param minPresentTime GLuint64EXT
 * \param beginPresentTimeId GLuint
 * \param presentDurationId GLuint
 * \param type GLenum
 * \param target0 GLenum
 * \param fill0 GLuint
 * \param target1 GLenum
 * \param fill1 GLuint
 * \param target2 GLenum
 * \param fill2 GLuint
 * \param target3 GLenum
 * \param fill3 GLuint
 * \return void
 */
STATICINLINE void present_frame_dual_fill(
    u32         video_slot,
    GLuint64EXT minPresentTime,
    u32         beginPresentTimeId,
    u32         presentDurationId,
    GLenum      type,
    GLenum      target0,
    u32         fill0,
    GLenum      target1,
    u32         fill1,
    GLenum      target2,
    u32         fill2,
    GLenum      target3,
    u32         fill3)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PresentFrameDualFillNV)
    }
    glPresentFrameDualFillNV(
        video_slot,
        minPresentTime,
        beginPresentTimeId,
        presentDurationId,
        type,
        target0,
        fill0,
        target1,
        fill1,
        target2,
        fill2,
        target3,
        fill3);
    detail::error_check("PresentFrameDualFillNV"sv);
}

/*!
 * \brief Part of GL_NV_present_video
 * \param video_slot GLuint
 * \param minPresentTime GLuint64EXT
 * \param beginPresentTimeId GLuint
 * \param presentDurationId GLuint
 * \param type GLenum
 * \param target0 GLenum
 * \param fill0 GLuint
 * \param key0 GLuint
 * \param target1 GLenum
 * \param fill1 GLuint
 * \param key1 GLuint
 * \return void
 */
STATICINLINE void present_frame_keyed(
    u32         video_slot,
    GLuint64EXT minPresentTime,
    u32         beginPresentTimeId,
    u32         presentDurationId,
    GLenum      type,
    GLenum      target0,
    u32         fill0,
    u32         key0,
    GLenum      target1,
    u32         fill1,
    u32         key1)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PresentFrameKeyedNV)
    }
    glPresentFrameKeyedNV(
        video_slot,
        minPresentTime,
        beginPresentTimeId,
        presentDurationId,
        type,
        target0,
        fill0,
        key0,
        target1,
        fill1,
        key1);
    detail::error_check("PresentFrameKeyedNV"sv);
}

} // namespace gl::nv::present_video
#endif // GL_NV_present_video
namespace gl::nv::present_video {
constexpr auto name = "GL_NV_present_video";
}
