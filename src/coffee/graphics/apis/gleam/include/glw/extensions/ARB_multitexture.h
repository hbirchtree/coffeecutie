#pragma once

#ifdef GL_ARB_multitexture
#include "../enums/CombinerRegisterNV.h"

namespace gl::arb::multitexture {
using gl::group::combiner_register_nv;

namespace values {
constexpr u32 texture2              = 0x84C2;
constexpr u32 texture3              = 0x84C3;
constexpr u32 texture4              = 0x84C4;
constexpr u32 texture5              = 0x84C5;
constexpr u32 texture6              = 0x84C6;
constexpr u32 texture7              = 0x84C7;
constexpr u32 texture8              = 0x84C8;
constexpr u32 texture9              = 0x84C9;
constexpr u32 texture10             = 0x84CA;
constexpr u32 texture11             = 0x84CB;
constexpr u32 texture12             = 0x84CC;
constexpr u32 texture13             = 0x84CD;
constexpr u32 texture14             = 0x84CE;
constexpr u32 texture15             = 0x84CF;
constexpr u32 texture16             = 0x84D0;
constexpr u32 texture17             = 0x84D1;
constexpr u32 texture18             = 0x84D2;
constexpr u32 texture19             = 0x84D3;
constexpr u32 texture20             = 0x84D4;
constexpr u32 texture21             = 0x84D5;
constexpr u32 texture22             = 0x84D6;
constexpr u32 texture23             = 0x84D7;
constexpr u32 texture24             = 0x84D8;
constexpr u32 texture25             = 0x84D9;
constexpr u32 texture26             = 0x84DA;
constexpr u32 texture27             = 0x84DB;
constexpr u32 texture28             = 0x84DC;
constexpr u32 texture29             = 0x84DD;
constexpr u32 texture30             = 0x84DE;
constexpr u32 texture31             = 0x84DF;
constexpr u32 active_texture        = 0x84E0;
constexpr u32 client_active_texture = 0x84E1;
constexpr u32 max_texture_units     = 0x84E2;
} // namespace values

/*!
 * \brief Part of GL_ARB_multitexture
 * \param texture GLenum
 * \return void
 */
STATICINLINE void active_texture(group::texture_unit texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ActiveTextureARB)
    }
    glActiveTextureARB(static_cast<GLenum>(texture));
    detail::error_check("ActiveTextureARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param texture GLenum
 * \return void
 */
STATICINLINE void client_active_texture(group::texture_unit texture)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClientActiveTextureARB)
    }
    glClientActiveTextureARB(static_cast<GLenum>(texture));
    detail::error_check("ClientActiveTextureARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLdouble
 * \return void
 */
STATICINLINE void multi_tex_coord1d(group::texture_unit target, f64 s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1dARB)
    }
    glMultiTexCoord1dARB(static_cast<GLenum>(target), s);
    detail::error_check("MultiTexCoord1dARB"sv);
}

template<class span_const_f64>
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void multi_tex_coord1dv(
    group::texture_unit target, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1dvARB)
    }
    glMultiTexCoord1dvARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("MultiTexCoord1dvARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLfloat
 * \return void
 */
STATICINLINE void multi_tex_coord1f(group::texture_unit target, f32 s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1fARB)
    }
    glMultiTexCoord1fARB(static_cast<GLenum>(target), s);
    detail::error_check("MultiTexCoord1fARB"sv);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void multi_tex_coord1fv(
    group::texture_unit target, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1fvARB)
    }
    glMultiTexCoord1fvARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("MultiTexCoord1fvARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLint
 * \return void
 */
STATICINLINE void multi_tex_coord1i(group::texture_unit target, i32 s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1iARB)
    }
    glMultiTexCoord1iARB(static_cast<GLenum>(target), s);
    detail::error_check("MultiTexCoord1iARB"sv);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLint *
 * \return void
 */
STATICINLINE void multi_tex_coord1iv(
    group::texture_unit target, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1ivARB)
    }
    glMultiTexCoord1ivARB(
        static_cast<GLenum>(target), reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("MultiTexCoord1ivARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLshort
 * \return void
 */
STATICINLINE void multi_tex_coord1s(group::texture_unit target, i16 s)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1sARB)
    }
    glMultiTexCoord1sARB(static_cast<GLenum>(target), s);
    detail::error_check("MultiTexCoord1sARB"sv);
}

template<class span_const_i16>
requires(
    concepts::span<span_const_i16> &&
    std::is_same_v<
        std::decay_t<typename span_const_i16::value_type>,
        std::decay_t<i16>>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void multi_tex_coord1sv(
    group::texture_unit target, span_const_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord1svARB)
    }
    glMultiTexCoord1svARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("MultiTexCoord1svARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLdouble
 * \param t GLdouble
 * \return void
 */
STATICINLINE void multi_tex_coord2d(group::texture_unit target, f64 s, f64 t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2dARB)
    }
    glMultiTexCoord2dARB(static_cast<GLenum>(target), s, t);
    detail::error_check("MultiTexCoord2dARB"sv);
}

template<class span_const_vec_2_f64>
requires(
    concepts::span<span_const_vec_2_f64> &&
    concepts::vector<typename span_const_vec_2_f64::value_type, f64, 2>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void multi_tex_coord2dv(
    group::texture_unit target, span_const_vec_2_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2dvARB)
    }
    glMultiTexCoord2dvARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("MultiTexCoord2dvARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLfloat
 * \param t GLfloat
 * \return void
 */
STATICINLINE void multi_tex_coord2f(group::texture_unit target, f32 s, f32 t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2fARB)
    }
    glMultiTexCoord2fARB(static_cast<GLenum>(target), s, t);
    detail::error_check("MultiTexCoord2fARB"sv);
}

template<class span_const_vec_2_f32>
requires(
    concepts::span<span_const_vec_2_f32> &&
    concepts::vector<typename span_const_vec_2_f32::value_type, f32, 2>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void multi_tex_coord2fv(
    group::texture_unit target, span_const_vec_2_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2fvARB)
    }
    glMultiTexCoord2fvARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("MultiTexCoord2fvARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLint
 * \param t GLint
 * \return void
 */
STATICINLINE void multi_tex_coord2i(group::texture_unit target, i32 s, i32 t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2iARB)
    }
    glMultiTexCoord2iARB(static_cast<GLenum>(target), s, t);
    detail::error_check("MultiTexCoord2iARB"sv);
}

template<class span_const_vec_2_i32>
requires(
    concepts::span<span_const_vec_2_i32> &&
    concepts::vector<typename span_const_vec_2_i32::value_type, i32, 2>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLint *
 * \return void
 */
STATICINLINE void multi_tex_coord2iv(
    group::texture_unit target, span_const_vec_2_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2ivARB)
    }
    glMultiTexCoord2ivARB(
        static_cast<GLenum>(target), reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("MultiTexCoord2ivARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLshort
 * \param t GLshort
 * \return void
 */
STATICINLINE void multi_tex_coord2s(group::texture_unit target, i16 s, i16 t)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2sARB)
    }
    glMultiTexCoord2sARB(static_cast<GLenum>(target), s, t);
    detail::error_check("MultiTexCoord2sARB"sv);
}

template<class span_const_vec_2_i16>
requires(
    concepts::span<span_const_vec_2_i16> &&
    concepts::vector<typename span_const_vec_2_i16::value_type, i16, 2>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void multi_tex_coord2sv(
    group::texture_unit target, span_const_vec_2_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord2svARB)
    }
    glMultiTexCoord2svARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("MultiTexCoord2svARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLdouble
 * \param t GLdouble
 * \param r GLdouble
 * \return void
 */
STATICINLINE void multi_tex_coord3d(
    group::texture_unit target, f64 s, f64 t, f64 r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3dARB)
    }
    glMultiTexCoord3dARB(static_cast<GLenum>(target), s, t, r);
    detail::error_check("MultiTexCoord3dARB"sv);
}

template<class span_const_vec_3_f64>
requires(
    concepts::span<span_const_vec_3_f64> &&
    concepts::vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void multi_tex_coord3dv(
    group::texture_unit target, span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3dvARB)
    }
    glMultiTexCoord3dvARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("MultiTexCoord3dvARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLfloat
 * \param t GLfloat
 * \param r GLfloat
 * \return void
 */
STATICINLINE void multi_tex_coord3f(
    group::texture_unit target, f32 s, f32 t, f32 r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3fARB)
    }
    glMultiTexCoord3fARB(static_cast<GLenum>(target), s, t, r);
    detail::error_check("MultiTexCoord3fARB"sv);
}

template<class span_const_vec_3_f32>
requires(
    concepts::span<span_const_vec_3_f32> &&
    concepts::vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void multi_tex_coord3fv(
    group::texture_unit target, span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3fvARB)
    }
    glMultiTexCoord3fvARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("MultiTexCoord3fvARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLint
 * \param t GLint
 * \param r GLint
 * \return void
 */
STATICINLINE void multi_tex_coord3i(
    group::texture_unit target, i32 s, i32 t, i32 r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3iARB)
    }
    glMultiTexCoord3iARB(static_cast<GLenum>(target), s, t, r);
    detail::error_check("MultiTexCoord3iARB"sv);
}

template<class span_const_vec_3_i32>
requires(
    concepts::span<span_const_vec_3_i32> &&
    concepts::vector<typename span_const_vec_3_i32::value_type, i32, 3>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLint *
 * \return void
 */
STATICINLINE void multi_tex_coord3iv(
    group::texture_unit target, span_const_vec_3_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3ivARB)
    }
    glMultiTexCoord3ivARB(
        static_cast<GLenum>(target), reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("MultiTexCoord3ivARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLshort
 * \param t GLshort
 * \param r GLshort
 * \return void
 */
STATICINLINE void multi_tex_coord3s(
    group::texture_unit target, i16 s, i16 t, i16 r)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3sARB)
    }
    glMultiTexCoord3sARB(static_cast<GLenum>(target), s, t, r);
    detail::error_check("MultiTexCoord3sARB"sv);
}

template<class span_const_vec_3_i16>
requires(
    concepts::span<span_const_vec_3_i16> &&
    concepts::vector<typename span_const_vec_3_i16::value_type, i16, 3>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void multi_tex_coord3sv(
    group::texture_unit target, span_const_vec_3_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord3svARB)
    }
    glMultiTexCoord3svARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("MultiTexCoord3svARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLdouble
 * \param t GLdouble
 * \param r GLdouble
 * \param q GLdouble
 * \return void
 */
STATICINLINE void multi_tex_coord4d(
    group::texture_unit target, f64 s, f64 t, f64 r, f64 q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4dARB)
    }
    glMultiTexCoord4dARB(static_cast<GLenum>(target), s, t, r, q);
    detail::error_check("MultiTexCoord4dARB"sv);
}

template<class span_const_vec_4_f64>
requires(
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void multi_tex_coord4dv(
    group::texture_unit target, span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4dvARB)
    }
    glMultiTexCoord4dvARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("MultiTexCoord4dvARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLfloat
 * \param t GLfloat
 * \param r GLfloat
 * \param q GLfloat
 * \return void
 */
STATICINLINE void multi_tex_coord4f(
    group::texture_unit target, f32 s, f32 t, f32 r, f32 q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4fARB)
    }
    glMultiTexCoord4fARB(static_cast<GLenum>(target), s, t, r, q);
    detail::error_check("MultiTexCoord4fARB"sv);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void multi_tex_coord4fv(
    group::texture_unit target, span_const_vec_4_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4fvARB)
    }
    glMultiTexCoord4fvARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("MultiTexCoord4fvARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLint
 * \param t GLint
 * \param r GLint
 * \param q GLint
 * \return void
 */
STATICINLINE void multi_tex_coord4i(
    group::texture_unit target, i32 s, i32 t, i32 r, i32 q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4iARB)
    }
    glMultiTexCoord4iARB(static_cast<GLenum>(target), s, t, r, q);
    detail::error_check("MultiTexCoord4iARB"sv);
}

template<class span_const_vec_4_i32>
requires(
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLint *
 * \return void
 */
STATICINLINE void multi_tex_coord4iv(
    group::texture_unit target, span_const_vec_4_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4ivARB)
    }
    glMultiTexCoord4ivARB(
        static_cast<GLenum>(target), reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("MultiTexCoord4ivARB"sv);
}

/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param s GLshort
 * \param t GLshort
 * \param r GLshort
 * \param q GLshort
 * \return void
 */
STATICINLINE void multi_tex_coord4s(
    group::texture_unit target, i16 s, i16 t, i16 r, i16 q)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4sARB)
    }
    glMultiTexCoord4sARB(static_cast<GLenum>(target), s, t, r, q);
    detail::error_check("MultiTexCoord4sARB"sv);
}

template<class span_const_vec_4_i16>
requires(
    concepts::span<span_const_vec_4_i16> &&
    concepts::vector<typename span_const_vec_4_i16::value_type, i16, 4>)
/*!
 * \brief Part of GL_ARB_multitexture
 * \param target GLenum
 * \param v const GLshort *
 * \return void
 */
STATICINLINE void multi_tex_coord4sv(
    group::texture_unit target, span_const_vec_4_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoord4svARB)
    }
    glMultiTexCoord4svARB(
        static_cast<GLenum>(target),
        reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("MultiTexCoord4svARB"sv);
}

} // namespace gl::arb::multitexture
#endif // GL_ARB_multitexture
namespace gl::arb::multitexture {
constexpr auto name = "GL_ARB_multitexture";
}
