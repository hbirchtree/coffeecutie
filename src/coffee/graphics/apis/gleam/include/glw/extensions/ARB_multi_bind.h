#pragma once

#ifdef GL_ARB_multi_bind
namespace gl::arb::multi_bind {
namespace values {
} // namespace values
template<class span_const_u32>
requires(concepts::span<span_const_u32>&& std::is_same_v<
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
        span_const_u32 const&    buffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBuffersBase)
    }
    glBindBuffersBase(
        static_cast<GLenum>(target),
        first,
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr);
    detail::error_check("BindBuffersBase"sv);
}

template<
    class span_const_GLintptr,
    class span_const_GLsizeiptr,
    class span_const_u32>
requires(concepts::span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& concepts::span<span_const_GLintptr>&&
                                          std::is_same_v<
                 std::decay_t<typename span_const_GLintptr::value_type>,
                 std::decay_t<GLintptr>>&&
                 concepts::span<span_const_GLsizeiptr>&& std::is_same_v<
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
        span_const_GLsizeiptr const& sizes)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBuffersRange)
    }
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
    detail::error_check("BindBuffersRange"sv);
}

template<class span_const_u32>
requires(concepts::span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_multi_bind
     * \param first GLuint
     * \param count GLsizei
     * \param textures const GLuint *
     * \return void
     */
    STATICINLINE
    void bind_image_textures(u32 first, span_const_u32 const& textures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindImageTextures)
    }
    glBindImageTextures(
        first,
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("BindImageTextures"sv);
}

template<class span_const_u32>
requires(concepts::span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_multi_bind
     * \param first GLuint
     * \param count GLsizei
     * \param samplers const GLuint *
     * \return void
     */
    STATICINLINE void bind_samplers(u32 first, span_const_u32 const& samplers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindSamplers)
    }
    glBindSamplers(
        first,
        samplers.size(),
        samplers.size() ? reinterpret_cast<const GLuint*>(samplers.data())
                        : nullptr);
    detail::error_check("BindSamplers"sv);
}

template<class span_const_u32>
requires(concepts::span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_multi_bind
     * \param first GLuint
     * \param count GLsizei
     * \param textures const GLuint *
     * \return void
     */
    STATICINLINE void bind_textures(u32 first, span_const_u32 const& textures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTextures)
    }
    glBindTextures(
        first,
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("BindTextures"sv);
}

template<class span_const_GLintptr, class span_const_i32, class span_const_u32>
requires(concepts::span<span_const_u32>&&  std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& concepts::span<span_const_GLintptr>&&
                                           std::is_same_v<
                 std::decay_t<typename span_const_GLintptr::value_type>,
                 std::decay_t<GLintptr>>&& concepts::span<span_const_i32>&&
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
        span_const_i32 const&      strides)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexBuffers)
    }
    glBindVertexBuffers(
        first,
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr,
        offsets.size() ? reinterpret_cast<const GLintptr*>(offsets.data())
                       : nullptr,
        strides.size() ? reinterpret_cast<const GLsizei*>(strides.data())
                       : nullptr);
    detail::error_check("BindVertexBuffers"sv);
}

} // namespace gl::arb::multi_bind
#endif // GL_ARB_multi_bind
namespace gl::arb::multi_bind {
constexpr auto name = "GL_ARB_multi_bind";
}
