#pragma once

#ifdef GL_IMG_bindless_texture
namespace gl::img::bindless_texture {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_IMG_bindless_texture
 * \param texture GLuint
 * \return GLuint64
 */
STATICINLINE GLuint64 get_texture_handle(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureHandleIMG)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    auto out = glGetTextureHandleIMG(texture);
    detail::error_check("GetTextureHandleIMG"sv);
    return out;
}

/*!
 * \brief Part of GL_IMG_bindless_texture
 * \param texture GLuint
 * \param sampler GLuint
 * \return GLuint64
 */
STATICINLINE GLuint64 get_texture_sampler_handle(u32 texture, u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureSamplerHandleIMG)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    auto out = glGetTextureSamplerHandleIMG(texture, sampler);
    detail::error_check("GetTextureSamplerHandleIMG"sv);
    return out;
}

/*!
 * \brief Part of GL_IMG_bindless_texture
 * \param program GLuint
 * \param location GLint
 * \param value GLuint64
 * \return void
 */
STATICINLINE void program_uniform_handle(u32 program, i32 location, u64 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformHandleui64IMG)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformHandleui64IMG(program, location, value);
    detail::error_check("ProgramUniformHandleui64IMG"sv);
}

template<class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>)
    /*!
     * \brief Part of GL_IMG_bindless_texture
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param values const GLuint64 *
     * \return void
     */
    STATICINLINE void program_uniform_handle(
        u32 program, i32 location, span_const_u64 const& values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformHandleui64vIMG)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformHandleui64vIMG(
        program,
        location,
        values.size(),
        values.size() ? reinterpret_cast<const GLuint64*>(values.data())
                      : nullptr);
    detail::error_check("ProgramUniformHandleui64vIMG"sv);
}

/*!
 * \brief Part of GL_IMG_bindless_texture
 * \param location GLint
 * \param value GLuint64
 * \return void
 */
STATICINLINE void uniform_handle(i32 location, u64 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformHandleui64IMG)
    }
    glUniformHandleui64IMG(location, value);
    detail::error_check("UniformHandleui64IMG"sv);
}

template<class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>)
    /*!
     * \brief Part of GL_IMG_bindless_texture
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64 *
     * \return void
     */
    STATICINLINE void uniform_handle(i32 location, span_const_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformHandleui64vIMG)
    }
    glUniformHandleui64vIMG(
        location,
        value.size(),
        value.size() ? reinterpret_cast<const GLuint64*>(value.data())
                     : nullptr);
    detail::error_check("UniformHandleui64vIMG"sv);
}

} // namespace gl::img::bindless_texture
#endif // GL_IMG_bindless_texture
namespace gl::img::bindless_texture {
constexpr auto name = "GL_IMG_bindless_texture";
}
