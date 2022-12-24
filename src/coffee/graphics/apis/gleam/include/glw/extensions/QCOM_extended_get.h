#pragma once

#ifdef GL_QCOM_extended_get
namespace gl::qcom::extended_get {
namespace values {
constexpr libc_types::u32 texture_width           = 0x8BD2;
constexpr libc_types::u32 texture_height          = 0x8BD3;
constexpr libc_types::u32 texture_depth           = 0x8BD4;
constexpr libc_types::u32 texture_internal_format = 0x8BD5;
constexpr libc_types::u32 texture_format          = 0x8BD6;
constexpr libc_types::u32 texture_type            = 0x8BD7;
constexpr libc_types::u32 texture_image_valid     = 0x8BD8;
constexpr libc_types::u32 texture_num_levels      = 0x8BD9;
constexpr libc_types::u32 texture_target          = 0x8BDA;
constexpr libc_types::u32 texture_object_valid    = 0x8BDB;
constexpr libc_types::u32 state_restore           = 0x8BDC;
} // namespace values
template<class span_void>
requires(semantic::concepts::Span<span_void>)
    /*!
     * \brief Part of GL_QCOM_extended_get
     * \param target GLenum
     * \param params void **
     * \return void
     */
    STATICINLINE void ext_get_buffer_pointerv(GLenum target, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetBufferPointervQCOM)
    }
    glExtGetBufferPointervQCOM(
        target,
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("ExtGetBufferPointervQCOM"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_QCOM_extended_get
     * \param buffers GLuint *
     * \param maxBuffers GLint
     * \param numBuffers GLint *
     * \return void
     */
    STATICINLINE
    void ext_get_buffers(span_u32 buffers, i32 maxBuffers, i32& numBuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetBuffersQCOM)
    }
    glExtGetBuffersQCOM(
        buffers.size() ? reinterpret_cast<GLuint*>(buffers.data()) : nullptr,
        maxBuffers,
        &numBuffers);
    detail::error_check("ExtGetBuffersQCOM"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_QCOM_extended_get
     * \param framebuffers GLuint *
     * \param maxFramebuffers GLint
     * \param numFramebuffers GLint *
     * \return void
     */
    STATICINLINE void ext_get_framebuffers(
        span_u32 framebuffers, i32 maxFramebuffers, i32& numFramebuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetFramebuffersQCOM)
    }
    glExtGetFramebuffersQCOM(
        framebuffers.size() ? reinterpret_cast<GLuint*>(framebuffers.data())
                            : nullptr,
        maxFramebuffers,
        &numFramebuffers);
    detail::error_check("ExtGetFramebuffersQCOM"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_QCOM_extended_get
     * \param renderbuffers GLuint *
     * \param maxRenderbuffers GLint
     * \param numRenderbuffers GLint *
     * \return void
     */
    STATICINLINE void ext_get_renderbuffers(
        span_u32 renderbuffers, i32 maxRenderbuffers, i32& numRenderbuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetRenderbuffersQCOM)
    }
    glExtGetRenderbuffersQCOM(
        renderbuffers.size() ? reinterpret_cast<GLuint*>(renderbuffers.data())
                             : nullptr,
        maxRenderbuffers,
        &numRenderbuffers);
    detail::error_check("ExtGetRenderbuffersQCOM"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_QCOM_extended_get
     * \param texture GLuint
     * \param face GLenum
     * \param level GLint
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void ext_get_tex_level_parameter(
        u32 texture, GLenum face, i32 level, GLenum pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetTexLevelParameterivQCOM)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glExtGetTexLevelParameterivQCOM(
        texture,
        face,
        level,
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("ExtGetTexLevelParameterivQCOM"sv);
}

template<class size_3_i32, class span_void, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_void>)
    /*!
     * \brief Part of GL_QCOM_extended_get
     * \param target GLenum
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param format GLenum
     * \param type GLenum
     * \param texels void *
     * \return void
     */
    STATICINLINE void ext_get_tex_sub_image(
        GLenum              target,
        i32                 level,
        vec_3_i32 const&    xoffset,
        size_3_i32 const&   width,
        group::pixel_format format,
        group::pixel_type   type,
        span_void           texels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetTexSubImageQCOM)
    }
    glExtGetTexSubImageQCOM(
        target,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        texels.size() ? reinterpret_cast<void*>(texels.data()) : nullptr);
    detail::error_check("ExtGetTexSubImageQCOM"sv);
}

template<class span_i32, class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_i32>&&
                              std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Part of GL_QCOM_extended_get
     * \param textures GLuint *
     * \param maxTextures GLint
     * \param numTextures GLint *
     * \return void
     */
    STATICINLINE void ext_get_textures(
        span_u32 textures, i32 maxTextures, span_i32 numTextures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetTexturesQCOM)
    }
    glExtGetTexturesQCOM(
        textures.size() ? reinterpret_cast<GLuint*>(textures.data()) : nullptr,
        maxTextures,
        numTextures.size() ? reinterpret_cast<GLint*>(numTextures.data())
                           : nullptr);
    detail::error_check("ExtGetTexturesQCOM"sv);
}

/*!
 * \brief Part of GL_QCOM_extended_get
 * \param target GLenum
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void ext_tex_object_state_overridei(
    GLenum target, GLenum pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtTexObjectStateOverrideiQCOM)
    }
    glExtTexObjectStateOverrideiQCOM(target, pname, param);
    detail::error_check("ExtTexObjectStateOverrideiQCOM"sv);
}

} // namespace gl::qcom::extended_get
#endif // GL_QCOM_extended_get
namespace gl::qcom::extended_get {
constexpr auto name = "GL_QCOM_extended_get";
}
