#pragma once

#ifdef GL_OES_fixed_point
namespace gl::oes::fixed_point {
namespace values {
constexpr u32 fixed = 0x140C;
} // namespace values

/*!
 * \brief Part of GL_OES_fixed_point
 * \param func GLenum
 * \param ref GLfixed
 * \return void
 */
STATICINLINE void alpha_funcx(
    group::alpha_function func,
    GLfixed               ref,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AlphaFuncxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glAlphaFuncxOES, func, ref);
#endif
    glAlphaFuncxOES(static_cast<GLenum>(func), ref);
    detail::error_check("AlphaFuncxOES"sv, check_errors);
}

template<class vec_4_GLfixed>
requires(concepts::vector<vec_4_GLfixed, GLfixed, 4>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param red GLfixed
 * \param green GLfixed
 * \param blue GLfixed
 * \param alpha GLfixed
 * \return void
 */
STATICINLINE void clear_colorx(
    vec_4_GLfixed const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearColorxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glClearColorxOES, red);
#endif
    glClearColorxOES(red[0], red[1], red[2], red[3]);
    detail::error_check("ClearColorxOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param depth GLfixed
 * \return void
 */
STATICINLINE void clear_depthx(
    GLfixed depth, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glClearDepthxOES, depth);
#endif
    glClearDepthxOES(depth);
    detail::error_check("ClearDepthxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param plane GLenum
 * \param equation const GLfixed *
 * \return void
 */
STATICINLINE void clip_planex(
    group::clip_plane_name    plane,
    span_const_GLfixed const& equation,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClipPlanexOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glClipPlanexOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(equation.data()),
            equation.size_bytes()),
        plane);
#endif
    glClipPlanexOES(
        static_cast<GLenum>(plane),
        equation.size() ? reinterpret_cast<const GLfixed*>(equation.data())
                        : nullptr);
    detail::error_check("ClipPlanexOES"sv, check_errors);
}

template<class vec_4_GLfixed>
requires(concepts::vector<vec_4_GLfixed, GLfixed, 4>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param red GLfixed
 * \param green GLfixed
 * \param blue GLfixed
 * \param alpha GLfixed
 * \return void
 */
STATICINLINE void color4x(
    vec_4_GLfixed const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color4xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glColor4xOES, red);
#endif
    glColor4xOES(red[0], red[1], red[2], red[3]);
    detail::error_check("Color4xOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param n GLfixed
 * \param f GLfixed
 * \return void
 */
STATICINLINE void depth_rangex(
    GLfixed n, GLfixed f, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangexOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDepthRangexOES, n, f);
#endif
    glDepthRangexOES(n, f);
    detail::error_check("DepthRangexOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void fogx(
    group::fog_prop pname,
    GLfixed         param,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glFogxOES, pname, param);
#endif
    glFogxOES(static_cast<GLenum>(pname), param);
    detail::error_check("FogxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param param const GLfixed *
 * \return void
 */
STATICINLINE void fogxv(
    group::fog_prop           pname,
    span_const_GLfixed const& param,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glFogxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        pname);
#endif
    glFogxvOES(
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfixed*>(param.data())
                     : nullptr);
    detail::error_check("FogxvOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param l GLfixed
 * \param r GLfixed
 * \param b GLfixed
 * \param t GLfixed
 * \param n GLfixed
 * \param f GLfixed
 * \return void
 */
STATICINLINE void frustumx(
    GLfixed     l,
    GLfixed     r,
    GLfixed     b,
    GLfixed     t,
    GLfixed     n,
    GLfixed     f,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FrustumxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glFrustumxOES, l, r, b, t, n, f);
#endif
    glFrustumxOES(l, r, b, t, n, f);
    detail::error_check("FrustumxOES"sv, check_errors);
}

template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param plane GLenum
 * \param equation GLfixed *
 * \return void
 */
STATICINLINE void get_clip_planex(
    group::clip_plane_name plane,
    span_GLfixed           equation,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetClipPlanexOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetClipPlanexOES,
        gsl::span<char>(
            reinterpret_cast<char*>(equation.data()), equation.size_bytes()),
        plane);
#endif
    glGetClipPlanexOES(
        static_cast<GLenum>(plane),
        equation.size() ? reinterpret_cast<GLfixed*>(equation.data())
                        : nullptr);
    detail::error_check("GetClipPlanexOES"sv, check_errors);
}

template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_fixedv(
    group::get_prop pname,
    span_GLfixed    params,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFixedvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetFixedvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        pname);
#endif
    glGetFixedvOES(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetFixedvOES"sv, check_errors);
}

template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_tex_envxv(
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    span_GLfixed                 params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexEnvxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetTexEnvxvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetTexEnvxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetTexEnvxvOES"sv, check_errors);
}

template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_tex_parameterxv(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_GLfixed                 params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetTexParameterxvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetTexParameterxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterxvOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void light_modelx(
    group::light_model_parameter pname,
    GLfixed                      param,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LightModelxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glLightModelxOES, pname, param);
#endif
    glLightModelxOES(static_cast<GLenum>(pname), param);
    detail::error_check("LightModelxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param param const GLfixed *
 * \return void
 */
STATICINLINE void light_modelxv(
    group::light_model_parameter pname,
    span_const_GLfixed const&    param,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LightModelxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glLightModelxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        pname);
#endif
    glLightModelxvOES(
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfixed*>(param.data())
                     : nullptr);
    detail::error_check("LightModelxvOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param light GLenum
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void lightx(
    group::light_name      light,
    group::light_parameter pname,
    GLfixed                param,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LightxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glLightxOES, light, pname, param);
#endif
    glLightxOES(static_cast<GLenum>(light), static_cast<GLenum>(pname), param);
    detail::error_check("LightxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param light GLenum
 * \param pname GLenum
 * \param params const GLfixed *
 * \return void
 */
STATICINLINE void lightxv(
    group::light_name         light,
    group::light_parameter    pname,
    span_const_GLfixed const& params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LightxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glLightxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        light,
        pname);
#endif
    glLightxvOES(
        static_cast<GLenum>(light),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("LightxvOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param width GLfixed
 * \return void
 */
STATICINLINE void line_widthx(
    GLfixed width, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LineWidthxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glLineWidthxOES, width);
#endif
    glLineWidthxOES(width);
    detail::error_check("LineWidthxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param m const GLfixed *
 * \return void
 */
STATICINLINE void load_matrixx(
    span_const_GLfixed const& m, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LoadMatrixxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glLoadMatrixxOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(m.data()), m.size_bytes()));
#endif
    glLoadMatrixxOES(
        m.size() ? reinterpret_cast<const GLfixed*>(m.data()) : nullptr);
    detail::error_check("LoadMatrixxOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param face GLenum
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void materialx(
    group::triangle_face      face,
    group::material_parameter pname,
    GLfixed                   param,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MaterialxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMaterialxOES, face, pname, param);
#endif
    glMaterialxOES(
        static_cast<GLenum>(face), static_cast<GLenum>(pname), param);
    detail::error_check("MaterialxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param face GLenum
 * \param pname GLenum
 * \param param const GLfixed *
 * \return void
 */
STATICINLINE void materialxv(
    group::triangle_face      face,
    group::material_parameter pname,
    span_const_GLfixed const& param,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MaterialxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMaterialxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        face,
        pname);
#endif
    glMaterialxvOES(
        static_cast<GLenum>(face),
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfixed*>(param.data())
                     : nullptr);
    detail::error_check("MaterialxvOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param m const GLfixed *
 * \return void
 */
STATICINLINE void mult_matrixx(
    span_const_GLfixed const& m, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultMatrixxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultMatrixxOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(m.data()), m.size_bytes()));
#endif
    glMultMatrixxOES(
        m.size() ? reinterpret_cast<const GLfixed*>(m.data()) : nullptr);
    detail::error_check("MultMatrixxOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param texture GLenum
 * \param s GLfixed
 * \param t GLfixed
 * \param r GLfixed
 * \param q GLfixed
 * \return void
 */
STATICINLINE void multi_tex_coord4x(
    group::texture_unit texture,
    GLfixed             s,
    GLfixed             t,
    GLfixed             r,
    GLfixed             q,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord4xOES, texture, s, t, r, q);
#endif
    glMultiTexCoord4xOES(static_cast<GLenum>(texture), s, t, r, q);
    detail::error_check("MultiTexCoord4xOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param nx GLfixed
 * \param ny GLfixed
 * \param nz GLfixed
 * \return void
 */
STATICINLINE void normal3x(
    GLfixed     nx,
    GLfixed     ny,
    GLfixed     nz,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Normal3xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glNormal3xOES, nx, ny, nz);
#endif
    glNormal3xOES(nx, ny, nz);
    detail::error_check("Normal3xOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param l GLfixed
 * \param r GLfixed
 * \param b GLfixed
 * \param t GLfixed
 * \param n GLfixed
 * \param f GLfixed
 * \return void
 */
STATICINLINE void orthox(
    GLfixed     l,
    GLfixed     r,
    GLfixed     b,
    GLfixed     t,
    GLfixed     n,
    GLfixed     f,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(OrthoxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glOrthoxOES, l, r, b, t, n, f);
#endif
    glOrthoxOES(l, r, b, t, n, f);
    detail::error_check("OrthoxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param params const GLfixed *
 * \return void
 */
STATICINLINE void point_parameterxv(
    group::point_parameter_name_arb pname,
    span_const_GLfixed const&       params,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glPointParameterxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        pname);
#endif
    glPointParameterxvOES(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("PointParameterxvOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param size GLfixed
 * \return void
 */
STATICINLINE void point_sizex(
    GLfixed size, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointSizexOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPointSizexOES, size);
#endif
    glPointSizexOES(size);
    detail::error_check("PointSizexOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param factor GLfixed
 * \param units GLfixed
 * \return void
 */
STATICINLINE void polygon_offsetx(
    GLfixed factor, GLfixed units, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffsetxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPolygonOffsetxOES, factor, units);
#endif
    glPolygonOffsetxOES(factor, units);
    detail::error_check("PolygonOffsetxOES"sv, check_errors);
}

template<class vec_3_GLfixed>
requires(concepts::vector<vec_3_GLfixed, GLfixed, 3>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param angle GLfixed
 * \param x GLfixed
 * \param y GLfixed
 * \param z GLfixed
 * \return void
 */
STATICINLINE void rotatex(
    GLfixed              angle,
    vec_3_GLfixed const& x,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RotatexOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glRotatexOES, angle, x);
#endif
    glRotatexOES(angle, x[0], x[1], x[2]);
    detail::error_check("RotatexOES"sv, check_errors);
}

template<class vec_3_GLfixed>
requires(concepts::vector<vec_3_GLfixed, GLfixed, 3>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x GLfixed
 * \param y GLfixed
 * \param z GLfixed
 * \return void
 */
STATICINLINE void scalex(
    vec_3_GLfixed const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScalexOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glScalexOES, x);
#endif
    glScalexOES(x[0], x[1], x[2]);
    detail::error_check("ScalexOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void tex_envx(
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    GLfixed                      param,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexEnvxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexEnvxOES, target, pname, param);
#endif
    glTexEnvxOES(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("TexEnvxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfixed *
 * \return void
 */
STATICINLINE void tex_envxv(
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    span_const_GLfixed const&    params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexEnvxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexEnvxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glTexEnvxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("TexEnvxvOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void tex_parameterx(
    group::texture_target        target,
    group::get_texture_parameter pname,
    GLfixed                      param,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexParameterxOES, target, pname, param);
#endif
    glTexParameterxOES(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("TexParameterxOES"sv, check_errors);
}

template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfixed *
 * \return void
 */
STATICINLINE void tex_parameterxv(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_const_GLfixed const&    params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexParameterxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glTexParameterxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterxvOES"sv, check_errors);
}

template<class vec_3_GLfixed>
requires(concepts::vector<vec_3_GLfixed, GLfixed, 3>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x GLfixed
 * \param y GLfixed
 * \param z GLfixed
 * \return void
 */
STATICINLINE void translatex(
    vec_3_GLfixed const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TranslatexOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTranslatexOES, x);
#endif
    glTranslatexOES(x[0], x[1], x[2]);
    detail::error_check("TranslatexOES"sv, check_errors);
}

#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param light GLenum
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_lightxv(
    group::light_name      light,
    group::light_parameter pname,
    span_GLfixed           params,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetLightxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetLightxvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        light,
        pname);
#endif
    glGetLightxvOES(
        static_cast<GLenum>(light),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetLightxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param face GLenum
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_materialxv(
    group::triangle_face      face,
    group::material_parameter pname,
    span_GLfixed              params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMaterialxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetMaterialxvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        face,
        pname);
#endif
    glGetMaterialxvOES(
        static_cast<GLenum>(face),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetMaterialxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void point_parameterx(
    group::point_parameter_name_arb pname,
    GLfixed                         param,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPointParameterxOES, pname, param);
#endif
    glPointParameterxOES(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameterxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param value GLclampx
 * \param invert GLboolean
 * \return void
 */
STATICINLINE void sample_coveragex(
    GLclampx value, bool invert, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleCoveragexOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glSampleCoveragexOES, value, invert);
#endif
    glSampleCoveragexOES(value, invert);
    detail::error_check("SampleCoveragexOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param op GLenum
 * \param value GLfixed
 * \return void
 */
STATICINLINE void accumx(
    GLenum op, GLfixed value, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AccumxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glAccumxOES, op, value);
#endif
    glAccumxOES(op, value);
    detail::error_check("AccumxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class size_2_i32, class span_const_u8>
requires(
    concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param width GLsizei
 * \param height GLsizei
 * \param xorig GLfixed
 * \param yorig GLfixed
 * \param xmove GLfixed
 * \param ymove GLfixed
 * \param bitmap const GLubyte *
 * \return void
 */
STATICINLINE void bitmapx(
    size_2_i32 const&    width,
    GLfixed              xorig,
    GLfixed              yorig,
    GLfixed              xmove,
    GLfixed              ymove,
    span_const_u8 const& bitmap,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BitmapxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glBitmapxOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(bitmap.data()), bitmap.size_bytes()),
        width,
        xorig,
        yorig,
        xmove,
        ymove);
#endif
    glBitmapxOES(
        width[0],
        width[1],
        xorig,
        yorig,
        xmove,
        ymove,
        bitmap.size() ? reinterpret_cast<const GLubyte*>(bitmap.data())
                      : nullptr);
    detail::error_check("BitmapxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_4_GLfixed>
requires(concepts::vector<vec_4_GLfixed, GLfixed, 4>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param red GLfixed
 * \param green GLfixed
 * \param blue GLfixed
 * \param alpha GLfixed
 * \return void
 */
STATICINLINE void blend_colorx(
    vec_4_GLfixed const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColorxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendColorxOES, red);
#endif
    glBlendColorxOES(red[0], red[1], red[2], red[3]);
    detail::error_check("BlendColorxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_4_GLfixed>
requires(concepts::vector<vec_4_GLfixed, GLfixed, 4>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param red GLfixed
 * \param green GLfixed
 * \param blue GLfixed
 * \param alpha GLfixed
 * \return void
 */
STATICINLINE void clear_accumx(
    vec_4_GLfixed const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearAccumxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glClearAccumxOES, red);
#endif
    glClearAccumxOES(red[0], red[1], red[2], red[3]);
    detail::error_check("ClearAccumxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_3_GLfixed>
requires(concepts::vector<vec_3_GLfixed, GLfixed, 3>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param red GLfixed
 * \param green GLfixed
 * \param blue GLfixed
 * \return void
 */
STATICINLINE void color3x(
    vec_3_GLfixed const& red, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color3xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glColor3xOES, red);
#endif
    glColor3xOES(red[0], red[1], red[2]);
    detail::error_check("Color3xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param components const GLfixed *
 * \return void
 */
STATICINLINE void color3xv(
    span_const_GLfixed const& components,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color3xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glColor3xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(components.data()),
            components.size_bytes()));
#endif
    glColor3xvOES(
        components.size() ? reinterpret_cast<const GLfixed*>(components.data())
                          : nullptr);
    detail::error_check("Color3xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param components const GLfixed *
 * \return void
 */
STATICINLINE void color4xv(
    span_const_GLfixed const& components,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color4xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glColor4xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(components.data()),
            components.size_bytes()));
#endif
    glColor4xvOES(
        components.size() ? reinterpret_cast<const GLfixed*>(components.data())
                          : nullptr);
    detail::error_check("Color4xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void convolution_parameterx(
    group::convolution_target_ext target,
    group::convolution_parameter  pname,
    GLfixed                       param,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glConvolutionParameterxOES, target, pname, param);
#endif
    glConvolutionParameterxOES(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("ConvolutionParameterxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLfixed *
 * \return void
 */
STATICINLINE void convolution_parameterxv(
    group::convolution_target_ext target,
    group::convolution_parameter  pname,
    span_const_GLfixed const&     params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glConvolutionParameterxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glConvolutionParameterxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("ConvolutionParameterxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param u GLfixed
 * \return void
 */
STATICINLINE void eval_coord1x(
    GLfixed u, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalCoord1xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEvalCoord1xOES, u);
#endif
    glEvalCoord1xOES(u);
    detail::error_check("EvalCoord1xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void eval_coord1xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalCoord1xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glEvalCoord1xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glEvalCoord1xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("EvalCoord1xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param u GLfixed
 * \param v GLfixed
 * \return void
 */
STATICINLINE void eval_coord2x(
    GLfixed u, GLfixed v, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalCoord2xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEvalCoord2xOES, u, v);
#endif
    glEvalCoord2xOES(u, v);
    detail::error_check("EvalCoord2xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void eval_coord2xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalCoord2xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glEvalCoord2xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glEvalCoord2xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("EvalCoord2xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param n GLsizei
 * \param type GLenum
 * \param buffer const GLfixed *
 * \return void
 */
STATICINLINE void feedback_bufferx(
    GLenum                    type,
    span_const_GLfixed const& buffer,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FeedbackBufferxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glFeedbackBufferxOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(buffer.data()), buffer.size_bytes()),
        type);
#endif
    glFeedbackBufferxOES(
        buffer.size(),
        type,
        buffer.size() ? reinterpret_cast<const GLfixed*>(buffer.data())
                      : nullptr);
    detail::error_check("FeedbackBufferxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_convolution_parameterxv(
    GLenum       target,
    GLenum       pname,
    span_GLfixed params,
    error_check  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetConvolutionParameterxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetConvolutionParameterxvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetConvolutionParameterxvOES(
        target,
        pname,
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetConvolutionParameterxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_histogram_parameterxv(
    group::histogram_target_ext             target,
    group::get_histogram_parameter_prop_ext pname,
    span_GLfixed                            params,
    error_check                             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHistogramParameterxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetHistogramParameterxvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetHistogramParameterxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetHistogramParameterxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param light GLenum
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_lightx(
    group::light_name      light,
    group::light_parameter pname,
    span_GLfixed           params,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetLightxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetLightxOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        light,
        pname);
#endif
    glGetLightxOES(
        static_cast<GLenum>(light),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetLightxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param query GLenum
 * \param v GLfixed *
 * \return void
 */
STATICINLINE void get_mapxv(
    group::map_target    target,
    group::get_map_query query,
    span_GLfixed         v,
    error_check          check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMapxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetMapxvOES,
        gsl::span<char>(reinterpret_cast<char*>(v.data()), v.size_bytes()),
        target,
        query);
#endif
    glGetMapxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(query),
        v.size() ? reinterpret_cast<GLfixed*>(v.data()) : nullptr);
    detail::error_check("GetMapxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param face GLenum
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void get_materialx(
    group::triangle_face      face,
    group::material_parameter pname,
    GLfixed                   param,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMaterialxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetMaterialxOES, face, pname, param);
#endif
    glGetMaterialxOES(
        static_cast<GLenum>(face), static_cast<GLenum>(pname), param);
    detail::error_check("GetMaterialxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param map GLenum
 * \param size GLint
 * \param values GLfixed *
 * \return void
 */
STATICINLINE void get_pixel_mapxv(
    group::pixel_map map,
    span_GLfixed     values,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPixelMapxv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetPixelMapxv,
        gsl::span<char>(
            reinterpret_cast<char*>(values.data()), values.size_bytes()),
        map);
#endif
    glGetPixelMapxv(
        static_cast<GLenum>(map),
        values.size(),
        values.size() ? reinterpret_cast<GLfixed*>(values.data()) : nullptr);
    detail::error_check("GetPixelMapxv"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coord GLenum
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_tex_genxv(
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    span_GLfixed                 params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexGenxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetTexGenxvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        coord,
        pname);
#endif
    glGetTexGenxvOES(
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetTexGenxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(
    concepts::span<span_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param level GLint
 * \param pname GLenum
 * \param params GLfixed *
 * \return void
 */
STATICINLINE void get_tex_level_parameterxv(
    group::texture_target        target,
    i32                          level,
    group::get_texture_parameter pname,
    span_GLfixed                 params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexLevelParameterxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetTexLevelParameterxvOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        level,
        pname);
#endif
    glGetTexLevelParameterxvOES(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetTexLevelParameterxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param component GLfixed
 * \return void
 */
STATICINLINE void indexx(
    GLfixed component, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IndexxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIndexxOES, component);
#endif
    glIndexxOES(component);
    detail::error_check("IndexxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param component const GLfixed *
 * \return void
 */
STATICINLINE void indexxv(
    span_const_GLfixed const& component,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IndexxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glIndexxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(component.data()),
            component.size_bytes()));
#endif
    glIndexxvOES(
        component.size() ? reinterpret_cast<const GLfixed*>(component.data())
                         : nullptr);
    detail::error_check("IndexxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param m const GLfixed *
 * \return void
 */
STATICINLINE void load_transpose_matrixx(
    span_const_GLfixed const& m, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LoadTransposeMatrixxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glLoadTransposeMatrixxOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(m.data()), m.size_bytes()));
#endif
    glLoadTransposeMatrixxOES(
        m.size() ? reinterpret_cast<const GLfixed*>(m.data()) : nullptr);
    detail::error_check("LoadTransposeMatrixxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param u1 GLfixed
 * \param u2 GLfixed
 * \param stride GLint
 * \param order GLint
 * \param points GLfixed
 * \return void
 */
STATICINLINE void map1x(
    group::map_target target,
    GLfixed           u1,
    GLfixed           u2,
    i32               stride,
    i32               order,
    GLfixed           points,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Map1xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMap1xOES, target, u1, u2, stride, order, points);
#endif
    glMap1xOES(static_cast<GLenum>(target), u1, u2, stride, order, points);
    detail::error_check("Map1xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param target GLenum
 * \param u1 GLfixed
 * \param u2 GLfixed
 * \param ustride GLint
 * \param uorder GLint
 * \param v1 GLfixed
 * \param v2 GLfixed
 * \param vstride GLint
 * \param vorder GLint
 * \param points GLfixed
 * \return void
 */
STATICINLINE void map2x(
    group::map_target target,
    GLfixed           u1,
    GLfixed           u2,
    i32               ustride,
    i32               uorder,
    GLfixed           v1,
    GLfixed           v2,
    i32               vstride,
    i32               vorder,
    GLfixed           points,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Map2xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glMap2xOES,
        target,
        u1,
        u2,
        ustride,
        uorder,
        v1,
        v2,
        vstride,
        vorder,
        points);
#endif
    glMap2xOES(
        static_cast<GLenum>(target),
        u1,
        u2,
        ustride,
        uorder,
        v1,
        v2,
        vstride,
        vorder,
        points);
    detail::error_check("Map2xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param n GLint
 * \param u1 GLfixed
 * \param u2 GLfixed
 * \return void
 */
STATICINLINE void map_grid1x(
    i32 n, GLfixed u1, GLfixed u2, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapGrid1xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMapGrid1xOES, n, u1, u2);
#endif
    glMapGrid1xOES(n, u1, u2);
    detail::error_check("MapGrid1xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param n GLint
 * \param u1 GLfixed
 * \param u2 GLfixed
 * \param v1 GLfixed
 * \param v2 GLfixed
 * \return void
 */
STATICINLINE void map_grid2x(
    i32         n,
    GLfixed     u1,
    GLfixed     u2,
    GLfixed     v1,
    GLfixed     v2,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapGrid2xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMapGrid2xOES, n, u1, u2, v1, v2);
#endif
    glMapGrid2xOES(n, u1, u2, v1, v2);
    detail::error_check("MapGrid2xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param m const GLfixed *
 * \return void
 */
STATICINLINE void mult_transpose_matrixx(
    span_const_GLfixed const& m, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultTransposeMatrixxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultTransposeMatrixxOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(m.data()), m.size_bytes()));
#endif
    glMultTransposeMatrixxOES(
        m.size() ? reinterpret_cast<const GLfixed*>(m.data()) : nullptr);
    detail::error_check("MultTransposeMatrixxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param texture GLenum
 * \param s GLfixed
 * \return void
 */
STATICINLINE void multi_tex_coord1x(
    group::texture_unit texture,
    GLfixed             s,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord1xOES, texture, s);
#endif
    glMultiTexCoord1xOES(static_cast<GLenum>(texture), s);
    detail::error_check("MultiTexCoord1xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param texture GLenum
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void multi_tex_coord1xv(
    group::texture_unit       texture,
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord1xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()),
        texture);
#endif
    glMultiTexCoord1xvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord1xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param texture GLenum
 * \param s GLfixed
 * \param t GLfixed
 * \return void
 */
STATICINLINE void multi_tex_coord2x(
    group::texture_unit texture,
    GLfixed             s,
    GLfixed             t,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord2xOES, texture, s, t);
#endif
    glMultiTexCoord2xOES(static_cast<GLenum>(texture), s, t);
    detail::error_check("MultiTexCoord2xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param texture GLenum
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void multi_tex_coord2xv(
    group::texture_unit       texture,
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord2xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()),
        texture);
#endif
    glMultiTexCoord2xvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord2xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param texture GLenum
 * \param s GLfixed
 * \param t GLfixed
 * \param r GLfixed
 * \return void
 */
STATICINLINE void multi_tex_coord3x(
    group::texture_unit texture,
    GLfixed             s,
    GLfixed             t,
    GLfixed             r,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMultiTexCoord3xOES, texture, s, t, r);
#endif
    glMultiTexCoord3xOES(static_cast<GLenum>(texture), s, t, r);
    detail::error_check("MultiTexCoord3xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param texture GLenum
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void multi_tex_coord3xv(
    group::texture_unit       texture,
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord3xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()),
        texture);
#endif
    glMultiTexCoord3xvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord3xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param texture GLenum
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void multi_tex_coord4xv(
    group::texture_unit       texture,
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glMultiTexCoord4xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()),
        texture);
#endif
    glMultiTexCoord4xvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord4xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void normal3xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Normal3xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glNormal3xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glNormal3xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("Normal3xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param token GLfixed
 * \return void
 */
STATICINLINE void pass_throughx(
    GLfixed token, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PassThroughxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPassThroughxOES, token);
#endif
    glPassThroughxOES(token);
    detail::error_check("PassThroughxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param map GLenum
 * \param size GLint
 * \param values const GLfixed *
 * \return void
 */
STATICINLINE void pixel_mapx(
    group::pixel_map          map,
    span_const_GLfixed const& values,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelMapx)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glPixelMapx,
        gsl::span<const char>(
            reinterpret_cast<const char*>(values.data()), values.size_bytes()),
        map);
#endif
    glPixelMapx(
        static_cast<GLenum>(map),
        values.size(),
        values.size() ? reinterpret_cast<const GLfixed*>(values.data())
                      : nullptr);
    detail::error_check("PixelMapx"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void pixel_storex(
    group::pixel_store_parameter pname,
    GLfixed                      param,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelStorex)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPixelStorex, pname, param);
#endif
    glPixelStorex(static_cast<GLenum>(pname), param);
    detail::error_check("PixelStorex"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void pixel_transferx(
    group::pixel_transfer_parameter pname,
    GLfixed                         param,
    error_check                     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelTransferxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPixelTransferxOES, pname, param);
#endif
    glPixelTransferxOES(static_cast<GLenum>(pname), param);
    detail::error_check("PixelTransferxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param xfactor GLfixed
 * \param yfactor GLfixed
 * \return void
 */
STATICINLINE void pixel_zoomx(
    GLfixed     xfactor,
    GLfixed     yfactor,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelZoomxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPixelZoomxOES, xfactor, yfactor);
#endif
    glPixelZoomxOES(xfactor, yfactor);
    detail::error_check("PixelZoomxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed, class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param n GLsizei
 * \param textures const GLuint *
 * \param priorities const GLfixed *
 * \return void
 */
STATICINLINE void prioritize_texturesx(
    span_const_u32 const&     textures,
    span_const_GLfixed const& priorities,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrioritizeTexturesxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glPrioritizeTexturesxOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(textures.data()),
            textures.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(priorities.data()),
            priorities.size_bytes()));
#endif
    glPrioritizeTexturesxOES(
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr,
        priorities.size() ? reinterpret_cast<const GLfixed*>(priorities.data())
                          : nullptr);
    detail::error_check("PrioritizeTexturesxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_GLfixed>
requires(concepts::vector<vec_2_GLfixed, GLfixed, 2>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x GLfixed
 * \param y GLfixed
 * \return void
 */
STATICINLINE void raster_pos2x(
    vec_2_GLfixed const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos2xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glRasterPos2xOES, x);
#endif
    glRasterPos2xOES(x[0], x[1]);
    detail::error_check("RasterPos2xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void raster_pos2xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos2xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glRasterPos2xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glRasterPos2xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("RasterPos2xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_3_GLfixed>
requires(concepts::vector<vec_3_GLfixed, GLfixed, 3>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x GLfixed
 * \param y GLfixed
 * \param z GLfixed
 * \return void
 */
STATICINLINE void raster_pos3x(
    vec_3_GLfixed const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos3xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glRasterPos3xOES, x);
#endif
    glRasterPos3xOES(x[0], x[1], x[2]);
    detail::error_check("RasterPos3xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void raster_pos3xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos3xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glRasterPos3xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glRasterPos3xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("RasterPos3xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_4_GLfixed>
requires(concepts::vector<vec_4_GLfixed, GLfixed, 4>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x GLfixed
 * \param y GLfixed
 * \param z GLfixed
 * \param w GLfixed
 * \return void
 */
STATICINLINE void raster_pos4x(
    vec_4_GLfixed const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos4xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glRasterPos4xOES, x);
#endif
    glRasterPos4xOES(x[0], x[1], x[2], x[3]);
    detail::error_check("RasterPos4xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void raster_pos4xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos4xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glRasterPos4xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glRasterPos4xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("RasterPos4xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x1 GLfixed
 * \param y1 GLfixed
 * \param x2 GLfixed
 * \param y2 GLfixed
 * \return void
 */
STATICINLINE void rectx(
    GLfixed     x1,
    GLfixed     y1,
    GLfixed     x2,
    GLfixed     y2,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RectxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glRectxOES, x1, y1, x2, y2);
#endif
    glRectxOES(x1, y1, x2, y2);
    detail::error_check("RectxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param v1 const GLfixed *
 * \param v2 const GLfixed *
 * \return void
 */
STATICINLINE void rectxv(
    span_const_GLfixed const& v1,
    span_const_GLfixed const& v2,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RectxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glRectxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v1.data()), v1.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(v2.data()), v2.size_bytes()));
#endif
    glRectxvOES(
        v1.size() ? reinterpret_cast<const GLfixed*>(v1.data()) : nullptr,
        v2.size() ? reinterpret_cast<const GLfixed*>(v2.data()) : nullptr);
    detail::error_check("RectxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param s GLfixed
 * \return void
 */
STATICINLINE void tex_coord1x(
    GLfixed s, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord1xOES, s);
#endif
    glTexCoord1xOES(s);
    detail::error_check("TexCoord1xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void tex_coord1xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexCoord1xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glTexCoord1xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord1xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param s GLfixed
 * \param t GLfixed
 * \return void
 */
STATICINLINE void tex_coord2x(
    GLfixed s, GLfixed t, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord2xOES, s, t);
#endif
    glTexCoord2xOES(s, t);
    detail::error_check("TexCoord2xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void tex_coord2xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexCoord2xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glTexCoord2xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord2xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param s GLfixed
 * \param t GLfixed
 * \param r GLfixed
 * \return void
 */
STATICINLINE void tex_coord3x(
    GLfixed s, GLfixed t, GLfixed r, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord3xOES, s, t, r);
#endif
    glTexCoord3xOES(s, t, r);
    detail::error_check("TexCoord3xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void tex_coord3xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexCoord3xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glTexCoord3xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord3xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param s GLfixed
 * \param t GLfixed
 * \param r GLfixed
 * \param q GLfixed
 * \return void
 */
STATICINLINE void tex_coord4x(
    GLfixed     s,
    GLfixed     t,
    GLfixed     r,
    GLfixed     q,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexCoord4xOES, s, t, r, q);
#endif
    glTexCoord4xOES(s, t, r, q);
    detail::error_check("TexCoord4xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void tex_coord4xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexCoord4xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glTexCoord4xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord4xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coord GLenum
 * \param pname GLenum
 * \param param GLfixed
 * \return void
 */
STATICINLINE void tex_genx(
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    GLfixed                      param,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexGenxOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexGenxOES, coord, pname, param);
#endif
    glTexGenxOES(static_cast<GLenum>(coord), static_cast<GLenum>(pname), param);
    detail::error_check("TexGenxOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coord GLenum
 * \param pname GLenum
 * \param params const GLfixed *
 * \return void
 */
STATICINLINE void tex_genxv(
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    span_const_GLfixed const&    params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexGenxvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexGenxvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        coord,
        pname);
#endif
    glTexGenxvOES(
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("TexGenxvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x GLfixed
 * \return void
 */
STATICINLINE void vertex2x(
    GLfixed x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex2xOES, x);
#endif
    glVertex2xOES(x);
    detail::error_check("Vertex2xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void vertex2xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertex2xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glVertex2xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex2xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_GLfixed>
requires(concepts::vector<vec_2_GLfixed, GLfixed, 2>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x GLfixed
 * \param y GLfixed
 * \return void
 */
STATICINLINE void vertex3x(
    vec_2_GLfixed const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex3xOES, x);
#endif
    glVertex3xOES(x[0], x[1]);
    detail::error_check("Vertex3xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void vertex3xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertex3xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glVertex3xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex3xvOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_3_GLfixed>
requires(concepts::vector<vec_3_GLfixed, GLfixed, 3>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param x GLfixed
 * \param y GLfixed
 * \param z GLfixed
 * \return void
 */
STATICINLINE void vertex4x(
    vec_3_GLfixed const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4xOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertex4xOES, x);
#endif
    glVertex4xOES(x[0], x[1], x[2]);
    detail::error_check("Vertex4xOES"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(
    concepts::span<span_const_GLfixed> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLfixed::value_type>,
        std::decay_t<GLfixed>>)
/*!
 * \brief Part of GL_OES_fixed_point
 * \param coords const GLfixed *
 * \return void
 */
STATICINLINE void vertex4xv(
    span_const_GLfixed const& coords,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4xvOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glVertex4xvOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(coords.data()), coords.size_bytes()));
#endif
    glVertex4xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex4xvOES"sv, check_errors);
}

#endif
} // namespace gl::oes::fixed_point
#endif // GL_OES_fixed_point
namespace gl::oes::fixed_point {
constexpr auto name = "GL_OES_fixed_point";
}
