#ifdef GL_ES_VERSION_3_0
/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void begin_query(group::query_target target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQuery)
    }
    glBeginQuery(static_cast<GLenum>(target), id);
    detail::error_check("BeginQuery"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void begin_transform_feedback(group::primitive_type primitiveMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginTransformFeedback)
    }
    glBeginTransformFeedback(static_cast<GLenum>(primitiveMode));
    detail::error_check("BeginTransformFeedback"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_buffer_base(
        group::buffer_target_arb target, u32 index, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferBase)
        glIsBuffer(buffer);
    }
    glBindBufferBase(static_cast<GLenum>(target), index, buffer);
    detail::error_check("BindBufferBase"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_buffer_range(
        group::buffer_target_arb target,
        u32                      index,
        u32                      buffer,
        GLintptr                 offset,
        GLsizeiptr               size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferRange)
        glIsBuffer(buffer);
    }
    glBindBufferRange(static_cast<GLenum>(target), index, buffer, offset, size);
    detail::error_check("BindBufferRange"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_sampler(u32 unit, u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindSampler)
        glIsSampler(sampler);
    }
    glBindSampler(unit, sampler);
    detail::error_check("BindSampler"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_transform_feedback(
        group::bind_transform_feedback_target target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTransformFeedback)
    }
    glBindTransformFeedback(static_cast<GLenum>(target), id);
    detail::error_check("BindTransformFeedback"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_vertex_array(u32 array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexArray)
        glIsVertexArray(array);
    }
    glBindVertexArray(array);
    detail::error_check("BindVertexArray"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void blit_framebuffer(
        i32                            srcX0,
        i32                            srcY0,
        i32                            srcX1,
        i32                            srcY1,
        i32                            dstX0,
        i32                            dstY0,
        i32                            dstX1,
        i32                            dstY1,
        group::clear_buffer_mask       mask,
        group::blit_framebuffer_filter filter)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlitFramebuffer)
    }
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
    detail::error_check("BlitFramebuffer"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void clear_bufferfi(
        group::buffer buffer, i32 drawbuffer, f32 depth, i32 stencil)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferfi)
    }
    glClearBufferfi(static_cast<GLenum>(buffer), drawbuffer, depth, stencil);
    detail::error_check("ClearBufferfi"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void clear_bufferfv(
        group::buffer buffer, i32 drawbuffer, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferfv)
    }
    glClearBufferfv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLfloat*>(value.data())
                     : nullptr);
    detail::error_check("ClearBufferfv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void clear_bufferiv(
        group::buffer buffer, i32 drawbuffer, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferiv)
    }
    glClearBufferiv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLint*>(value.data()) : nullptr);
    detail::error_check("ClearBufferiv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void clear_bufferuiv(
        group::buffer buffer, i32 drawbuffer, span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferuiv)
    }
    glClearBufferuiv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLuint*>(value.data()) : nullptr);
    detail::error_check("ClearBufferuiv"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLenum
    client_wait_sync(GLsync sync, group::sync_object_mask flags, u64 timeout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClientWaitSync)
    }
    auto out = glClientWaitSync(sync, static_cast<GLenum>(flags), timeout);
    detail::error_check("ClientWaitSync"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<class size_3_i32, class span_const_void>
requires(MinimumVersion<Current, Version<3, 0>>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_image_3d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_3_i32 const&      width,
        i32                    border,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage3D)
    }
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
    detail::error_check("CompressedTexImage3D"sv);
}

/* Introduced in GL es 3.0 */
template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_sub_image_3d(
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage3D)
    }
    glCompressedTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage3D"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void copy_buffer_sub_data(
        group::copy_buffer_sub_data_target readTarget,
        group::copy_buffer_sub_data_target writeTarget,
        GLintptr                           readOffset,
        GLintptr                           writeOffset,
        GLsizeiptr                         size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyBufferSubData)
    }
    glCopyBufferSubData(
        static_cast<GLenum>(readTarget),
        static_cast<GLenum>(writeTarget),
        readOffset,
        writeOffset,
        size);
    detail::error_check("CopyBufferSubData"sv);
}

/* Introduced in GL es 3.0 */
template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_tex_sub_image_3d(
        group::texture_target target,
        i32                   level,
        vec_3_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage3D)
    }
    glCopyTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage3D"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_queries(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteQueries)
    }
    glDeleteQueries(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteQueries"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_samplers(span_const_u32 const& samplers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSamplers)
    }
    glDeleteSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<const GLuint*>(samplers.data())
                        : nullptr);
    detail::error_check("DeleteSamplers"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void delete_sync(GLsync sync)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSync)
    }
    glDeleteSync(sync);
    detail::error_check("DeleteSync"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_transform_feedbacks(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTransformFeedbacks)
    }
    glDeleteTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteTransformFeedbacks"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_vertex_arrays(span_const_u32 const& arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteVertexArrays)
    }
    glDeleteVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<const GLuint*>(arrays.data())
                      : nullptr);
    detail::error_check("DeleteVertexArrays"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void draw_arrays_instanced(
        group::primitive_type mode, i32 first, i32 count, i32 instancecount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysInstanced)
    }
    glDrawArraysInstanced(
        static_cast<GLenum>(mode), first, count, instancecount);
    detail::error_check("DrawArraysInstanced"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_draw_buffer_mode>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_draw_buffer_mode>&& std::is_same_v<
            std::decay_t<typename span_const_draw_buffer_mode::value_type>,
            std::decay_t<group::draw_buffer_mode>>) STATICINLINE
    void draw_buffers(span_const_draw_buffer_mode const& bufs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawBuffers)
    }
    glDrawBuffers(
        bufs.size(),
        bufs.size() ? reinterpret_cast<const GLenum*>(bufs.data()) : nullptr);
    detail::error_check("DrawBuffers"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void draw_elements_instanced(
        group::primitive_type     mode,
        i32                       count,
        group::draw_elements_type type,
        ptroff                    indices,
        i32                       instancecount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstanced)
    }
    glDrawElementsInstanced(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount);
    detail::error_check("DrawElementsInstanced"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void draw_range_elements(
        group::primitive_type     mode,
        u32                       start,
        u32                       end,
        i32                       count,
        group::draw_elements_type type,
        span_const_void const&    indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawRangeElements)
    }
    glDrawRangeElements(
        static_cast<GLenum>(mode),
        start,
        end,
        count,
        static_cast<GLenum>(type),
        indices.size() ? reinterpret_cast<const void*>(indices.data())
                       : nullptr);
    detail::error_check("DrawRangeElements"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void end_query(group::query_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQuery)
    }
    glEndQuery(static_cast<GLenum>(target));
    detail::error_check("EndQuery"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void end_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndTransformFeedback)
    }
    glEndTransformFeedback();
    detail::error_check("EndTransformFeedback"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLsync fence_sync(
    group::sync_condition condition, group::sync_behavior_flags flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FenceSync)
    }
    auto out =
        glFenceSync(static_cast<GLenum>(condition), static_cast<GLenum>(flags));
    detail::error_check("FenceSync"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void flush_mapped_buffer_range(
        group::buffer_target_arb target, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushMappedBufferRange)
    }
    glFlushMappedBufferRange(static_cast<GLenum>(target), offset, length);
    detail::error_check("FlushMappedBufferRange"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void framebuffer_texture_layer(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        u32                           texture,
        i32                           level,
        i32                           layer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureLayer)
        glIsTexture(texture);
    }
    glFramebufferTextureLayer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        layer);
    detail::error_check("FramebufferTextureLayer"sv);
}

/* Introduced in GL es 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE void gen_queries(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueries)
    }
    glGenQueries(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenQueries"sv);
}

/* Introduced in GL es 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_samplers(span_u32 samplers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenSamplers)
    }
    glGenSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<GLuint*>(samplers.data()) : nullptr);
    detail::error_check("GenSamplers"sv);
}

/* Introduced in GL es 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_transform_feedbacks(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTransformFeedbacks)
    }
    glGenTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenTransformFeedbacks"sv);
}

/* Introduced in GL es 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_vertex_arrays(span_u32 arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenVertexArrays)
    }
    glGenVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<GLuint*>(arrays.data()) : nullptr);
    detail::error_check("GenVertexArrays"sv);
}

/* Introduced in GL es 3.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_uniform_block_name(
        u32         program,
        u32         uniformBlockIndex,
        i32&        length,
        span_GLchar uniformBlockName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockName)
        glIsProgram(program);
    }
    glGetActiveUniformBlockName(
        program,
        uniformBlockIndex,
        uniformBlockName.size(),
        &length,
        uniformBlockName.data());
    detail::error_check("GetActiveUniformBlockName"sv);
}

/* Introduced in GL es 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_active_uniform_blockiv(
        u32                       program,
        u32                       uniformBlockIndex,
        group::uniform_block_prop pname,
        span_i32                  params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformBlockiv)
        glIsProgram(program);
    }
    glGetActiveUniformBlockiv(
        program,
        uniformBlockIndex,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformBlockiv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_u32, class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>&& semantic::concepts::Span<span_i32>&&
                                      std::is_same_v<
                     std::decay_t<typename span_i32::value_type>,
                     std::decay_t<i32>>) STATICINLINE
    void get_active_uniformsiv(
        u32                   program,
        span_const_u32 const& uniformIndices,
        group::uniform_prop   pname,
        span_i32              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformsiv)
        glIsProgram(program);
    }
    glGetActiveUniformsiv(
        program,
        uniformIndices.size(),
        uniformIndices.size()
            ? reinterpret_cast<const GLuint*>(uniformIndices.data())
            : nullptr,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetActiveUniformsiv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_i64>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>) STATICINLINE
    void get_buffer_parameteri64v(
        group::buffer_target_arb target,
        group::buffer_prop_arb   pname,
        span_i64                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferParameteri64v)
    }
    glGetBufferParameteri64v(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetBufferParameteri64v"sv);
}

/* Introduced in GL es 3.0 */
template<class span_void>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_buffer_pointerv(
        group::buffer_target_arb       target,
        group::buffer_pointer_name_arb pname,
        span_void                      params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferPointerv)
    }
    glGetBufferPointerv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetBufferPointerv"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLint
    get_frag_data_location(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataLocation)
        glIsProgram(program);
    }
    auto out = glGetFragDataLocation(program, name.data());
    detail::error_check("GetFragDataLocation"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<class span_i64>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>) STATICINLINE
    void get_integer64i_v(group::get_prop target, u32 index, span_i64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64i_v)
    }
    glGetInteger64i_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64i_v"sv);
}

/* Introduced in GL es 3.0 */
template<class span_i64>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>) STATICINLINE
    void get_integer64v(group::get_prop pname, span_i64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64v)
    }
    glGetInteger64v(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64v"sv);
}

/* Introduced in GL es 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_integeri_v(group::get_prop target, u32 index, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegeri_v)
    }
    glGetIntegeri_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegeri_v"sv);
}

/* Introduced in GL es 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
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

/* Introduced in GL es 3.0 */
template<class span_void>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_program_binary(
        u32 program, i32& length, GLenum& binaryFormat, span_void binary)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramBinary)
        glIsProgram(program);
    }
    glGetProgramBinary(
        program,
        binary.size() * sizeof(typename std::decay_t<span_void>::value_type),
        &length,
        &binaryFormat,
        binary.size() ? reinterpret_cast<void*>(binary.data()) : nullptr);
    detail::error_check("GetProgramBinary"sv);
}

/* Introduced in GL es 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_query_objectuiv(
        u32 id, group::query_object_parameter_name pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectuiv)
    }
    glGetQueryObjectuiv(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectuiv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_queryiv(
        group::query_target         target,
        group::query_parameter_name pname,
        span_i32                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryiv)
    }
    glGetQueryiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryiv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_sampler_parameter(
        u32 sampler, group::sampler_parameter_f pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterfv)
        glIsSampler(sampler);
    }
    glGetSamplerParameterfv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterfv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_sampler_parameter(
        u32 sampler, group::sampler_parameter_i pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameteriv)
        glIsSampler(sampler);
    }
    glGetSamplerParameteriv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameteriv"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE stl_types::String
    get_stringi(group::string_name name, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetStringi)
    }
    auto out = glGetStringi(static_cast<GLenum>(name), index);
    detail::error_check("GetStringi"sv);
    return reinterpret_cast<const char*>(out);
}

/* Introduced in GL es 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_synciv(
        GLsync                     sync,
        group::sync_parameter_name pname,
        i32&                       length,
        span_i32                   values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSynciv)
    }
    glGetSynciv(
        sync,
        static_cast<GLenum>(pname),
        values.size(),
        &length,
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("GetSynciv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_transform_feedback_varying(
        u32         program,
        u32         index,
        i32&        length,
        i32&        size,
        GLenum&     type,
        span_GLchar name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbackVarying)
        glIsProgram(program);
    }
    glGetTransformFeedbackVarying(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetTransformFeedbackVarying"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLuint
    get_uniform_block_index(
        u32 program, std::string_view const& uniformBlockName)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformBlockIndex)
        glIsProgram(program);
    }
    auto out = glGetUniformBlockIndex(program, uniformBlockName.data());
    detail::error_check("GetUniformBlockIndex"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_uniform_indices(
        u32                           program,
        i32                           uniformCount,
        std::vector<std::string_view> uniformNames,
        span_u32                      uniformIndices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformIndices)
        glIsProgram(program);
    }
    auto [uniformNames_lens, uniformNames_cstr, uniformNames_store] =
        detail::transform_strings(uniformNames);
    glGetUniformIndices(
        program,
        uniformCount,
        uniformNames_cstr.data(),
        uniformIndices.size() ? reinterpret_cast<GLuint*>(uniformIndices.data())
                              : nullptr);
    detail::error_check("GetUniformIndices"sv);
}

/* Introduced in GL es 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_uniformuiv(u32 program, i32 location, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformuiv)
        glIsProgram(program);
    }
    glGetUniformuiv(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetUniformuiv"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void get_vertex_attrib_iiv(
        u32 index, group::vertex_attrib_enum pname, i32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIiv)
    }
    glGetVertexAttribIiv(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIiv"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void get_vertex_attrib_iuiv(
        u32 index, group::vertex_attrib_enum pname, u32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIuiv)
    }
    glGetVertexAttribIuiv(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIuiv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_invalidate_framebuffer_attachment>
requires(
    MinimumVersion<Current, Version<3, 0>>&& semantic::concepts::
        Span<span_const_invalidate_framebuffer_attachment>&& std::is_same_v<
            std::decay_t<typename span_const_invalidate_framebuffer_attachment::
                             value_type>,
            std::decay_t<group::invalidate_framebuffer_attachment>>)
    STATICINLINE void invalidate_framebuffer(
        group::framebuffer_target                           target,
        span_const_invalidate_framebuffer_attachment const& attachments)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateFramebuffer)
    }
    glInvalidateFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr);
    detail::error_check("InvalidateFramebuffer"sv);
}

/* Introduced in GL es 3.0 */
template<
    class size_2_i32,
    class span_const_invalidate_framebuffer_attachment,
    class vec_2_i32>
requires(
    MinimumVersion<Current, Version<3, 0>>&& semantic::concepts::
        Span<span_const_invalidate_framebuffer_attachment>&& std::is_same_v<
            std::decay_t<typename span_const_invalidate_framebuffer_attachment::
                             value_type>,
            std::decay_t<group::invalidate_framebuffer_attachment>>&&
                semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void invalidate_sub_framebuffer(
        group::framebuffer_target                           target,
        span_const_invalidate_framebuffer_attachment const& attachments,
        vec_2_i32 const&                                    x,
        size_2_i32 const&                                   width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateSubFramebuffer)
    }
    glInvalidateSubFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr,
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("InvalidateSubFramebuffer"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_query(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsQuery)
    }
    auto out = glIsQuery(id);
    detail::error_check("IsQuery"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_sampler(u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSampler)
        glIsSampler(sampler);
    }
    auto out = glIsSampler(sampler);
    detail::error_check("IsSampler"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_sync(GLsync sync)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSync)
    }
    auto out = glIsSync(sync);
    detail::error_check("IsSync"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_transform_feedback(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTransformFeedback)
    }
    auto out = glIsTransformFeedback(id);
    detail::error_check("IsTransformFeedback"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_vertex_array(u32 array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsVertexArray)
        glIsVertexArray(array);
    }
    auto out = glIsVertexArray(array);
    detail::error_check("IsVertexArray"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void* map_buffer_range(
        group::buffer_target_arb      target,
        GLintptr                      offset,
        GLsizeiptr                    length,
        group::map_buffer_access_mask access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapBufferRange)
    }
    auto out = glMapBufferRange(
        static_cast<GLenum>(target),
        offset,
        length,
        static_cast<GLenum>(access));
    detail::error_check("MapBufferRange"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void pause_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PauseTransformFeedback)
    }
    glPauseTransformFeedback();
    detail::error_check("PauseTransformFeedback"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void program_binary(
        u32                    program,
        GLenum                 binaryFormat,
        span_const_void const& binary,
        i32                    length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBinary)
        glIsProgram(program);
    }
    glProgramBinary(
        program,
        binaryFormat,
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ProgramBinary"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void program_parameter(
        u32 program, group::program_parameter_prop pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameteri)
        glIsProgram(program);
    }
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteri"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void read_buffer(group::read_buffer_mode src)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadBuffer)
    }
    glReadBuffer(static_cast<GLenum>(src));
    detail::error_check("ReadBuffer"sv);
}

/* Introduced in GL es 3.0 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void renderbuffer_storage_multisample(
        group::renderbuffer_target target,
        i32                        samples,
        group::internal_format     internalformat,
        size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisample)
    }
    glRenderbufferStorageMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisample"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void resume_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResumeTransformFeedback)
    }
    glResumeTransformFeedback();
    detail::error_check("ResumeTransformFeedback"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void sampler_parameter(
        u32 sampler, group::sampler_parameter_f pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterf)
        glIsSampler(sampler);
    }
    glSamplerParameterf(sampler, static_cast<GLenum>(pname), param);
    detail::error_check("SamplerParameterf"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void sampler_parameter(
        u32                        sampler,
        group::sampler_parameter_f pname,
        span_const_f32 const&      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterfv)
        glIsSampler(sampler);
    }
    glSamplerParameterfv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfloat*>(param.data())
                     : nullptr);
    detail::error_check("SamplerParameterfv"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void sampler_parameter(
        u32 sampler, group::sampler_parameter_i pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameteri)
        glIsSampler(sampler);
    }
    glSamplerParameteri(sampler, static_cast<GLenum>(pname), param);
    detail::error_check("SamplerParameteri"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void sampler_parameter(
        u32                        sampler,
        group::sampler_parameter_i pname,
        span_const_i32 const&      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameteriv)
        glIsSampler(sampler);
    }
    glSamplerParameteriv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameteriv"sv);
}

/* Introduced in GL es 3.0 */
template<class size_3_i32, class span_const_void>
requires(MinimumVersion<Current, Version<3, 0>>&&
                 semantic::concepts::Size2D<size_3_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void tex_image_3d(
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        size_3_i32 const&      width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage3D)
    }
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
    detail::error_check("TexImage3D"sv);
}

/* Introduced in GL es 3.0 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
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

/* Introduced in GL es 3.0 */
template<class size_3_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
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

/* Introduced in GL es 3.0 */
template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void tex_sub_image_3d(
        group::texture_target  target,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage3D)
    }
    glTexSubImage3D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage3D"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void transform_feedback_varyings(
        u32                                   program,
        std::vector<std::string_view>         varyings,
        group::transform_feedback_buffer_mode bufferMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackVaryings)
        glIsProgram(program);
    }
    auto [varyings_lens, varyings_cstr, varyings_store] =
        detail::transform_strings(varyings);
    glTransformFeedbackVaryings(
        program,
        varyings_cstr.size(),
        varyings_cstr.data(),
        static_cast<GLenum>(bufferMode));
    detail::error_check("TransformFeedbackVaryings"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void uniform(i32 location, u32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ui)
    }
    glUniform1ui(location, v0);
    detail::error_check("Uniform1ui"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1uiv)
    }
    glUniform1uiv(
        location, count, reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform1uiv"sv);
}

/* Introduced in GL es 3.0 */
template<class vec_2_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_2_u32, u32, 2>) STATICINLINE
    void uniform(i32 location, vec_2_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui)
    }
    glUniform2ui(location, v0.x(), v0.y());
    detail::error_check("Uniform2ui"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
            Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2uiv)
    }
    glUniform2uiv(
        location, count, reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform2uiv"sv);
}

/* Introduced in GL es 3.0 */
template<class vec_3_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_3_u32, u32, 3>) STATICINLINE
    void uniform(i32 location, vec_3_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui)
    }
    glUniform3ui(location, v0.x(), v0.y(), v0.z());
    detail::error_check("Uniform3ui"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
            Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3uiv)
    }
    glUniform3uiv(
        location, count, reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform3uiv"sv);
}

/* Introduced in GL es 3.0 */
template<class vec_4_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
    void uniform(i32 location, vec_4_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui)
    }
    glUniform4ui(location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("Uniform4ui"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
            Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4uiv)
    }
    glUniform4uiv(
        location, count, reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform4uiv"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void uniform_block_binding(
        u32 program, u32 uniformBlockIndex, u32 uniformBlockBinding)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformBlockBinding)
        glIsProgram(program);
    }
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    detail::error_check("UniformBlockBinding"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_mat_2x3_f32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_mat_2x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3fv)
    }
    glUniformMatrix2x3fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x3fv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_mat_2x4_f32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_mat_2x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4fv)
    }
    glUniformMatrix2x4fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2x4fv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_mat_3x2_f32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2fv)
    }
    glUniformMatrix3x2fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x2fv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_mat_3x4_f32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_mat_3x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4fv)
    }
    glUniformMatrix3x4fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3x4fv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_mat_4x2_f32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_mat_4x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2fv)
    }
    glUniformMatrix4x2fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x2fv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_mat_4x3_f32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_mat_4x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3fv)
    }
    glUniformMatrix4x3fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4x3fv"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    unmap_buffer(group::buffer_target_arb target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapBuffer)
    }
    auto out = glUnmapBuffer(static_cast<GLenum>(target));
    detail::error_check("UnmapBuffer"sv);
    return out;
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void vertex_attrib_divisor(u32 index, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribDivisor)
    }
    glVertexAttribDivisor(index, divisor);
    detail::error_check("VertexAttribDivisor"sv);
}

/* Introduced in GL es 3.0 */
template<class vec_4_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
    void vertex_attrib_i4i(u32 index, vec_4_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4i)
    }
    glVertexAttribI4i(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribI4i"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
            Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE
    void vertex_attrib_i4iv(u32 index, span_const_vec_4_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4iv)
    }
    glVertexAttribI4iv(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI4iv"sv);
}

/* Introduced in GL es 3.0 */
template<class vec_4_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
    void vertex_attrib_i4ui(u32 index, vec_4_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4ui)
    }
    glVertexAttribI4ui(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribI4ui"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
            Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE
    void vertex_attrib_i4uiv(u32 index, span_const_vec_4_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4uiv)
    }
    glVertexAttribI4uiv(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI4uiv"sv);
}

/* Introduced in GL es 3.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void vertex_attrib_i_pointer(
        u32                      index,
        i32                      size,
        group::vertex_attrib_int type,
        i32                      stride,
        span_const_void const&   pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribIPointer)
    }
    glVertexAttribIPointer(
        index,
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribIPointer"sv);
}

/* Introduced in GL es 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void wait_sync(GLsync sync, group::sync_behavior_flags flags, u64 timeout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WaitSync)
    }
    glWaitSync(sync, static_cast<GLenum>(flags), timeout);
    detail::error_check("WaitSync"sv);
}

#endif // GL_ES_VERSION_3_0
