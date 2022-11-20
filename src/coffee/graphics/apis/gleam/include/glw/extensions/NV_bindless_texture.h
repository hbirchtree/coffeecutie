#pragma once

#ifdef GL_NV_bindless_texture
namespace gl::nv::bindless_texture {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_NV_bindless_texture
 * \param texture GLuint
 * \param level GLint
 * \param layered GLboolean
 * \param layer GLint
 * \param format GLenum
 * \return GLuint64
 */
STATICINLINE GLuint64 get_image_handle(
    u32 texture, i32 level, bool layered, i32 layer, group::pixel_format format)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetImageHandleNV)
        glIsTexture(texture);
    }
    auto out = glGetImageHandleNV(
        texture, level, layered, layer, static_cast<GLenum>(format));
    detail::error_check("GetImageHandleNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param texture GLuint
 * \return GLuint64
 */
STATICINLINE GLuint64 get_texture_handle(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureHandleNV)
        glIsTexture(texture);
    }
    auto out = glGetTextureHandleNV(texture);
    detail::error_check("GetTextureHandleNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param texture GLuint
 * \param sampler GLuint
 * \return GLuint64
 */
STATICINLINE GLuint64 get_texture_sampler_handle(u32 texture, u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureSamplerHandleNV)
        glIsTexture(texture);
        glIsSampler(sampler);
    }
    auto out = glGetTextureSamplerHandleNV(texture, sampler);
    detail::error_check("GetTextureSamplerHandleNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param handle GLuint64
 * \return Boolean
 */
STATICINLINE GLboolean is_image_handle_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsImageHandleResidentNV)
    }
    auto out = glIsImageHandleResidentNV(handle);
    detail::error_check("IsImageHandleResidentNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param handle GLuint64
 * \return Boolean
 */
STATICINLINE GLboolean is_texture_handle_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTextureHandleResidentNV)
    }
    auto out = glIsTextureHandleResidentNV(handle);
    detail::error_check("IsTextureHandleResidentNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param handle GLuint64
 * \return void
 */
STATICINLINE void make_image_handle_non_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeImageHandleNonResidentNV)
    }
    glMakeImageHandleNonResidentNV(handle);
    detail::error_check("MakeImageHandleNonResidentNV"sv);
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param handle GLuint64
 * \param access GLenum
 * \return void
 */
STATICINLINE void make_image_handle_resident(u64 handle, GLenum access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeImageHandleResidentNV)
    }
    glMakeImageHandleResidentNV(handle, access);
    detail::error_check("MakeImageHandleResidentNV"sv);
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param handle GLuint64
 * \return void
 */
STATICINLINE void make_texture_handle_non_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeTextureHandleNonResidentNV)
    }
    glMakeTextureHandleNonResidentNV(handle);
    detail::error_check("MakeTextureHandleNonResidentNV"sv);
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param handle GLuint64
 * \return void
 */
STATICINLINE void make_texture_handle_resident(u64 handle)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeTextureHandleResidentNV)
    }
    glMakeTextureHandleResidentNV(handle);
    detail::error_check("MakeTextureHandleResidentNV"sv);
}

/*!
 * \brief Part of GL_NV_bindless_texture
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
        GLW_FPTR_CHECK(ProgramUniformHandleui64NV)
        glIsProgram(program);
    }
    glProgramUniformHandleui64NV(program, location, value);
    detail::error_check("ProgramUniformHandleui64NV"sv);
}

template<class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>)
    /*!
     * \brief Part of GL_NV_bindless_texture
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
        GLW_FPTR_CHECK(ProgramUniformHandleui64vNV)
        glIsProgram(program);
    }
    glProgramUniformHandleui64vNV(
        program,
        location,
        values.size(),
        values.size() ? reinterpret_cast<const GLuint64*>(values.data())
                      : nullptr);
    detail::error_check("ProgramUniformHandleui64vNV"sv);
}

/*!
 * \brief Part of GL_NV_bindless_texture
 * \param location GLint
 * \param value GLuint64
 * \return void
 */
STATICINLINE void uniform_handle(i32 location, u64 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformHandleui64NV)
    }
    glUniformHandleui64NV(location, value);
    detail::error_check("UniformHandleui64NV"sv);
}

template<class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>)
    /*!
     * \brief Part of GL_NV_bindless_texture
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
        GLW_FPTR_CHECK(UniformHandleui64vNV)
    }
    glUniformHandleui64vNV(
        location,
        value.size(),
        value.size() ? reinterpret_cast<const GLuint64*>(value.data())
                     : nullptr);
    detail::error_check("UniformHandleui64vNV"sv);
}

} // namespace gl::nv::bindless_texture
#endif // GL_NV_bindless_texture
namespace gl::nv::bindless_texture {
constexpr auto name = "GL_NV_bindless_texture";
}
