#ifdef GL_VERSION_4_4
template<class span_const_void>
requires(
    MinimumVersion<Current, Version<4, 4>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glBufferStorage. Introduced in GL core 4.4
 * \param target GLenum
 * \param size GLsizeiptr
 * \param data const void *
 * \param flags GLbitfield
 * \return void
 */
STATICINLINE void buffer_storage(
    group::buffer_storage_target target,
    span_const_void const&       data,
    group::buffer_storage_mask   flags,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferStorage)
    }
    glBufferStorage(
        static_cast<GLenum>(target),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(flags));
    detail::error_check("BufferStorage"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<4, 4>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glClearTexImage. Introduced in GL core 4.4
 * \param texture GLuint
 * \param level GLint
 * \param format GLenum
 * \param type GLenum
 * \param data const void *
 * \return void
 */
STATICINLINE void clear_tex_image(
    u32                    texture,
    i32                    level,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearTexImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glClearTexImage(
        texture,
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearTexImage"sv, check_errors);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(
    MinimumVersion<Current, Version<4, 4>> &&
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Wraps around glClearTexSubImage. Introduced in GL core 4.4
 * \param texture GLuint
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param data const void *
 * \return void
 */
STATICINLINE void clear_tex_sub_image(
    u32                    texture,
    i32                    level,
    vec_3_i32 const&       xoffset,
    size_3_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearTexSubImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glClearTexSubImage(
        texture,
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearTexSubImage"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 4>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glBindBuffersBase. Introduced in GL core 4.4
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
    detail::error_check("BindBuffersBase"sv, check_errors);
}

template<
    class span_const_GLintptr,
    class span_const_GLsizeiptr,
    class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 4>> && concepts::span<span_const_u32> &&
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
 * \brief Wraps around glBindBuffersRange. Introduced in GL core 4.4
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
    detail::error_check("BindBuffersRange"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 4>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glBindImageTextures. Introduced in GL core 4.4
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
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindImageTextures)
    }
    glBindImageTextures(
        first,
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("BindImageTextures"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 4>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glBindSamplers. Introduced in GL core 4.4
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
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindSamplers)
    }
    glBindSamplers(
        first,
        samplers.size(),
        samplers.size() ? reinterpret_cast<const GLuint*>(samplers.data())
                        : nullptr);
    detail::error_check("BindSamplers"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 4>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glBindTextures. Introduced in GL core 4.4
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
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTextures)
    }
    glBindTextures(
        first,
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("BindTextures"sv, check_errors);
}

template<class span_const_GLintptr, class span_const_i32, class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 4>> && concepts::span<span_const_u32> &&
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
 * \brief Wraps around glBindVertexBuffers. Introduced in GL core 4.4
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
    detail::error_check("BindVertexBuffers"sv, check_errors);
}

#endif // GL_VERSION_4_4
