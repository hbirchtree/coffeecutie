#pragma once

#ifdef GL_OES_fixed_point
namespace gl::oes::fixed_point {
namespace values {
constexpr libc_types::u32 fixed = 0x140C;
} // namespace values
STATICINLINE void alpha_funcx(group::alpha_function func, GLfixed ref)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AlphaFuncxOES)
    }
    glAlphaFuncxOES(static_cast<GLenum>(func), ref);
    detail::error_check("AlphaFuncxOES"sv);
}

template<class vec_4_GLfixed>
requires(semantic::concepts::Vector<vec_4_GLfixed, GLfixed, 4>) STATICINLINE
    void clear_colorx(vec_4_GLfixed const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearColorxOES)
    }
    glClearColorxOES(red.x(), red.y(), red.z(), red.w());
    detail::error_check("ClearColorxOES"sv);
}

STATICINLINE void clear_depthx(GLfixed depth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthxOES)
    }
    glClearDepthxOES(depth);
    detail::error_check("ClearDepthxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void clip_planex(
        group::clip_plane_name plane, span_const_GLfixed const& equation)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClipPlanexOES)
    }
    glClipPlanexOES(
        static_cast<GLenum>(plane),
        equation.size() ? reinterpret_cast<const GLfixed*>(equation.data())
                        : nullptr);
    detail::error_check("ClipPlanexOES"sv);
}

template<class vec_4_GLfixed>
requires(semantic::concepts::Vector<vec_4_GLfixed, GLfixed, 4>) STATICINLINE
    void color4x(vec_4_GLfixed const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color4xOES)
    }
    glColor4xOES(red.x(), red.y(), red.z(), red.w());
    detail::error_check("Color4xOES"sv);
}

STATICINLINE void depth_rangex(GLfixed n, GLfixed f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangexOES)
    }
    glDepthRangexOES(n, f);
    detail::error_check("DepthRangexOES"sv);
}

STATICINLINE void fogx(group::fog_prop pname, GLfixed param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogxOES)
    }
    glFogxOES(static_cast<GLenum>(pname), param);
    detail::error_check("FogxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void fogxv(group::fog_prop pname, span_const_GLfixed const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FogxvOES)
    }
    glFogxvOES(
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfixed*>(param.data())
                     : nullptr);
    detail::error_check("FogxvOES"sv);
}

STATICINLINE void frustumx(
    GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FrustumxOES)
    }
    glFrustumxOES(l, r, b, t, n, f);
    detail::error_check("FrustumxOES"sv);
}

template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_clip_planex(group::clip_plane_name plane, span_GLfixed equation)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetClipPlanexOES)
    }
    glGetClipPlanexOES(
        static_cast<GLenum>(plane),
        equation.size() ? reinterpret_cast<GLfixed*>(equation.data())
                        : nullptr);
    detail::error_check("GetClipPlanexOES"sv);
}

template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_fixedv(group::get_prop pname, span_GLfixed params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFixedvOES)
    }
    glGetFixedvOES(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetFixedvOES"sv);
}

template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_tex_envxv(
        group::texture_env_target    target,
        group::texture_env_parameter pname,
        span_GLfixed                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexEnvxvOES)
    }
    glGetTexEnvxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetTexEnvxvOES"sv);
}

template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_tex_parameterxv(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_GLfixed                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterxvOES)
    }
    glGetTexParameterxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterxvOES"sv);
}

STATICINLINE void light_modelx(
    group::light_model_parameter pname, GLfixed param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LightModelxOES)
    }
    glLightModelxOES(static_cast<GLenum>(pname), param);
    detail::error_check("LightModelxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void light_modelxv(
        group::light_model_parameter pname, span_const_GLfixed const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LightModelxvOES)
    }
    glLightModelxvOES(
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfixed*>(param.data())
                     : nullptr);
    detail::error_check("LightModelxvOES"sv);
}

STATICINLINE void lightx(
    group::light_name light, group::light_parameter pname, GLfixed param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LightxOES)
    }
    glLightxOES(static_cast<GLenum>(light), static_cast<GLenum>(pname), param);
    detail::error_check("LightxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void lightxv(
        group::light_name         light,
        group::light_parameter    pname,
        span_const_GLfixed const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LightxvOES)
    }
    glLightxvOES(
        static_cast<GLenum>(light),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("LightxvOES"sv);
}

STATICINLINE void line_widthx(GLfixed width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LineWidthxOES)
    }
    glLineWidthxOES(width);
    detail::error_check("LineWidthxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void load_matrixx(span_const_GLfixed const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LoadMatrixxOES)
    }
    glLoadMatrixxOES(
        m.size() ? reinterpret_cast<const GLfixed*>(m.data()) : nullptr);
    detail::error_check("LoadMatrixxOES"sv);
}

STATICINLINE void materialx(
    group::material_face face, group::material_parameter pname, GLfixed param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MaterialxOES)
    }
    glMaterialxOES(
        static_cast<GLenum>(face), static_cast<GLenum>(pname), param);
    detail::error_check("MaterialxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void materialxv(
        group::material_face      face,
        group::material_parameter pname,
        span_const_GLfixed const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MaterialxvOES)
    }
    glMaterialxvOES(
        static_cast<GLenum>(face),
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfixed*>(param.data())
                     : nullptr);
    detail::error_check("MaterialxvOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void mult_matrixx(span_const_GLfixed const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultMatrixxOES)
    }
    glMultMatrixxOES(
        m.size() ? reinterpret_cast<const GLfixed*>(m.data()) : nullptr);
    detail::error_check("MultMatrixxOES"sv);
}

STATICINLINE void multi_tex_coord4x(
    group::texture_unit texture, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4xOES)
    }
    glMultiTexCoord4xOES(static_cast<GLenum>(texture), s, t, r, q);
    detail::error_check("MultiTexCoord4xOES"sv);
}

STATICINLINE void normal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Normal3xOES)
    }
    glNormal3xOES(nx, ny, nz);
    detail::error_check("Normal3xOES"sv);
}

STATICINLINE void orthox(
    GLfixed l, GLfixed r, GLfixed b, GLfixed t, GLfixed n, GLfixed f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(OrthoxOES)
    }
    glOrthoxOES(l, r, b, t, n, f);
    detail::error_check("OrthoxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void point_parameterxv(
        group::point_parameter_name_arb pname, span_const_GLfixed const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterxvOES)
    }
    glPointParameterxvOES(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("PointParameterxvOES"sv);
}

STATICINLINE void point_sizex(GLfixed size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointSizexOES)
    }
    glPointSizexOES(size);
    detail::error_check("PointSizexOES"sv);
}

STATICINLINE void polygon_offsetx(GLfixed factor, GLfixed units)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PolygonOffsetxOES)
    }
    glPolygonOffsetxOES(factor, units);
    detail::error_check("PolygonOffsetxOES"sv);
}

template<class vec_3_GLfixed>
requires(semantic::concepts::Vector<vec_3_GLfixed, GLfixed, 3>) STATICINLINE
    void rotatex(GLfixed angle, vec_3_GLfixed const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RotatexOES)
    }
    glRotatexOES(angle, x.x(), x.y(), x.z());
    detail::error_check("RotatexOES"sv);
}

template<class vec_3_GLfixed>
requires(semantic::concepts::Vector<vec_3_GLfixed, GLfixed, 3>) STATICINLINE
    void scalex(vec_3_GLfixed const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScalexOES)
    }
    glScalexOES(x.x(), x.y(), x.z());
    detail::error_check("ScalexOES"sv);
}

STATICINLINE void tex_envx(
    group::texture_env_target    target,
    group::texture_env_parameter pname,
    GLfixed                      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexEnvxOES)
    }
    glTexEnvxOES(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("TexEnvxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void tex_envxv(
        group::texture_env_target    target,
        group::texture_env_parameter pname,
        span_const_GLfixed const&    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexEnvxvOES)
    }
    glTexEnvxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("TexEnvxvOES"sv);
}

STATICINLINE void tex_parameterx(
    group::texture_target        target,
    group::get_texture_parameter pname,
    GLfixed                      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterxOES)
    }
    glTexParameterxOES(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("TexParameterxOES"sv);
}

template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void tex_parameterxv(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_const_GLfixed const&    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterxvOES)
    }
    glTexParameterxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterxvOES"sv);
}

template<class vec_3_GLfixed>
requires(semantic::concepts::Vector<vec_3_GLfixed, GLfixed, 3>) STATICINLINE
    void translatex(vec_3_GLfixed const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TranslatexOES)
    }
    glTranslatexOES(x.x(), x.y(), x.z());
    detail::error_check("TranslatexOES"sv);
}

#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_lightxv(
        group::light_name      light,
        group::light_parameter pname,
        span_GLfixed           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetLightxvOES)
    }
    glGetLightxvOES(
        static_cast<GLenum>(light),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetLightxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_materialxv(
        group::material_face      face,
        group::material_parameter pname,
        span_GLfixed              params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMaterialxvOES)
    }
    glGetMaterialxvOES(
        static_cast<GLenum>(face),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetMaterialxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void point_parameterx(
    group::point_parameter_name_arb pname, GLfixed param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterxOES)
    }
    glPointParameterxOES(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameterxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void sample_coveragex(GLclampx value, bool invert)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleCoveragexOES)
    }
    glSampleCoveragexOES(value, invert);
    detail::error_check("SampleCoveragexOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void accumx(GLenum op, GLfixed value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AccumxOES)
    }
    glAccumxOES(op, value);
    detail::error_check("AccumxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class size_2_i32, class span_const_u8>
requires(semantic::concepts::Size2D<size_2_i32, i32>&&
             semantic::concepts::Span<span_const_u8>&& std::is_same_v<
                 std::decay_t<typename span_const_u8::value_type>,
                 std::decay_t<u8>>) STATICINLINE
    void bitmapx(
        size_2_i32 const&    width,
        GLfixed              xorig,
        GLfixed              yorig,
        GLfixed              xmove,
        GLfixed              ymove,
        span_const_u8 const& bitmap)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BitmapxOES)
    }
    glBitmapxOES(
        width[0],
        width[1],
        xorig,
        yorig,
        xmove,
        ymove,
        bitmap.size() ? reinterpret_cast<const GLubyte*>(bitmap.data())
                      : nullptr);
    detail::error_check("BitmapxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_4_GLfixed>
requires(semantic::concepts::Vector<vec_4_GLfixed, GLfixed, 4>) STATICINLINE
    void blend_colorx(vec_4_GLfixed const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColorxOES)
    }
    glBlendColorxOES(red.x(), red.y(), red.z(), red.w());
    detail::error_check("BlendColorxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_4_GLfixed>
requires(semantic::concepts::Vector<vec_4_GLfixed, GLfixed, 4>) STATICINLINE
    void clear_accumx(vec_4_GLfixed const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearAccumxOES)
    }
    glClearAccumxOES(red.x(), red.y(), red.z(), red.w());
    detail::error_check("ClearAccumxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_3_GLfixed>
requires(semantic::concepts::Vector<vec_3_GLfixed, GLfixed, 3>) STATICINLINE
    void color3x(vec_3_GLfixed const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color3xOES)
    }
    glColor3xOES(red.x(), red.y(), red.z());
    detail::error_check("Color3xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void color3xv(span_const_GLfixed const& components)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color3xvOES)
    }
    glColor3xvOES(
        components.size() ? reinterpret_cast<const GLfixed*>(components.data())
                          : nullptr);
    detail::error_check("Color3xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void color4xv(span_const_GLfixed const& components)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Color4xvOES)
    }
    glColor4xvOES(
        components.size() ? reinterpret_cast<const GLfixed*>(components.data())
                          : nullptr);
    detail::error_check("Color4xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void convolution_parameterx(
    group::convolution_target_ext    target,
    group::convolution_parameter_ext pname,
    GLfixed                          param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterxOES)
    }
    glConvolutionParameterxOES(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("ConvolutionParameterxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void convolution_parameterxv(
        group::convolution_target_ext    target,
        group::convolution_parameter_ext pname,
        span_const_GLfixed const&        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ConvolutionParameterxvOES)
    }
    glConvolutionParameterxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("ConvolutionParameterxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void eval_coord1x(GLfixed u)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalCoord1xOES)
    }
    glEvalCoord1xOES(u);
    detail::error_check("EvalCoord1xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void eval_coord1xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalCoord1xvOES)
    }
    glEvalCoord1xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("EvalCoord1xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void eval_coord2x(GLfixed u, GLfixed v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalCoord2xOES)
    }
    glEvalCoord2xOES(u, v);
    detail::error_check("EvalCoord2xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void eval_coord2xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EvalCoord2xvOES)
    }
    glEvalCoord2xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("EvalCoord2xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void feedback_bufferx(GLenum type, span_const_GLfixed const& buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FeedbackBufferxOES)
    }
    glFeedbackBufferxOES(
        buffer.size(),
        type,
        buffer.size() ? reinterpret_cast<const GLfixed*>(buffer.data())
                      : nullptr);
    detail::error_check("FeedbackBufferxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_convolution_parameterxv(
        GLenum target, GLenum pname, span_GLfixed params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetConvolutionParameterxvOES)
    }
    glGetConvolutionParameterxvOES(
        target,
        pname,
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetConvolutionParameterxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_histogram_parameterxv(
        group::histogram_target_ext             target,
        group::get_histogram_parameter_prop_ext pname,
        span_GLfixed                            params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHistogramParameterxvOES)
    }
    glGetHistogramParameterxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetHistogramParameterxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_lightx(
        group::light_name      light,
        group::light_parameter pname,
        span_GLfixed           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetLightxOES)
    }
    glGetLightxOES(
        static_cast<GLenum>(light),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetLightxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_mapxv(
        group::map_target target, group::get_map_query query, span_GLfixed v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMapxvOES)
    }
    glGetMapxvOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(query),
        v.size() ? reinterpret_cast<GLfixed*>(v.data()) : nullptr);
    detail::error_check("GetMapxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void get_materialx(
    group::material_face face, group::material_parameter pname, GLfixed param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMaterialxOES)
    }
    glGetMaterialxOES(
        static_cast<GLenum>(face), static_cast<GLenum>(pname), param);
    detail::error_check("GetMaterialxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_pixel_mapxv(group::pixel_map map, span_GLfixed values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPixelMapxv)
    }
    glGetPixelMapxv(
        static_cast<GLenum>(map),
        values.size(),
        values.size() ? reinterpret_cast<GLfixed*>(values.data()) : nullptr);
    detail::error_check("GetPixelMapxv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_tex_genxv(
        group::texture_coord_name    coord,
        group::texture_gen_parameter pname,
        span_GLfixed                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexGenxvOES)
    }
    glGetTexGenxvOES(
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetTexGenxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLfixed>
requires(semantic::concepts::Span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void get_tex_level_parameterxv(
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_GLfixed                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexLevelParameterxvOES)
    }
    glGetTexLevelParameterxvOES(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfixed*>(params.data()) : nullptr);
    detail::error_check("GetTexLevelParameterxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void indexx(GLfixed component)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IndexxOES)
    }
    glIndexxOES(component);
    detail::error_check("IndexxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void indexxv(span_const_GLfixed const& component)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IndexxvOES)
    }
    glIndexxvOES(
        component.size() ? reinterpret_cast<const GLfixed*>(component.data())
                         : nullptr);
    detail::error_check("IndexxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void load_transpose_matrixx(span_const_GLfixed const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LoadTransposeMatrixxOES)
    }
    glLoadTransposeMatrixxOES(
        m.size() ? reinterpret_cast<const GLfixed*>(m.data()) : nullptr);
    detail::error_check("LoadTransposeMatrixxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void map1x(
    group::map_target target,
    GLfixed           u1,
    GLfixed           u2,
    i32               stride,
    i32               order,
    GLfixed           points)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Map1xOES)
    }
    glMap1xOES(static_cast<GLenum>(target), u1, u2, stride, order, points);
    detail::error_check("Map1xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
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
    GLfixed           points)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Map2xOES)
    }
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
    detail::error_check("Map2xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void map_grid1x(i32 n, GLfixed u1, GLfixed u2)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapGrid1xOES)
    }
    glMapGrid1xOES(n, u1, u2);
    detail::error_check("MapGrid1xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void map_grid2x(
    i32 n, GLfixed u1, GLfixed u2, GLfixed v1, GLfixed v2)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapGrid2xOES)
    }
    glMapGrid2xOES(n, u1, u2, v1, v2);
    detail::error_check("MapGrid2xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void mult_transpose_matrixx(span_const_GLfixed const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultTransposeMatrixxOES)
    }
    glMultTransposeMatrixxOES(
        m.size() ? reinterpret_cast<const GLfixed*>(m.data()) : nullptr);
    detail::error_check("MultTransposeMatrixxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void multi_tex_coord1x(group::texture_unit texture, GLfixed s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1xOES)
    }
    glMultiTexCoord1xOES(static_cast<GLenum>(texture), s);
    detail::error_check("MultiTexCoord1xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void multi_tex_coord1xv(
        group::texture_unit texture, span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1xvOES)
    }
    glMultiTexCoord1xvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord1xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void multi_tex_coord2x(
    group::texture_unit texture, GLfixed s, GLfixed t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2xOES)
    }
    glMultiTexCoord2xOES(static_cast<GLenum>(texture), s, t);
    detail::error_check("MultiTexCoord2xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void multi_tex_coord2xv(
        group::texture_unit texture, span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2xvOES)
    }
    glMultiTexCoord2xvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord2xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void multi_tex_coord3x(
    group::texture_unit texture, GLfixed s, GLfixed t, GLfixed r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3xOES)
    }
    glMultiTexCoord3xOES(static_cast<GLenum>(texture), s, t, r);
    detail::error_check("MultiTexCoord3xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void multi_tex_coord3xv(
        group::texture_unit texture, span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3xvOES)
    }
    glMultiTexCoord3xvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord3xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void multi_tex_coord4xv(
        group::texture_unit texture, span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4xvOES)
    }
    glMultiTexCoord4xvOES(
        static_cast<GLenum>(texture),
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("MultiTexCoord4xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void normal3xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Normal3xvOES)
    }
    glNormal3xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("Normal3xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void pass_throughx(GLfixed token)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PassThroughxOES)
    }
    glPassThroughxOES(token);
    detail::error_check("PassThroughxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void pixel_mapx(group::pixel_map map, span_const_GLfixed const& values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelMapx)
    }
    glPixelMapx(
        static_cast<GLenum>(map),
        values.size(),
        values.size() ? reinterpret_cast<const GLfixed*>(values.data())
                      : nullptr);
    detail::error_check("PixelMapx"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void pixel_storex(
    group::pixel_store_parameter pname, GLfixed param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelStorex)
    }
    glPixelStorex(static_cast<GLenum>(pname), param);
    detail::error_check("PixelStorex"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void pixel_transferx(
    group::pixel_transfer_parameter pname, GLfixed param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelTransferxOES)
    }
    glPixelTransferxOES(static_cast<GLenum>(pname), param);
    detail::error_check("PixelTransferxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void pixel_zoomx(GLfixed xfactor, GLfixed yfactor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PixelZoomxOES)
    }
    glPixelZoomxOES(xfactor, yfactor);
    detail::error_check("PixelZoomxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed, class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_const_GLfixed>&&
                              std::is_same_v<
                 std::decay_t<typename span_const_GLfixed::value_type>,
                 std::decay_t<GLfixed>>) STATICINLINE
    void prioritize_texturesx(
        span_const_u32 const& textures, span_const_GLfixed const& priorities)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrioritizeTexturesxOES)
    }
    glPrioritizeTexturesxOES(
        textures.size(),
        textures.size() ? reinterpret_cast<const GLuint*>(textures.data())
                        : nullptr,
        priorities.size() ? reinterpret_cast<const GLfixed*>(priorities.data())
                          : nullptr);
    detail::error_check("PrioritizeTexturesxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_GLfixed>
requires(semantic::concepts::Vector<vec_2_GLfixed, GLfixed, 2>) STATICINLINE
    void raster_pos2x(vec_2_GLfixed const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos2xOES)
    }
    glRasterPos2xOES(x.x(), x.y());
    detail::error_check("RasterPos2xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void raster_pos2xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos2xvOES)
    }
    glRasterPos2xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("RasterPos2xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_3_GLfixed>
requires(semantic::concepts::Vector<vec_3_GLfixed, GLfixed, 3>) STATICINLINE
    void raster_pos3x(vec_3_GLfixed const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos3xOES)
    }
    glRasterPos3xOES(x.x(), x.y(), x.z());
    detail::error_check("RasterPos3xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void raster_pos3xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos3xvOES)
    }
    glRasterPos3xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("RasterPos3xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_4_GLfixed>
requires(semantic::concepts::Vector<vec_4_GLfixed, GLfixed, 4>) STATICINLINE
    void raster_pos4x(vec_4_GLfixed const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos4xOES)
    }
    glRasterPos4xOES(x.x(), x.y(), x.z(), x.w());
    detail::error_check("RasterPos4xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void raster_pos4xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RasterPos4xvOES)
    }
    glRasterPos4xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("RasterPos4xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void rectx(GLfixed x1, GLfixed y1, GLfixed x2, GLfixed y2)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RectxOES)
    }
    glRectxOES(x1, y1, x2, y2);
    detail::error_check("RectxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void rectxv(span_const_GLfixed const& v1, span_const_GLfixed const& v2)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RectxvOES)
    }
    glRectxvOES(
        v1.size() ? reinterpret_cast<const GLfixed*>(v1.data()) : nullptr,
        v2.size() ? reinterpret_cast<const GLfixed*>(v2.data()) : nullptr);
    detail::error_check("RectxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_coord1x(GLfixed s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1xOES)
    }
    glTexCoord1xOES(s);
    detail::error_check("TexCoord1xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void tex_coord1xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord1xvOES)
    }
    glTexCoord1xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord1xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_coord2x(GLfixed s, GLfixed t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2xOES)
    }
    glTexCoord2xOES(s, t);
    detail::error_check("TexCoord2xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void tex_coord2xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord2xvOES)
    }
    glTexCoord2xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord2xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_coord3x(GLfixed s, GLfixed t, GLfixed r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3xOES)
    }
    glTexCoord3xOES(s, t, r);
    detail::error_check("TexCoord3xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void tex_coord3xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord3xvOES)
    }
    glTexCoord3xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord3xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_coord4x(GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4xOES)
    }
    glTexCoord4xOES(s, t, r, q);
    detail::error_check("TexCoord4xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void tex_coord4xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoord4xvOES)
    }
    glTexCoord4xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("TexCoord4xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void tex_genx(
    group::texture_coord_name    coord,
    group::texture_gen_parameter pname,
    GLfixed                      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexGenxOES)
    }
    glTexGenxOES(static_cast<GLenum>(coord), static_cast<GLenum>(pname), param);
    detail::error_check("TexGenxOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void tex_genxv(
        group::texture_coord_name    coord,
        group::texture_gen_parameter pname,
        span_const_GLfixed const&    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexGenxvOES)
    }
    glTexGenxvOES(
        static_cast<GLenum>(coord),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfixed*>(params.data())
                      : nullptr);
    detail::error_check("TexGenxvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void vertex2x(GLfixed x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2xOES)
    }
    glVertex2xOES(x);
    detail::error_check("Vertex2xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void vertex2xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex2xvOES)
    }
    glVertex2xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex2xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_2_GLfixed>
requires(semantic::concepts::Vector<vec_2_GLfixed, GLfixed, 2>) STATICINLINE
    void vertex3x(vec_2_GLfixed const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3xOES)
    }
    glVertex3xOES(x.x(), x.y());
    detail::error_check("Vertex3xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void vertex3xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex3xvOES)
    }
    glVertex3xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex3xvOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class vec_3_GLfixed>
requires(semantic::concepts::Vector<vec_3_GLfixed, GLfixed, 3>) STATICINLINE
    void vertex4x(vec_3_GLfixed const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4xOES)
    }
    glVertex4xOES(x.x(), x.y(), x.z());
    detail::error_check("Vertex4xOES"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_GLfixed>
requires(semantic::concepts::Span<span_const_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_const_GLfixed::value_type>,
         std::decay_t<GLfixed>>) STATICINLINE
    void vertex4xv(span_const_GLfixed const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Vertex4xvOES)
    }
    glVertex4xvOES(
        coords.size() ? reinterpret_cast<const GLfixed*>(coords.data())
                      : nullptr);
    detail::error_check("Vertex4xvOES"sv);
}

#endif
} // namespace gl::oes::fixed_point
#endif // GL_OES_fixed_point
namespace gl::oes::fixed_point {
constexpr auto name = "GL_OES_fixed_point";
}
