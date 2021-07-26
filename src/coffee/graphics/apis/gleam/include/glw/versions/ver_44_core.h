#ifdef GL_VERSION_4_4
/* Introduced in GL core 4.4 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 4>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void buffer_storage(
        group::buffer_storage_target target,
        span_const_void const&       data,
        group::buffer_storage_mask   flags)
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
    detail::error_check("BufferStorage"sv);
}

/* Introduced in GL core 4.4 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 4>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_tex_image(
        u32                    texture,
        i32                    level,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearTexImage)
        glIsTexture(texture);
    }
    glClearTexImage(
        texture,
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearTexImage"sv);
}

/* Introduced in GL core 4.4 */
template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 4>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_tex_sub_image(
        u32                    texture,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearTexSubImage)
        glIsTexture(texture);
    }
    glClearTexSubImage(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearTexSubImage"sv);
}

/* Introduced in GL core 4.4 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 4>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void bind_buffers_base(
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

/* Introduced in GL core 4.4 */
template<
    class span_const_GLintptr,
    class span_const_GLsizeiptr,
    class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 4>>&&
        semantic::concepts::Span<span_const_u32>&& std::is_same_v<
            std::decay_t<typename span_const_u32::value_type>,
            std::decay_t<u32>>&& semantic::concepts::Span<span_const_GLintptr>&&
                                 std::is_same_v<
                std::decay_t<typename span_const_GLintptr::value_type>,
                std::decay_t<GLintptr>>&&     semantic::concepts::
                Span<span_const_GLsizeiptr>&& std::is_same_v<
                    std::decay_t<typename span_const_GLsizeiptr::value_type>,
                    std::decay_t<GLsizeiptr>>) STATICINLINE
    void bind_buffers_range(
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

/* Introduced in GL core 4.4 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 4>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
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

/* Introduced in GL core 4.4 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 4>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void bind_samplers(u32 first, span_const_u32 const& samplers)
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

/* Introduced in GL core 4.4 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 4>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void bind_textures(u32 first, span_const_u32 const& textures)
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

/* Introduced in GL core 4.4 */
template<class span_const_GLintptr, class span_const_i32, class span_const_u32>
requires(MinimumVersion<Current, Version<4, 4>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>&&
                 semantic::concepts::Span<span_const_GLintptr>&& std::is_same_v<
                     std::decay_t<typename span_const_GLintptr::value_type>,
                     std::decay_t<GLintptr>>&&
                     semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                         std::decay_t<typename span_const_i32::value_type>,
                         std::decay_t<i32>>) STATICINLINE
    void bind_vertex_buffers(
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

#endif // GL_VERSION_4_4
