#pragma once

#ifdef GL_ARB_multi_bind
namespace gl::arb::multi_bind {
namespace values {
} // namespace values

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_multi_bind
 * \param target GLenum
 * \param first GLuint
 * \param count GLsizei
 * \param buffers const GLuint *
 * \return void
 */
STATICINLINE void bind_buffers_base(
    group::buffer_target_arb target,
    u32                      first,
    span_const_u32 const&    buffers,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BindBuffersBase)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glBindBuffersBase,
        gsl::span<const char>(
            reinterpret_cast<const char*>(buffers.data()),
            buffers.size_bytes()),
        target,
        first);
#endif
    glBindBuffersBase(
        static_cast<GLenum>(target),
        first,
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr);
    detail::error_check("BindBuffersBase"sv, check_errors);
}

template<
    class span_const_GLintptr,
    class span_const_GLsizeiptr,
    class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_const_GLintptr> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLintptr::value_type>,
        std::decay_t<GLintptr>> &&
    concepts::span<span_const_GLsizeiptr> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLsizeiptr::value_type>,
        std::decay_t<GLsizeiptr>>)
/*!
 * \brief Part of GL_ARB_multi_bind
 * \param target GLenum
 * \param first GLuint
 * \param count GLsizei
 * \param buffers const GLuint *
 * \param offsets const GLintptr *
 * \param sizes const GLsizeiptr *
 * \return void
 */
STATICINLINE void bind_buffers_range(
    group::buffer_target_arb     target,
    u32                          first,
    span_const_u32 const&        buffers,
    span_const_GLintptr const&   offsets,
    span_const_GLsizeiptr const& sizes,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BindBuffersRange)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glBindBuffersRange,
        target,
        first,
        gsl::span<const char>(
            reinterpret_cast<const char*>(buffers.data()),
            buffers.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(offsets.data()),
            offsets.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(sizes.data()), sizes.size_bytes()));
#endif
    glBindBuffersRange(
        static_cast<GLenum>(target),
        first,
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr,
        offsets.size() ? reinterpret_cast<const GLintptr*>(offsets.data())
                       : nullptr,
        sizes.size() ? reinterpret_cast<const GLsizeiptr*>(sizes.data())
                     : nullptr);
    detail::error_check("BindBuffersRange"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_multi_bind
 * \param first GLuint
 * \param count GLsizei
 * \param textures const GLuint *
 * \return void
 */
STATICINLINE void bind_image_textures(
    u32                   first,
    span_const_u32 const& textures,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BindImageTextures)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glBindImageTextures,
        gsl::span<const char>(
            reinterpret_cast<const char*>(textures.data()),
            textures.size_bytes()),
        first);
#endif
    glBindImageTextures(
        first,
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("BindImageTextures"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_multi_bind
 * \param first GLuint
 * \param count GLsizei
 * \param samplers const GLuint *
 * \return void
 */
STATICINLINE void bind_samplers(
    u32                   first,
    span_const_u32 const& samplers,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BindSamplers)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glBindSamplers,
        gsl::span<const char>(
            reinterpret_cast<const char*>(samplers.data()),
            samplers.size_bytes()),
        first);
#endif
    glBindSamplers(
        first,
        samplers.size(),
        samplers.size() ? reinterpret_cast<const GLuint*>(samplers.data())
                        : nullptr);
    detail::error_check("BindSamplers"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_multi_bind
 * \param first GLuint
 * \param count GLsizei
 * \param textures const GLuint *
 * \return void
 */
STATICINLINE void bind_textures(
    u32                   first,
    span_const_u32 const& textures,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BindTextures)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glBindTextures,
        gsl::span<const char>(
            reinterpret_cast<const char*>(textures.data()),
            textures.size_bytes()),
        first);
#endif
    glBindTextures(
        first,
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("BindTextures"sv, check_errors);
}

template<class span_const_GLintptr, class span_const_i32, class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_const_GLintptr> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLintptr::value_type>,
        std::decay_t<GLintptr>> &&
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_multi_bind
 * \param first GLuint
 * \param count GLsizei
 * \param buffers const GLuint *
 * \param offsets const GLintptr *
 * \param strides const GLsizei *
 * \return void
 */
STATICINLINE void bind_vertex_buffers(
    u32                        first,
    span_const_u32 const&      buffers,
    span_const_GLintptr const& offsets,
    span_const_i32 const&      strides,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BindVertexBuffers)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glBindVertexBuffers,
        first,
        gsl::span<const char>(
            reinterpret_cast<const char*>(buffers.data()),
            buffers.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(offsets.data()),
            offsets.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(strides.data()),
            strides.size_bytes()));
#endif
    glBindVertexBuffers(
        first,
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr,
        offsets.size() ? reinterpret_cast<const GLintptr*>(offsets.data())
                       : nullptr,
        strides.size() ? reinterpret_cast<const GLsizei*>(strides.data())
                       : nullptr);
    detail::error_check("BindVertexBuffers"sv, check_errors);
}

} // namespace gl::arb::multi_bind
#endif // GL_ARB_multi_bind
namespace gl::arb::multi_bind {
constexpr auto name = "GL_ARB_multi_bind";
}
