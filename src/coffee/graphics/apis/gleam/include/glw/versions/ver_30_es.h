#ifdef GL_ES_VERSION_3_0
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glBeginQuery. Introduced in GL es 3.0
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void begin_query(
    group::query_target target,
    u32                 id,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQuery)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBeginQuery, target, id);
#endif
    glBeginQuery(static_cast<GLenum>(target), id);
    detail::error_check("BeginQuery"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glBeginTransformFeedback. Introduced in GL es 3.0
 * \param primitiveMode GLenum
 * \return void
 */
STATICINLINE void begin_transform_feedback(
    group::primitive_type primitiveMode,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBeginTransformFeedback, primitiveMode);
#endif
    glBeginTransformFeedback(static_cast<GLenum>(primitiveMode));
    detail::error_check("BeginTransformFeedback"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glBindBufferBase. Introduced in GL es 3.0
 * \param target GLenum
 * \param index GLuint
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void bind_buffer_base(
    group::buffer_target_arb target,
    u32                      index,
    u32                      buffer,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferBase)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindBufferBase, target, index, buffer);
#endif
    glBindBufferBase(static_cast<GLenum>(target), index, buffer);
    detail::error_check("BindBufferBase"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glBindBufferRange. Introduced in GL es 3.0
 * \param target GLenum
 * \param index GLuint
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \return void
 */
STATICINLINE void bind_buffer_range(
    group::buffer_target_arb target,
    u32                      index,
    u32                      buffer,
    GLintptr                 offset,
    GLsizeiptr               size,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferRange)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindBufferRange, target, index, buffer, offset, size);
#endif
    glBindBufferRange(static_cast<GLenum>(target), index, buffer, offset, size);
    detail::error_check("BindBufferRange"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glBindSampler. Introduced in GL es 3.0
 * \param unit GLuint
 * \param sampler GLuint
 * \return void
 */
STATICINLINE void bind_sampler(
    u32 unit, u32 sampler, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindSampler)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindSampler, unit, sampler);
#endif
    glBindSampler(unit, sampler);
    detail::error_check("BindSampler"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glBindTransformFeedback. Introduced in GL es 3.0
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void bind_transform_feedback(
    group::bind_transform_feedback_target target,
    u32                                   id,
    error_check                           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindTransformFeedback, target, id);
#endif
    glBindTransformFeedback(static_cast<GLenum>(target), id);
    detail::error_check("BindTransformFeedback"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glBindVertexArray. Introduced in GL es 3.0
 * \param array GLuint
 * \return void
 */
STATICINLINE void bind_vertex_array(
    u32 array, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexArray)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(array);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindVertexArray, array);
#endif
    glBindVertexArray(array);
    detail::error_check("BindVertexArray"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glBlitFramebuffer. Introduced in GL es 3.0
 * \param srcX0 GLint
 * \param srcY0 GLint
 * \param srcX1 GLint
 * \param srcY1 GLint
 * \param dstX0 GLint
 * \param dstY0 GLint
 * \param dstX1 GLint
 * \param dstY1 GLint
 * \param mask GLbitfield
 * \param filter GLenum
 * \return void
 */
STATICINLINE void blit_framebuffer(
    i32                            srcX0,
    i32                            srcY0,
    i32                            srcX1,
    i32                            srcY1,
    i32                            dstX0,
    i32                            dstY0,
    i32                            dstX1,
    i32                            dstY1,
    group::clear_buffer_mask       mask,
    group::blit_framebuffer_filter filter,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlitFramebuffer)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glBlitFramebuffer,
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        mask,
        filter);
#endif
    glBlitFramebuffer(
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        static_cast<GLenum>(mask),
        static_cast<GLenum>(filter));
    detail::error_check("BlitFramebuffer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glClearBufferfi. Introduced in GL es 3.0
 * \param buffer GLenum
 * \param drawbuffer GLint
 * \param depth GLfloat
 * \param stencil GLint
 * \return void
 */
STATICINLINE void clear_bufferfi(
    group::buffer buffer,
    i32           drawbuffer,
    f32           depth,
    i32           stencil,
    error_check   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferfi)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glClearBufferfi, buffer, drawbuffer, depth, stencil);
#endif
    glClearBufferfi(static_cast<GLenum>(buffer), drawbuffer, depth, stencil);
    detail::error_check("ClearBufferfi"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glClearBufferfv. Introduced in GL es 3.0
 * \param buffer GLenum
 * \param drawbuffer GLint
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void clear_bufferfv(
    group::buffer         buffer,
    i32                   drawbuffer,
    span_const_f32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferfv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glClearBufferfv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        buffer,
        drawbuffer);
#endif
    glClearBufferfv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLfloat*>(value.data())
                     : nullptr);
    detail::error_check("ClearBufferfv"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glClearBufferiv. Introduced in GL es 3.0
 * \param buffer GLenum
 * \param drawbuffer GLint
 * \param value const GLint *
 * \return void
 */
STATICINLINE void clear_bufferiv(
    group::buffer         buffer,
    i32                   drawbuffer,
    span_const_i32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glClearBufferiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        buffer,
        drawbuffer);
#endif
    glClearBufferiv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLint*>(value.data()) : nullptr);
    detail::error_check("ClearBufferiv"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glClearBufferuiv. Introduced in GL es 3.0
 * \param buffer GLenum
 * \param drawbuffer GLint
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void clear_bufferuiv(
    group::buffer         buffer,
    i32                   drawbuffer,
    span_const_u32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferuiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glClearBufferuiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        buffer,
        drawbuffer);
#endif
    glClearBufferuiv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLuint*>(value.data()) : nullptr);
    detail::error_check("ClearBufferuiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glClientWaitSync. Introduced in GL es 3.0
 * \param sync GLsync
 * \param flags GLbitfield
 * \param timeout GLuint64
 * \return SyncStatus
 */
STATICINLINE group::sync_status client_wait_sync(
    GLsync                  sync,
    group::sync_object_mask flags,
    u64                     timeout,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClientWaitSync)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glClientWaitSync, sync, flags, timeout);
#endif
    auto out = glClientWaitSync(sync, static_cast<GLenum>(flags), timeout);
    detail::error_check("ClientWaitSync"sv, check_errors);
    return static_cast<group::sync_status>(out);
}

template<class size_3_i32, class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::size_2d<size_3_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glCompressedTexImage3D. Introduced in GL es 3.0
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_image_3d(
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    size_3_i32 const&      width,
    i32                    border,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage3D)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCompressedTexImage3D,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        level,
        internalformat,
        width,
        border);
#endif
    glCompressedTexImage3D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage3D"sv, check_errors);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Wraps around glCompressedTexSubImage3D. Introduced in GL es 3.0
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_sub_image_3d(
    group::texture_target  target,
    i32                    level,
    vec_3_i32 const&       xoffset,
    size_3_i32 const&      width,
    group::internal_format format,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage3D)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCompressedTexSubImage3D,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format);
#endif
    glCompressedTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage3D"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glCopyBufferSubData. Introduced in GL es 3.0
 * \param readTarget GLenum
 * \param writeTarget GLenum
 * \param readOffset GLintptr
 * \param writeOffset GLintptr
 * \param size GLsizeiptr
 * \return void
 */
STATICINLINE void copy_buffer_sub_data(
    group::copy_buffer_sub_data_target readTarget,
    group::copy_buffer_sub_data_target writeTarget,
    GLintptr                           readOffset,
    GLintptr                           writeOffset,
    GLsizeiptr                         size,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyBufferSubData)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glCopyBufferSubData,
        readTarget,
        writeTarget,
        readOffset,
        writeOffset,
        size);
#endif
    glCopyBufferSubData(
        static_cast<GLenum>(readTarget),
        static_cast<GLenum>(writeTarget),
        readOffset,
        writeOffset,
        size);
    detail::error_check("CopyBufferSubData"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::vector<vec_3_i32, i32, 3> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glCopyTexSubImage3D. Introduced in GL es 3.0
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void copy_tex_sub_image_3d(
    group::texture_target target,
    i32                   level,
    vec_3_i32 const&      xoffset,
    vec_2_i32 const&      x,
    size_2_i32 const&     width,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage3D)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCopyTexSubImage3D, target, level, xoffset, x, width);
#endif
    glCopyTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage3D"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteQueries. Introduced in GL es 3.0
 * \param n GLsizei
 * \param ids const GLuint *
 * \return void
 */
STATICINLINE void delete_queries(
    span_const_u32 const& ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteQueries)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteQueries,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()));
#endif
    glDeleteQueries(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteQueries"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteSamplers. Introduced in GL es 3.0
 * \param count GLsizei
 * \param samplers const GLuint *
 * \return void
 */
STATICINLINE void delete_samplers(
    span_const_u32 const& samplers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSamplers)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteSamplers,
        gsl::span<const char>(
            reinterpret_cast<const char*>(samplers.data()),
            samplers.size_bytes()));
#endif
    glDeleteSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<const GLuint*>(samplers.data())
                        : nullptr);
    detail::error_check("DeleteSamplers"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glDeleteSync. Introduced in GL es 3.0
 * \param sync GLsync
 * \return void
 */
STATICINLINE void delete_sync(
    GLsync sync, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSync)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDeleteSync, sync);
#endif
    glDeleteSync(sync);
    detail::error_check("DeleteSync"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteTransformFeedbacks. Introduced in GL es 3.0
 * \param n GLsizei
 * \param ids const GLuint *
 * \return void
 */
STATICINLINE void delete_transform_feedbacks(
    span_const_u32 const& ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTransformFeedbacks)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteTransformFeedbacks,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()));
#endif
    glDeleteTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteTransformFeedbacks"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteVertexArrays. Introduced in GL es 3.0
 * \param n GLsizei
 * \param arrays const GLuint *
 * \return void
 */
STATICINLINE void delete_vertex_arrays(
    span_const_u32 const& arrays, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteVertexArrays)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteVertexArrays,
        gsl::span<const char>(
            reinterpret_cast<const char*>(arrays.data()), arrays.size_bytes()));
#endif
    glDeleteVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<const GLuint*>(arrays.data())
                      : nullptr);
    detail::error_check("DeleteVertexArrays"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glDrawArraysInstanced. Introduced in GL es 3.0
 * \param mode GLenum
 * \param first GLint
 * \param count GLsizei
 * \param instancecount GLsizei
 * \return void
 */
STATICINLINE void draw_arrays_instanced(
    group::primitive_type mode,
    i32                   first,
    i32                   count,
    i32                   instancecount,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstanced)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawArraysInstanced, mode, first, count, instancecount);
#endif
    glDrawArraysInstanced(
        static_cast<GLenum>(mode), first, count, instancecount);
    detail::error_check("DrawArraysInstanced"sv, check_errors);
}

template<class span_const_draw_buffer_mode>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_draw_buffer_mode> &&
    std::is_same_v<
        std::decay_t<typename span_const_draw_buffer_mode::value_type>,
        std::decay_t<group::draw_buffer_mode>>)
/*!
 * \brief Wraps around glDrawBuffers. Introduced in GL es 3.0
 * \param n GLsizei
 * \param bufs const GLenum *
 * \return void
 */
STATICINLINE void draw_buffers(
    span_const_draw_buffer_mode const& bufs,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawBuffers)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDrawBuffers,
        gsl::span<const char>(
            reinterpret_cast<const char*>(bufs.data()), bufs.size_bytes()));
#endif
    glDrawBuffers(
        bufs.size(),
        bufs.size() ? reinterpret_cast<const GLenum*>(bufs.data()) : nullptr);
    detail::error_check("DrawBuffers"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glDrawElementsInstanced. Introduced in GL es 3.0
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param instancecount GLsizei
 * \return void
 */
STATICINLINE void draw_elements_instanced(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       instancecount,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstanced)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawElementsInstanced, mode, count, type, indices, instancecount);
#endif
    glDrawElementsInstanced(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount);
    detail::error_check("DrawElementsInstanced"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glDrawRangeElements. Introduced in GL es 3.0
 * \param mode GLenum
 * \param start GLuint
 * \param end GLuint
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \return void
 */
STATICINLINE void draw_range_elements(
    group::primitive_type     mode,
    u32                       start,
    u32                       end,
    i32                       count,
    group::draw_elements_type type,
    span_const_void const&    indices,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawRangeElements)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDrawRangeElements,
        gsl::span<const char>(
            reinterpret_cast<const char*>(indices.data()),
            indices.size_bytes()),
        mode,
        start,
        end,
        count,
        type);
#endif
    glDrawRangeElements(
        static_cast<GLenum>(mode),
        start,
        end,
        count,
        static_cast<GLenum>(type),
        indices.size() ? reinterpret_cast<const void*>(indices.data())
                       : nullptr);
    detail::error_check("DrawRangeElements"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glEndQuery. Introduced in GL es 3.0
 * \param target GLenum
 * \return void
 */
STATICINLINE void end_query(
    group::query_target target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQuery)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEndQuery, target);
#endif
    glEndQuery(static_cast<GLenum>(target));
    detail::error_check("EndQuery"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glEndTransformFeedback. Introduced in GL es 3.0

 * \return void
 */
STATICINLINE void end_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEndTransformFeedback);
#endif
    glEndTransformFeedback();
    detail::error_check("EndTransformFeedback"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glFenceSync. Introduced in GL es 3.0
 * \param condition GLenum
 * \param flags GLbitfield
 * \return GLsync
 */
STATICINLINE GLsync fence_sync(
    group::sync_condition      condition,
    group::sync_behavior_flags flags,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FenceSync)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glFenceSync, condition, flags);
#endif
    auto out =
        glFenceSync(static_cast<GLenum>(condition), static_cast<GLenum>(flags));
    detail::error_check("FenceSync"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glFlushMappedBufferRange. Introduced in GL es 3.0
 * \param target GLenum
 * \param offset GLintptr
 * \param length GLsizeiptr
 * \return void
 */
STATICINLINE void flush_mapped_buffer_range(
    group::buffer_target_arb target,
    GLintptr                 offset,
    GLsizeiptr               length,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushMappedBufferRange)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glFlushMappedBufferRange, target, offset, length);
#endif
    glFlushMappedBufferRange(static_cast<GLenum>(target), offset, length);
    detail::error_check("FlushMappedBufferRange"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glFramebufferTextureLayer. Introduced in GL es 3.0
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \param layer GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_layer(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    i32                           layer,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureLayer)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glFramebufferTextureLayer, target, attachment, texture, level, layer);
#endif
    glFramebufferTextureLayer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        layer);
    detail::error_check("FramebufferTextureLayer"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenQueries. Introduced in GL es 3.0
 * \param n GLsizei
 * \param ids GLuint *
 * \return void
 */
STATICINLINE void gen_queries(
    span_u32 ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueries)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenQueries,
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()));
#endif
    glGenQueries(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenQueries"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenSamplers. Introduced in GL es 3.0
 * \param count GLsizei
 * \param samplers GLuint *
 * \return void
 */
STATICINLINE void gen_samplers(
    span_u32 samplers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenSamplers)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenSamplers,
        gsl::span<char>(
            reinterpret_cast<char*>(samplers.data()), samplers.size_bytes()));
#endif
    glGenSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<GLuint*>(samplers.data()) : nullptr);
    detail::error_check("GenSamplers"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenTransformFeedbacks. Introduced in GL es 3.0
 * \param n GLsizei
 * \param ids GLuint *
 * \return void
 */
STATICINLINE void gen_transform_feedbacks(
    span_u32 ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTransformFeedbacks)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenTransformFeedbacks,
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()));
#endif
    glGenTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenTransformFeedbacks"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenVertexArrays. Introduced in GL es 3.0
 * \param n GLsizei
 * \param arrays GLuint *
 * \return void
 */
STATICINLINE void gen_vertex_arrays(
    span_u32 arrays, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenVertexArrays)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenVertexArrays,
        gsl::span<char>(
            reinterpret_cast<char*>(arrays.data()), arrays.size_bytes()));
#endif
    glGenVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<GLuint*>(arrays.data()) : nullptr);
    detail::error_check("GenVertexArrays"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetActiveUniformBlockName. Introduced in GL es 3.0
 * \param program GLuint
 * \param uniformBlockIndex GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param uniformBlockName GLchar *
 * \return void
 */
STATICINLINE void get_active_uniform_block_name(
    u32         program,
    u32         uniformBlockIndex,
    i32&        length,
    span_GLchar uniformBlockName,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockName)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetActiveUniformBlockName,
        gsl::span<char>(
            reinterpret_cast<char*>(uniformBlockName.data()),
            uniformBlockName.size_bytes()),
        program,
        uniformBlockIndex,
        length);
#endif
    glGetActiveUniformBlockName(
        program,
        uniformBlockIndex,
        uniformBlockName.size(),
        &length,
        uniformBlockName.data());
    detail::error_check("GetActiveUniformBlockName"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetActiveUniformBlockiv. Introduced in GL es 3.0
 * \param program GLuint
 * \param uniformBlockIndex GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_active_uniform_blockiv(
    u32                       program,
    u32                       uniformBlockIndex,
    group::uniform_block_prop pname,
    span_i32                  params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetActiveUniformBlockiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        program,
        uniformBlockIndex,
        pname);
#endif
    glGetActiveUniformBlockiv(
        program,
        uniformBlockIndex,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformBlockiv"sv, check_errors);
}

template<class span_const_u32, class span_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetActiveUniformsiv. Introduced in GL es 3.0
 * \param program GLuint
 * \param uniformCount GLsizei
 * \param uniformIndices const GLuint *
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_active_uniformsiv(
    u32                   program,
    span_const_u32 const& uniformIndices,
    group::uniform_prop   pname,
    span_i32              params,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformsiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetActiveUniformsiv,
        program,
        gsl::span<const char>(
            reinterpret_cast<const char*>(uniformIndices.data()),
            uniformIndices.size_bytes()),
        pname,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()));
#endif
    glGetActiveUniformsiv(
        program,
        uniformIndices.size(),
        uniformIndices.size()
            ? reinterpret_cast<const GLuint*>(uniformIndices.data())
            : nullptr,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformsiv"sv, check_errors);
}

template<class span_i64>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i64> &&
    std::is_same_v<
        std::decay_t<typename span_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Wraps around glGetBufferParameteri64v. Introduced in GL es 3.0
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint64 *
 * \return void
 */
STATICINLINE void get_buffer_parameteri64v(
    group::buffer_target_arb target,
    group::buffer_prop_arb   pname,
    span_i64                 params,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferParameteri64v)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetBufferParameteri64v,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetBufferParameteri64v(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetBufferParameteri64v"sv, check_errors);
}

template<class span_void>
requires(MinimumVersion<Current, Version<3, 0>> && concepts::span<span_void>)
/*!
 * \brief Wraps around glGetBufferPointerv. Introduced in GL es 3.0
 * \param target GLenum
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_buffer_pointerv(
    group::buffer_target_arb       target,
    group::buffer_pointer_name_arb pname,
    span_void                      params,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferPointerv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetBufferPointerv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetBufferPointerv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetBufferPointerv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glGetFragDataLocation. Introduced in GL es 3.0
 * \param program GLuint
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_frag_data_location(
    u32                     program,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataLocation)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetFragDataLocation, program, name);
#endif
    auto out = glGetFragDataLocation(program, name.data());
    detail::error_check("GetFragDataLocation"sv, check_errors);
    return out;
}

template<class span_i64>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i64> &&
    std::is_same_v<
        std::decay_t<typename span_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Wraps around glGetInteger64i_v. Introduced in GL es 3.0
 * \param target GLenum
 * \param index GLuint
 * \param data GLint64 *
 * \return void
 */
STATICINLINE void get_integer64i_v(
    group::get_prop target,
    u32             index,
    span_i64        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64i_v)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetInteger64i_v,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        target,
        index);
#endif
    glGetInteger64i_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64i_v"sv, check_errors);
}

template<class span_i64>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i64> &&
    std::is_same_v<
        std::decay_t<typename span_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Wraps around glGetInteger64v. Introduced in GL es 3.0
 * \param pname GLenum
 * \param data GLint64 *
 * \return void
 */
STATICINLINE void get_integer64v(
    group::get_prop pname,
    span_i64        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64v)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetInteger64v,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        pname);
#endif
    glGetInteger64v(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64v"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetIntegeri_v. Introduced in GL es 3.0
 * \param target GLenum
 * \param index GLuint
 * \param data GLint *
 * \return void
 */
STATICINLINE void get_integeri_v(
    group::get_prop target,
    u32             index,
    span_i32        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegeri_v)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetIntegeri_v,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        target,
        index);
#endif
    glGetIntegeri_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegeri_v"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetInternalformativ. Introduced in GL es 3.0
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
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetInternalformativ,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        internalformat,
        pname);
#endif
    glGetInternalformativ(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(pname),
        params.size(),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetInternalformativ"sv, check_errors);
}

template<class span_void>
requires(MinimumVersion<Current, Version<3, 0>> && concepts::span<span_void>)
/*!
 * \brief Wraps around glGetProgramBinary. Introduced in GL es 3.0
 * \param program GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param binaryFormat GLenum *
 * \param binary void *
 * \return void
 */
STATICINLINE void get_program_binary(
    u32         program,
    i32&        length,
    GLenum&     binaryFormat,
    span_void   binary,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramBinary)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetProgramBinary,
        gsl::span<char>(
            reinterpret_cast<char*>(binary.data()), binary.size_bytes()),
        program,
        length,
        binaryFormat);
#endif
    glGetProgramBinary(
        program,
        binary.size() * sizeof(typename std::decay_t<span_void>::value_type),
        &length,
        &binaryFormat,
        binary.size() ? reinterpret_cast<void*>(binary.data()) : nullptr);
    detail::error_check("GetProgramBinary"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetQueryObjectuiv. Introduced in GL es 3.0
 * \param id GLuint
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_query_objectuiv(
    u32                                id,
    group::query_object_parameter_name pname,
    span_u32                           params,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectuiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetQueryObjectuiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        id,
        pname);
#endif
    glGetQueryObjectuiv(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectuiv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetQueryiv. Introduced in GL es 3.0
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_queryiv(
    group::query_target         target,
    group::query_parameter_name pname,
    span_i32                    params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetQueryiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetQueryiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryiv"sv, check_errors);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetSamplerParameterfv. Introduced in GL es 3.0
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_sampler_parameter(
    u32                        sampler,
    group::sampler_parameter_f pname,
    span_f32                   params,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterfv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetSamplerParameterfv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        sampler,
        pname);
#endif
    glGetSamplerParameterfv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterfv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetSamplerParameteriv. Introduced in GL es 3.0
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_sampler_parameter(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_i32                   params,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameteriv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetSamplerParameteriv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        sampler,
        pname);
#endif
    glGetSamplerParameteriv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameteriv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glGetStringi. Introduced in GL es 3.0
 * \param name GLenum
 * \param index GLuint
 * \return String
 */
STATICINLINE std::string get_stringi(
    group::string_name name,
    u32                index,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetStringi)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetStringi, name, index);
#endif
    auto out = glGetStringi(static_cast<GLenum>(name), index);
    detail::error_check("GetStringi"sv, check_errors);
    return reinterpret_cast<const char*>(out);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetSynciv. Introduced in GL es 3.0
 * \param sync GLsync
 * \param pname GLenum
 * \param count GLsizei
 * \param length GLsizei *
 * \param values GLint *
 * \return void
 */
STATICINLINE void get_synciv(
    GLsync                     sync,
    group::sync_parameter_name pname,
    i32&                       length,
    span_i32                   values,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSynciv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetSynciv,
        gsl::span<char>(
            reinterpret_cast<char*>(values.data()), values.size_bytes()),
        sync,
        pname,
        length);
#endif
    glGetSynciv(
        sync,
        static_cast<GLenum>(pname),
        values.size(),
        &length,
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("GetSynciv"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetTransformFeedbackVarying. Introduced in GL es 3.0
 * \param program GLuint
 * \param index GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param size GLsizei *
 * \param type GLenum *
 * \param name GLchar *
 * \return void
 */
STATICINLINE void get_transform_feedback_varying(
    u32         program,
    u32         index,
    i32&        length,
    i32&        size,
    GLenum&     type,
    span_GLchar name,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbackVarying)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetTransformFeedbackVarying,
        gsl::span<char>(
            reinterpret_cast<char*>(name.data()), name.size_bytes()),
        program,
        index,
        length,
        size,
        type);
#endif
    glGetTransformFeedbackVarying(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetTransformFeedbackVarying"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glGetUniformBlockIndex. Introduced in GL es 3.0
 * \param program GLuint
 * \param uniformBlockName const GLchar *
 * \return GLuint
 */
STATICINLINE GLuint get_uniform_block_index(
    u32                     program,
    std::string_view const& uniformBlockName,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformBlockIndex)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetUniformBlockIndex, program, uniformBlockName);
#endif
    auto out = glGetUniformBlockIndex(program, uniformBlockName.data());
    detail::error_check("GetUniformBlockIndex"sv, check_errors);
    return out;
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetUniformIndices. Introduced in GL es 3.0
 * \param program GLuint
 * \param uniformCount GLsizei
 * \param uniformNames const GLchar *const*
 * \param uniformIndices GLuint *
 * \return void
 */
STATICINLINE void get_uniform_indices(
    u32                           program,
    i32                           uniformCount,
    std::vector<std::string_view> uniformNames,
    span_u32                      uniformIndices,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformIndices)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto [uniformNames_lens, uniformNames_cstr, uniformNames_store] =
        detail::transform_strings(uniformNames);
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUniformIndices,
        gsl::span<char>(
            reinterpret_cast<char*>(uniformIndices.data()),
            uniformIndices.size_bytes()),
        program,
        uniformCount,
        uniformNames);
#endif
    glGetUniformIndices(
        program,
        uniformCount,
        uniformNames_cstr.data(),
        uniformIndices.size() ? reinterpret_cast<GLuint*>(uniformIndices.data())
                              : nullptr);
    detail::error_check("GetUniformIndices"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetUniformuiv. Introduced in GL es 3.0
 * \param program GLuint
 * \param location GLint
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_uniformuiv(
    u32         program,
    i32         location,
    span_u32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformuiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUniformuiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        program,
        location);
#endif
    glGetUniformuiv(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetUniformuiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glGetVertexAttribIiv. Introduced in GL es 3.0
 * \param index GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_vertex_attrib_iiv(
    u32                       index,
    group::vertex_attrib_enum pname,
    i32&                      params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIiv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetVertexAttribIiv, index, pname, params);
#endif
    glGetVertexAttribIiv(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glGetVertexAttribIuiv. Introduced in GL es 3.0
 * \param index GLuint
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_vertex_attrib_iuiv(
    u32                       index,
    group::vertex_attrib_enum pname,
    u32&                      params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIuiv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetVertexAttribIuiv, index, pname, params);
#endif
    glGetVertexAttribIuiv(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIuiv"sv, check_errors);
}

template<class span_const_invalidate_framebuffer_attachment>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_invalidate_framebuffer_attachment> &&
    std::is_same_v<
        std::decay_t<
            typename span_const_invalidate_framebuffer_attachment::value_type>,
        std::decay_t<group::invalidate_framebuffer_attachment>>)
/*!
 * \brief Wraps around glInvalidateFramebuffer. Introduced in GL es 3.0
 * \param target GLenum
 * \param numAttachments GLsizei
 * \param attachments const GLenum *
 * \return void
 */
STATICINLINE void invalidate_framebuffer(
    group::framebuffer_target                           target,
    span_const_invalidate_framebuffer_attachment const& attachments,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateFramebuffer)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glInvalidateFramebuffer,
        gsl::span<const char>(
            reinterpret_cast<const char*>(attachments.data()),
            attachments.size_bytes()),
        target);
#endif
    glInvalidateFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr);
    detail::error_check("InvalidateFramebuffer"sv, check_errors);
}

template<
    class size_2_i32,
    class span_const_invalidate_framebuffer_attachment,
    class vec_2_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_invalidate_framebuffer_attachment> &&
    std::is_same_v<
        std::decay_t<
            typename span_const_invalidate_framebuffer_attachment::value_type>,
        std::decay_t<group::invalidate_framebuffer_attachment>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glInvalidateSubFramebuffer. Introduced in GL es 3.0
 * \param target GLenum
 * \param numAttachments GLsizei
 * \param attachments const GLenum *
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void invalidate_sub_framebuffer(
    group::framebuffer_target                           target,
    span_const_invalidate_framebuffer_attachment const& attachments,
    vec_2_i32 const&                                    x,
    size_2_i32 const&                                   width,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateSubFramebuffer)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glInvalidateSubFramebuffer,
        gsl::span<const char>(
            reinterpret_cast<const char*>(attachments.data()),
            attachments.size_bytes()),
        target,
        x,
        width);
#endif
    glInvalidateSubFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr,
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("InvalidateSubFramebuffer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glIsQuery. Introduced in GL es 3.0
 * \param id GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_query(u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsQuery)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsQuery, id);
#endif
    auto out = glIsQuery(id);
    detail::error_check("IsQuery"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glIsSampler. Introduced in GL es 3.0
 * \param sampler GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_sampler(u32 sampler, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSampler)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsSampler, sampler);
#endif
    auto out = glIsSampler(sampler);
    detail::error_check("IsSampler"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glIsSync. Introduced in GL es 3.0
 * \param sync GLsync
 * \return GLboolean
 */
STATICINLINE GLboolean
is_sync(GLsync sync, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSync)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsSync, sync);
#endif
    auto out = glIsSync(sync);
    detail::error_check("IsSync"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glIsTransformFeedback. Introduced in GL es 3.0
 * \param id GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_transform_feedback(u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsTransformFeedback, id);
#endif
    auto out = glIsTransformFeedback(id);
    detail::error_check("IsTransformFeedback"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glIsVertexArray. Introduced in GL es 3.0
 * \param array GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_vertex_array(u32 array, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsVertexArray)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(array);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsVertexArray, array);
#endif
    auto out = glIsVertexArray(array);
    detail::error_check("IsVertexArray"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glMapBufferRange. Introduced in GL es 3.0
 * \param target GLenum
 * \param offset GLintptr
 * \param length GLsizeiptr
 * \param access GLbitfield
 * \return void *
 */
STATICINLINE void* map_buffer_range(
    group::buffer_target_arb      target,
    GLintptr                      offset,
    GLsizeiptr                    length,
    group::map_buffer_access_mask access,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapBufferRange)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMapBufferRange, target, offset, length, access);
#endif
    auto out = glMapBufferRange(
        static_cast<GLenum>(target),
        offset,
        length,
        static_cast<GLenum>(access));
    detail::error_check("MapBufferRange"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glPauseTransformFeedback. Introduced in GL es 3.0

 * \return void
 */
STATICINLINE void pause_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PauseTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPauseTransformFeedback);
#endif
    glPauseTransformFeedback();
    detail::error_check("PauseTransformFeedback"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glProgramBinary. Introduced in GL es 3.0
 * \param program GLuint
 * \param binaryFormat GLenum
 * \param binary const void *
 * \param length GLsizei
 * \return void
 */
STATICINLINE void program_binary(
    u32                    program,
    GLenum                 binaryFormat,
    span_const_void const& binary,
    i32                    length,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBinary)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramBinary,
        gsl::span<const char>(
            reinterpret_cast<const char*>(binary.data()), binary.size_bytes()),
        program,
        binaryFormat,
        length);
#endif
    glProgramBinary(
        program,
        binaryFormat,
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ProgramBinary"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glProgramParameteri. Introduced in GL es 3.0
 * \param program GLuint
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void program_parameter(
    u32                           program,
    group::program_parameter_prop pname,
    i32                           value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameteri)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramParameteri, program, pname, value);
#endif
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteri"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glReadBuffer. Introduced in GL es 3.0
 * \param src GLenum
 * \return void
 */
STATICINLINE void read_buffer(
    group::read_buffer_mode src, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadBuffer)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glReadBuffer, src);
#endif
    glReadBuffer(static_cast<GLenum>(src));
    detail::error_check("ReadBuffer"sv, check_errors);
}

template<class size_2_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glRenderbufferStorageMultisample. Introduced in GL es 3.0
 * \param target GLenum
 * \param samples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void renderbuffer_storage_multisample(
    group::renderbuffer_target target,
    i32                        samples,
    group::internal_format     internalformat,
    size_2_i32 const&          width,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisample)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glRenderbufferStorageMultisample,
        target,
        samples,
        internalformat,
        width);
#endif
    glRenderbufferStorageMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisample"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glResumeTransformFeedback. Introduced in GL es 3.0

 * \return void
 */
STATICINLINE void resume_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResumeTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glResumeTransformFeedback);
#endif
    glResumeTransformFeedback();
    detail::error_check("ResumeTransformFeedback"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glSamplerParameterf. Introduced in GL es 3.0
 * \param sampler GLuint
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32                        sampler,
    group::sampler_parameter_f pname,
    f32                        param,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterf)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glSamplerParameterf, sampler, pname, param);
#endif
    glSamplerParameterf(sampler, static_cast<GLenum>(pname), param);
    detail::error_check("SamplerParameterf"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glSamplerParameterfv. Introduced in GL es 3.0
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLfloat *
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32                        sampler,
    group::sampler_parameter_f pname,
    span_const_f32 const&      param,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterfv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glSamplerParameterfv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        sampler,
        pname);
#endif
    glSamplerParameterfv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfloat*>(param.data())
                     : nullptr);
    detail::error_check("SamplerParameterfv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glSamplerParameteri. Introduced in GL es 3.0
 * \param sampler GLuint
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32                        sampler,
    group::sampler_parameter_i pname,
    i32                        param,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameteri)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glSamplerParameteri, sampler, pname, param);
#endif
    glSamplerParameteri(sampler, static_cast<GLenum>(pname), param);
    detail::error_check("SamplerParameteri"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glSamplerParameteriv. Introduced in GL es 3.0
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLint *
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_const_i32 const&      param,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameteriv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glSamplerParameteriv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        sampler,
        pname);
#endif
    glSamplerParameteriv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameteriv"sv, check_errors);
}

template<class size_3_i32, class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::size_2d<size_3_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glTexImage3D. Introduced in GL es 3.0
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_image_3d(
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    size_3_i32 const&      width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage3D)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexImage3D,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pixels.data()), pixels.size_bytes()),
        target,
        level,
        internalformat,
        width,
        border,
        format,
        type);
#endif
    glTexImage3D(
        static_cast<GLenum>(target),
        level,
        internalformat,
        width[0],
        width[1],
        width[2],
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexImage3D"sv, check_errors);
}

template<class size_2_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glTexStorage2D. Introduced in GL es 3.0
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
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexStorage2D, target, levels, internalformat, width);
#endif
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
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Wraps around glTexStorage3D. Introduced in GL es 3.0
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
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexStorage3D, target, levels, internalformat, width);
#endif
    glTexStorage3D(
        static_cast<GLenum>(target),
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TexStorage3D"sv, check_errors);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Wraps around glTexSubImage3D. Introduced in GL es 3.0
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
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_sub_image_3d(
    group::texture_target  target,
    i32                    level,
    vec_3_i32 const&       xoffset,
    size_3_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage3D)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexSubImage3D,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pixels.data()), pixels.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format,
        type);
#endif
    glTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage3D"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glTransformFeedbackVaryings. Introduced in GL es 3.0
 * \param program GLuint
 * \param count GLsizei
 * \param varyings const GLchar *const*
 * \param bufferMode GLenum
 * \return void
 */
STATICINLINE void transform_feedback_varyings(
    u32                                   program,
    std::vector<std::string_view>         varyings,
    group::transform_feedback_buffer_mode bufferMode,
    error_check                           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackVaryings)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto [varyings_lens, varyings_cstr, varyings_store] =
        detail::transform_strings(varyings);
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTransformFeedbackVaryings, program, varyings, bufferMode);
#endif
    glTransformFeedbackVaryings(
        program,
        varyings_cstr.size(),
        varyings_cstr.data(),
        static_cast<GLenum>(bufferMode));
    detail::error_check("TransformFeedbackVaryings"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glUniform1ui. Introduced in GL es 3.0
 * \param location GLint
 * \param v0 GLuint
 * \return void
 */
STATICINLINE void uniform(
    i32 location, u32 v0, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ui)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform1ui, location, v0);
#endif
    glUniform1ui(location, v0);
    detail::error_check("Uniform1ui"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glUniform1uiv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void uniform(
    i32                   location,
    span_const_u32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1uiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform1uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform1uiv(
        location, value.size(), reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform1uiv"sv, check_errors);
}

template<class vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::vector<vec_2_u32, u32, 2>)
/*!
 * \brief Wraps around glUniform2ui. Introduced in GL es 3.0
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_2_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform2ui, location, v0);
#endif
    glUniform2ui(location, v0[0], v0[1]);
    detail::error_check("Uniform2ui"sv, check_errors);
}

template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_vec_2_u32> &&
    concepts::vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Wraps around glUniform2uiv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_2_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2uiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform2uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform2uiv(
        location, value.size(), reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform2uiv"sv, check_errors);
}

template<class vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::vector<vec_3_u32, u32, 3>)
/*!
 * \brief Wraps around glUniform3ui. Introduced in GL es 3.0
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \param v2 GLuint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_3_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform3ui, location, v0);
#endif
    glUniform3ui(location, v0[0], v0[1], v0[2]);
    detail::error_check("Uniform3ui"sv, check_errors);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_vec_3_u32> &&
    concepts::vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glUniform3uiv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_3_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3uiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform3uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform3uiv(
        location, value.size(), reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform3uiv"sv, check_errors);
}

template<class vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::vector<vec_4_u32, u32, 4>)
/*!
 * \brief Wraps around glUniform4ui. Introduced in GL es 3.0
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \param v2 GLuint
 * \param v3 GLuint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_4_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform4ui, location, v0);
#endif
    glUniform4ui(location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("Uniform4ui"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Wraps around glUniform4uiv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_4_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4uiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform4uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform4uiv(
        location, value.size(), reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform4uiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glUniformBlockBinding. Introduced in GL es 3.0
 * \param program GLuint
 * \param uniformBlockIndex GLuint
 * \param uniformBlockBinding GLuint
 * \return void
 */
STATICINLINE void uniform_block_binding(
    u32         program,
    u32         uniformBlockIndex,
    u32         uniformBlockBinding,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformBlockBinding)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glUniformBlockBinding, program, uniformBlockIndex, uniformBlockBinding);
#endif
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    detail::error_check("UniformBlockBinding"sv, check_errors);
}

template<class span_const_mat_2x3_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_mat_2x3_f32> &&
    concepts::matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
/*!
 * \brief Wraps around glUniformMatrix2x3fv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3fv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2x3fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2x3fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x3fv"sv, check_errors);
}

template<class span_const_mat_2x4_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_mat_2x4_f32> &&
    concepts::matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
/*!
 * \brief Wraps around glUniformMatrix2x4fv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4fv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2x4fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2x4fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x4fv"sv, check_errors);
}

template<class span_const_mat_3x2_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_mat_3x2_f32> &&
    concepts::matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
/*!
 * \brief Wraps around glUniformMatrix3x2fv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2fv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3x2fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3x2fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x2fv"sv, check_errors);
}

template<class span_const_mat_3x4_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_mat_3x4_f32> &&
    concepts::matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
/*!
 * \brief Wraps around glUniformMatrix3x4fv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4fv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3x4fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3x4fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x4fv"sv, check_errors);
}

template<class span_const_mat_4x2_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_mat_4x2_f32> &&
    concepts::matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
/*!
 * \brief Wraps around glUniformMatrix4x2fv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2fv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4x2fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4x2fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x2fv"sv, check_errors);
}

template<class span_const_mat_4x3_f32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_mat_4x3_f32> &&
    concepts::matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
/*!
 * \brief Wraps around glUniformMatrix4x3fv. Introduced in GL es 3.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3fv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4x3fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4x3fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x3fv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glUnmapBuffer. Introduced in GL es 3.0
 * \param target GLenum
 * \return GLboolean
 */
STATICINLINE GLboolean unmap_buffer(
    group::buffer_target_arb target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapBuffer)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUnmapBuffer, target);
#endif
    auto out = glUnmapBuffer(static_cast<GLenum>(target));
    detail::error_check("UnmapBuffer"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glVertexAttribDivisor. Introduced in GL es 3.0
 * \param index GLuint
 * \param divisor GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_divisor(
    u32 index, u32 divisor, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribDivisor)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribDivisor, index, divisor);
#endif
    glVertexAttribDivisor(index, divisor);
    detail::error_check("VertexAttribDivisor"sv, check_errors);
}

template<class vec_4_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::vector<vec_4_i32, i32, 4>)
/*!
 * \brief Wraps around glVertexAttribI4i. Introduced in GL es 3.0
 * \param index GLuint
 * \param x GLint
 * \param y GLint
 * \param z GLint
 * \param w GLint
 * \return void
 */
STATICINLINE void vertex_attrib_i4i(
    u32 index, vec_4_i32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4i)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribI4i, index, x);
#endif
    glVertexAttribI4i(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttribI4i"sv, check_errors);
}

template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Wraps around glVertexAttribI4iv. Introduced in GL es 3.0
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void vertex_attrib_i4iv(
    u32                         index,
    span_const_vec_4_i32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4iv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribI4iv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribI4iv(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI4iv"sv, check_errors);
}

template<class vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::vector<vec_4_u32, u32, 4>)
/*!
 * \brief Wraps around glVertexAttribI4ui. Introduced in GL es 3.0
 * \param index GLuint
 * \param x GLuint
 * \param y GLuint
 * \param z GLuint
 * \param w GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_i4ui(
    u32 index, vec_4_u32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4ui)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribI4ui, index, x);
#endif
    glVertexAttribI4ui(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttribI4ui"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 0>> &&
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Wraps around glVertexAttribI4uiv. Introduced in GL es 3.0
 * \param index GLuint
 * \param v const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_i4uiv(
    u32                         index,
    span_const_vec_4_u32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4uiv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribI4uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribI4uiv(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI4uiv"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 0>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glVertexAttribIPointer. Introduced in GL es 3.0
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void vertex_attrib_i_pointer(
    u32                      index,
    i32                      size,
    group::vertex_attrib_int type,
    i32                      stride,
    span_const_void const&   pointer,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribIPointer)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribIPointer,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pointer.data()),
            pointer.size_bytes()),
        index,
        size,
        type,
        stride);
#endif
    glVertexAttribIPointer(
        index,
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribIPointer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>)
/*!
 * \brief Wraps around glWaitSync. Introduced in GL es 3.0
 * \param sync GLsync
 * \param flags GLbitfield
 * \param timeout GLuint64
 * \return void
 */
STATICINLINE void wait_sync(
    GLsync                     sync,
    group::sync_behavior_flags flags,
    u64                        timeout,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WaitSync)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glWaitSync, sync, flags, timeout);
#endif
    glWaitSync(sync, static_cast<GLenum>(flags), timeout);
    detail::error_check("WaitSync"sv, check_errors);
}

#endif // GL_ES_VERSION_3_0
