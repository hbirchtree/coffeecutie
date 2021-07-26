#ifdef GL_VERSION_4_5
/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE void clip_control(
    group::clip_control_origin origin, group::clip_control_depth depth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClipControl)
    }
    glClipControl(static_cast<GLenum>(origin), static_cast<GLenum>(depth));
    detail::error_check("ClipControl"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void bind_texture_unit(u32 unit, u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTextureUnit)
        glIsTexture(texture);
    }
    glBindTextureUnit(unit, texture);
    detail::error_check("BindTextureUnit"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void blit_named_framebuffer(
        u32                            readFramebuffer,
        u32                            drawFramebuffer,
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
        GLW_FPTR_CHECK(BlitNamedFramebuffer)
    }
    glBlitNamedFramebuffer(
        readFramebuffer,
        drawFramebuffer,
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
    detail::error_check("BlitNamedFramebuffer"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE GLenum
    check_named_framebuffer_status(
        u32 framebuffer, group::framebuffer_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckNamedFramebufferStatus)
        glIsFramebuffer(framebuffer);
    }
    auto out =
        glCheckNamedFramebufferStatus(framebuffer, static_cast<GLenum>(target));
    detail::error_check("CheckNamedFramebufferStatus"sv);
    return out;
}

/* Introduced in GL core 4.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_named_buffer_data(
        u32                          buffer,
        group::sized_internal_format internalformat,
        group::pixel_format          format,
        group::pixel_type            type,
        span_const_void const&       data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedBufferData)
        glIsBuffer(buffer);
    }
    glClearNamedBufferData(
        buffer,
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearNamedBufferData"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_named_buffer_sub_data(
        u32                          buffer,
        group::sized_internal_format internalformat,
        GLintptr                     offset,
        GLsizeiptr                   size,
        group::pixel_format          format,
        group::pixel_type            type,
        span_const_void const&       data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedBufferSubData)
        glIsBuffer(buffer);
    }
    glClearNamedBufferSubData(
        buffer,
        static_cast<GLenum>(internalformat),
        offset,
        size,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearNamedBufferSubData"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void clear_named_framebufferfi(
        u32           framebuffer,
        group::buffer buffer,
        i32           drawbuffer,
        f32           depth,
        i32           stencil)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedFramebufferfi)
        glIsFramebuffer(framebuffer);
    }
    glClearNamedFramebufferfi(
        framebuffer, static_cast<GLenum>(buffer), drawbuffer, depth, stencil);
    detail::error_check("ClearNamedFramebufferfi"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void clear_named_framebufferfv(
        u32                   framebuffer,
        group::buffer         buffer,
        i32                   drawbuffer,
        span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedFramebufferfv)
        glIsFramebuffer(framebuffer);
    }
    glClearNamedFramebufferfv(
        framebuffer,
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLfloat*>(value.data())
                     : nullptr);
    detail::error_check("ClearNamedFramebufferfv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void clear_named_framebufferiv(
        u32                   framebuffer,
        group::buffer         buffer,
        i32                   drawbuffer,
        span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedFramebufferiv)
        glIsFramebuffer(framebuffer);
    }
    glClearNamedFramebufferiv(
        framebuffer,
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLint*>(value.data()) : nullptr);
    detail::error_check("ClearNamedFramebufferiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void clear_named_framebufferuiv(
        u32                   framebuffer,
        group::buffer         buffer,
        i32                   drawbuffer,
        span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedFramebufferuiv)
        glIsFramebuffer(framebuffer);
    }
    glClearNamedFramebufferuiv(
        framebuffer,
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLuint*>(value.data()) : nullptr);
    detail::error_check("ClearNamedFramebufferuiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_sub_image_1d(
        u32                    texture,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::pixel_format    format,
        i32                    imageSize,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage1D)
        glIsTexture(texture);
    }
    glCompressedTextureSubImage1D(
        texture,
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        imageSize,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage1D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_sub_image_2d(
        u32                    texture,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        i32                    imageSize,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage2D)
        glIsTexture(texture);
    }
    glCompressedTextureSubImage2D(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        imageSize,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage2D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_texture_sub_image_3d(
        u32                    texture,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::pixel_format    format,
        i32                    imageSize,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage3D)
        glIsTexture(texture);
    }
    glCompressedTextureSubImage3D(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        imageSize,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage3D"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void copy_named_buffer_sub_data(
        u32        readBuffer,
        u32        writeBuffer,
        GLintptr   readOffset,
        GLintptr   writeOffset,
        GLsizeiptr size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyNamedBufferSubData)
    }
    glCopyNamedBufferSubData(
        readBuffer, writeBuffer, readOffset, writeOffset, size);
    detail::error_check("CopyNamedBufferSubData"sv);
}

/* Introduced in GL core 4.5 */
template<class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void copy_texture_sub_image_1d(
        u32 texture, i32 level, i32 xoffset, vec_2_i32 const& x, i32 width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage1D)
        glIsTexture(texture);
    }
    glCopyTextureSubImage1D(texture, level, xoffset, x.x(), x.y(), width);
    detail::error_check("CopyTextureSubImage1D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32, class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_texture_sub_image_2d(
        u32               texture,
        i32               level,
        vec_2_i32 const&  xoffset,
        vec_2_i32 const&  x,
        size_2_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage2D)
        glIsTexture(texture);
    }
    glCopyTextureSubImage2D(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTextureSubImage2D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_texture_sub_image_3d(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        vec_2_i32 const&  x,
        size_2_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage3D)
        glIsTexture(texture);
    }
    glCopyTextureSubImage3D(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTextureSubImage3D"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_buffers(span_u32 buffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateBuffers)
    }
    glCreateBuffers(
        buffers.size(),
        buffers.size() ? reinterpret_cast<GLuint*>(buffers.data()) : nullptr);
    detail::error_check("CreateBuffers"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_framebuffers(span_u32 framebuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateFramebuffers)
    }
    glCreateFramebuffers(
        framebuffers.size(),
        framebuffers.size() ? reinterpret_cast<GLuint*>(framebuffers.data())
                            : nullptr);
    detail::error_check("CreateFramebuffers"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_program_pipelines(span_u32 pipelines)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateProgramPipelines)
    }
    glCreateProgramPipelines(
        pipelines.size(),
        pipelines.size() ? reinterpret_cast<GLuint*>(pipelines.data())
                         : nullptr);
    detail::error_check("CreateProgramPipelines"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_queries(group::query_target target, span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateQueries)
    }
    glCreateQueries(
        static_cast<GLenum>(target),
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("CreateQueries"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_renderbuffers(span_u32 renderbuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateRenderbuffers)
    }
    glCreateRenderbuffers(
        renderbuffers.size(),
        renderbuffers.size() ? reinterpret_cast<GLuint*>(renderbuffers.data())
                             : nullptr);
    detail::error_check("CreateRenderbuffers"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_samplers(span_u32 samplers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateSamplers)
    }
    glCreateSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<GLuint*>(samplers.data()) : nullptr);
    detail::error_check("CreateSamplers"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_textures(group::texture_target target, span_u32 textures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateTextures)
    }
    glCreateTextures(
        static_cast<GLenum>(target),
        textures.size(),
        textures.size() ? reinterpret_cast<GLuint*>(textures.data()) : nullptr);
    detail::error_check("CreateTextures"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_transform_feedbacks(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateTransformFeedbacks)
    }
    glCreateTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("CreateTransformFeedbacks"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void create_vertex_arrays(span_u32 arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateVertexArrays)
    }
    glCreateVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<GLuint*>(arrays.data()) : nullptr);
    detail::error_check("CreateVertexArrays"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void disable_vertex_array_attrib(u32 vaobj, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexArrayAttrib)
        glIsVertexArray(vaobj);
    }
    glDisableVertexArrayAttrib(vaobj, index);
    detail::error_check("DisableVertexArrayAttrib"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void enable_vertex_array_attrib(u32 vaobj, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexArrayAttrib)
        glIsVertexArray(vaobj);
    }
    glEnableVertexArrayAttrib(vaobj, index);
    detail::error_check("EnableVertexArrayAttrib"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void flush_mapped_named_buffer_range(
        u32 buffer, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushMappedNamedBufferRange)
        glIsBuffer(buffer);
    }
    glFlushMappedNamedBufferRange(buffer, offset, length);
    detail::error_check("FlushMappedNamedBufferRange"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void generate_texture_mipmap(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateTextureMipmap)
        glIsTexture(texture);
    }
    glGenerateTextureMipmap(texture);
    detail::error_check("GenerateTextureMipmap"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_compressed_texture_image(
        u32 texture, i32 level, i32 bufSize, span_void pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTextureImage)
        glIsTexture(texture);
    }
    glGetCompressedTextureImage(
        texture,
        level,
        bufSize,
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetCompressedTextureImage"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>) STATICINLINE
    void get_named_buffer_parameteri64v(
        u32 buffer, group::buffer_prop_arb pname, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferParameteri64v)
        glIsBuffer(buffer);
    }
    glGetNamedBufferParameteri64v(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferParameteri64v"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_named_buffer_parameter(
        u32 buffer, group::buffer_prop_arb pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferParameteriv)
        glIsBuffer(buffer);
    }
    glGetNamedBufferParameteriv(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferParameteriv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_named_buffer_pointerv(
        u32 buffer, group::buffer_pointer_name_arb pname, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferPointerv)
        glIsBuffer(buffer);
    }
    glGetNamedBufferPointerv(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferPointerv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_named_buffer_sub_data(
        u32 buffer, GLintptr offset, GLsizeiptr size, span_void data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferSubData)
        glIsBuffer(buffer);
    }
    glGetNamedBufferSubData(
        buffer,
        offset,
        size,
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetNamedBufferSubData"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_named_framebuffer_attachment_parameter(
        u32                                          framebuffer,
        group::framebuffer_attachment                attachment,
        group::framebuffer_attachment_parameter_name pname,
        span_i32                                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferAttachmentParameteriv)
        glIsFramebuffer(framebuffer);
    }
    glGetNamedFramebufferAttachmentParameteriv(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedFramebufferAttachmentParameteriv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_named_framebuffer_parameter(
        u32 framebuffer, group::get_framebuffer_parameter pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferParameteriv)
        glIsFramebuffer(framebuffer);
    }
    glGetNamedFramebufferParameteriv(
        framebuffer,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetNamedFramebufferParameteriv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_named_renderbuffer_parameter(
        u32                                renderbuffer,
        group::renderbuffer_parameter_name pname,
        span_i32                           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedRenderbufferParameteriv)
        glIsRenderbuffer(renderbuffer);
    }
    glGetNamedRenderbufferParameteriv(
        renderbuffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedRenderbufferParameteriv"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void get_query_buffer_objecti64v(
        u32                                id,
        u32                                buffer,
        group::query_object_parameter_name pname,
        GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryBufferObjecti64v)
        glIsBuffer(buffer);
    }
    glGetQueryBufferObjecti64v(id, buffer, static_cast<GLenum>(pname), offset);
    detail::error_check("GetQueryBufferObjecti64v"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void get_query_buffer_objectiv(
        u32                                id,
        u32                                buffer,
        group::query_object_parameter_name pname,
        GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryBufferObjectiv)
        glIsBuffer(buffer);
    }
    glGetQueryBufferObjectiv(id, buffer, static_cast<GLenum>(pname), offset);
    detail::error_check("GetQueryBufferObjectiv"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void get_query_buffer_objectui64v(
        u32                                id,
        u32                                buffer,
        group::query_object_parameter_name pname,
        GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryBufferObjectui64v)
        glIsBuffer(buffer);
    }
    glGetQueryBufferObjectui64v(id, buffer, static_cast<GLenum>(pname), offset);
    detail::error_check("GetQueryBufferObjectui64v"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void get_query_buffer_objectuiv(
        u32                                id,
        u32                                buffer,
        group::query_object_parameter_name pname,
        GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryBufferObjectuiv)
        glIsBuffer(buffer);
    }
    glGetQueryBufferObjectuiv(id, buffer, static_cast<GLenum>(pname), offset);
    detail::error_check("GetQueryBufferObjectuiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_texture_image(
        u32                 texture,
        i32                 level,
        group::pixel_format format,
        group::pixel_type   type,
        i32                 bufSize,
        span_void           pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureImage)
        glIsTexture(texture);
    }
    glGetTextureImage(
        texture,
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        bufSize,
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTextureImage"sv);
}

/* Introduced in GL core 4.5 */
template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_texture_level_parameter(
        u32                          texture,
        i32                          level,
        group::get_texture_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureLevelParameterfv)
        glIsTexture(texture);
    }
    glGetTextureLevelParameterfv(
        texture,
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTextureLevelParameterfv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_texture_level_parameter(
        u32                          texture,
        i32                          level,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureLevelParameteriv)
        glIsTexture(texture);
    }
    glGetTextureLevelParameteriv(
        texture,
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureLevelParameteriv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_texture_parameter_iiv(
        u32 texture, group::get_texture_parameter pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterIiv)
        glIsTexture(texture);
    }
    glGetTextureParameterIiv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterIiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_texture_parameter_iuiv(
        u32 texture, group::get_texture_parameter pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterIuiv)
        glIsTexture(texture);
    }
    glGetTextureParameterIuiv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterIuiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_texture_parameter(
        u32 texture, group::get_texture_parameter pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterfv)
        glIsTexture(texture);
    }
    glGetTextureParameterfv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterfv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_texture_parameter(
        u32 texture, group::get_texture_parameter pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameteriv)
        glIsTexture(texture);
    }
    glGetTextureParameteriv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameteriv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>) STATICINLINE
    void get_transform_feedbacki64_v(
        u32                            xfb,
        group::transform_feedback_prop pname,
        u32                            index,
        span_i64                       param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbacki64_v)
        glIsTransformFeedback(xfb);
    }
    glGetTransformFeedbacki64_v(
        xfb,
        static_cast<GLenum>(pname),
        index,
        param.size() ? reinterpret_cast<GLint64*>(param.data()) : nullptr);
    detail::error_check("GetTransformFeedbacki64_v"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_transform_feedbacki_v(
        u32                            xfb,
        group::transform_feedback_prop pname,
        u32                            index,
        span_i32                       param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbacki_v)
        glIsTransformFeedback(xfb);
    }
    glGetTransformFeedbacki_v(
        xfb,
        static_cast<GLenum>(pname),
        index,
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetTransformFeedbacki_v"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_transform_feedbackiv(
        u32 xfb, group::transform_feedback_prop pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbackiv)
        glIsTransformFeedback(xfb);
    }
    glGetTransformFeedbackiv(
        xfb,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetTransformFeedbackiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>) STATICINLINE
    void get_vertex_array_indexed64iv(
        u32 vaobj, u32 index, group::vertex_array_prop pname, span_i64 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayIndexed64iv)
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayIndexed64iv(
        vaobj,
        index,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint64*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayIndexed64iv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_vertex_array_indexediv(
        u32 vaobj, u32 index, group::vertex_array_prop pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayIndexediv)
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayIndexediv(
        vaobj,
        index,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayIndexediv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_vertex_arrayiv(
        u32 vaobj, group::vertex_array_prop pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayiv)
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayiv(
        vaobj,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_framebuffer_attachment>
requires(MinimumVersion<Current, Version<4, 5>>&&      semantic::concepts::
             Span<span_const_framebuffer_attachment>&& std::is_same_v<
                 std::decay_t<
                     typename span_const_framebuffer_attachment::value_type>,
                 std::decay_t<group::framebuffer_attachment>>) STATICINLINE
    void invalidate_named_framebuffer_data(
        u32                                      framebuffer,
        i32                                      numAttachments,
        span_const_framebuffer_attachment const& attachments)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateNamedFramebufferData)
        glIsFramebuffer(framebuffer);
    }
    glInvalidateNamedFramebufferData(
        framebuffer,
        numAttachments,
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr);
    detail::error_check("InvalidateNamedFramebufferData"sv);
}

/* Introduced in GL core 4.5 */
template<
    class size_2_i32,
    class span_const_framebuffer_attachment,
    class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&      semantic::concepts::
             Span<span_const_framebuffer_attachment>&& std::is_same_v<
                 std::decay_t<
                     typename span_const_framebuffer_attachment::value_type>,
                 std::decay_t<group::framebuffer_attachment>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void invalidate_named_framebuffer_sub_data(
        u32                                      framebuffer,
        i32                                      numAttachments,
        span_const_framebuffer_attachment const& attachments,
        vec_2_i32 const&                         x,
        size_2_i32 const&                        width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateNamedFramebufferSubData)
        glIsFramebuffer(framebuffer);
    }
    glInvalidateNamedFramebufferSubData(
        framebuffer,
        numAttachments,
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr,
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("InvalidateNamedFramebufferSubData"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void* map_named_buffer(u32 buffer, group::buffer_access_arb access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapNamedBuffer)
        glIsBuffer(buffer);
    }
    auto out = glMapNamedBuffer(buffer, static_cast<GLenum>(access));
    detail::error_check("MapNamedBuffer"sv);
    return out;
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void* map_named_buffer_range(
        u32                           buffer,
        GLintptr                      offset,
        GLsizeiptr                    length,
        group::map_buffer_access_mask access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapNamedBufferRange)
        glIsBuffer(buffer);
    }
    auto out = glMapNamedBufferRange(
        buffer, offset, length, static_cast<GLenum>(access));
    detail::error_check("MapNamedBufferRange"sv);
    return out;
}

/* Introduced in GL core 4.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void named_buffer_data(
        u32                               buffer,
        GLsizeiptr                        size,
        span_const_void const&            data,
        group::vertex_buffer_object_usage usage)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferData)
        glIsBuffer(buffer);
    }
    glNamedBufferData(
        buffer,
        size,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(usage));
    detail::error_check("NamedBufferData"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void named_buffer_storage(
        u32                        buffer,
        span_const_void const&     data,
        group::buffer_storage_mask flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferStorage)
        glIsBuffer(buffer);
    }
    glNamedBufferStorage(
        buffer,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(flags));
    detail::error_check("NamedBufferStorage"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void named_buffer_sub_data(
        u32                    buffer,
        GLintptr               offset,
        GLsizeiptr             size,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferSubData)
        glIsBuffer(buffer);
    }
    glNamedBufferSubData(
        buffer,
        offset,
        size,
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("NamedBufferSubData"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void named_framebuffer_draw_buffer(u32 framebuffer, group::color_buffer buf)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferDrawBuffer)
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferDrawBuffer(framebuffer, static_cast<GLenum>(buf));
    detail::error_check("NamedFramebufferDrawBuffer"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_color_buffer>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_color_buffer>&& std::is_same_v<
                 std::decay_t<typename span_const_color_buffer::value_type>,
                 std::decay_t<group::color_buffer>>) STATICINLINE
    void named_framebuffer_draw_buffers(
        u32 framebuffer, i32 n, span_const_color_buffer const& bufs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferDrawBuffers)
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferDrawBuffers(
        framebuffer,
        n,
        bufs.size() ? reinterpret_cast<const GLenum*>(bufs.data()) : nullptr);
    detail::error_check("NamedFramebufferDrawBuffers"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void named_framebuffer_parameter(
        u32 framebuffer, group::framebuffer_parameter_name pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferParameteri)
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferParameteri(
        framebuffer, static_cast<GLenum>(pname), param);
    detail::error_check("NamedFramebufferParameteri"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void named_framebuffer_read_buffer(u32 framebuffer, group::color_buffer src)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferReadBuffer)
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferReadBuffer(framebuffer, static_cast<GLenum>(src));
    detail::error_check("NamedFramebufferReadBuffer"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void named_framebuffer_renderbuffer(
        u32                           framebuffer,
        group::framebuffer_attachment attachment,
        group::renderbuffer_target    renderbuffertarget,
        u32                           renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferRenderbuffer)
        glIsFramebuffer(framebuffer);
        glIsRenderbuffer(renderbuffer);
    }
    glNamedFramebufferRenderbuffer(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("NamedFramebufferRenderbuffer"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void named_framebuffer_texture(
        u32                           framebuffer,
        group::framebuffer_attachment attachment,
        u32                           texture,
        i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTexture)
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTexture(
        framebuffer, static_cast<GLenum>(attachment), texture, level);
    detail::error_check("NamedFramebufferTexture"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void named_framebuffer_texture_layer(
        u32                           framebuffer,
        group::framebuffer_attachment attachment,
        u32                           texture,
        i32                           level,
        i32                           layer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTextureLayer)
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTextureLayer(
        framebuffer, static_cast<GLenum>(attachment), texture, level, layer);
    detail::error_check("NamedFramebufferTextureLayer"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void named_renderbuffer_storage(
        u32                    renderbuffer,
        group::internal_format internalformat,
        size_2_i32 const&      width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorage)
        glIsRenderbuffer(renderbuffer);
    }
    glNamedRenderbufferStorage(
        renderbuffer, static_cast<GLenum>(internalformat), width[0], width[1]);
    detail::error_check("NamedRenderbufferStorage"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void named_renderbuffer_storage_multisample(
        u32                    renderbuffer,
        i32                    samples,
        group::internal_format internalformat,
        size_2_i32 const&      width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageMultisample)
        glIsRenderbuffer(renderbuffer);
    }
    glNamedRenderbufferStorageMultisample(
        renderbuffer,
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("NamedRenderbufferStorageMultisample"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void texture_buffer(
        u32 texture, group::sized_internal_format internalformat, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBuffer)
        glIsTexture(texture);
        glIsBuffer(buffer);
    }
    glTextureBuffer(texture, static_cast<GLenum>(internalformat), buffer);
    detail::error_check("TextureBuffer"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void texture_buffer_range(
        u32                          texture,
        group::sized_internal_format internalformat,
        u32                          buffer,
        GLintptr                     offset,
        GLsizeiptr                   size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBufferRange)
        glIsTexture(texture);
        glIsBuffer(buffer);
    }
    glTextureBufferRange(
        texture, static_cast<GLenum>(internalformat), buffer, offset, size);
    detail::error_check("TextureBufferRange"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void texture_parameter_iiv(
        u32                           texture,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterIiv)
        glIsTexture(texture);
    }
    glTextureParameterIiv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterIiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void texture_parameter_iuiv(
        u32                           texture,
        group::texture_parameter_name pname,
        span_const_u32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterIuiv)
        glIsTexture(texture);
    }
    glTextureParameterIuiv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterIuiv"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void texture_parameter(
        u32 texture, group::texture_parameter_name pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterf)
        glIsTexture(texture);
    }
    glTextureParameterf(texture, static_cast<GLenum>(pname), param);
    detail::error_check("TextureParameterf"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void texture_parameter(
        u32                           texture,
        group::texture_parameter_name pname,
        span_const_f32 const&         param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterfv)
        glIsTexture(texture);
    }
    glTextureParameterfv(
        texture,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfloat*>(param.data())
                     : nullptr);
    detail::error_check("TextureParameterfv"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void texture_parameter(
        u32 texture, group::texture_parameter_name pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameteri)
        glIsTexture(texture);
    }
    glTextureParameteri(texture, static_cast<GLenum>(pname), param);
    detail::error_check("TextureParameteri"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void texture_parameter(
        u32                           texture,
        group::texture_parameter_name pname,
        span_const_i32 const&         param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameteriv)
        glIsTexture(texture);
    }
    glTextureParameteriv(
        texture,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("TextureParameteriv"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void texture_storage_1d(
        u32                          texture,
        i32                          levels,
        group::sized_internal_format internalformat,
        i32                          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage1D)
        glIsTexture(texture);
    }
    glTextureStorage1D(
        texture, levels, static_cast<GLenum>(internalformat), width);
    detail::error_check("TextureStorage1D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void texture_storage_2d(
        u32                          texture,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage2D)
        glIsTexture(texture);
    }
    glTextureStorage2D(
        texture,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TextureStorage2D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void texture_storage_2d_multisample(
        u32                          texture,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage2DMultisample)
        glIsTexture(texture);
    }
    glTextureStorage2DMultisample(
        texture,
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        fixedsamplelocations);
    detail::error_check("TextureStorage2DMultisample"sv);
}

/* Introduced in GL core 4.5 */
template<class size_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void texture_storage_3d(
        u32                          texture,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage3D)
        glIsTexture(texture);
    }
    glTextureStorage3D(
        texture,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2]);
    detail::error_check("TextureStorage3D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void texture_storage_3d_multisample(
        u32                          texture,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage3DMultisample)
        glIsTexture(texture);
    }
    glTextureStorage3DMultisample(
        texture,
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        fixedsamplelocations);
    detail::error_check("TextureStorage3DMultisample"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_sub_image_1d(
        u32                    texture,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureSubImage1D)
        glIsTexture(texture);
    }
    glTextureSubImage1D(
        texture,
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureSubImage1D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_sub_image_2d(
        u32                    texture,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureSubImage2D)
        glIsTexture(texture);
    }
    glTextureSubImage2D(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureSubImage2D"sv);
}

/* Introduced in GL core 4.5 */
template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void texture_sub_image_3d(
        u32                    texture,
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
        GLW_FPTR_CHECK(TextureSubImage3D)
        glIsTexture(texture);
    }
    glTextureSubImage3D(
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
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TextureSubImage3D"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void transform_feedback_buffer_base(u32 xfb, u32 index, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackBufferBase)
        glIsTransformFeedback(xfb);
        glIsBuffer(buffer);
    }
    glTransformFeedbackBufferBase(xfb, index, buffer);
    detail::error_check("TransformFeedbackBufferBase"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void transform_feedback_buffer_range(
        u32 xfb, u32 index, u32 buffer, GLintptr offset, GLsizeiptr size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackBufferRange)
        glIsTransformFeedback(xfb);
        glIsBuffer(buffer);
    }
    glTransformFeedbackBufferRange(xfb, index, buffer, offset, size);
    detail::error_check("TransformFeedbackBufferRange"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE GLboolean
    unmap_named_buffer(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapNamedBuffer)
        glIsBuffer(buffer);
    }
    auto out = glUnmapNamedBuffer(buffer);
    detail::error_check("UnmapNamedBuffer"sv);
    return out;
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void vertex_array_attrib_binding(
        u32 vaobj, u32 attribindex, u32 bindingindex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayAttribBinding)
        glIsVertexArray(vaobj);
    }
    glVertexArrayAttribBinding(vaobj, attribindex, bindingindex);
    detail::error_check("VertexArrayAttribBinding"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void vertex_array_attrib_format(
        u32                       vaobj,
        u32                       attribindex,
        i32                       size,
        group::vertex_attrib_type type,
        bool                      normalized,
        u32                       relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayAttribFormat)
        glIsVertexArray(vaobj);
    }
    glVertexArrayAttribFormat(
        vaobj,
        attribindex,
        size,
        static_cast<GLenum>(type),
        normalized,
        relativeoffset);
    detail::error_check("VertexArrayAttribFormat"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void vertex_array_attrib_i_format(
        u32                      vaobj,
        u32                      attribindex,
        i32                      size,
        group::vertex_attrib_int type,
        u32                      relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayAttribIFormat)
        glIsVertexArray(vaobj);
    }
    glVertexArrayAttribIFormat(
        vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexArrayAttribIFormat"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void vertex_array_attrib_l_format(
        u32                       vaobj,
        u32                       attribindex,
        i32                       size,
        group::vertex_attrib_long type,
        u32                       relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayAttribLFormat)
        glIsVertexArray(vaobj);
    }
    glVertexArrayAttribLFormat(
        vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexArrayAttribLFormat"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void vertex_array_binding_divisor(u32 vaobj, u32 bindingindex, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayBindingDivisor)
        glIsVertexArray(vaobj);
    }
    glVertexArrayBindingDivisor(vaobj, bindingindex, divisor);
    detail::error_check("VertexArrayBindingDivisor"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void vertex_array_element_buffer(u32 vaobj, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayElementBuffer)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayElementBuffer(vaobj, buffer);
    detail::error_check("VertexArrayElementBuffer"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void vertex_array_vertex_buffer(
        u32 vaobj, u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexBuffer)
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayVertexBuffer(vaobj, bindingindex, buffer, offset, stride);
    detail::error_check("VertexArrayVertexBuffer"sv);
}

/* Introduced in GL core 4.5 */
template<class span_const_GLintptr, class span_const_i32, class span_const_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>&&
                 semantic::concepts::Span<span_const_GLintptr>&& std::is_same_v<
                     std::decay_t<typename span_const_GLintptr::value_type>,
                     std::decay_t<GLintptr>>&&
                     semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                         std::decay_t<typename span_const_i32::value_type>,
                         std::decay_t<i32>>) STATICINLINE
    void vertex_array_vertex_buffers(
        u32                        vaobj,
        u32                        first,
        i32                        count,
        span_const_u32 const&      buffers,
        span_const_GLintptr const& offsets,
        span_const_i32 const&      strides)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexBuffers)
        glIsVertexArray(vaobj);
    }
    glVertexArrayVertexBuffers(
        vaobj,
        first,
        count,
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr,
        offsets.size() ? reinterpret_cast<const GLintptr*>(offsets.data())
                       : nullptr,
        strides.size() ? reinterpret_cast<const GLsizei*>(strides.data())
                       : nullptr);
    detail::error_check("VertexArrayVertexBuffers"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void memory_barrier_by_region(group::memory_barrier_mask barriers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrierByRegion)
    }
    glMemoryBarrierByRegion(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrierByRegion"sv);
}

/* Introduced in GL core 4.5 */
template<class size_3_i32, class span_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_void>) STATICINLINE
    void get_compressed_texture_sub_image(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width,
        i32               bufSize,
        span_void         pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTextureSubImage)
        glIsTexture(texture);
    }
    glGetCompressedTextureSubImage(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2],
        bufSize,
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetCompressedTextureSubImage"sv);
}

/* Introduced in GL core 4.5 */
template<class size_3_i32, class span_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_void>) STATICINLINE
    void get_texture_sub_image(
        u32                 texture,
        i32                 level,
        vec_3_i32 const&    xoffset,
        size_3_i32 const&   width,
        group::pixel_format format,
        group::pixel_type   type,
        i32                 bufSize,
        span_void           pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureSubImage)
        glIsTexture(texture);
    }
    glGetTextureSubImage(
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
        bufSize,
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTextureSubImage"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE GLenum
    get_graphics_reset_status()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatus)
    }
    auto out = glGetGraphicsResetStatus();
    detail::error_check("GetGraphicsResetStatus"sv);
    return out;
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void getn_compressed_tex_image(
        group::texture_target target, i32 lod, span_void pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnCompressedTexImage)
    }
    glGetnCompressedTexImage(
        static_cast<GLenum>(target),
        lod,
        pixels.size() * sizeof(typename std::decay_t<span_void>::value_type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetnCompressedTexImage"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void getn_tex_image(
        group::texture_target target,
        i32                   level,
        group::pixel_format   format,
        group::pixel_type     type,
        span_void             pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnTexImage)
    }
    glGetnTexImage(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() * sizeof(typename std::decay_t<span_void>::value_type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetnTexImage"sv);
}

/* Introduced in GL core 4.5 */
template<class span_f64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void getn_uniformdv(u32 program, i32 location, i32 bufSize, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformdv)
        glIsProgram(program);
    }
    glGetnUniformdv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetnUniformdv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfv)
        glIsProgram(program);
    }
    glGetnUniformfv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetnUniformfv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformiv)
        glIsProgram(program);
    }
    glGetnUniformiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void getn_uniformuiv(
        u32 program, i32 location, i32 bufSize, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformuiv)
        glIsProgram(program);
    }
    glGetnUniformuiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformuiv"sv);
}

/* Introduced in GL core 4.5 */
template<class size_2_i32, class span_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_void>) STATICINLINE
    void readn_pixels(
        vec_2_i32 const&    x,
        size_2_i32 const&   width,
        group::pixel_format format,
        group::pixel_type   type,
        span_void           data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadnPixels)
    }
    glReadnPixels(
        x.x(),
        x.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("ReadnPixels"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void getn_color_table(
        group::color_table_target target,
        group::pixel_format       format,
        group::pixel_type         type,
        span_void                 table)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnColorTable)
    }
    glGetnColorTable(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        table.size() * sizeof(typename std::decay_t<span_void>::value_type),
        table.size() ? reinterpret_cast<void*>(table.data()) : nullptr);
    detail::error_check("GetnColorTable"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void getn_convolution_filter(
        group::convolution_target target,
        group::pixel_format       format,
        group::pixel_type         type,
        span_void                 image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnConvolutionFilter)
    }
    glGetnConvolutionFilter(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() * sizeof(typename std::decay_t<span_void>::value_type),
        image.size() ? reinterpret_cast<void*>(image.data()) : nullptr);
    detail::error_check("GetnConvolutionFilter"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void getn_histogram(
        group::histogram_target target,
        bool                    reset,
        group::pixel_format     format,
        group::pixel_type       type,
        span_void               values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnHistogram)
    }
    glGetnHistogram(
        static_cast<GLenum>(target),
        reset,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        values.size() * sizeof(typename std::decay_t<span_void>::value_type),
        values.size() ? reinterpret_cast<void*>(values.data()) : nullptr);
    detail::error_check("GetnHistogram"sv);
}

/* Introduced in GL core 4.5 */
template<class span_f64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void getn_mapdv(
        group::map_target target,
        group::map_query  query,
        i32               bufSize,
        span_f64          v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnMapdv)
    }
    glGetnMapdv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(query),
        bufSize,
        v.size() ? reinterpret_cast<GLdouble*>(v.data()) : nullptr);
    detail::error_check("GetnMapdv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void getn_mapfv(
        group::map_target target,
        group::map_query  query,
        i32               bufSize,
        span_f32          v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnMapfv)
    }
    glGetnMapfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(query),
        bufSize,
        v.size() ? reinterpret_cast<GLfloat*>(v.data()) : nullptr);
    detail::error_check("GetnMapfv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void getn_mapiv(
        group::map_target target,
        group::map_query  query,
        i32               bufSize,
        span_i32          v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnMapiv)
    }
    glGetnMapiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(query),
        bufSize,
        v.size() ? reinterpret_cast<GLint*>(v.data()) : nullptr);
    detail::error_check("GetnMapiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void getn_minmax(
        group::minmax_target target,
        bool                 reset,
        group::pixel_format  format,
        group::pixel_type    type,
        span_void            values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnMinmax)
    }
    glGetnMinmax(
        static_cast<GLenum>(target),
        reset,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        values.size() * sizeof(typename std::decay_t<span_void>::value_type),
        values.size() ? reinterpret_cast<void*>(values.data()) : nullptr);
    detail::error_check("GetnMinmax"sv);
}

/* Introduced in GL core 4.5 */
template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void getn_pixel_mapfv(group::pixel_map map, i32 bufSize, span_f32 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnPixelMapfv)
    }
    glGetnPixelMapfv(
        static_cast<GLenum>(map),
        bufSize,
        values.size() ? reinterpret_cast<GLfloat*>(values.data()) : nullptr);
    detail::error_check("GetnPixelMapfv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void getn_pixel_mapuiv(group::pixel_map map, i32 bufSize, span_u32 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnPixelMapuiv)
    }
    glGetnPixelMapuiv(
        static_cast<GLenum>(map),
        bufSize,
        values.size() ? reinterpret_cast<GLuint*>(values.data()) : nullptr);
    detail::error_check("GetnPixelMapuiv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u16>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u16>&& std::is_same_v<
                 std::decay_t<typename span_u16::value_type>,
                 std::decay_t<u16>>) STATICINLINE
    void getn_pixel_mapusv(group::pixel_map map, i32 bufSize, span_u16 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnPixelMapusv)
    }
    glGetnPixelMapusv(
        static_cast<GLenum>(map),
        bufSize,
        values.size() ? reinterpret_cast<GLushort*>(values.data()) : nullptr);
    detail::error_check("GetnPixelMapusv"sv);
}

/* Introduced in GL core 4.5 */
template<class span_u8>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u8>&& std::is_same_v<
                 std::decay_t<typename span_u8::value_type>,
                 std::decay_t<u8>>) STATICINLINE
    void getn_polygon_stipple(span_u8 pattern)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnPolygonStipple)
    }
    glGetnPolygonStipple(
        pattern.size(),
        pattern.size() ? reinterpret_cast<GLubyte*>(pattern.data()) : nullptr);
    detail::error_check("GetnPolygonStipple"sv);
}

/* Introduced in GL core 4.5 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void getn_separable_filter(
        group::separable_target target,
        group::pixel_format     format,
        group::pixel_type       type,
        span_void               row,
        span_void               column,
        span_void               span)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnSeparableFilter)
    }
    glGetnSeparableFilter(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        row.size() * sizeof(typename std::decay_t<span_void>::value_type),
        row.size() ? reinterpret_cast<void*>(row.data()) : nullptr,
        column.size() * sizeof(typename std::decay_t<span_void>::value_type),
        column.size() ? reinterpret_cast<void*>(column.data()) : nullptr,
        span.size() ? reinterpret_cast<void*>(span.data()) : nullptr);
    detail::error_check("GetnSeparableFilter"sv);
}

/* Introduced in GL core 4.5 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>) STATICINLINE
    void texture_barrier()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBarrier)
    }
    glTextureBarrier();
    detail::error_check("TextureBarrier"sv);
}

#endif // GL_VERSION_4_5
