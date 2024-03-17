#ifdef GL_VERSION_4_2
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>)
/*!
 * \brief Wraps around glDrawArraysInstancedBaseInstance. Introduced in GL
 * core 4.2 \param mode GLenum \param first GLint \param count GLsizei \param
 * instancecount GLsizei \param baseinstance GLuint \return void
 */
STATICINLINE void draw_arrays_instanced_base_instance(
    group::primitive_type mode,
    i32                   first,
    i32                   count,
    i32                   instancecount,
    u32                   baseinstance,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstancedBaseInstance)
    }
    glDrawArraysInstancedBaseInstance(
        static_cast<GLenum>(mode), first, count, instancecount, baseinstance);
    detail::error_check("DrawArraysInstancedBaseInstance"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>)
/*!
 * \brief Wraps around glDrawElementsInstancedBaseInstance. Introduced in GL
 * core 4.2 \param mode GLenum \param count GLsizei \param type GLenum \param
 * indices const void * \param instancecount GLsizei \param baseinstance GLuint
 * \return void
 */
STATICINLINE void draw_elements_instanced_base_instance(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       instancecount,
    u32                       baseinstance,
    error_check               check_errors = error_check::on)
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
    detail::error_check("DrawElementsInstancedBaseInstance"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>)
/*!
 * \brief Wraps around glDrawElementsInstancedBaseVertexBaseInstance. Introduced
 * in GL core 4.2 \param mode GLenum \param count GLsizei \param type GLenum
 * \param indices const void *
 * \param instancecount GLsizei
 * \param basevertex GLint
 * \param baseinstance GLuint
 * \return void
 */
STATICINLINE void draw_elements_instanced_base_vertex_base_instance(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       instancecount,
    i32                       basevertex,
    u32                       baseinstance,
    error_check               check_errors = error_check::on)
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
    detail::error_check(
        "DrawElementsInstancedBaseVertexBaseInstance"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<4, 2>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetInternalformativ. Introduced in GL core 4.2
 * \param target GLenum
 * \param internalformat GLenum
 * \param pname GLenum
 * \param count GLsizei
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_internalformativ(
    group::texture_target       target,
    group::internal_format      internalformat,
    group::internal_format_prop pname,
    span_i32                    params,
    error_check                 check_errors = error_check::on)
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
    detail::error_check("GetInternalformativ"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<4, 2>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetActiveAtomicCounterBufferiv. Introduced in GL
 * core 4.2 \param program GLuint \param bufferIndex GLuint \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_active_atomic_counter_bufferiv(
    u32                               program,
    u32                               bufferIndex,
    group::atomic_counter_buffer_prop pname,
    span_i32                          params,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveAtomicCounterBufferiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveAtomicCounterBufferiv(
        program,
        bufferIndex,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveAtomicCounterBufferiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>)
/*!
 * \brief Wraps around glBindImageTexture. Introduced in GL core 4.2
 * \param unit GLuint
 * \param texture GLuint
 * \param level GLint
 * \param layered GLboolean
 * \param layer GLint
 * \param access GLenum
 * \param format GLenum
 * \return void
 */
STATICINLINE void bind_image_texture(
    u32                      unit,
    u32                      texture,
    i32                      level,
    bool                     layered,
    i32                      layer,
    group::buffer_access_arb access,
    group::internal_format   format,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindImageTexture)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindImageTexture(
        unit,
        texture,
        level,
        layered,
        layer,
        static_cast<GLenum>(access),
        static_cast<GLenum>(format));
    detail::error_check("BindImageTexture"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>)
/*!
 * \brief Wraps around glMemoryBarrier. Introduced in GL core 4.2
 * \param barriers GLbitfield
 * \return void
 */
STATICINLINE void memory_barrier(
    group::memory_barrier_mask barriers,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrier)
    }
    glMemoryBarrier(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrier"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>)
/*!
 * \brief Wraps around glTexStorage1D. Introduced in GL core 4.2
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \return void
 */
STATICINLINE void tex_storage_1d(
    group::texture_target        target,
    i32                          levels,
    group::sized_internal_format internalformat,
    i32                          width,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("TexStorage1D"sv, check_errors);
}

template<class size_2_i32>
requires(
    MinimumVersion<Current, Version<4, 2>> &&
    concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glTexStorage2D. Introduced in GL core 4.2
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void tex_storage_2d(
    group::texture_target        target,
    i32                          levels,
    group::sized_internal_format internalformat,
    size_2_i32 const&            width,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("TexStorage2D"sv, check_errors);
}

template<class size_3_i32>
requires(
    MinimumVersion<Current, Version<4, 2>> &&
    concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Wraps around glTexStorage3D. Introduced in GL core 4.2
 * \param target GLenum
 * \param levels GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \return void
 */
STATICINLINE void tex_storage_3d(
    group::texture_target        target,
    i32                          levels,
    group::sized_internal_format internalformat,
    size_3_i32 const&            width,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("TexStorage3D"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>)
/*!
 * \brief Wraps around glDrawTransformFeedbackInstanced. Introduced in GL
 * core 4.2 \param mode GLenum \param id GLuint \param instancecount GLsizei
 * \return void
 */
STATICINLINE void draw_transform_feedback_instanced(
    group::primitive_type mode,
    u32                   id,
    i32                   instancecount,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackInstanced)
    }
    glDrawTransformFeedbackInstanced(
        static_cast<GLenum>(mode), id, instancecount);
    detail::error_check("DrawTransformFeedbackInstanced"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 2>>)
/*!
 * \brief Wraps around glDrawTransformFeedbackStreamInstanced. Introduced in GL
 * core 4.2 \param mode GLenum \param id GLuint \param stream GLuint \param
 * instancecount GLsizei \return void
 */
STATICINLINE void draw_transform_feedback_stream_instanced(
    group::primitive_type mode,
    u32                   id,
    u32                   stream,
    i32                   instancecount,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackStreamInstanced)
    }
    glDrawTransformFeedbackStreamInstanced(
        static_cast<GLenum>(mode), id, stream, instancecount);
    detail::error_check("DrawTransformFeedbackStreamInstanced"sv, check_errors);
}

#endif // GL_VERSION_4_2
