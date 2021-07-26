#ifdef GL_VERSION_4_2
/* Introduced in GL core 4.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>) STATICINLINE
    void draw_arrays_instanced_base_instance(
        group::primitive_type mode,
        i32                   first,
        i32                   count,
        i32                   instancecount,
        u32                   baseinstance)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstancedBaseInstance)
    }
    glDrawArraysInstancedBaseInstance(
        static_cast<GLenum>(mode), first, count, instancecount, baseinstance);
    detail::error_check("DrawArraysInstancedBaseInstance"sv);
}

/* Introduced in GL core 4.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>) STATICINLINE
    void draw_elements_instanced_base_instance(
        group::primitive_type mode,
        i32                   count,
        group::primitive_type type,
        ptroff                indices,
        i32                   instancecount,
        u32                   baseinstance)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedBaseInstance)
    }
    glDrawElementsInstancedBaseInstance(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount,
        baseinstance);
    detail::error_check("DrawElementsInstancedBaseInstance"sv);
}

/* Introduced in GL core 4.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>) STATICINLINE
    void draw_elements_instanced_base_vertex_base_instance(
        group::primitive_type     mode,
        i32                       count,
        group::draw_elements_type type,
        ptroff                    indices,
        i32                       instancecount,
        i32                       basevertex,
        u32                       baseinstance)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedBaseVertexBaseInstance)
    }
    glDrawElementsInstancedBaseVertexBaseInstance(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount,
        basevertex,
        baseinstance);
    detail::error_check("DrawElementsInstancedBaseVertexBaseInstance"sv);
}

/* Introduced in GL core 4.2 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 2>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_internalformativ(
        group::texture_target       target,
        group::internal_format      internalformat,
        group::internal_format_prop pname,
        span_i32                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInternalformativ)
    }
    glGetInternalformativ(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(pname),
        params.size(),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetInternalformativ"sv);
}

/* Introduced in GL core 4.2 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 2>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_active_atomic_counter_bufferiv(
        u32                               program,
        u32                               bufferIndex,
        group::atomic_counter_buffer_prop pname,
        span_i32                          params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveAtomicCounterBufferiv)
        glIsProgram(program);
    }
    glGetActiveAtomicCounterBufferiv(
        program,
        bufferIndex,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveAtomicCounterBufferiv"sv);
}

/* Introduced in GL core 4.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>) STATICINLINE
    void bind_image_texture(
        u32                      unit,
        u32                      texture,
        i32                      level,
        bool                     layered,
        i32                      layer,
        group::buffer_access_arb access,
        group::internal_format   format)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindImageTexture)
        glIsTexture(texture);
    }
    glBindImageTexture(
        unit,
        texture,
        level,
        layered,
        layer,
        static_cast<GLenum>(access),
        static_cast<GLenum>(format));
    detail::error_check("BindImageTexture"sv);
}

/* Introduced in GL core 4.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>) STATICINLINE
    void memory_barrier(group::memory_barrier_mask barriers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrier)
    }
    glMemoryBarrier(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrier"sv);
}

/* Introduced in GL core 4.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>) STATICINLINE
    void tex_storage_1d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        i32                          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage1D)
    }
    glTexStorage1D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width);
    detail::error_check("TexStorage1D"sv);
}

/* Introduced in GL core 4.2 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 2>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void tex_storage_2d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage2D)
    }
    glTexStorage2D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TexStorage2D"sv);
}

/* Introduced in GL core 4.2 */
template<class size_3_i32>
requires(MinimumVersion<Current, Version<4, 2>>&&
             semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void tex_storage_3d(
        group::texture_target        target,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage3D)
    }
    glTexStorage3D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TexStorage3D"sv);
}

/* Introduced in GL core 4.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>) STATICINLINE
    void draw_transform_feedback_instanced(
        group::primitive_type mode, u32 id, i32 instancecount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackInstanced)
    }
    glDrawTransformFeedbackInstanced(
        static_cast<GLenum>(mode), id, instancecount);
    detail::error_check("DrawTransformFeedbackInstanced"sv);
}

/* Introduced in GL core 4.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>) STATICINLINE
    void draw_transform_feedback_stream_instanced(
        group::primitive_type mode, u32 id, u32 stream, i32 instancecount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackStreamInstanced)
    }
    glDrawTransformFeedbackStreamInstanced(
        static_cast<GLenum>(mode), id, stream, instancecount);
    detail::error_check("DrawTransformFeedbackStreamInstanced"sv);
}

#endif // GL_VERSION_4_2
