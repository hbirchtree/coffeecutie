#ifdef GL_VERSION_1_0
/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE void blend_func(
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
    void clear_depth(f64 depth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepth)
    }
    glClearDepth(depth);
    detail::error_check("ClearDepth"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<class vec_4_bool>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
    void depth_range(f64 n, f64 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRange)
    }
    glDepthRange(n, f);
    detail::error_check("DepthRange"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
    void draw_buffer(group::draw_buffer_mode buf)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawBuffer)
    }
    glDrawBuffer(static_cast<GLenum>(buf));
    detail::error_check("DrawBuffer"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE void finish()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Finish)
    }
    glFinish();
    detail::error_check("Finish"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE void flush()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Flush)
    }
    glFlush();
    detail::error_check("Flush"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<class span_bool>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<class span_f64>
requires(MinimumVersion<Current, Version<1, 0>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void get_doublev(group::get_prop pname, span_f64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDoublev)
    }
    glGetDoublev(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLdouble*>(data.data()) : nullptr);
    detail::error_check("GetDoublev"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE GLenum get_error()
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

/* Introduced in GL core 1.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE stl_types::String
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

/* Introduced in GL core 1.0 */
template<class span_void>
requires(MinimumVersion<Current, Version<1, 0>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_tex_image(
        group::texture_target target,
        i32                   level,
        group::pixel_format   format,
        group::pixel_type     type,
        span_void             pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexImage)
    }
    glGetTexImage(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<void*>(pixels.data()) : nullptr);
    detail::error_check("GetTexImage"sv);
}

/* Introduced in GL core 1.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<1, 0>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_tex_level_parameter(
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexLevelParameterfv)
    }
    glGetTexLevelParameterfv(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTexLevelParameterfv"sv);
}

/* Introduced in GL core 1.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<1, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_tex_level_parameter(
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexLevelParameteriv)
    }
    glGetTexLevelParameteriv(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexLevelParameteriv"sv);
}

/* Introduced in GL core 1.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE GLboolean
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
    void logic_op(group::logic_op opcode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LogicOp)
    }
    glLogicOp(static_cast<GLenum>(opcode));
    detail::error_check("LogicOp"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
    void pixel_storef(group::pixel_store_parameter pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelStoref)
    }
    glPixelStoref(static_cast<GLenum>(pname), param);
    detail::error_check("PixelStoref"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
    void point_size(f32 size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointSize)
    }
    glPointSize(size);
    detail::error_check("PointSize"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
    void polygon_mode(group::material_face face, group::polygon_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonMode)
    }
    glPolygonMode(static_cast<GLenum>(face), static_cast<GLenum>(mode));
    detail::error_check("PolygonMode"sv);
}

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<class size_2_i32, class span_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<class size_2_i32, class vec_2_i32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE void stencil_op(
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

/* Introduced in GL core 1.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<1, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void tex_image_1d(
        group::texture_target  target,
        i32                    level,
        i32                    internalformat,
        i32                    width,
        i32                    border,
        group::pixel_format    format,
        group::pixel_type      type,
        span_const_void const& pixels)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexImage1D)
    }
    glTexImage1D(
        static_cast<GLenum>(target),
        level,
        internalformat,
        width,
        border,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexImage1D"sv);
}

/* Introduced in GL core 1.0 */
template<class size_2_i32, class span_const_void>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>) STATICINLINE
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

/* Introduced in GL core 1.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

/* Introduced in GL core 1.0 */
template<class size_2_i32, class vec_2_i32>
requires(MinimumVersion<Current, Version<1, 0>>&&
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

#endif // GL_VERSION_1_0
