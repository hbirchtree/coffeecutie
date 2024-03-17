#ifdef GL_VERSION_1_0
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glBlendFunc. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glClear. Introduced in GL core 1.0
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
    MinimumVersion<Current, Version<1, 0>> &&
    concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Wraps around glClearColor. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glClearDepth. Introduced in GL core 1.0
 * \param depth GLdouble
 * \return void
 */
STATICINLINE void clear_depth(
    f64 depth, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepth)
    }
    glClearDepth(depth);
    detail::error_check("ClearDepth"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glClearStencil. Introduced in GL core 1.0
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
    MinimumVersion<Current, Version<1, 0>> &&
    concepts::vector<vec_4_bool, bool, 4>)
/*!
 * \brief Wraps around glColorMask. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glCullFace. Introduced in GL core 1.0
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glDepthFunc. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glDepthMask. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glDepthRange. Introduced in GL core 1.0
 * \param n GLdouble
 * \param f GLdouble
 * \return void
 */
STATICINLINE void depth_range(
    f64 n, f64 f, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRange)
    }
    glDepthRange(n, f);
    detail::error_check("DepthRange"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glDisable. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glDrawBuffer. Introduced in GL core 1.0
 * \param buf GLenum
 * \return void
 */
STATICINLINE void draw_buffer(
    group::draw_buffer_mode buf, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawBuffer)
    }
    glDrawBuffer(static_cast<GLenum>(buf));
    detail::error_check("DrawBuffer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glEnable. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glFinish. Introduced in GL core 1.0

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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glFlush. Introduced in GL core 1.0

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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glFrontFace. Introduced in GL core 1.0
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

template<class span_bool>
requires(
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_bool> &&
    std::is_same_v<
        std::decay_t<typename span_bool::value_type>,
        std::decay_t<bool>>)
/*!
 * \brief Wraps around glGetBooleanv. Introduced in GL core 1.0
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

template<class span_f64>
requires(
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_f64> &&
    std::is_same_v<
        std::decay_t<typename span_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Wraps around glGetDoublev. Introduced in GL core 1.0
 * \param pname GLenum
 * \param data GLdouble *
 * \return void
 */
STATICINLINE void get_doublev(
    group::get_prop pname,
    span_f64        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDoublev)
    }
    glGetDoublev(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLdouble*>(data.data()) : nullptr);
    detail::error_check("GetDoublev"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glGetError. Introduced in GL core 1.0

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
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetFloatv. Introduced in GL core 1.0
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
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetIntegerv. Introduced in GL core 1.0
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glGetString. Introduced in GL core 1.0
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

template<class span_void>
requires(MinimumVersion<Current, Version<1, 0>> && concepts::span<span_void>)
/*!
 * \brief Wraps around glGetTexImage. Introduced in GL core 1.0
 * \param target GLenum
 * \param level GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels void *
 * \return void
 */
STATICINLINE void get_tex_image(
    group::texture_target target,
    i32                   level,
    group::pixel_format   format,
    group::pixel_type     type,
    span_void             pixels,
    error_check           check_errors = error_check::on)
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
    detail::error_check("GetTexImage"sv, check_errors);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetTexLevelParameterfv. Introduced in GL core 1.0
 * \param target GLenum
 * \param level GLint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_tex_level_parameter(
    group::texture_target        target,
    i32                          level,
    group::get_texture_parameter pname,
    span_f32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetTexLevelParameterfv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetTexLevelParameteriv. Introduced in GL core 1.0
 * \param target GLenum
 * \param level GLint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_tex_level_parameter(
    group::texture_target        target,
    i32                          level,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
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
    detail::error_check("GetTexLevelParameteriv"sv, check_errors);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetTexParameterfv. Introduced in GL core 1.0
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
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetTexParameteriv. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glHint. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glIsEnabled. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glLineWidth. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glLogicOp. Introduced in GL core 1.0
 * \param opcode GLenum
 * \return void
 */
STATICINLINE void logic_op(
    group::logic_op opcode, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LogicOp)
    }
    glLogicOp(static_cast<GLenum>(opcode));
    detail::error_check("LogicOp"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glPixelStoref. Introduced in GL core 1.0
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void pixel_storef(
    group::pixel_store_parameter pname,
    f32                          param,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelStoref)
    }
    glPixelStoref(static_cast<GLenum>(pname), param);
    detail::error_check("PixelStoref"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glPixelStorei. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glPointSize. Introduced in GL core 1.0
 * \param size GLfloat
 * \return void
 */
STATICINLINE void point_size(
    f32 size, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointSize)
    }
    glPointSize(size);
    detail::error_check("PointSize"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glPolygonMode. Introduced in GL core 1.0
 * \param face GLenum
 * \param mode GLenum
 * \return void
 */
STATICINLINE void polygon_mode(
    group::triangle_face face,
    group::polygon_mode  mode,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonMode)
    }
    glPolygonMode(static_cast<GLenum>(face), static_cast<GLenum>(mode));
    detail::error_check("PolygonMode"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glReadBuffer. Introduced in GL core 1.0
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
    glReadBuffer(static_cast<GLenum>(src));
    detail::error_check("ReadBuffer"sv, check_errors);
}

template<class size_2_i32, class span_void, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_void>)
/*!
 * \brief Wraps around glReadPixels. Introduced in GL core 1.0
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

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glScissor. Introduced in GL core 1.0
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glStencilFunc. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glStencilMask. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glStencilOp. Introduced in GL core 1.0
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

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glTexImage1D. Introduced in GL core 1.0
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLint
 * \param width GLsizei
 * \param border GLint
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_image_1d(
    group::texture_target  target,
    i32                    level,
    i32                    internalformat,
    i32                    width,
    i32                    border,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
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
    detail::error_check("TexImage1D"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(
    MinimumVersion<Current, Version<1, 0>> &&
    concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glTexImage2D. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glTexParameterf. Introduced in GL core 1.0
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
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glTexParameterfv. Introduced in GL core 1.0
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
requires(MinimumVersion<Current, Version<1, 0>>)
/*!
 * \brief Wraps around glTexParameteri. Introduced in GL core 1.0
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
    MinimumVersion<Current, Version<1, 0>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glTexParameteriv. Introduced in GL core 1.0
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

template<class size_2_i32, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<1, 0>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glViewport. Introduced in GL core 1.0
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

#endif // GL_VERSION_1_0
