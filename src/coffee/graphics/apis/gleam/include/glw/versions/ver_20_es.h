#ifdef GL_ES_VERSION_2_0
/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void active_texture(group::texture_unit texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ActiveTexture)
    }
    glActiveTexture(static_cast<GLenum>(texture));
    detail::error_check("ActiveTexture"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void attach_shader(u32 program, u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AttachShader)
        glIsProgram(program);
        glIsShader(shader);
    }
    glAttachShader(program, shader);
    detail::error_check("AttachShader"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void bind_attrib_location(
        u32 program, u32 index, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindAttribLocation)
        glIsProgram(program);
    }
    glBindAttribLocation(program, index, name.data());
    detail::error_check("BindAttribLocation"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void bind_buffer(group::buffer_target_arb target, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBuffer)
        glIsBuffer(buffer);
    }
    glBindBuffer(static_cast<GLenum>(target), buffer);
    detail::error_check("BindBuffer"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void bind_framebuffer(group::framebuffer_target target, u32 framebuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFramebuffer)
        glIsFramebuffer(framebuffer);
    }
    glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
    detail::error_check("BindFramebuffer"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void bind_renderbuffer(group::renderbuffer_target target, u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindRenderbuffer)
        glIsRenderbuffer(renderbuffer);
    }
    glBindRenderbuffer(static_cast<GLenum>(target), renderbuffer);
    detail::error_check("BindRenderbuffer"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void bind_texture(group::texture_target target, u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTexture)
        glIsTexture(texture);
    }
    glBindTexture(static_cast<GLenum>(target), texture);
    detail::error_check("BindTexture"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void blend_color(vec_4_f32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColor)
    }
    glBlendColor(red.x(), red.y(), red.z(), red.w());
    detail::error_check("BlendColor"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void blend_equation(group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquation)
    }
    glBlendEquation(static_cast<GLenum>(mode));
    detail::error_check("BlendEquation"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void blend_equation_separate(
        group::blend_equation_mode_ext modeRGB,
        group::blend_equation_mode_ext modeAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparate)
    }
    glBlendEquationSeparate(
        static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparate"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE void blend_func(
    group::blending_factor sfactor, group::blending_factor dfactor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFunc)
    }
    glBlendFunc(static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor));
    detail::error_check("BlendFunc"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void blend_func_separate(
        group::blending_factor sfactorRGB,
        group::blending_factor dfactorRGB,
        group::blending_factor sfactorAlpha,
        group::blending_factor dfactorAlpha)
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
    detail::error_check("BlendFuncSeparate"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void buffer_data(
        group::buffer_target_arb target,
        span_const_void const&   data,
        group::buffer_usage_arb  usage)
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
    detail::error_check("BufferData"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void buffer_sub_data(
        group::buffer_target_arb target,
        GLintptr                 offset,
        span_const_void const&   data)
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
    detail::error_check("BufferSubData"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLenum
    check_framebuffer_status(group::framebuffer_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckFramebufferStatus)
    }
    auto out = glCheckFramebufferStatus(static_cast<GLenum>(target));
    detail::error_check("CheckFramebufferStatus"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void clear(group::clear_buffer_mask mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Clear)
    }
    glClear(static_cast<GLenum>(mask));
    detail::error_check("Clear"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void clear_color(vec_4_f32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearColor)
    }
    glClearColor(red.x(), red.y(), red.z(), red.w());
    detail::error_check("ClearColor"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void clear_depthf(f32 d)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthf)
    }
    glClearDepthf(d);
    detail::error_check("ClearDepthf"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void clear_stencil(i32 s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearStencil)
    }
    glClearStencil(s);
    detail::error_check("ClearStencil"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_4_bool>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_bool, bool, 4>) STATICINLINE
    void color_mask(vec_4_bool const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorMask)
    }
    glColorMask(red.x(), red.y(), red.z(), red.w());
    detail::error_check("ColorMask"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void compile_shader(u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompileShader)
        glIsShader(shader);
    }
    glCompileShader(shader);
    detail::error_check("CompileShader"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class span_const_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_image_2d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        size_2_i32 const&      width,
        i32                    border,
        span_const_void const& data)
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
    detail::error_check("CompressedTexImage2D"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void compressed_tex_sub_image_2d(
        group::texture_target  target,
        i32                    level,
        vec_2_i32 const&       xoffset,
        size_2_i32 const&      width,
        group::pixel_format    format,
        span_const_void const& data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage2D)
    }
    glCompressedTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage2D"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_tex_image_2d(
        group::texture_target  target,
        i32                    level,
        group::internal_format internalformat,
        vec_2_i32 const&       x,
        size_2_i32 const&      width,
        i32                    border)
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
        x.x(),
        x.y(),
        width[0],
        width[1],
        border);
    detail::error_check("CopyTexImage2D"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void copy_tex_sub_image_2d(
        group::texture_target target,
        i32                   level,
        vec_2_i32 const&      xoffset,
        vec_2_i32 const&      x,
        size_2_i32 const&     width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyTexSubImage2D)
    }
    glCopyTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("CopyTexSubImage2D"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLuint
    create_program()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateProgram)
    }
    auto out = glCreateProgram();
    detail::error_check("CreateProgram"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLuint
    create_shader(group::shader_type type)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateShader)
    }
    auto out = glCreateShader(static_cast<GLenum>(type));
    detail::error_check("CreateShader"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void cull_face(group::cull_face_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CullFace)
    }
    glCullFace(static_cast<GLenum>(mode));
    detail::error_check("CullFace"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_buffers(span_const_u32 const& buffers)
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
    detail::error_check("DeleteBuffers"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_framebuffers(span_const_u32 const& framebuffers)
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
    detail::error_check("DeleteFramebuffers"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void delete_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteProgram)
        glIsProgram(program);
    }
    glDeleteProgram(program);
    detail::error_check("DeleteProgram"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_renderbuffers(span_const_u32 const& renderbuffers)
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
    detail::error_check("DeleteRenderbuffers"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void delete_shader(u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteShader)
        glIsShader(shader);
    }
    glDeleteShader(shader);
    detail::error_check("DeleteShader"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_textures(span_const_u32 const& textures)
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
    detail::error_check("DeleteTextures"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void depth_func(group::depth_function func)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthFunc)
    }
    glDepthFunc(static_cast<GLenum>(func));
    detail::error_check("DepthFunc"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void depth_mask(bool flag)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthMask)
    }
    glDepthMask(flag);
    detail::error_check("DepthMask"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void depth_rangef(f32 n, f32 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangef)
    }
    glDepthRangef(n, f);
    detail::error_check("DepthRangef"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void detach_shader(u32 program, u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DetachShader)
        glIsProgram(program);
        glIsShader(shader);
    }
    glDetachShader(program, shader);
    detail::error_check("DetachShader"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void disable(group::enable_cap cap)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Disable)
    }
    glDisable(static_cast<GLenum>(cap));
    detail::error_check("Disable"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void disable_vertex_attrib_array(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexAttribArray)
    }
    glDisableVertexAttribArray(index);
    detail::error_check("DisableVertexAttribArray"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void draw_arrays(group::primitive_type mode, i32 first, i32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArrays)
    }
    glDrawArrays(static_cast<GLenum>(mode), first, count);
    detail::error_check("DrawArrays"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void draw_elements(
        group::primitive_type     mode,
        i32                       count,
        group::draw_elements_type type,
        ptroff                    indices)
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
    detail::error_check("DrawElements"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void enable(group::enable_cap cap)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Enable)
    }
    glEnable(static_cast<GLenum>(cap));
    detail::error_check("Enable"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void enable_vertex_attrib_array(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexAttribArray)
    }
    glEnableVertexAttribArray(index);
    detail::error_check("EnableVertexAttribArray"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE void finish()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Finish)
    }
    glFinish();
    detail::error_check("Finish"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE void flush()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Flush)
    }
    glFlush();
    detail::error_check("Flush"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void framebuffer_renderbuffer(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        group::renderbuffer_target    renderbuffertarget,
        u32                           renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferRenderbuffer)
        glIsRenderbuffer(renderbuffer);
    }
    glFramebufferRenderbuffer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("FramebufferRenderbuffer"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void framebuffer_texture_2d(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        group::texture_target         textarget,
        u32                           texture,
        i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture2D)
        glIsTexture(texture);
    }
    glFramebufferTexture2D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("FramebufferTexture2D"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void front_face(group::front_face_direction mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FrontFace)
    }
    glFrontFace(static_cast<GLenum>(mode));
    detail::error_check("FrontFace"sv);
}

/* Introduced in GL es 2.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_buffers(span_u32 buffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenBuffers)
    }
    glGenBuffers(
        buffers.size(),
        buffers.size() ? reinterpret_cast<GLuint*>(buffers.data()) : nullptr);
    detail::error_check("GenBuffers"sv);
}

/* Introduced in GL es 2.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_framebuffers(span_u32 framebuffers)
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
    detail::error_check("GenFramebuffers"sv);
}

/* Introduced in GL es 2.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_renderbuffers(span_u32 renderbuffers)
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
    detail::error_check("GenRenderbuffers"sv);
}

/* Introduced in GL es 2.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_textures(span_u32 textures)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTextures)
    }
    glGenTextures(
        textures.size(),
        textures.size() ? reinterpret_cast<GLuint*>(textures.data()) : nullptr);
    detail::error_check("GenTextures"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void generate_mipmap(group::texture_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateMipmap)
    }
    glGenerateMipmap(static_cast<GLenum>(target));
    detail::error_check("GenerateMipmap"sv);
}

/* Introduced in GL es 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_attrib(
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
        GLW_FPTR_CHECK(GetActiveAttrib)
        glIsProgram(program);
    }
    glGetActiveAttrib(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetActiveAttrib"sv);
}

/* Introduced in GL es 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_uniform(
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
        GLW_FPTR_CHECK(GetActiveUniform)
        glIsProgram(program);
    }
    glGetActiveUniform(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetActiveUniform"sv);
}

/* Introduced in GL es 2.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_attached_shaders(u32 program, i32& count, span_u32 shaders)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttachedShaders)
        glIsProgram(program);
    }
    glGetAttachedShaders(
        program,
        shaders.size(),
        &count,
        shaders.size() ? reinterpret_cast<GLuint*>(shaders.data()) : nullptr);
    detail::error_check("GetAttachedShaders"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLint
    get_attrib_location(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttribLocation)
        glIsProgram(program);
    }
    auto out = glGetAttribLocation(program, name.data());
    detail::error_check("GetAttribLocation"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<class span_bool>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_bool>&& std::is_same_v<
                 std::decay_t<typename span_bool::value_type>,
                 std::decay_t<bool>>) STATICINLINE
    void get_booleanv(group::get_prop pname, span_bool data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBooleanv)
    }
    glGetBooleanv(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLboolean*>(data.data()) : nullptr);
    detail::error_check("GetBooleanv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_buffer_parameter(
        group::buffer_target_arb target,
        group::buffer_prop_arb   pname,
        span_i32                 params)
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
    detail::error_check("GetBufferParameteriv"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLenum get_error()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetError)
    }
    auto out = glGetError();
    detail::error_check("GetError"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_floatv(group::get_prop pname, span_f32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFloatv)
    }
    glGetFloatv(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetFloatv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_framebuffer_attachment_parameter(
        group::framebuffer_target                    target,
        group::framebuffer_attachment                attachment,
        group::framebuffer_attachment_parameter_name pname,
        span_i32                                     params)
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
    detail::error_check("GetFramebufferAttachmentParameteriv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_integerv(group::get_prop pname, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegerv)
    }
    glGetIntegerv(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegerv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_program_info_log(u32 program, i32& length, span_GLchar infoLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramInfoLog)
        glIsProgram(program);
    }
    glGetProgramInfoLog(program, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetProgramInfoLog"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_programiv(
        u32 program, group::program_property_arb pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramiv)
        glIsProgram(program);
    }
    glGetProgramiv(
        program,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramiv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_renderbuffer_parameter(
        group::renderbuffer_target         target,
        group::renderbuffer_parameter_name pname,
        span_i32                           params)
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
    detail::error_check("GetRenderbufferParameteriv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_shader_info_log(u32 shader, i32& length, span_GLchar infoLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderInfoLog)
        glIsShader(shader);
    }
    glGetShaderInfoLog(shader, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetShaderInfoLog"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_shader_precision_format(
        group::shader_type    shadertype,
        group::precision_type precisiontype,
        span_i32              range,
        i32&                  precision)
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
    detail::error_check("GetShaderPrecisionFormat"sv);
}

/* Introduced in GL es 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_shader_source(u32 shader, i32& length, span_GLchar source)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderSource)
        glIsShader(shader);
    }
    glGetShaderSource(shader, source.size(), &length, source.data());
    detail::error_check("GetShaderSource"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_shaderiv(
        u32 shader, group::shader_parameter_name pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderiv)
        glIsShader(shader);
    }
    glGetShaderiv(
        shader,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetShaderiv"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE stl_types::String
    get_string(group::string_name name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetString)
    }
    auto out = glGetString(static_cast<GLenum>(name));
    detail::error_check("GetString"sv);
    return reinterpret_cast<const char*>(out);
}

/* Introduced in GL es 2.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_tex_parameter(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_f32                     params)
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
    detail::error_check("GetTexParameterfv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_tex_parameter(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_i32                     params)
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
    detail::error_check("GetTexParameteriv"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLint
    get_uniform_location(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformLocation)
        glIsProgram(program);
    }
    auto out = glGetUniformLocation(program, name.data());
    detail::error_check("GetUniformLocation"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_uniformfv(u32 program, i32 location, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformfv)
        glIsProgram(program);
    }
    glGetUniformfv(
        program,
        location,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetUniformfv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_uniformiv(u32 program, i32 location, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformiv)
        glIsProgram(program);
    }
    glGetUniformiv(
        program,
        location,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetUniformiv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_vertex_attrib_pointerv(
        u32                                       index,
        group::vertex_attrib_pointer_property_arb pname,
        span_void                                 pointer)
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
    detail::error_check("GetVertexAttribPointerv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_vertex_attribfv(
        u32 index, group::vertex_attrib_property_arb pname, span_f32 params)
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
    detail::error_check("GetVertexAttribfv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_vertex_attribiv(
        u32 index, group::vertex_attrib_property_arb pname, span_i32 params)
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
    detail::error_check("GetVertexAttribiv"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void hint(group::hint_target target, group::hint_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Hint)
    }
    glHint(static_cast<GLenum>(target), static_cast<GLenum>(mode));
    detail::error_check("Hint"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_buffer(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsBuffer)
        glIsBuffer(buffer);
    }
    auto out = glIsBuffer(buffer);
    detail::error_check("IsBuffer"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_enabled(group::enable_cap cap)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnabled)
    }
    auto out = glIsEnabled(static_cast<GLenum>(cap));
    detail::error_check("IsEnabled"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_framebuffer(u32 framebuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsFramebuffer)
        glIsFramebuffer(framebuffer);
    }
    auto out = glIsFramebuffer(framebuffer);
    detail::error_check("IsFramebuffer"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsProgram)
        glIsProgram(program);
    }
    auto out = glIsProgram(program);
    detail::error_check("IsProgram"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_renderbuffer(u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsRenderbuffer)
        glIsRenderbuffer(renderbuffer);
    }
    auto out = glIsRenderbuffer(renderbuffer);
    detail::error_check("IsRenderbuffer"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_shader(u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsShader)
        glIsShader(shader);
    }
    auto out = glIsShader(shader);
    detail::error_check("IsShader"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_texture(u32 texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTexture)
        glIsTexture(texture);
    }
    auto out = glIsTexture(texture);
    detail::error_check("IsTexture"sv);
    return out;
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void line_width(f32 width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LineWidth)
    }
    glLineWidth(width);
    detail::error_check("LineWidth"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void link_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LinkProgram)
        glIsProgram(program);
    }
    glLinkProgram(program);
    detail::error_check("LinkProgram"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void pixel_storei(group::pixel_store_parameter pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelStorei)
    }
    glPixelStorei(static_cast<GLenum>(pname), param);
    detail::error_check("PixelStorei"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void polygon_offset(f32 factor, f32 units)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffset)
    }
    glPolygonOffset(factor, units);
    detail::error_check("PolygonOffset"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class span_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_void>) STATICINLINE
    void read_pixels(
        vec_2_i32 const&    x,
        size_2_i32 const&   width,
        group::pixel_format format,
        group::pixel_type   type,
        span_void           pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadPixels)
    }
    glReadPixels(
        x.x(),
        x.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("ReadPixels"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void release_shader_compiler()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReleaseShaderCompiler)
    }
    glReleaseShaderCompiler();
    detail::error_check("ReleaseShaderCompiler"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void renderbuffer_storage(
        group::renderbuffer_target target,
        group::internal_format     internalformat,
        size_2_i32 const&          width)
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
    detail::error_check("RenderbufferStorage"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void sample_coverage(f32 value, bool invert)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleCoverage)
    }
    glSampleCoverage(value, invert);
    detail::error_check("SampleCoverage"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void scissor(vec_2_i32 const& x, size_2_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Scissor)
    }
    glScissor(x.x(), x.y(), width[0], width[1]);
    detail::error_check("Scissor"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_u32, class span_const_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>&& semantic::concepts::Span<span_const_void>)
    STATICINLINE void shader_binary(
        span_const_u32 const&       shaders,
        group::shader_binary_format binaryFormat,
        span_const_void const&      binary,
        i32                         length)
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
    detail::error_check("ShaderBinary"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void shader_source(
        u32                           shader,
        std::vector<std::string_view> string,
        span_const_i32 const&         length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderSource)
        glIsShader(shader);
    }
    auto [string_lens, string_cstr, string_store] =
        detail::transform_strings(string);
    glShaderSource(
        shader,
        string_cstr.size(),
        string_cstr.data(),
        length.size() ? reinterpret_cast<const GLint*>(length.data())
                      : nullptr);
    detail::error_check("ShaderSource"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void stencil_func(group::stencil_function func, i32 ref, u32 mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilFunc)
    }
    glStencilFunc(static_cast<GLenum>(func), ref, mask);
    detail::error_check("StencilFunc"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void stencil_func_separate(
        group::stencil_face_direction face,
        group::stencil_function       func,
        i32                           ref,
        u32                           mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilFuncSeparate)
    }
    glStencilFuncSeparate(
        static_cast<GLenum>(face), static_cast<GLenum>(func), ref, mask);
    detail::error_check("StencilFuncSeparate"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void stencil_mask(u32 mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilMask)
    }
    glStencilMask(mask);
    detail::error_check("StencilMask"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void stencil_mask_separate(group::stencil_face_direction face, u32 mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilMaskSeparate)
    }
    glStencilMaskSeparate(static_cast<GLenum>(face), mask);
    detail::error_check("StencilMaskSeparate"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE void stencil_op(
    group::stencil_op fail, group::stencil_op zfail, group::stencil_op zpass)
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
    detail::error_check("StencilOp"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void stencil_op_separate(
        group::stencil_face_direction face,
        group::stencil_op             sfail,
        group::stencil_op             dpfail,
        group::stencil_op             dppass)
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
    detail::error_check("StencilOpSeparate"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class span_const_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_const_void>) STATICINLINE
    void tex_image_2d(
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        size_2_i32 const&      width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
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
    detail::error_check("TexImage2D"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void tex_parameter(
        group::texture_target         target,
        group::texture_parameter_name pname,
        f32                           param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterf)
    }
    glTexParameterf(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("TexParameterf"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void tex_parameter(
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_f32 const&         params)
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
    detail::error_check("TexParameterfv"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void tex_parameter(
        group::texture_target         target,
        group::texture_parameter_name pname,
        i32                           param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameteri)
    }
    glTexParameteri(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("TexParameteri"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void tex_parameter(
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
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
    detail::error_check("TexParameteriv"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_const_void>) STATICINLINE
    void tex_sub_image_2d(
        group::texture_target  target,
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
        GLW_FPTR_CHECK(TexSubImage2D)
    }
    glTexSubImage2D(
        static_cast<GLenum>(target),
        level,
        xoffset.x(),
        xoffset.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage2D"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void uniform(i32 location, f32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1f)
    }
    glUniform1f(location, v0);
    detail::error_check("Uniform1f"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1fv)
    }
    glUniform1fv(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform1fv"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void uniform(i32 location, i32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1i)
    }
    glUniform1i(location, v0);
    detail::error_check("Uniform1i"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1iv)
    }
    glUniform1iv(location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform1iv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_2_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_2_f32, f32, 2>) STATICINLINE
    void uniform(i32 location, vec_2_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2f)
    }
    glUniform2f(location, v0.x(), v0.y());
    detail::error_check("Uniform2f"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
            Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2fv)
    }
    glUniform2fv(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform2fv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void uniform(i32 location, vec_2_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i)
    }
    glUniform2i(location, v0.x(), v0.y());
    detail::error_check("Uniform2i"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
            Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2iv)
    }
    glUniform2iv(location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform2iv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_3_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void uniform(i32 location, vec_3_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3f)
    }
    glUniform3f(location, v0.x(), v0.y(), v0.z());
    detail::error_check("Uniform3f"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
            Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3fv)
    }
    glUniform3fv(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform3fv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_3_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_3_i32, i32, 3>) STATICINLINE
    void uniform(i32 location, vec_3_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i)
    }
    glUniform3i(location, v0.x(), v0.y(), v0.z());
    detail::error_check("Uniform3i"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_3_i32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
            Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3iv)
    }
    glUniform3iv(location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform3iv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void uniform(i32 location, vec_4_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4f)
    }
    glUniform4f(location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("Uniform4f"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
            Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4fv)
    }
    glUniform4fv(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform4fv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_4_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
    void uniform(i32 location, vec_4_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i)
    }
    glUniform4i(location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("Uniform4i"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
            Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4iv)
    }
    glUniform4iv(location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform4iv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_mat_2x2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_mat_2x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2fv)
    }
    glUniformMatrix2fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2fv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_mat_3x3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3fv)
    }
    glUniformMatrix3fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3fv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_mat_4x4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_mat_4x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4fv)
    }
    glUniformMatrix4fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4fv"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void use_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UseProgram)
        glIsProgram(program);
    }
    glUseProgram(program);
    detail::error_check("UseProgram"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void validate_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ValidateProgram)
        glIsProgram(program);
    }
    glValidateProgram(program);
    detail::error_check("ValidateProgram"sv);
}

/* Introduced in GL es 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void vertex_attrib1f(u32 index, f32 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1f)
    }
    glVertexAttrib1f(index, x);
    detail::error_check("VertexAttrib1f"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void vertex_attrib1fv(u32 index, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1fv)
    }
    glVertexAttrib1fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib1fv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_2_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_2_f32, f32, 2>) STATICINLINE
    void vertex_attrib2f(u32 index, vec_2_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2f)
    }
    glVertexAttrib2f(index, x.x(), x.y());
    detail::error_check("VertexAttrib2f"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
            Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    STATICINLINE void vertex_attrib2fv(u32 index, span_const_vec_2_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2fv)
    }
    glVertexAttrib2fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib2fv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_3_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void vertex_attrib3f(u32 index, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3f)
    }
    glVertexAttrib3f(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3f"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
            Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE void vertex_attrib3fv(u32 index, span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3fv)
    }
    glVertexAttrib3fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib3fv"sv);
}

/* Introduced in GL es 2.0 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void vertex_attrib4f(u32 index, vec_4_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4f)
    }
    glVertexAttrib4f(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4f"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
            Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void vertex_attrib4fv(u32 index, span_const_vec_4_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4fv)
    }
    glVertexAttrib4fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib4fv"sv);
}

/* Introduced in GL es 2.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void vertex_attrib_pointer(
        u32                               index,
        i32                               size,
        group::vertex_attrib_pointer_type type,
        bool                              normalized,
        i32                               stride,
        span_const_void const&            pointer)
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
    detail::error_check("VertexAttribPointer"sv);
}

/* Introduced in GL es 2.0 */
template<class size_2_i32, class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
                 semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void viewport(vec_2_i32 const& x, size_2_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Viewport)
    }
    glViewport(x.x(), x.y(), width[0], width[1]);
    detail::error_check("Viewport"sv);
}

#endif // GL_ES_VERSION_2_0
