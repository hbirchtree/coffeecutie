#ifdef GL_VERSION_4_5
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glClipControl. Introduced in GL core 4.5
     * \param origin GLenum
     * \param depth GLenum
     * \return void
     */
    STATICINLINE void clip_control(
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glBindTextureUnit. Introduced in GL core 4.5
     * \param unit GLuint
     * \param texture GLuint
     * \return void
     */
    STATICINLINE void bind_texture_unit(u32 unit, u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTextureUnit)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindTextureUnit(unit, texture);
    detail::error_check("BindTextureUnit"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glBlitNamedFramebuffer. Introduced in GL core 4.5
     * \param readFramebuffer GLuint
     * \param drawFramebuffer GLuint
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
    STATICINLINE void blit_named_framebuffer(
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glCheckNamedFramebufferStatus. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param target GLenum \return
     * FramebufferStatus
     */
    STATICINLINE GLenum check_named_framebuffer_status(
        u32 framebuffer, group::framebuffer_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckNamedFramebufferStatus)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    auto out =
        glCheckNamedFramebufferStatus(framebuffer, static_cast<GLenum>(target));
    detail::error_check("CheckNamedFramebufferStatus"sv);
    return out;
}

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glClearNamedBufferData. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param internalformat GLenum
     * \param format GLenum
     * \param type GLenum
     * \param data const void *
     * \return void
     */
    STATICINLINE void clear_named_buffer_data(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glClearNamedBufferData(
        buffer,
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearNamedBufferData"sv);
}

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glClearNamedBufferSubData. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param internalformat GLenum
     * \param offset GLintptr
     * \param size GLsizeiptr
     * \param format GLenum
     * \param type GLenum
     * \param data const void *
     * \return void
     */
    STATICINLINE void clear_named_buffer_sub_data(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glClearNamedFramebufferfi. Introduced in GL core 4.5
     * \param framebuffer GLuint
     * \param buffer GLenum
     * \param drawbuffer GLint
     * \param depth GLfloat
     * \param stencil GLint
     * \return void
     */
    STATICINLINE void clear_named_framebufferfi(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glClearNamedFramebufferfi(
        framebuffer, static_cast<GLenum>(buffer), drawbuffer, depth, stencil);
    detail::error_check("ClearNamedFramebufferfi"sv);
}

template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glClearNamedFramebufferfv. Introduced in GL core 4.5
     * \param framebuffer GLuint
     * \param buffer GLenum
     * \param drawbuffer GLint
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void clear_named_framebufferfv(
        u32                   framebuffer,
        group::buffer         buffer,
        i32                   drawbuffer,
        span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedFramebufferfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glClearNamedFramebufferfv(
        framebuffer,
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLfloat*>(value.data())
                     : nullptr);
    detail::error_check("ClearNamedFramebufferfv"sv);
}

template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glClearNamedFramebufferiv. Introduced in GL core 4.5
     * \param framebuffer GLuint
     * \param buffer GLenum
     * \param drawbuffer GLint
     * \param value const GLint *
     * \return void
     */
    STATICINLINE void clear_named_framebufferiv(
        u32                   framebuffer,
        group::buffer         buffer,
        i32                   drawbuffer,
        span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedFramebufferiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glClearNamedFramebufferiv(
        framebuffer,
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLint*>(value.data()) : nullptr);
    detail::error_check("ClearNamedFramebufferiv"sv);
}

template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glClearNamedFramebufferuiv. Introduced in GL core 4.5
     * \param framebuffer GLuint
     * \param buffer GLenum
     * \param drawbuffer GLint
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void clear_named_framebufferuiv(
        u32                   framebuffer,
        group::buffer         buffer,
        i32                   drawbuffer,
        span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearNamedFramebufferuiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glClearNamedFramebufferuiv(
        framebuffer,
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLuint*>(value.data()) : nullptr);
    detail::error_check("ClearNamedFramebufferuiv"sv);
}

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glCompressedTextureSubImage1D. Introduced in GL
     * core 4.5 \param texture GLuint \param level GLint \param xoffset GLint
     * \param width GLsizei
     * \param format GLenum
     * \param imageSize GLsizei
     * \param data const void *
     * \return void
     */
    STATICINLINE void compressed_texture_sub_image_1d(
        u32                    texture,
        i32                    level,
        i32                    xoffset,
        i32                    width,
        group::internal_format format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage1D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCompressedTextureSubImage1D(
        texture,
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage1D"sv);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glCompressedTextureSubImage2D. Introduced in GL
     * core 4.5 \param texture GLuint \param level GLint \param xoffset GLint
     * \param yoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param format GLenum
     * \param imageSize GLsizei
     * \param data const void *
     * \return void
     */
    STATICINLINE void compressed_texture_sub_image_2d(
        u32                    texture,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::internal_format format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage2D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCompressedTextureSubImage2D(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage2D"sv);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glCompressedTextureSubImage3D. Introduced in GL
     * core 4.5 \param texture GLuint \param level GLint \param xoffset GLint
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
    STATICINLINE void compressed_texture_sub_image_3d(
        u32                    texture,
        i32                    level,
        vec_3_i32 const&       xoffset,
        size_3_i32 const&      width,
        group::internal_format format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTextureSubImage3D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTextureSubImage3D"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glCopyNamedBufferSubData. Introduced in GL core 4.5
     * \param readBuffer GLuint
     * \param writeBuffer GLuint
     * \param readOffset GLintptr
     * \param writeOffset GLintptr
     * \param size GLsizeiptr
     * \return void
     */
    STATICINLINE void copy_named_buffer_sub_data(
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

template<class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Vector<vec_2_i32, i32, 2>)
    /*!
     * \brief Wraps around glCopyTextureSubImage1D. Introduced in GL core 4.5
     * \param texture GLuint
     * \param level GLint
     * \param xoffset GLint
     * \param x GLint
     * \param y GLint
     * \param width GLsizei
     * \return void
     */
    STATICINLINE void copy_texture_sub_image_1d(
        u32 texture, i32 level, i32 xoffset, vec_2_i32 const& x, i32 width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTextureSubImage1D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glCopyTextureSubImage1D(texture, level, xoffset, x.x(), x.y(), width);
    detail::error_check("CopyTextureSubImage1D"sv);
}

template<class size_2_i32, class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glCopyTextureSubImage2D. Introduced in GL core 4.5
     * \param texture GLuint
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param x GLint
     * \param y GLint
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void copy_texture_sub_image_2d(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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

template<class size_2_i32, class vec_2_i32, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glCopyTextureSubImage3D. Introduced in GL core 4.5
     * \param texture GLuint
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
    STATICINLINE void copy_texture_sub_image_3d(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateBuffers. Introduced in GL core 4.5
     * \param n GLsizei
     * \param buffers GLuint *
     * \return void
     */
    STATICINLINE void create_buffers(span_u32 buffers)
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateFramebuffers. Introduced in GL core 4.5
     * \param n GLsizei
     * \param framebuffers GLuint *
     * \return void
     */
    STATICINLINE void create_framebuffers(span_u32 framebuffers)
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateProgramPipelines. Introduced in GL core 4.5
     * \param n GLsizei
     * \param pipelines GLuint *
     * \return void
     */
    STATICINLINE void create_program_pipelines(span_u32 pipelines)
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateQueries. Introduced in GL core 4.5
     * \param target GLenum
     * \param n GLsizei
     * \param ids GLuint *
     * \return void
     */
    STATICINLINE void create_queries(group::query_target target, span_u32 ids)
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateRenderbuffers. Introduced in GL core 4.5
     * \param n GLsizei
     * \param renderbuffers GLuint *
     * \return void
     */
    STATICINLINE void create_renderbuffers(span_u32 renderbuffers)
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateSamplers. Introduced in GL core 4.5
     * \param n GLsizei
     * \param samplers GLuint *
     * \return void
     */
    STATICINLINE void create_samplers(span_u32 samplers)
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateTextures. Introduced in GL core 4.5
     * \param target GLenum
     * \param n GLsizei
     * \param textures GLuint *
     * \return void
     */
    STATICINLINE
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateTransformFeedbacks. Introduced in GL core 4.5
     * \param n GLsizei
     * \param ids GLuint *
     * \return void
     */
    STATICINLINE void create_transform_feedbacks(span_u32 ids)
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glCreateVertexArrays. Introduced in GL core 4.5
     * \param n GLsizei
     * \param arrays GLuint *
     * \return void
     */
    STATICINLINE void create_vertex_arrays(span_u32 arrays)
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glDisableVertexArrayAttrib. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param index GLuint
     * \return void
     */
    STATICINLINE void disable_vertex_array_attrib(u32 vaobj, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexArrayAttrib)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glDisableVertexArrayAttrib(vaobj, index);
    detail::error_check("DisableVertexArrayAttrib"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glEnableVertexArrayAttrib. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param index GLuint
     * \return void
     */
    STATICINLINE void enable_vertex_array_attrib(u32 vaobj, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexArrayAttrib)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glEnableVertexArrayAttrib(vaobj, index);
    detail::error_check("EnableVertexArrayAttrib"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glFlushMappedNamedBufferRange. Introduced in GL
     * core 4.5 \param buffer GLuint \param offset GLintptr \param length
     * GLsizeiptr \return void
     */
    STATICINLINE void flush_mapped_named_buffer_range(
        u32 buffer, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushMappedNamedBufferRange)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glFlushMappedNamedBufferRange(buffer, offset, length);
    detail::error_check("FlushMappedNamedBufferRange"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glGenerateTextureMipmap. Introduced in GL core 4.5
     * \param texture GLuint
     * \return void
     */
    STATICINLINE void generate_texture_mipmap(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateTextureMipmap)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGenerateTextureMipmap(texture);
    detail::error_check("GenerateTextureMipmap"sv);
}

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetCompressedTextureImage. Introduced in GL
     * core 4.5 \param texture GLuint \param level GLint \param bufSize GLsizei
     * \param pixels void *
     * \return void
     */
    STATICINLINE
    void get_compressed_texture_image(u32 texture, i32 level, span_void pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTextureImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetCompressedTextureImage(
        texture,
        level,
        pixels.size() * sizeof(typename std::decay_t<span_void>::value_type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetCompressedTextureImage"sv);
}

template<class span_i64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>)
    /*!
     * \brief Wraps around glGetNamedBufferParameteri64v. Introduced in GL
     * core 4.5 \param buffer GLuint \param pname GLenum \param params GLint64 *
     * \return void
     */
    STATICINLINE void get_named_buffer_parameteri64v(
        u32 buffer, group::buffer_prop_arb pname, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferParameteri64v)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetNamedBufferParameteri64v(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferParameteri64v"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetNamedBufferParameteriv. Introduced in GL
     * core 4.5 \param buffer GLuint \param pname GLenum \param params GLint *
     * \return void
     */
    STATICINLINE void get_named_buffer_parameter(
        u32 buffer, group::buffer_prop_arb pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetNamedBufferParameteriv(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferParameteriv"sv);
}

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetNamedBufferPointerv. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param pname GLenum
     * \param params void **
     * \return void
     */
    STATICINLINE void get_named_buffer_pointerv(
        u32 buffer, group::buffer_pointer_name_arb pname, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferPointerv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetNamedBufferPointerv(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetNamedBufferPointerv"sv);
}

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetNamedBufferSubData. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param offset GLintptr
     * \param size GLsizeiptr
     * \param data void *
     * \return void
     */
    STATICINLINE
    void get_named_buffer_sub_data(u32 buffer, GLintptr offset, span_void data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferSubData)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetNamedBufferSubData(
        buffer,
        offset,
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetNamedBufferSubData"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetNamedFramebufferAttachmentParameteriv.
     * Introduced in GL core 4.5 \param framebuffer GLuint \param attachment
     * GLenum \param pname GLenum \param params GLint * \return void
     */
    STATICINLINE void get_named_framebuffer_attachment_parameter(
        u32                                          framebuffer,
        group::framebuffer_attachment                attachment,
        group::framebuffer_attachment_parameter_name pname,
        span_i32                                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferAttachmentParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glGetNamedFramebufferAttachmentParameteriv(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedFramebufferAttachmentParameteriv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetNamedFramebufferParameteriv. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param pname GLenum \param param GLint
     * * \return void
     */
    STATICINLINE void get_named_framebuffer_parameter(
        u32 framebuffer, group::get_framebuffer_parameter pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glGetNamedFramebufferParameteriv(
        framebuffer,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetNamedFramebufferParameteriv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetNamedRenderbufferParameteriv. Introduced in GL
     * core 4.5 \param renderbuffer GLuint \param pname GLenum \param params
     * GLint * \return void
     */
    STATICINLINE void get_named_renderbuffer_parameter(
        u32                                renderbuffer,
        group::renderbuffer_parameter_name pname,
        span_i32                           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedRenderbufferParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glGetNamedRenderbufferParameteriv(
        renderbuffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetNamedRenderbufferParameteriv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glGetQueryBufferObjecti64v. Introduced in GL core 4.5
     * \param id GLuint
     * \param buffer GLuint
     * \param pname GLenum
     * \param offset GLintptr
     * \return void
     */
    STATICINLINE void get_query_buffer_objecti64v(
        u32                                id,
        u32                                buffer,
        group::query_object_parameter_name pname,
        GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryBufferObjecti64v)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetQueryBufferObjecti64v(id, buffer, static_cast<GLenum>(pname), offset);
    detail::error_check("GetQueryBufferObjecti64v"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glGetQueryBufferObjectiv. Introduced in GL core 4.5
     * \param id GLuint
     * \param buffer GLuint
     * \param pname GLenum
     * \param offset GLintptr
     * \return void
     */
    STATICINLINE void get_query_buffer_objectiv(
        u32                                id,
        u32                                buffer,
        group::query_object_parameter_name pname,
        GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryBufferObjectiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetQueryBufferObjectiv(id, buffer, static_cast<GLenum>(pname), offset);
    detail::error_check("GetQueryBufferObjectiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glGetQueryBufferObjectui64v. Introduced in GL
     * core 4.5 \param id GLuint \param buffer GLuint \param pname GLenum \param
     * offset GLintptr \return void
     */
    STATICINLINE void get_query_buffer_objectui64v(
        u32                                id,
        u32                                buffer,
        group::query_object_parameter_name pname,
        GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryBufferObjectui64v)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetQueryBufferObjectui64v(id, buffer, static_cast<GLenum>(pname), offset);
    detail::error_check("GetQueryBufferObjectui64v"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glGetQueryBufferObjectuiv. Introduced in GL core 4.5
     * \param id GLuint
     * \param buffer GLuint
     * \param pname GLenum
     * \param offset GLintptr
     * \return void
     */
    STATICINLINE void get_query_buffer_objectuiv(
        u32                                id,
        u32                                buffer,
        group::query_object_parameter_name pname,
        GLintptr                           offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryBufferObjectuiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetQueryBufferObjectuiv(id, buffer, static_cast<GLenum>(pname), offset);
    detail::error_check("GetQueryBufferObjectuiv"sv);
}

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetTextureImage. Introduced in GL core 4.5
     * \param texture GLuint
     * \param level GLint
     * \param format GLenum
     * \param type GLenum
     * \param bufSize GLsizei
     * \param pixels void *
     * \return void
     */
    STATICINLINE void get_texture_image(
        u32                 texture,
        i32                 level,
        group::pixel_format format,
        group::pixel_type   type,
        span_void           pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureImage(
        texture,
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() * sizeof(typename std::decay_t<span_void>::value_type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTextureImage"sv);
}

template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glGetTextureLevelParameterfv. Introduced in GL
     * core 4.5 \param texture GLuint \param level GLint \param pname GLenum
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE void get_texture_level_parameter(
        u32                          texture,
        i32                          level,
        group::get_texture_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureLevelParameterfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureLevelParameterfv(
        texture,
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTextureLevelParameterfv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetTextureLevelParameteriv. Introduced in GL
     * core 4.5 \param texture GLuint \param level GLint \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_texture_level_parameter(
        u32                          texture,
        i32                          level,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureLevelParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureLevelParameteriv(
        texture,
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureLevelParameteriv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetTextureParameterIiv. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_texture_parameter_iiv(
        u32 texture, group::get_texture_parameter pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterIiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureParameterIiv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterIiv"sv);
}

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glGetTextureParameterIuiv. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void get_texture_parameter_iuiv(
        u32 texture, group::get_texture_parameter pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterIuiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureParameterIuiv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterIuiv"sv);
}

template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glGetTextureParameterfv. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE void get_texture_parameter(
        u32 texture, group::get_texture_parameter pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameterfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureParameterfv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameterfv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetTextureParameteriv. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_texture_parameter(
        u32 texture, group::get_texture_parameter pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glGetTextureParameteriv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTextureParameteriv"sv);
}

template<class span_i64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>)
    /*!
     * \brief Wraps around glGetTransformFeedbacki64_v. Introduced in GL
     * core 4.5 \param xfb GLuint \param pname GLenum \param index GLuint \param
     * param GLint64 * \return void
     */
    STATICINLINE void get_transform_feedbacki64_v(
        u32                            xfb,
        group::transform_feedback_prop pname,
        u32                            index,
        span_i64                       param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbacki64_v)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTransformFeedback)
            glIsTransformFeedback(xfb);
#endif
    }
    glGetTransformFeedbacki64_v(
        xfb,
        static_cast<GLenum>(pname),
        index,
        param.size() ? reinterpret_cast<GLint64*>(param.data()) : nullptr);
    detail::error_check("GetTransformFeedbacki64_v"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetTransformFeedbacki_v. Introduced in GL core 4.5
     * \param xfb GLuint
     * \param pname GLenum
     * \param index GLuint
     * \param param GLint *
     * \return void
     */
    STATICINLINE void get_transform_feedbacki_v(
        u32                            xfb,
        group::transform_feedback_prop pname,
        u32                            index,
        span_i32                       param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbacki_v)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTransformFeedback)
            glIsTransformFeedback(xfb);
#endif
    }
    glGetTransformFeedbacki_v(
        xfb,
        static_cast<GLenum>(pname),
        index,
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetTransformFeedbacki_v"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetTransformFeedbackiv. Introduced in GL core 4.5
     * \param xfb GLuint
     * \param pname GLenum
     * \param param GLint *
     * \return void
     */
    STATICINLINE void get_transform_feedbackiv(
        u32 xfb, group::transform_feedback_prop pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbackiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTransformFeedback)
            glIsTransformFeedback(xfb);
#endif
    }
    glGetTransformFeedbackiv(
        xfb,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetTransformFeedbackiv"sv);
}

template<class span_i64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>)
    /*!
     * \brief Wraps around glGetVertexArrayIndexed64iv. Introduced in GL
     * core 4.5 \param vaobj GLuint \param index GLuint \param pname GLenum
     * \param param GLint64 *
     * \return void
     */
    STATICINLINE void get_vertex_array_indexed64iv(
        u32 vaobj, u32 index, group::vertex_array_prop pname, span_i64 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayIndexed64iv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glGetVertexArrayIndexed64iv(
        vaobj,
        index,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint64*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayIndexed64iv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetVertexArrayIndexediv. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param index GLuint
     * \param pname GLenum
     * \param param GLint *
     * \return void
     */
    STATICINLINE void get_vertex_array_indexediv(
        u32 vaobj, u32 index, group::vertex_array_prop pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayIndexediv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glGetVertexArrayIndexediv(
        vaobj,
        index,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayIndexediv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetVertexArrayiv. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param pname GLenum
     * \param param GLint *
     * \return void
     */
    STATICINLINE void get_vertex_arrayiv(
        u32 vaobj, group::vertex_array_prop pname, span_i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexArrayiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glGetVertexArrayiv(
        vaobj,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<GLint*>(param.data()) : nullptr);
    detail::error_check("GetVertexArrayiv"sv);
}

template<class span_const_framebuffer_attachment>
requires(MinimumVersion<Current, Version<4, 5>>&&      semantic::concepts::
             Span<span_const_framebuffer_attachment>&& std::is_same_v<
                 std::decay_t<
                     typename span_const_framebuffer_attachment::value_type>,
                 std::decay_t<group::framebuffer_attachment>>)
    /*!
     * \brief Wraps around glInvalidateNamedFramebufferData. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param numAttachments GLsizei \param
     * attachments const GLenum * \return void
     */
    STATICINLINE void invalidate_named_framebuffer_data(
        u32 framebuffer, span_const_framebuffer_attachment const& attachments)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateNamedFramebufferData)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glInvalidateNamedFramebufferData(
        framebuffer,
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr);
    detail::error_check("InvalidateNamedFramebufferData"sv);
}

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
                     semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glInvalidateNamedFramebufferSubData. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param numAttachments GLsizei \param
     * attachments const GLenum * \param x GLint \param y GLint \param width
     * GLsizei \param height GLsizei \return void
     */
    STATICINLINE void invalidate_named_framebuffer_sub_data(
        u32                                      framebuffer,
        span_const_framebuffer_attachment const& attachments,
        vec_2_i32 const&                         x,
        size_2_i32 const&                        width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateNamedFramebufferSubData)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glInvalidateNamedFramebufferSubData(
        framebuffer,
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr,
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("InvalidateNamedFramebufferSubData"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glMapNamedBuffer. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param access GLenum
     * \return void *
     */
    STATICINLINE
    void* map_named_buffer(u32 buffer, group::buffer_access_arb access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapNamedBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glMapNamedBuffer(buffer, static_cast<GLenum>(access));
    detail::error_check("MapNamedBuffer"sv);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glMapNamedBufferRange. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param offset GLintptr
     * \param length GLsizeiptr
     * \param access GLbitfield
     * \return void *
     */
    STATICINLINE void* map_named_buffer_range(
        u32                           buffer,
        GLintptr                      offset,
        GLsizeiptr                    length,
        group::map_buffer_access_mask access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapNamedBufferRange)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glMapNamedBufferRange(
        buffer, offset, length, static_cast<GLenum>(access));
    detail::error_check("MapNamedBufferRange"sv);
    return out;
}

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glNamedBufferData. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param size GLsizeiptr
     * \param data const void *
     * \param usage GLenum
     * \return void
     */
    STATICINLINE void named_buffer_data(
        u32                               buffer,
        span_const_void const&            data,
        group::vertex_buffer_object_usage usage)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferData)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferData(
        buffer,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(usage));
    detail::error_check("NamedBufferData"sv);
}

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glNamedBufferStorage. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param size GLsizeiptr
     * \param data const void *
     * \param flags GLbitfield
     * \return void
     */
    STATICINLINE void named_buffer_storage(
        u32                        buffer,
        span_const_void const&     data,
        group::buffer_storage_mask flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferStorage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferStorage(
        buffer,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(flags));
    detail::error_check("NamedBufferStorage"sv);
}

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glNamedBufferSubData. Introduced in GL core 4.5
     * \param buffer GLuint
     * \param offset GLintptr
     * \param size GLsizeiptr
     * \param data const void *
     * \return void
     */
    STATICINLINE void named_buffer_sub_data(
        u32 buffer, GLintptr offset, span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedBufferSubData)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glNamedBufferSubData(
        buffer,
        offset,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("NamedBufferSubData"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glNamedFramebufferDrawBuffer. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param buf GLenum \return void
     */
    STATICINLINE
    void named_framebuffer_draw_buffer(u32 framebuffer, group::color_buffer buf)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferDrawBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glNamedFramebufferDrawBuffer(framebuffer, static_cast<GLenum>(buf));
    detail::error_check("NamedFramebufferDrawBuffer"sv);
}

template<class span_const_color_buffer>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_color_buffer>&& std::is_same_v<
                 std::decay_t<typename span_const_color_buffer::value_type>,
                 std::decay_t<group::color_buffer>>)
    /*!
     * \brief Wraps around glNamedFramebufferDrawBuffers. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param n GLsizei \param bufs const
     * GLenum * \return void
     */
    STATICINLINE void named_framebuffer_draw_buffers(
        u32 framebuffer, span_const_color_buffer const& bufs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferDrawBuffers)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glNamedFramebufferDrawBuffers(
        framebuffer,
        bufs.size(),
        bufs.size() ? reinterpret_cast<const GLenum*>(bufs.data()) : nullptr);
    detail::error_check("NamedFramebufferDrawBuffers"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glNamedFramebufferParameteri. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param pname GLenum \param param GLint
     * \return void
     */
    STATICINLINE void named_framebuffer_parameter(
        u32 framebuffer, group::framebuffer_parameter_name pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferParameteri)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glNamedFramebufferParameteri(
        framebuffer, static_cast<GLenum>(pname), param);
    detail::error_check("NamedFramebufferParameteri"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glNamedFramebufferReadBuffer. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param src GLenum \return void
     */
    STATICINLINE
    void named_framebuffer_read_buffer(u32 framebuffer, group::color_buffer src)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferReadBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glNamedFramebufferReadBuffer(framebuffer, static_cast<GLenum>(src));
    detail::error_check("NamedFramebufferReadBuffer"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glNamedFramebufferRenderbuffer. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param attachment GLenum \param
     * renderbuffertarget GLenum \param renderbuffer GLuint \return void
     */
    STATICINLINE void named_framebuffer_renderbuffer(
        u32                           framebuffer,
        group::framebuffer_attachment attachment,
        group::renderbuffer_target    renderbuffertarget,
        u32                           renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferRenderbuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glNamedFramebufferRenderbuffer(
        framebuffer,
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("NamedFramebufferRenderbuffer"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glNamedFramebufferTexture. Introduced in GL core 4.5
     * \param framebuffer GLuint
     * \param attachment GLenum
     * \param texture GLuint
     * \param level GLint
     * \return void
     */
    STATICINLINE void named_framebuffer_texture(
        u32                           framebuffer,
        group::framebuffer_attachment attachment,
        u32                           texture,
        i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferTexture)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glNamedFramebufferTexture(
        framebuffer, static_cast<GLenum>(attachment), texture, level);
    detail::error_check("NamedFramebufferTexture"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glNamedFramebufferTextureLayer. Introduced in GL
     * core 4.5 \param framebuffer GLuint \param attachment GLenum \param
     * texture GLuint \param level GLint \param layer GLint \return void
     */
    STATICINLINE void named_framebuffer_texture_layer(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glNamedFramebufferTextureLayer(
        framebuffer, static_cast<GLenum>(attachment), texture, level, layer);
    detail::error_check("NamedFramebufferTextureLayer"sv);
}

template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glNamedRenderbufferStorage. Introduced in GL core 4.5
     * \param renderbuffer GLuint
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void named_renderbuffer_storage(
        u32                    renderbuffer,
        group::internal_format internalformat,
        size_2_i32 const&      width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glNamedRenderbufferStorage(
        renderbuffer, static_cast<GLenum>(internalformat), width[0], width[1]);
    detail::error_check("NamedRenderbufferStorage"sv);
}

template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glNamedRenderbufferStorageMultisample. Introduced in
     * GL core 4.5 \param renderbuffer GLuint \param samples GLsizei \param
     * internalformat GLenum \param width GLsizei \param height GLsizei \return
     * void
     */
    STATICINLINE void named_renderbuffer_storage_multisample(
        u32                    renderbuffer,
        i32                    samples,
        group::internal_format internalformat,
        size_2_i32 const&      width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageMultisample)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glNamedRenderbufferStorageMultisample(
        renderbuffer,
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("NamedRenderbufferStorageMultisample"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glTextureBuffer. Introduced in GL core 4.5
     * \param texture GLuint
     * \param internalformat GLenum
     * \param buffer GLuint
     * \return void
     */
    STATICINLINE void texture_buffer(
        u32 texture, group::sized_internal_format internalformat, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTextureBuffer(texture, static_cast<GLenum>(internalformat), buffer);
    detail::error_check("TextureBuffer"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glTextureBufferRange. Introduced in GL core 4.5
     * \param texture GLuint
     * \param internalformat GLenum
     * \param buffer GLuint
     * \param offset GLintptr
     * \param size GLsizeiptr
     * \return void
     */
    STATICINLINE void texture_buffer_range(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTextureBufferRange(
        texture, static_cast<GLenum>(internalformat), buffer, offset, size);
    detail::error_check("TextureBufferRange"sv);
}

template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glTextureParameterIiv. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param params const GLint *
     * \return void
     */
    STATICINLINE void texture_parameter_iiv(
        u32                           texture,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterIiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterIiv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterIiv"sv);
}

template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glTextureParameterIuiv. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param params const GLuint *
     * \return void
     */
    STATICINLINE void texture_parameter_iuiv(
        u32                           texture,
        group::texture_parameter_name pname,
        span_const_u32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterIuiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterIuiv(
        texture,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TextureParameterIuiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glTextureParameterf. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param param GLfloat
     * \return void
     */
    STATICINLINE void texture_parameter(
        u32 texture, group::texture_parameter_name pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterf)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterf(texture, static_cast<GLenum>(pname), param);
    detail::error_check("TextureParameterf"sv);
}

template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glTextureParameterfv. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param param const GLfloat *
     * \return void
     */
    STATICINLINE void texture_parameter(
        u32                           texture,
        group::texture_parameter_name pname,
        span_const_f32 const&         param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameterfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameterfv(
        texture,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfloat*>(param.data())
                     : nullptr);
    detail::error_check("TextureParameterfv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glTextureParameteri. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param param GLint
     * \return void
     */
    STATICINLINE void texture_parameter(
        u32 texture, group::texture_parameter_name pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameteri)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameteri(texture, static_cast<GLenum>(pname), param);
    detail::error_check("TextureParameteri"sv);
}

template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glTextureParameteriv. Introduced in GL core 4.5
     * \param texture GLuint
     * \param pname GLenum
     * \param param const GLint *
     * \return void
     */
    STATICINLINE void texture_parameter(
        u32                           texture,
        group::texture_parameter_name pname,
        span_const_i32 const&         param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureParameteriv(
        texture,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("TextureParameteriv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glTextureStorage1D. Introduced in GL core 4.5
     * \param texture GLuint
     * \param levels GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \return void
     */
    STATICINLINE void texture_storage_1d(
        u32                          texture,
        i32                          levels,
        group::sized_internal_format internalformat,
        i32                          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage1D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage1D(
        texture, levels, static_cast<GLenum>(internalformat), width);
    detail::error_check("TextureStorage1D"sv);
}

template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glTextureStorage2D. Introduced in GL core 4.5
     * \param texture GLuint
     * \param levels GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void texture_storage_2d(
        u32                          texture,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage2D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glTextureStorage2D(
        texture,
        levels,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("TextureStorage2D"sv);
}

template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glTextureStorage2DMultisample. Introduced in GL
     * core 4.5 \param texture GLuint \param samples GLsizei \param
     * internalformat GLenum \param width GLsizei \param height GLsizei \param
     * fixedsamplelocations GLboolean \return void
     */
    STATICINLINE void texture_storage_2d_multisample(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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

template<class size_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_3_i32, i32>)
    /*!
     * \brief Wraps around glTextureStorage3D. Introduced in GL core 4.5
     * \param texture GLuint
     * \param levels GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \return void
     */
    STATICINLINE void texture_storage_3d(
        u32                          texture,
        i32                          levels,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureStorage3D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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

template<class size_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Size2D<size_3_i32, i32>)
    /*!
     * \brief Wraps around glTextureStorage3DMultisample. Introduced in GL
     * core 4.5 \param texture GLuint \param samples GLsizei \param
     * internalformat GLenum \param width GLsizei \param height GLsizei \param
     * depth GLsizei \param fixedsamplelocations GLboolean \return void
     */
    STATICINLINE void texture_storage_3d_multisample(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glTextureSubImage1D. Introduced in GL core 4.5
     * \param texture GLuint
     * \param level GLint
     * \param xoffset GLint
     * \param width GLsizei
     * \param format GLenum
     * \param type GLenum
     * \param pixels const void *
     * \return void
     */
    STATICINLINE void texture_sub_image_1d(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glTextureSubImage2D. Introduced in GL core 4.5
     * \param texture GLuint
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param format GLenum
     * \param type GLenum
     * \param pixels const void *
     * \return void
     */
    STATICINLINE void texture_sub_image_2d(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Wraps around glTextureSubImage3D. Introduced in GL core 4.5
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
     * \param pixels const void *
     * \return void
     */
    STATICINLINE void texture_sub_image_3d(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glTransformFeedbackBufferBase. Introduced in GL
     * core 4.5 \param xfb GLuint \param index GLuint \param buffer GLuint
     * \return void
     */
    STATICINLINE
    void transform_feedback_buffer_base(u32 xfb, u32 index, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackBufferBase)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTransformFeedback)
            glIsTransformFeedback(xfb);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTransformFeedbackBufferBase(xfb, index, buffer);
    detail::error_check("TransformFeedbackBufferBase"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glTransformFeedbackBufferRange. Introduced in GL
     * core 4.5 \param xfb GLuint \param index GLuint \param buffer GLuint
     * \param offset GLintptr
     * \param size GLsizeiptr
     * \return void
     */
    STATICINLINE void transform_feedback_buffer_range(
        u32 xfb, u32 index, u32 buffer, GLintptr offset, GLsizeiptr size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackBufferRange)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTransformFeedback)
            glIsTransformFeedback(xfb);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glTransformFeedbackBufferRange(xfb, index, buffer, offset, size);
    detail::error_check("TransformFeedbackBufferRange"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glUnmapNamedBuffer. Introduced in GL core 4.5
     * \param buffer GLuint
     * \return Boolean
     */
    STATICINLINE GLboolean unmap_named_buffer(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UnmapNamedBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glUnmapNamedBuffer(buffer);
    detail::error_check("UnmapNamedBuffer"sv);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glVertexArrayAttribBinding. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param attribindex GLuint
     * \param bindingindex GLuint
     * \return void
     */
    STATICINLINE void vertex_array_attrib_binding(
        u32 vaobj, u32 attribindex, u32 bindingindex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayAttribBinding)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayAttribBinding(vaobj, attribindex, bindingindex);
    detail::error_check("VertexArrayAttribBinding"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glVertexArrayAttribFormat. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param attribindex GLuint
     * \param size GLint
     * \param type GLenum
     * \param normalized GLboolean
     * \param relativeoffset GLuint
     * \return void
     */
    STATICINLINE void vertex_array_attrib_format(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glVertexArrayAttribIFormat. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param attribindex GLuint
     * \param size GLint
     * \param type GLenum
     * \param relativeoffset GLuint
     * \return void
     */
    STATICINLINE void vertex_array_attrib_i_format(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayAttribIFormat(
        vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexArrayAttribIFormat"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glVertexArrayAttribLFormat. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param attribindex GLuint
     * \param size GLint
     * \param type GLenum
     * \param relativeoffset GLuint
     * \return void
     */
    STATICINLINE void vertex_array_attrib_l_format(
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayAttribLFormat(
        vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexArrayAttribLFormat"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glVertexArrayBindingDivisor. Introduced in GL
     * core 4.5 \param vaobj GLuint \param bindingindex GLuint \param divisor
     * GLuint \return void
     */
    STATICINLINE
    void vertex_array_binding_divisor(u32 vaobj, u32 bindingindex, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayBindingDivisor)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayBindingDivisor(vaobj, bindingindex, divisor);
    detail::error_check("VertexArrayBindingDivisor"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glVertexArrayElementBuffer. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param buffer GLuint
     * \return void
     */
    STATICINLINE void vertex_array_element_buffer(u32 vaobj, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayElementBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayElementBuffer(vaobj, buffer);
    detail::error_check("VertexArrayElementBuffer"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glVertexArrayVertexBuffer. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param bindingindex GLuint
     * \param buffer GLuint
     * \param offset GLintptr
     * \param stride GLsizei
     * \return void
     */
    STATICINLINE void vertex_array_vertex_buffer(
        u32 vaobj, u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glVertexArrayVertexBuffer(vaobj, bindingindex, buffer, offset, stride);
    detail::error_check("VertexArrayVertexBuffer"sv);
}

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
                         std::decay_t<i32>>)
    /*!
     * \brief Wraps around glVertexArrayVertexBuffers. Introduced in GL core 4.5
     * \param vaobj GLuint
     * \param first GLuint
     * \param count GLsizei
     * \param buffers const GLuint *
     * \param offsets const GLintptr *
     * \param strides const GLsizei *
     * \return void
     */
    STATICINLINE void vertex_array_vertex_buffers(
        u32                        vaobj,
        u32                        first,
        span_const_u32 const&      buffers,
        span_const_GLintptr const& offsets,
        span_const_i32 const&      strides)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexArrayVertexBuffers)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(vaobj);
#endif
    }
    glVertexArrayVertexBuffers(
        vaobj,
        first,
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr,
        offsets.size() ? reinterpret_cast<const GLintptr*>(offsets.data())
                       : nullptr,
        strides.size() ? reinterpret_cast<const GLsizei*>(strides.data())
                       : nullptr);
    detail::error_check("VertexArrayVertexBuffers"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glMemoryBarrierByRegion. Introduced in GL core 4.5
     * \param barriers GLbitfield
     * \return void
     */
    STATICINLINE
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

template<class size_3_i32, class span_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetCompressedTextureSubImage. Introduced in GL
     * core 4.5 \param texture GLuint \param level GLint \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \param bufSize GLsizei
     * \param pixels void *
     * \return void
     */
    STATICINLINE void get_compressed_texture_sub_image(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width,
        span_void         pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTextureSubImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
        pixels.size() * sizeof(typename std::decay_t<span_void>::value_type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetCompressedTextureSubImage"sv);
}

template<class size_3_i32, class span_void, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                     semantic::concepts::Size2D<size_3_i32, i32>&&
                     semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetTextureSubImage. Introduced in GL core 4.5
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
     * \param bufSize GLsizei
     * \param pixels void *
     * \return void
     */
    STATICINLINE void get_texture_sub_image(
        u32                 texture,
        i32                 level,
        vec_3_i32 const&    xoffset,
        size_3_i32 const&   width,
        group::pixel_format format,
        group::pixel_type   type,
        span_void           pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTextureSubImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
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
        pixels.size() * sizeof(typename std::decay_t<span_void>::value_type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTextureSubImage"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glGetGraphicsResetStatus. Introduced in GL core 4.5

     * \return GraphicsResetStatus
     */
    STATICINLINE GLenum get_graphics_reset_status()
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

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetnCompressedTexImage. Introduced in GL core 4.5
     * \param target GLenum
     * \param lod GLint
     * \param bufSize GLsizei
     * \param pixels void *
     * \return void
     */
    STATICINLINE void getn_compressed_tex_image(
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

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetnTexImage. Introduced in GL core 4.5
     * \param target GLenum
     * \param level GLint
     * \param format GLenum
     * \param type GLenum
     * \param bufSize GLsizei
     * \param pixels void *
     * \return void
     */
    STATICINLINE void getn_tex_image(
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

template<class span_f64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>)
    /*!
     * \brief Wraps around glGetnUniformdv. Introduced in GL core 4.5
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLdouble *
     * \return void
     */
    STATICINLINE
    void getn_uniformdv(u32 program, i32 location, i32 bufSize, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformdv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformdv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetnUniformdv"sv);
}

template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glGetnUniformfv. Introduced in GL core 4.5
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE
    void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformfv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetnUniformfv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetnUniformiv. Introduced in GL core 4.5
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLint *
     * \return void
     */
    STATICINLINE
    void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformiv"sv);
}

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glGetnUniformuiv. Introduced in GL core 4.5
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void getn_uniformuiv(
        u32 program, i32 location, i32 bufSize, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformuiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformuiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformuiv"sv);
}

template<class size_2_i32, class span_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glReadnPixels. Introduced in GL core 4.5
     * \param x GLint
     * \param y GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param format GLenum
     * \param type GLenum
     * \param bufSize GLsizei
     * \param data void *
     * \return void
     */
    STATICINLINE void readn_pixels(
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

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetnColorTable. Introduced in GL core 4.5
     * \param target GLenum
     * \param format GLenum
     * \param type GLenum
     * \param bufSize GLsizei
     * \param table void *
     * \return void
     */
    STATICINLINE void getn_color_table(
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

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetnConvolutionFilter. Introduced in GL core 4.5
     * \param target GLenum
     * \param format GLenum
     * \param type GLenum
     * \param bufSize GLsizei
     * \param image void *
     * \return void
     */
    STATICINLINE void getn_convolution_filter(
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

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetnHistogram. Introduced in GL core 4.5
     * \param target GLenum
     * \param reset GLboolean
     * \param format GLenum
     * \param type GLenum
     * \param bufSize GLsizei
     * \param values void *
     * \return void
     */
    STATICINLINE void getn_histogram(
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

template<class span_f64>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>)
    /*!
     * \brief Wraps around glGetnMapdv. Introduced in GL core 4.5
     * \param target GLenum
     * \param query GLenum
     * \param bufSize GLsizei
     * \param v GLdouble *
     * \return void
     */
    STATICINLINE void getn_mapdv(
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

template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glGetnMapfv. Introduced in GL core 4.5
     * \param target GLenum
     * \param query GLenum
     * \param bufSize GLsizei
     * \param v GLfloat *
     * \return void
     */
    STATICINLINE void getn_mapfv(
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

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetnMapiv. Introduced in GL core 4.5
     * \param target GLenum
     * \param query GLenum
     * \param bufSize GLsizei
     * \param v GLint *
     * \return void
     */
    STATICINLINE void getn_mapiv(
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

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetnMinmax. Introduced in GL core 4.5
     * \param target GLenum
     * \param reset GLboolean
     * \param format GLenum
     * \param type GLenum
     * \param bufSize GLsizei
     * \param values void *
     * \return void
     */
    STATICINLINE void getn_minmax(
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

template<class span_f32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glGetnPixelMapfv. Introduced in GL core 4.5
     * \param map GLenum
     * \param bufSize GLsizei
     * \param values GLfloat *
     * \return void
     */
    STATICINLINE
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

template<class span_u32>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glGetnPixelMapuiv. Introduced in GL core 4.5
     * \param map GLenum
     * \param bufSize GLsizei
     * \param values GLuint *
     * \return void
     */
    STATICINLINE
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

template<class span_u16>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u16>&& std::is_same_v<
                 std::decay_t<typename span_u16::value_type>,
                 std::decay_t<u16>>)
    /*!
     * \brief Wraps around glGetnPixelMapusv. Introduced in GL core 4.5
     * \param map GLenum
     * \param bufSize GLsizei
     * \param values GLushort *
     * \return void
     */
    STATICINLINE
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

template<class span_u8>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_u8>&& std::is_same_v<
                 std::decay_t<typename span_u8::value_type>,
                 std::decay_t<u8>>)
    /*!
     * \brief Wraps around glGetnPolygonStipple. Introduced in GL core 4.5
     * \param bufSize GLsizei
     * \param pattern GLubyte *
     * \return void
     */
    STATICINLINE void getn_polygon_stipple(span_u8 pattern)
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

template<class span_void>
requires(MinimumVersion<Current, Version<4, 5>>&&
             semantic::concepts::Span<span_void>)
    /*!
     * \brief Wraps around glGetnSeparableFilter. Introduced in GL core 4.5
     * \param target GLenum
     * \param format GLenum
     * \param type GLenum
     * \param rowBufSize GLsizei
     * \param row void *
     * \param columnBufSize GLsizei
     * \param column void *
     * \param span void *
     * \return void
     */
    STATICINLINE void getn_separable_filter(
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 5>>)
    /*!
     * \brief Wraps around glTextureBarrier. Introduced in GL core 4.5

     * \return void
     */
    STATICINLINE void texture_barrier()
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
