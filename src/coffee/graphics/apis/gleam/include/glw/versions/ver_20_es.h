#ifdef GL_ES_VERSION_2_0
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glActiveTexture. Introduced in GL es 2.0
 * \param texture GLenum
 * \return void
 */
STATICINLINE void active_texture(
    group::texture_unit texture, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ActiveTexture)
    }
    glActiveTexture(static_cast<GLenum>(texture));
    detail::error_check("ActiveTexture"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glAttachShader. Introduced in GL es 2.0
 * \param program GLuint
 * \param shader GLuint
 * \return void
 */
STATICINLINE void attach_shader(
    u32 program, u32 shader, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AttachShader)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    glAttachShader(program, shader);
    detail::error_check("AttachShader"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBindAttribLocation. Introduced in GL es 2.0
 * \param program GLuint
 * \param index GLuint
 * \param name const GLchar *
 * \return void
 */
STATICINLINE void bind_attrib_location(
    u32                     program,
    u32                     index,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindAttribLocation)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glBindAttribLocation(program, index, name.data());
    detail::error_check("BindAttribLocation"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBindBuffer. Introduced in GL es 2.0
 * \param target GLenum
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void bind_buffer(
    group::buffer_target_arb target,
    u32                      buffer,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glBindBuffer(static_cast<GLenum>(target), buffer);
    detail::error_check("BindBuffer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBindFramebuffer. Introduced in GL es 2.0
 * \param target GLenum
 * \param framebuffer GLuint
 * \return void
 */
STATICINLINE void bind_framebuffer(
    group::framebuffer_target target,
    u32                       framebuffer,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFramebuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
    detail::error_check("BindFramebuffer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBindRenderbuffer. Introduced in GL es 2.0
 * \param target GLenum
 * \param renderbuffer GLuint
 * \return void
 */
STATICINLINE void bind_renderbuffer(
    group::renderbuffer_target target,
    u32                        renderbuffer,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindRenderbuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glBindRenderbuffer(static_cast<GLenum>(target), renderbuffer);
    detail::error_check("BindRenderbuffer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBindTexture. Introduced in GL es 2.0
 * \param target GLenum
 * \param texture GLuint
 * \return void
 */
STATICINLINE void bind_texture(
    group::texture_target target,
    u32                   texture,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTexture)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindTexture(static_cast<GLenum>(target), texture);
    detail::error_check("BindTexture"sv, check_errors);
}

template<class vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Wraps around glBlendColor. Introduced in GL es 2.0
 * \param red GLfloat
 * \param green GLfloat
 * \param blue GLfloat
 * \param alpha GLfloat
 * \return void
 */
STATICINLINE void blend_color(
    vec_4_f32 const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColor)
    }
    glBlendColor(red[0], red[1], red[2], red[3]);
    detail::error_check("BlendColor"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBlendEquation. Introduced in GL es 2.0
 * \param mode GLenum
 * \return void
 */
STATICINLINE void blend_equation(
    group::blend_equation_mode_ext mode,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquation)
    }
    glBlendEquation(static_cast<GLenum>(mode));
    detail::error_check("BlendEquation"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBlendEquationSeparate. Introduced in GL es 2.0
 * \param modeRGB GLenum
 * \param modeAlpha GLenum
 * \return void
 */
STATICINLINE void blend_equation_separate(
    group::blend_equation_mode_ext modeRGB,
    group::blend_equation_mode_ext modeAlpha,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparate)
    }
    glBlendEquationSeparate(
        static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparate"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBlendFunc. Introduced in GL es 2.0
 * \param sfactor GLenum
 * \param dfactor GLenum
 * \return void
 */
STATICINLINE void blend_func(
    group::blending_factor sfactor,
    group::blending_factor dfactor,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFunc)
    }
    glBlendFunc(static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor));
    detail::error_check("BlendFunc"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glBlendFuncSeparate. Introduced in GL es 2.0
 * \param sfactorRGB GLenum
 * \param dfactorRGB GLenum
 * \param sfactorAlpha GLenum
 * \param dfactorAlpha GLenum
 * \return void
 */
STATICINLINE void blend_func_separate(
    group::blending_factor sfactorRGB,
    group::blending_factor dfactorRGB,
    group::blending_factor sfactorAlpha,
    group::blending_factor dfactorAlpha,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncSeparate)
    }
    glBlendFuncSeparate(
        static_cast<GLenum>(sfactorRGB),
        static_cast<GLenum>(dfactorRGB),
        static_cast<GLenum>(sfactorAlpha),
        static_cast<GLenum>(dfactorAlpha));
    detail::error_check("BlendFuncSeparate"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glBufferData. Introduced in GL es 2.0
 * \param target GLenum
 * \param size GLsizeiptr
 * \param data const void *
 * \param usage GLenum
 * \return void
 */
STATICINLINE void buffer_data(
    group::buffer_target_arb target,
    span_const_void const&   data,
    group::buffer_usage_arb  usage,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferData)
    }
    glBufferData(
        static_cast<GLenum>(target),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr,
        static_cast<GLenum>(usage));
    detail::error_check("BufferData"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glBufferSubData. Introduced in GL es 2.0
 * \param target GLenum
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \param data const void *
 * \return void
 */
STATICINLINE void buffer_sub_data(
    group::buffer_target_arb target,
    GLintptr                 offset,
    span_const_void const&   data,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BufferSubData)
    }
    glBufferSubData(
        static_cast<GLenum>(target),
        offset,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("BufferSubData"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glCheckFramebufferStatus. Introduced in GL es 2.0
 * \param target GLenum
 * \return FramebufferStatus
 */
STATICINLINE group::framebuffer_status check_framebuffer_status(
    group::framebuffer_target target,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckFramebufferStatus)
    }
    auto out = glCheckFramebufferStatus(static_cast<GLenum>(target));
    detail::error_check("CheckFramebufferStatus"sv, check_errors);
    return static_cast<group::framebuffer_status>(out);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glClear. Introduced in GL es 2.0
 * \param mask GLbitfield
 * \return void
 */
STATICINLINE void clear(
    group::clear_buffer_mask mask, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Clear)
    }
    glClear(static_cast<GLenum>(mask));
    detail::error_check("Clear"sv, check_errors);
}

template<class vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Wraps around glClearColor. Introduced in GL es 2.0
 * \param red GLfloat
 * \param green GLfloat
 * \param blue GLfloat
 * \param alpha GLfloat
 * \return void
 */
STATICINLINE void clear_color(
    vec_4_f32 const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearColor)
    }
    glClearColor(red[0], red[1], red[2], red[3]);
    detail::error_check("ClearColor"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glClearDepthf. Introduced in GL es 2.0
 * \param d GLfloat
 * \return void
 */
STATICINLINE void clear_depthf(
    f32 d, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthf)
    }
    glClearDepthf(d);
    detail::error_check("ClearDepthf"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glClearStencil. Introduced in GL es 2.0
 * \param s GLint
 * \return void
 */
STATICINLINE void clear_stencil(
    i32 s, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearStencil)
    }
    glClearStencil(s);
    detail::error_check("ClearStencil"sv, check_errors);
}

template<class vec_4_bool>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_4_bool, bool, 4>)
/*!
 * \brief Wraps around glColorMask. Introduced in GL es 2.0
 * \param red GLboolean
 * \param green GLboolean
 * \param blue GLboolean
 * \param alpha GLboolean
 * \return void
 */
STATICINLINE void color_mask(
    vec_4_bool const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorMask)
    }
    glColorMask(red[0], red[1], red[2], red[3]);
    detail::error_check("ColorMask"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glCompileShader. Introduced in GL es 2.0
 * \param shader GLuint
 * \return void
 */
STATICINLINE void compile_shader(
    u32 shader, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompileShader)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    glCompileShader(shader);
    detail::error_check("CompileShader"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glCompressedTexImage2D. Introduced in GL es 2.0
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_image_2d(
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    size_2_i32 const&      width,
    i32                    border,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage2D)
    }
    glCompressedTexImage2D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage2D"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Wraps around glCompressedTexSubImage2D. Introduced in GL es 2.0
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_sub_image_2d(
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::internal_format format,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage2D)
    }
    glCompressedTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage2D"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glCopyTexImage2D. Introduced in GL es 2.0
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \return void
 */
STATICINLINE void copy_tex_image_2d(
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    vec_2_i32 const&       x,
    size_2_i32 const&      width,
    i32                    border,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexImage2D)
    }
    glCopyTexImage2D(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        x[0],
        x[1],
        width[0],
        width[1],
        border);
    detail::error_check("CopyTexImage2D"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glCopyTexSubImage2D. Introduced in GL es 2.0
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void copy_tex_sub_image_2d(
    group::texture_target target,
    i32                   level,
    vec_2_i32 const&      xoffset,
    vec_2_i32 const&      x,
    size_2_i32 const&     width,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage2D)
    }
    glCopyTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage2D"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glCreateProgram. Introduced in GL es 2.0

 * \return GLuint
 */
STATICINLINE GLuint create_program(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateProgram)
    }
    auto out = glCreateProgram();
    detail::error_check("CreateProgram"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glCreateShader. Introduced in GL es 2.0
 * \param type GLenum
 * \return GLuint
 */
STATICINLINE GLuint create_shader(
    group::shader_type type, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateShader)
    }
    auto out = glCreateShader(static_cast<GLenum>(type));
    detail::error_check("CreateShader"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glCullFace. Introduced in GL es 2.0
 * \param mode GLenum
 * \return void
 */
STATICINLINE void cull_face(
    group::triangle_face mode, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CullFace)
    }
    glCullFace(static_cast<GLenum>(mode));
    detail::error_check("CullFace"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteBuffers. Introduced in GL es 2.0
 * \param n GLsizei
 * \param buffers const GLuint *
 * \return void
 */
STATICINLINE void delete_buffers(
    span_const_u32 const& buffers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteBuffers)
    }
    glDeleteBuffers(
        buffers.size(),
        buffers.size() ? reinterpret_cast<const GLuint*>(buffers.data())
                       : nullptr);
    detail::error_check("DeleteBuffers"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteFramebuffers. Introduced in GL es 2.0
 * \param n GLsizei
 * \param framebuffers const GLuint *
 * \return void
 */
STATICINLINE void delete_framebuffers(
    span_const_u32 const& framebuffers,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteFramebuffers)
    }
    glDeleteFramebuffers(
        framebuffers.size(),
        framebuffers.size()
            ? reinterpret_cast<const GLuint*>(framebuffers.data())
            : nullptr);
    detail::error_check("DeleteFramebuffers"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDeleteProgram. Introduced in GL es 2.0
 * \param program GLuint
 * \return void
 */
STATICINLINE void delete_program(
    u32 program, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteProgram)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glDeleteProgram(program);
    detail::error_check("DeleteProgram"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteRenderbuffers. Introduced in GL es 2.0
 * \param n GLsizei
 * \param renderbuffers const GLuint *
 * \return void
 */
STATICINLINE void delete_renderbuffers(
    span_const_u32 const& renderbuffers,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteRenderbuffers)
    }
    glDeleteRenderbuffers(
        renderbuffers.size(),
        renderbuffers.size()
            ? reinterpret_cast<const GLuint*>(renderbuffers.data())
            : nullptr);
    detail::error_check("DeleteRenderbuffers"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDeleteShader. Introduced in GL es 2.0
 * \param shader GLuint
 * \return void
 */
STATICINLINE void delete_shader(
    u32 shader, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteShader)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    glDeleteShader(shader);
    detail::error_check("DeleteShader"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteTextures. Introduced in GL es 2.0
 * \param n GLsizei
 * \param textures const GLuint *
 * \return void
 */
STATICINLINE void delete_textures(
    span_const_u32 const& textures, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTextures)
    }
    glDeleteTextures(
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr);
    detail::error_check("DeleteTextures"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDepthFunc. Introduced in GL es 2.0
 * \param func GLenum
 * \return void
 */
STATICINLINE void depth_func(
    group::depth_function func, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthFunc)
    }
    glDepthFunc(static_cast<GLenum>(func));
    detail::error_check("DepthFunc"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDepthMask. Introduced in GL es 2.0
 * \param flag GLboolean
 * \return void
 */
STATICINLINE void depth_mask(
    bool flag, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthMask)
    }
    glDepthMask(flag);
    detail::error_check("DepthMask"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDepthRangef. Introduced in GL es 2.0
 * \param n GLfloat
 * \param f GLfloat
 * \return void
 */
STATICINLINE void depth_rangef(
    f32 n, f32 f, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangef)
    }
    glDepthRangef(n, f);
    detail::error_check("DepthRangef"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDetachShader. Introduced in GL es 2.0
 * \param program GLuint
 * \param shader GLuint
 * \return void
 */
STATICINLINE void detach_shader(
    u32 program, u32 shader, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DetachShader)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    glDetachShader(program, shader);
    detail::error_check("DetachShader"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDisable. Introduced in GL es 2.0
 * \param cap GLenum
 * \return void
 */
STATICINLINE void disable(
    group::enable_cap cap, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Disable)
    }
    glDisable(static_cast<GLenum>(cap));
    detail::error_check("Disable"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDisableVertexAttribArray. Introduced in GL es 2.0
 * \param index GLuint
 * \return void
 */
STATICINLINE void disable_vertex_attrib_array(
    u32 index, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexAttribArray)
    }
    glDisableVertexAttribArray(index);
    detail::error_check("DisableVertexAttribArray"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDrawArrays. Introduced in GL es 2.0
 * \param mode GLenum
 * \param first GLint
 * \param count GLsizei
 * \return void
 */
STATICINLINE void draw_arrays(
    group::primitive_type mode,
    i32                   first,
    i32                   count,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArrays)
    }
    glDrawArrays(static_cast<GLenum>(mode), first, count);
    detail::error_check("DrawArrays"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glDrawElements. Introduced in GL es 2.0
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \return void
 */
STATICINLINE void draw_elements(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElements)
    }
    glDrawElements(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices));
    detail::error_check("DrawElements"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glEnable. Introduced in GL es 2.0
 * \param cap GLenum
 * \return void
 */
STATICINLINE void enable(
    group::enable_cap cap, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Enable)
    }
    glEnable(static_cast<GLenum>(cap));
    detail::error_check("Enable"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glEnableVertexAttribArray. Introduced in GL es 2.0
 * \param index GLuint
 * \return void
 */
STATICINLINE void enable_vertex_attrib_array(
    u32 index, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexAttribArray)
    }
    glEnableVertexAttribArray(index);
    detail::error_check("EnableVertexAttribArray"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glFinish. Introduced in GL es 2.0

 * \return void
 */
STATICINLINE void finish(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Finish)
    }
    glFinish();
    detail::error_check("Finish"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glFlush. Introduced in GL es 2.0

 * \return void
 */
STATICINLINE void flush(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Flush)
    }
    glFlush();
    detail::error_check("Flush"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glFramebufferRenderbuffer. Introduced in GL es 2.0
 * \param target GLenum
 * \param attachment GLenum
 * \param renderbuffertarget GLenum
 * \param renderbuffer GLuint
 * \return void
 */
STATICINLINE void framebuffer_renderbuffer(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::renderbuffer_target    renderbuffertarget,
    u32                           renderbuffer,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferRenderbuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glFramebufferRenderbuffer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("FramebufferRenderbuffer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glFramebufferTexture2D. Introduced in GL es 2.0
 * \param target GLenum
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_2d(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture2D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTexture2D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("FramebufferTexture2D"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glFrontFace. Introduced in GL es 2.0
 * \param mode GLenum
 * \return void
 */
STATICINLINE void front_face(
    group::front_face_direction mode,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FrontFace)
    }
    glFrontFace(static_cast<GLenum>(mode));
    detail::error_check("FrontFace"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenBuffers. Introduced in GL es 2.0
 * \param n GLsizei
 * \param buffers GLuint *
 * \return void
 */
STATICINLINE void gen_buffers(
    span_u32 buffers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenBuffers)
    }
    glGenBuffers(
        buffers.size(),
        buffers.size() ? reinterpret_cast<GLuint*>(buffers.data()) : nullptr);
    detail::error_check("GenBuffers"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenFramebuffers. Introduced in GL es 2.0
 * \param n GLsizei
 * \param framebuffers GLuint *
 * \return void
 */
STATICINLINE void gen_framebuffers(
    span_u32 framebuffers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenFramebuffers)
    }
    glGenFramebuffers(
        framebuffers.size(),
        framebuffers.size() ? reinterpret_cast<GLuint*>(framebuffers.data())
                            : nullptr);
    detail::error_check("GenFramebuffers"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenRenderbuffers. Introduced in GL es 2.0
 * \param n GLsizei
 * \param renderbuffers GLuint *
 * \return void
 */
STATICINLINE void gen_renderbuffers(
    span_u32 renderbuffers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenRenderbuffers)
    }
    glGenRenderbuffers(
        renderbuffers.size(),
        renderbuffers.size() ? reinterpret_cast<GLuint*>(renderbuffers.data())
                             : nullptr);
    detail::error_check("GenRenderbuffers"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenTextures. Introduced in GL es 2.0
 * \param n GLsizei
 * \param textures GLuint *
 * \return void
 */
STATICINLINE void gen_textures(
    span_u32 textures, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTextures)
    }
    glGenTextures(
        textures.size(),
        textures.size() ? reinterpret_cast<GLuint*>(textures.data()) : nullptr);
    detail::error_check("GenTextures"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glGenerateMipmap. Introduced in GL es 2.0
 * \param target GLenum
 * \return void
 */
STATICINLINE void generate_mipmap(
    group::texture_target target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateMipmap)
    }
    glGenerateMipmap(static_cast<GLenum>(target));
    detail::error_check("GenerateMipmap"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetActiveAttrib. Introduced in GL es 2.0
 * \param program GLuint
 * \param index GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param size GLint *
 * \param type GLenum *
 * \param name GLchar *
 * \return void
 */
STATICINLINE void get_active_attrib(
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
        GLW_FPTR_CHECK(GetActiveAttrib)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveAttrib(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetActiveAttrib"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetActiveUniform. Introduced in GL es 2.0
 * \param program GLuint
 * \param index GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param size GLint *
 * \param type GLenum *
 * \param name GLchar *
 * \return void
 */
STATICINLINE void get_active_uniform(
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
        GLW_FPTR_CHECK(GetActiveUniform)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveUniform(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetActiveUniform"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetAttachedShaders. Introduced in GL es 2.0
 * \param program GLuint
 * \param maxCount GLsizei
 * \param count GLsizei *
 * \param shaders GLuint *
 * \return void
 */
STATICINLINE void get_attached_shaders(
    u32         program,
    i32&        count,
    span_u32    shaders,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttachedShaders)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetAttachedShaders(
        program,
        shaders.size(),
        &count,
        shaders.size() ? reinterpret_cast<GLuint*>(shaders.data()) : nullptr);
    detail::error_check("GetAttachedShaders"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glGetAttribLocation. Introduced in GL es 2.0
 * \param program GLuint
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_attrib_location(
    u32                     program,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttribLocation)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetAttribLocation(program, name.data());
    detail::error_check("GetAttribLocation"sv, check_errors);
    return out;
}

template<class span_bool>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_bool> &&
    std::is_same_v<
        std::decay_t<typename span_bool::value_type>,
        std::decay_t<bool>>)
/*!
 * \brief Wraps around glGetBooleanv. Introduced in GL es 2.0
 * \param pname GLenum
 * \param data GLboolean *
 * \return void
 */
STATICINLINE void get_booleanv(
    group::get_prop pname,
    span_bool       data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBooleanv)
    }
    glGetBooleanv(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLboolean*>(data.data()) : nullptr);
    detail::error_check("GetBooleanv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetBufferParameteriv. Introduced in GL es 2.0
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_buffer_parameter(
    group::buffer_target_arb target,
    group::buffer_prop_arb   pname,
    span_i32                 params,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferParameteriv)
    }
    glGetBufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetBufferParameteriv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glGetError. Introduced in GL es 2.0

 * \return ErrorCode
 */
STATICINLINE group::error_code get_error(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetError)
    }
    auto out = glGetError();
    detail::error_check("GetError"sv, check_errors);
    return static_cast<group::error_code>(out);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetFloatv. Introduced in GL es 2.0
 * \param pname GLenum
 * \param data GLfloat *
 * \return void
 */
STATICINLINE void get_floatv(
    group::get_prop pname,
    span_f32        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFloatv)
    }
    glGetFloatv(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetFloatv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetFramebufferAttachmentParameteriv. Introduced in GL
 * es 2.0 \param target GLenum \param attachment GLenum \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_framebuffer_attachment_parameter(
    group::framebuffer_target                    target,
    group::framebuffer_attachment                attachment,
    group::framebuffer_attachment_parameter_name pname,
    span_i32                                     params,
    error_check                                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferAttachmentParameteriv)
    }
    glGetFramebufferAttachmentParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferAttachmentParameteriv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetIntegerv. Introduced in GL es 2.0
 * \param pname GLenum
 * \param data GLint *
 * \return void
 */
STATICINLINE void get_integerv(
    group::get_prop pname,
    span_i32        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegerv)
    }
    glGetIntegerv(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegerv"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetProgramInfoLog. Introduced in GL es 2.0
 * \param program GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param infoLog GLchar *
 * \return void
 */
STATICINLINE void get_program_info_log(
    u32         program,
    i32&        length,
    span_GLchar infoLog,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramInfoLog)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramInfoLog(program, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetProgramInfoLog"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetProgramiv. Introduced in GL es 2.0
 * \param program GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_programiv(
    u32                         program,
    group::program_property_arb pname,
    span_i32                    params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramiv(
        program,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramiv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetRenderbufferParameteriv. Introduced in GL es 2.0
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_renderbuffer_parameter(
    group::renderbuffer_target         target,
    group::renderbuffer_parameter_name pname,
    span_i32                           params,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetRenderbufferParameteriv)
    }
    glGetRenderbufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetRenderbufferParameteriv"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetShaderInfoLog. Introduced in GL es 2.0
 * \param shader GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param infoLog GLchar *
 * \return void
 */
STATICINLINE void get_shader_info_log(
    u32         shader,
    i32&        length,
    span_GLchar infoLog,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderInfoLog)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    glGetShaderInfoLog(shader, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetShaderInfoLog"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetShaderPrecisionFormat. Introduced in GL es 2.0
 * \param shadertype GLenum
 * \param precisiontype GLenum
 * \param range GLint *
 * \param precision GLint *
 * \return void
 */
STATICINLINE void get_shader_precision_format(
    group::shader_type    shadertype,
    group::precision_type precisiontype,
    span_i32              range,
    i32&                  precision,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderPrecisionFormat)
    }
    glGetShaderPrecisionFormat(
        static_cast<GLenum>(shadertype),
        static_cast<GLenum>(precisiontype),
        range.size() ? reinterpret_cast<GLint*>(range.data()) : nullptr,
        &precision);
    detail::error_check("GetShaderPrecisionFormat"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetShaderSource. Introduced in GL es 2.0
 * \param shader GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param source GLchar *
 * \return void
 */
STATICINLINE void get_shader_source(
    u32         shader,
    i32&        length,
    span_GLchar source,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderSource)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    glGetShaderSource(shader, source.size(), &length, source.data());
    detail::error_check("GetShaderSource"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetShaderiv. Introduced in GL es 2.0
 * \param shader GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_shaderiv(
    u32                          shader,
    group::shader_parameter_name pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    glGetShaderiv(
        shader,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetShaderiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glGetString. Introduced in GL es 2.0
 * \param name GLenum
 * \return String
 */
STATICINLINE std::string get_string(
    group::string_name name, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetString)
    }
    auto out = glGetString(static_cast<GLenum>(name));
    detail::error_check("GetString"sv, check_errors);
    return reinterpret_cast<const char*>(out);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetTexParameterfv. Introduced in GL es 2.0
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_tex_parameter(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_f32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterfv)
    }
    glGetTexParameterfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterfv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetTexParameteriv. Introduced in GL es 2.0
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_tex_parameter(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameteriv)
    }
    glGetTexParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameteriv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glGetUniformLocation. Introduced in GL es 2.0
 * \param program GLuint
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_uniform_location(
    u32                     program,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformLocation)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetUniformLocation(program, name.data());
    detail::error_check("GetUniformLocation"sv, check_errors);
    return out;
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetUniformfv. Introduced in GL es 2.0
 * \param program GLuint
 * \param location GLint
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_uniformfv(
    u32         program,
    i32         location,
    span_f32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformfv(
        program,
        location,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetUniformfv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetUniformiv. Introduced in GL es 2.0
 * \param program GLuint
 * \param location GLint
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_uniformiv(
    u32         program,
    i32         location,
    span_i32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformiv(
        program,
        location,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetUniformiv"sv, check_errors);
}

template<class span_void>
requires(MinimumVersion<Current, Version<2, 0>> && concepts::span<span_void>)
/*!
 * \brief Wraps around glGetVertexAttribPointerv. Introduced in GL es 2.0
 * \param index GLuint
 * \param pname GLenum
 * \param pointer void **
 * \return void
 */
STATICINLINE void get_vertex_attrib_pointerv(
    u32                                       index,
    group::vertex_attrib_pointer_property_arb pname,
    span_void                                 pointer,
    error_check                               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribPointerv)
    }
    glGetVertexAttribPointerv(
        index,
        static_cast<GLenum>(pname),
        pointer.size() ? reinterpret_cast<void**>(pointer.data()) : nullptr);
    detail::error_check("GetVertexAttribPointerv"sv, check_errors);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetVertexAttribfv. Introduced in GL es 2.0
 * \param index GLuint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_vertex_attribfv(
    u32                               index,
    group::vertex_attrib_property_arb pname,
    span_f32                          params,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribfv)
    }
    glGetVertexAttribfv(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribfv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetVertexAttribiv. Introduced in GL es 2.0
 * \param index GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_vertex_attribiv(
    u32                               index,
    group::vertex_attrib_property_arb pname,
    span_i32                          params,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribiv)
    }
    glGetVertexAttribiv(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glHint. Introduced in GL es 2.0
 * \param target GLenum
 * \param mode GLenum
 * \return void
 */
STATICINLINE void hint(
    group::hint_target target,
    group::hint_mode   mode,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Hint)
    }
    glHint(static_cast<GLenum>(target), static_cast<GLenum>(mode));
    detail::error_check("Hint"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glIsBuffer. Introduced in GL es 2.0
 * \param buffer GLuint
 * \return Boolean
 */
STATICINLINE bool is_buffer(
    u32 buffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glIsBuffer(buffer);
    detail::error_check("IsBuffer"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glIsEnabled. Introduced in GL es 2.0
 * \param cap GLenum
 * \return Boolean
 */
STATICINLINE bool is_enabled(
    group::enable_cap cap, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnabled)
    }
    auto out = glIsEnabled(static_cast<GLenum>(cap));
    detail::error_check("IsEnabled"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glIsFramebuffer. Introduced in GL es 2.0
 * \param framebuffer GLuint
 * \return Boolean
 */
STATICINLINE bool is_framebuffer(
    u32 framebuffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsFramebuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    auto out = glIsFramebuffer(framebuffer);
    detail::error_check("IsFramebuffer"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glIsProgram. Introduced in GL es 2.0
 * \param program GLuint
 * \return Boolean
 */
STATICINLINE bool is_program(
    u32 program, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsProgram)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glIsProgram(program);
    detail::error_check("IsProgram"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glIsRenderbuffer. Introduced in GL es 2.0
 * \param renderbuffer GLuint
 * \return Boolean
 */
STATICINLINE bool is_renderbuffer(
    u32 renderbuffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsRenderbuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    auto out = glIsRenderbuffer(renderbuffer);
    detail::error_check("IsRenderbuffer"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glIsShader. Introduced in GL es 2.0
 * \param shader GLuint
 * \return Boolean
 */
STATICINLINE bool is_shader(
    u32 shader, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsShader)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    auto out = glIsShader(shader);
    detail::error_check("IsShader"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glIsTexture. Introduced in GL es 2.0
 * \param texture GLuint
 * \return Boolean
 */
STATICINLINE bool is_texture(
    u32 texture, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTexture)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    auto out = glIsTexture(texture);
    detail::error_check("IsTexture"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glLineWidth. Introduced in GL es 2.0
 * \param width GLfloat
 * \return void
 */
STATICINLINE void line_width(
    f32 width, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LineWidth)
    }
    glLineWidth(width);
    detail::error_check("LineWidth"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glLinkProgram. Introduced in GL es 2.0
 * \param program GLuint
 * \return void
 */
STATICINLINE void link_program(
    u32 program, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LinkProgram)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glLinkProgram(program);
    detail::error_check("LinkProgram"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glPixelStorei. Introduced in GL es 2.0
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void pixel_storei(
    group::pixel_store_parameter pname,
    i32                          param,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelStorei)
    }
    glPixelStorei(static_cast<GLenum>(pname), param);
    detail::error_check("PixelStorei"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glPolygonOffset. Introduced in GL es 2.0
 * \param factor GLfloat
 * \param units GLfloat
 * \return void
 */
STATICINLINE void polygon_offset(
    f32 factor, f32 units, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffset)
    }
    glPolygonOffset(factor, units);
    detail::error_check("PolygonOffset"sv, check_errors);
}

template<class size_2_i32, class span_void, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_void>)
/*!
 * \brief Wraps around glReadPixels. Introduced in GL es 2.0
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels void *
 * \return void
 */
STATICINLINE void read_pixels(
    vec_2_i32 const&    x,
    size_2_i32 const&   width,
    group::pixel_format format,
    group::pixel_type   type,
    span_void           pixels,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadPixels)
    }
    glReadPixels(
        x[0],
        x[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("ReadPixels"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glReleaseShaderCompiler. Introduced in GL es 2.0

 * \return void
 */
STATICINLINE void release_shader_compiler(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReleaseShaderCompiler)
    }
    glReleaseShaderCompiler();
    detail::error_check("ReleaseShaderCompiler"sv, check_errors);
}

template<class size_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glRenderbufferStorage. Introduced in GL es 2.0
 * \param target GLenum
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void renderbuffer_storage(
    group::renderbuffer_target target,
    group::internal_format     internalformat,
    size_2_i32 const&          width,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorage)
    }
    glRenderbufferStorage(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorage"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glSampleCoverage. Introduced in GL es 2.0
 * \param value GLfloat
 * \param invert GLboolean
 * \return void
 */
STATICINLINE void sample_coverage(
    f32 value, bool invert, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleCoverage)
    }
    glSampleCoverage(value, invert);
    detail::error_check("SampleCoverage"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glScissor. Introduced in GL es 2.0
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void scissor(
    vec_2_i32 const&  x,
    size_2_i32 const& width,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Scissor)
    }
    glScissor(x[0], x[1], width[0], width[1]);
    detail::error_check("Scissor"sv, check_errors);
}

template<class span_const_u32, class span_const_void>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_const_void>)
/*!
 * \brief Wraps around glShaderBinary. Introduced in GL es 2.0
 * \param count GLsizei
 * \param shaders const GLuint *
 * \param binaryFormat GLenum
 * \param binary const void *
 * \param length GLsizei
 * \return void
 */
STATICINLINE void shader_binary(
    span_const_u32 const&       shaders,
    group::shader_binary_format binaryFormat,
    span_const_void const&      binary,
    i32                         length,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderBinary)
    }
    glShaderBinary(
        shaders.size(),
        shaders.size() ? reinterpret_cast<const GLuint*>(shaders.data())
                       : nullptr,
        static_cast<GLenum>(binaryFormat),
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ShaderBinary"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glShaderSource. Introduced in GL es 2.0
 * \param shader GLuint
 * \param count GLsizei
 * \param string const GLchar *const*
 * \param length const GLint *
 * \return void
 */
STATICINLINE void shader_source(
    u32                           shader,
    std::vector<std::string_view> string,
    span_const_i32 const&         length,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderSource)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsShader)
            glIsShader(shader);
#endif
    }
    auto [string_lens, string_cstr, string_store] =
        detail::transform_strings(string);
    glShaderSource(
        shader,
        string_cstr.size(),
        string_cstr.data(),
        length.size() ? reinterpret_cast<const GLint*>(length.data())
                      : nullptr);
    detail::error_check("ShaderSource"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glStencilFunc. Introduced in GL es 2.0
 * \param func GLenum
 * \param ref GLint
 * \param mask GLuint
 * \return void
 */
STATICINLINE void stencil_func(
    group::stencil_function func,
    i32                     ref,
    u32                     mask,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilFunc)
    }
    glStencilFunc(static_cast<GLenum>(func), ref, mask);
    detail::error_check("StencilFunc"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glStencilFuncSeparate. Introduced in GL es 2.0
 * \param face GLenum
 * \param func GLenum
 * \param ref GLint
 * \param mask GLuint
 * \return void
 */
STATICINLINE void stencil_func_separate(
    group::triangle_face    face,
    group::stencil_function func,
    i32                     ref,
    u32                     mask,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilFuncSeparate)
    }
    glStencilFuncSeparate(
        static_cast<GLenum>(face), static_cast<GLenum>(func), ref, mask);
    detail::error_check("StencilFuncSeparate"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glStencilMask. Introduced in GL es 2.0
 * \param mask GLuint
 * \return void
 */
STATICINLINE void stencil_mask(
    u32 mask, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilMask)
    }
    glStencilMask(mask);
    detail::error_check("StencilMask"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glStencilMaskSeparate. Introduced in GL es 2.0
 * \param face GLenum
 * \param mask GLuint
 * \return void
 */
STATICINLINE void stencil_mask_separate(
    group::triangle_face face,
    u32                  mask,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilMaskSeparate)
    }
    glStencilMaskSeparate(static_cast<GLenum>(face), mask);
    detail::error_check("StencilMaskSeparate"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glStencilOp. Introduced in GL es 2.0
 * \param fail GLenum
 * \param zfail GLenum
 * \param zpass GLenum
 * \return void
 */
STATICINLINE void stencil_op(
    group::stencil_op fail,
    group::stencil_op zfail,
    group::stencil_op zpass,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilOp)
    }
    glStencilOp(
        static_cast<GLenum>(fail),
        static_cast<GLenum>(zfail),
        static_cast<GLenum>(zpass));
    detail::error_check("StencilOp"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glStencilOpSeparate. Introduced in GL es 2.0
 * \param face GLenum
 * \param sfail GLenum
 * \param dpfail GLenum
 * \param dppass GLenum
 * \return void
 */
STATICINLINE void stencil_op_separate(
    group::triangle_face face,
    group::stencil_op    sfail,
    group::stencil_op    dpfail,
    group::stencil_op    dppass,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilOpSeparate)
    }
    glStencilOpSeparate(
        static_cast<GLenum>(face),
        static_cast<GLenum>(sfail),
        static_cast<GLenum>(dpfail),
        static_cast<GLenum>(dppass));
    detail::error_check("StencilOpSeparate"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glTexImage2D. Introduced in GL es 2.0
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_image_2d(
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    size_2_i32 const&      width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage2D)
    }
    glTexImage2D(
        static_cast<GLenum>(target),
        level,
        internalformat,
        width[0],
        width[1],
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexImage2D"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glTexParameterf. Introduced in GL es 2.0
 * \param target GLenum
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void tex_parameter(
    group::texture_target         target,
    group::texture_parameter_name pname,
    f32                           param,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterf)
    }
    glTexParameterf(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("TexParameterf"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glTexParameterfv. Introduced in GL es 2.0
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfloat *
 * \return void
 */
STATICINLINE void tex_parameter(
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_f32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterfv)
    }
    glTexParameterfv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterfv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glTexParameteri. Introduced in GL es 2.0
 * \param target GLenum
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void tex_parameter(
    group::texture_target         target,
    group::texture_parameter_name pname,
    i32                           param,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameteri)
    }
    glTexParameteri(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("TexParameteri"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glTexParameteriv. Introduced in GL es 2.0
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void tex_parameter(
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_i32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameteriv)
    }
    glTexParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameteriv"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Wraps around glTexSubImage2D. Introduced in GL es 2.0
 * \param target GLenum
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
STATICINLINE void tex_sub_image_2d(
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexSubImage2D)
    }
    glTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage2D"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glUniform1f. Introduced in GL es 2.0
 * \param location GLint
 * \param v0 GLfloat
 * \return void
 */
STATICINLINE void uniform(
    i32 location, f32 v0, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1f)
    }
    glUniform1f(location, v0);
    detail::error_check("Uniform1f"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glUniform1fv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                   location,
    span_const_f32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1fv)
    }
    glUniform1fv(
        location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform1fv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glUniform1i. Introduced in GL es 2.0
 * \param location GLint
 * \param v0 GLint
 * \return void
 */
STATICINLINE void uniform(
    i32 location, i32 v0, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1i)
    }
    glUniform1i(location, v0);
    detail::error_check("Uniform1i"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glUniform1iv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void uniform(
    i32                   location,
    span_const_i32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1iv)
    }
    glUniform1iv(
        location, value.size(), reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform1iv"sv, check_errors);
}

template<class vec_2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_f32, f32, 2>)
/*!
 * \brief Wraps around glUniform2f. Introduced in GL es 2.0
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_2_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2f)
    }
    glUniform2f(location, v0[0], v0[1]);
    detail::error_check("Uniform2f"sv, check_errors);
}

template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_2_f32> &&
    concepts::vector<typename span_const_vec_2_f32::value_type, f32, 2>)
/*!
 * \brief Wraps around glUniform2fv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_2_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2fv)
    }
    glUniform2fv(
        location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform2fv"sv, check_errors);
}

template<class vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Wraps around glUniform2i. Introduced in GL es 2.0
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_2_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i)
    }
    glUniform2i(location, v0[0], v0[1]);
    detail::error_check("Uniform2i"sv, check_errors);
}

template<class span_const_vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_2_i32> &&
    concepts::vector<typename span_const_vec_2_i32::value_type, i32, 2>)
/*!
 * \brief Wraps around glUniform2iv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_2_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2iv)
    }
    glUniform2iv(
        location, value.size(), reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform2iv"sv, check_errors);
}

template<class vec_3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Wraps around glUniform3f. Introduced in GL es 2.0
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \param v2 GLfloat
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_3_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3f)
    }
    glUniform3f(location, v0[0], v0[1], v0[2]);
    detail::error_check("Uniform3f"sv, check_errors);
}

template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_3_f32> &&
    concepts::vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Wraps around glUniform3fv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_3_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3fv)
    }
    glUniform3fv(
        location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform3fv"sv, check_errors);
}

template<class vec_3_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_3_i32, i32, 3>)
/*!
 * \brief Wraps around glUniform3i. Introduced in GL es 2.0
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \param v2 GLint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_3_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i)
    }
    glUniform3i(location, v0[0], v0[1], v0[2]);
    detail::error_check("Uniform3i"sv, check_errors);
}

template<class span_const_vec_3_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_3_i32> &&
    concepts::vector<typename span_const_vec_3_i32::value_type, i32, 3>)
/*!
 * \brief Wraps around glUniform3iv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_3_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3iv)
    }
    glUniform3iv(
        location, value.size(), reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform3iv"sv, check_errors);
}

template<class vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Wraps around glUniform4f. Introduced in GL es 2.0
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \param v2 GLfloat
 * \param v3 GLfloat
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_4_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4f)
    }
    glUniform4f(location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("Uniform4f"sv, check_errors);
}

template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Wraps around glUniform4fv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_4_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4fv)
    }
    glUniform4fv(
        location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform4fv"sv, check_errors);
}

template<class vec_4_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_4_i32, i32, 4>)
/*!
 * \brief Wraps around glUniform4i. Introduced in GL es 2.0
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \param v2 GLint
 * \param v3 GLint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_4_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i)
    }
    glUniform4i(location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("Uniform4i"sv, check_errors);
}

template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Wraps around glUniform4iv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_4_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4iv)
    }
    glUniform4iv(
        location, value.size(), reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform4iv"sv, check_errors);
}

template<class span_const_mat_2x2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_mat_2x2_f32> &&
    concepts::matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
/*!
 * \brief Wraps around glUniformMatrix2fv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2fv)
    }
    glUniformMatrix2fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2fv"sv, check_errors);
}

template<class span_const_mat_3x3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_mat_3x3_f32> &&
    concepts::matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
/*!
 * \brief Wraps around glUniformMatrix3fv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3fv)
    }
    glUniformMatrix3fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3fv"sv, check_errors);
}

template<class span_const_mat_4x4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_mat_4x4_f32> &&
    concepts::matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
/*!
 * \brief Wraps around glUniformMatrix4fv. Introduced in GL es 2.0
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4fv)
    }
    glUniformMatrix4fv(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4fv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glUseProgram. Introduced in GL es 2.0
 * \param program GLuint
 * \return void
 */
STATICINLINE void use_program(
    u32 program, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UseProgram)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glUseProgram(program);
    detail::error_check("UseProgram"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glValidateProgram. Introduced in GL es 2.0
 * \param program GLuint
 * \return void
 */
STATICINLINE void validate_program(
    u32 program, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ValidateProgram)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glValidateProgram(program);
    detail::error_check("ValidateProgram"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>)
/*!
 * \brief Wraps around glVertexAttrib1f. Introduced in GL es 2.0
 * \param index GLuint
 * \param x GLfloat
 * \return void
 */
STATICINLINE void vertex_attrib1f(
    u32 index, f32 x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1f)
    }
    glVertexAttrib1f(index, x);
    detail::error_check("VertexAttrib1f"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glVertexAttrib1fv. Introduced in GL es 2.0
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void vertex_attrib1fv(
    u32                   index,
    span_const_f32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1fv)
    }
    glVertexAttrib1fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib1fv"sv, check_errors);
}

template<class vec_2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_f32, f32, 2>)
/*!
 * \brief Wraps around glVertexAttrib2f. Introduced in GL es 2.0
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \return void
 */
STATICINLINE void vertex_attrib2f(
    u32 index, vec_2_f32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2f)
    }
    glVertexAttrib2f(index, x[0], x[1]);
    detail::error_check("VertexAttrib2f"sv, check_errors);
}

template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_2_f32> &&
    concepts::vector<typename span_const_vec_2_f32::value_type, f32, 2>)
/*!
 * \brief Wraps around glVertexAttrib2fv. Introduced in GL es 2.0
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void vertex_attrib2fv(
    u32                         index,
    span_const_vec_2_f32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2fv)
    }
    glVertexAttrib2fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib2fv"sv, check_errors);
}

template<class vec_3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Wraps around glVertexAttrib3f. Introduced in GL es 2.0
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \return void
 */
STATICINLINE void vertex_attrib3f(
    u32 index, vec_3_f32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3f)
    }
    glVertexAttrib3f(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttrib3f"sv, check_errors);
}

template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_3_f32> &&
    concepts::vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Wraps around glVertexAttrib3fv. Introduced in GL es 2.0
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void vertex_attrib3fv(
    u32                         index,
    span_const_vec_3_f32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3fv)
    }
    glVertexAttrib3fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib3fv"sv, check_errors);
}

template<class vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Wraps around glVertexAttrib4f. Introduced in GL es 2.0
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param z GLfloat
 * \param w GLfloat
 * \return void
 */
STATICINLINE void vertex_attrib4f(
    u32 index, vec_4_f32 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4f)
    }
    glVertexAttrib4f(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttrib4f"sv, check_errors);
}

template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Wraps around glVertexAttrib4fv. Introduced in GL es 2.0
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void vertex_attrib4fv(
    u32                         index,
    span_const_vec_4_f32 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4fv)
    }
    glVertexAttrib4fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib4fv"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<2, 0>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glVertexAttribPointer. Introduced in GL es 2.0
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param normalized GLboolean
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void vertex_attrib_pointer(
    u32                               index,
    i32                               size,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    i32                               stride,
    span_const_void const&            pointer,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribPointer)
    }
    glVertexAttribPointer(
        index,
        size,
        static_cast<GLenum>(type),
        normalized,
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribPointer"sv, check_errors);
}

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glViewport. Introduced in GL es 2.0
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void viewport(
    vec_2_i32 const&  x,
    size_2_i32 const& width,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Viewport)
    }
    glViewport(x[0], x[1], width[0], width[1]);
    detail::error_check("Viewport"sv, check_errors);
}

#endif // GL_ES_VERSION_2_0
