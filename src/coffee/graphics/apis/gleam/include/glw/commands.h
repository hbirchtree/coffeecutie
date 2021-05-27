#pragma once

#include "base.h"

#include "groups.h"

#include <cstddef>
#include <tuple>
#include <string_view>
#include <vector>
#include <peripherals/concepts/span.h>
#include <peripherals/concepts/vector.h>

namespace gl {

namespace detail {
inline std::tuple<
    std::vector<GLint>,
    std::vector<const char*>,
    std::vector<std::string>>
transform_strings(std::vector<std::string_view> const& strings)
{
    std::vector<GLint> string_lengths;
    std::vector<const char*> cstrings;
    std::vector<std::string> stringstorage;

    for(auto const& view : strings)
    {
        stringstorage.push_back(std::string(view.begin(), view.end()));
        cstrings.push_back(stringstorage.back().c_str());
        string_lengths.push_back(stringstorage.back().size());
    }

    return std::make_tuple(
        std::move(string_lengths), 
        std::move(cstrings), 
        std::move(stringstorage));
}

inline std::string error_to_hex()
{
    return "0x0000";
}

template<typename T1, typename T2>
inline void assert_equal(T1 const& v1, T2 const& v2)
{
    if (v1 != v2)
        Throw(std::runtime_error("assertion failed"));
}
}

static_assert(std::is_same_v<GLubyte, ::libc_types::u8>, "GLubyte does not match u8");
using ::libc_types::u8;
static_assert(std::is_same_v<GLbyte, ::libc_types::i8>, "GLbyte does not match i8");
using ::libc_types::i8;
static_assert(std::is_same_v<GLushort, ::libc_types::u16>, "GLushort does not match u16");
using ::libc_types::u16;
static_assert(std::is_same_v<GLshort, ::libc_types::i16>, "GLshort does not match i16");
using ::libc_types::i16;
static_assert(std::is_same_v<GLuint, ::libc_types::u32>, "GLuint does not match u32");
using ::libc_types::u32;
static_assert(std::is_same_v<GLint, ::libc_types::i32>, "GLint does not match i32");
using ::libc_types::i32;
static_assert(std::is_same_v<GLuint64, ::libc_types::u64>, "GLuint64 does not match u64");
using ::libc_types::u64;
static_assert(std::is_same_v<GLint64, ::libc_types::i64>, "GLint64 does not match i64");
using ::libc_types::i64;
static_assert(std::is_same_v<GLfloat, ::libc_types::f32>, "GLfloat does not match f32");
using ::libc_types::f32;
static_assert(std::is_same_v<GLdouble, ::libc_types::f64>, "GLdouble does not match f64");
using ::libc_types::f64;
static_assert(std::is_same_v<GLsizei, ::libc_types::i32>, "GLsizei does not match i32");

}



namespace gl::impl {


template<typename Current>
struct core
{
#ifdef GL_VERSION_1_0
/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void blend_func(groups::blending_factor sfactor, groups::blending_factor dfactor)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendFunc)
                Throw(undefined_behavior(
                    "unloaded function BlendFunc"));
    }
    glBlendFunc(static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void clear(groups::clear_buffer_mask mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClear)
                Throw(undefined_behavior(
                    "unloaded function Clear"));
    }
    glClear(static_cast<GLenum>(mask));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void clear_color(vec_4_f32 const& red)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearColor)
                Throw(undefined_behavior(
                    "unloaded function ClearColor"));
    }
    glClearColor(red.x(), red.y(), red.z(), red.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void clear_depth(f64 depth)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearDepth)
                Throw(undefined_behavior(
                    "unloaded function ClearDepth"));
    }
    glClearDepth(depth);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void clear_stencil(i32 s)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearStencil)
                Throw(undefined_behavior(
                    "unloaded function ClearStencil"));
    }
    glClearStencil(s);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class vec_4_bool
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Vector<vec_4_bool, bool, 4>)
STATICINLINE void color_mask(vec_4_bool const& red)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glColorMask)
                Throw(undefined_behavior(
                    "unloaded function ColorMask"));
    }
    glColorMask(red.x(), red.y(), red.z(), red.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void cull_face(groups::cull_face_mode mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCullFace)
                Throw(undefined_behavior(
                    "unloaded function CullFace"));
    }
    glCullFace(static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void depth_func(groups::depth_function func)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthFunc)
                Throw(undefined_behavior(
                    "unloaded function DepthFunc"));
    }
    glDepthFunc(static_cast<GLenum>(func));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void depth_mask(bool flag)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthMask)
                Throw(undefined_behavior(
                    "unloaded function DepthMask"));
    }
    glDepthMask(flag);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void depth_range(f64 n, f64 f)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthRange)
                Throw(undefined_behavior(
                    "unloaded function DepthRange"));
    }
    glDepthRange(n, f);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void disable(groups::enable_cap cap)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDisable)
                Throw(undefined_behavior(
                    "unloaded function Disable"));
    }
    glDisable(static_cast<GLenum>(cap));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void draw_buffer(groups::draw_buffer_mode buf)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawBuffer)
                Throw(undefined_behavior(
                    "unloaded function DrawBuffer"));
    }
    glDrawBuffer(static_cast<GLenum>(buf));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void enable(groups::enable_cap cap)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEnable)
                Throw(undefined_behavior(
                    "unloaded function Enable"));
    }
    glEnable(static_cast<GLenum>(cap));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void finish()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFinish)
                Throw(undefined_behavior(
                    "unloaded function Finish"));
    }
    glFinish();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void flush()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFlush)
                Throw(undefined_behavior(
                    "unloaded function Flush"));
    }
    glFlush();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void front_face(groups::front_face_direction mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFrontFace)
                Throw(undefined_behavior(
                    "unloaded function FrontFace"));
    }
    glFrontFace(static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_bool
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_bool> &&
    std::is_same_v<std::decay_t<typename span_bool::value_type>, bool>)
STATICINLINE void get_booleanv(groups::get_prop pname, span_bool data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBooleanv)
                Throw(undefined_behavior(
                    "unloaded function GetBooleanv"));
    }
    glGetBooleanv(static_cast<GLenum>(pname), reinterpret_cast<GLboolean *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_f64
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<std::decay_t<typename span_f64::value_type>, f64>)
STATICINLINE void get_doublev(groups::get_prop pname, span_f64 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetDoublev)
                Throw(undefined_behavior(
                    "unloaded function GetDoublev"));
    }
    glGetDoublev(static_cast<GLenum>(pname), reinterpret_cast<GLdouble *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE GLenum get_error()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetError)
                Throw(undefined_behavior(
                    "unloaded function GetError"));
    }
    auto out = glGetError();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 1.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_floatv(groups::get_prop pname, span_f32 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFloatv)
                Throw(undefined_behavior(
                    "unloaded function GetFloatv"));
    }
    glGetFloatv(static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_integerv(groups::get_prop pname, span_i32 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetIntegerv)
                Throw(undefined_behavior(
                    "unloaded function GetIntegerv"));
    }
    glGetIntegerv(static_cast<GLenum>(pname), reinterpret_cast<GLint *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE const GLubyte * get_string(groups::string_name name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetString)
                Throw(undefined_behavior(
                    "unloaded function GetString"));
    }
    auto out = glGetString(static_cast<GLenum>(name));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 1.0 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_tex_image(groups::texture_target target, i32 level, groups::pixel_format format, groups::pixel_type type, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexImage)
                Throw(undefined_behavior(
                    "unloaded function GetTexImage"));
    }
    glGetTexImage(static_cast<GLenum>(target), level, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_tex_level_parameter(groups::texture_target target, i32 level, groups::get_texture_parameter pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexLevelParameterfv)
                Throw(undefined_behavior(
                    "unloaded function GetTexLevelParameterfv"));
    }
    glGetTexLevelParameterfv(static_cast<GLenum>(target), level, static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_tex_level_parameter(groups::texture_target target, i32 level, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexLevelParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetTexLevelParameteriv"));
    }
    glGetTexLevelParameteriv(static_cast<GLenum>(target), level, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_tex_parameter(groups::texture_target target, groups::get_texture_parameter pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexParameterfv)
                Throw(undefined_behavior(
                    "unloaded function GetTexParameterfv"));
    }
    glGetTexParameterfv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_tex_parameter(groups::texture_target target, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetTexParameteriv"));
    }
    glGetTexParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void hint(groups::hint_target target, groups::hint_mode mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glHint)
                Throw(undefined_behavior(
                    "unloaded function Hint"));
    }
    glHint(static_cast<GLenum>(target), static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE GLboolean is_enabled(groups::enable_cap cap)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsEnabled)
                Throw(undefined_behavior(
                    "unloaded function IsEnabled"));
    }
    auto out = glIsEnabled(static_cast<GLenum>(cap));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void line_width(f32 width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glLineWidth)
                Throw(undefined_behavior(
                    "unloaded function LineWidth"));
    }
    glLineWidth(width);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void logic_op(groups::logic_op opcode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glLogicOp)
                Throw(undefined_behavior(
                    "unloaded function LogicOp"));
    }
    glLogicOp(static_cast<GLenum>(opcode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void pixel_storef(groups::pixel_store_parameter pname, f32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPixelStoref)
                Throw(undefined_behavior(
                    "unloaded function PixelStoref"));
    }
    glPixelStoref(static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void pixel_storei(groups::pixel_store_parameter pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPixelStorei)
                Throw(undefined_behavior(
                    "unloaded function PixelStorei"));
    }
    glPixelStorei(static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void point_size(f32 size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPointSize)
                Throw(undefined_behavior(
                    "unloaded function PointSize"));
    }
    glPointSize(size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void polygon_mode(groups::material_face face, groups::polygon_mode mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPolygonMode)
                Throw(undefined_behavior(
                    "unloaded function PolygonMode"));
    }
    glPolygonMode(static_cast<GLenum>(face), static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void read_buffer(groups::read_buffer_mode src)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glReadBuffer)
                Throw(undefined_behavior(
                    "unloaded function ReadBuffer"));
    }
    glReadBuffer(static_cast<GLenum>(src));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void read_pixels(vec_2_i32 const& x, vec_2_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glReadPixels)
                Throw(undefined_behavior(
                    "unloaded function ReadPixels"));
    }
    glReadPixels(x.x(), x.y(), width[0], width[1], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void scissor(vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glScissor)
                Throw(undefined_behavior(
                    "unloaded function Scissor"));
    }
    glScissor(x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void stencil_func(groups::stencil_function func, i32 ref, u32 mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilFunc)
                Throw(undefined_behavior(
                    "unloaded function StencilFunc"));
    }
    glStencilFunc(static_cast<GLenum>(func), ref, mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void stencil_mask(u32 mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilMask)
                Throw(undefined_behavior(
                    "unloaded function StencilMask"));
    }
    glStencilMask(mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void stencil_op(groups::stencil_op fail, groups::stencil_op zfail, groups::stencil_op zpass)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilOp)
                Throw(undefined_behavior(
                    "unloaded function StencilOp"));
    }
    glStencilOp(static_cast<GLenum>(fail), static_cast<GLenum>(zfail), static_cast<GLenum>(zpass));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_image_1d(groups::texture_target target, i32 level, i32 internalformat, i32 width, i32 border, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexImage1D)
                Throw(undefined_behavior(
                    "unloaded function TexImage1D"));
    }
    glTexImage1D(static_cast<GLenum>(target), level, internalformat, width, border, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_image_2d(groups::texture_target target, i32 level, i32 internalformat, vec_2_i32 const& width, i32 border, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexImage2D)
                Throw(undefined_behavior(
                    "unloaded function TexImage2D"));
    }
    glTexImage2D(static_cast<GLenum>(target), level, internalformat, width[0], width[1], border, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void tex_parameter(groups::texture_target target, groups::texture_parameter_name pname, f32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameterf)
                Throw(undefined_behavior(
                    "unloaded function TexParameterf"));
    }
    glTexParameterf(static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void tex_parameter(groups::texture_target target, groups::texture_parameter_name pname, span_const_f32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameterfv)
                Throw(undefined_behavior(
                    "unloaded function TexParameterfv"));
    }
    glTexParameterfv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<const GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 0>>)
STATICINLINE void tex_parameter(groups::texture_target target, groups::texture_parameter_name pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameteri)
                Throw(undefined_behavior(
                    "unloaded function TexParameteri"));
    }
    glTexParameteri(static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void tex_parameter(groups::texture_target target, groups::texture_parameter_name pname, span_const_i32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameteriv)
                Throw(undefined_behavior(
                    "unloaded function TexParameteriv"));
    }
    glTexParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void viewport(vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glViewport)
                Throw(undefined_behavior(
                    "unloaded function Viewport"));
    }
    glViewport(x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_1_0
#ifdef GL_VERSION_1_1
/* Introduced in GL core 1.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 1>>)
STATICINLINE void bind_texture(groups::texture_target target, u32 texture)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindTexture)
                Throw(undefined_behavior(
                    "unloaded function BindTexture"));
        glIsTexture(texture);
    }
    glBindTexture(static_cast<GLenum>(target), texture);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_tex_image_1d(groups::texture_target target, i32 level, groups::internal_format internalformat, vec_2_i32 const& x, i32 width, i32 border)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTexImage1D)
                Throw(undefined_behavior(
                    "unloaded function CopyTexImage1D"));
    }
    glCopyTexImage1D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalformat), x.x(), x.y(), width, border);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_tex_image_2d(groups::texture_target target, i32 level, groups::internal_format internalformat, vec_2_i32 const& x, vec_2_i32 const& width, i32 border)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTexImage2D)
                Throw(undefined_behavior(
                    "unloaded function CopyTexImage2D"));
    }
    glCopyTexImage2D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalformat), x.x(), x.y(), width[0], width[1], border);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_tex_sub_image_1d(groups::texture_target target, i32 level, i32 xoffset, vec_2_i32 const& x, i32 width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTexSubImage1D)
                Throw(undefined_behavior(
                    "unloaded function CopyTexSubImage1D"));
    }
    glCopyTexSubImage1D(static_cast<GLenum>(target), level, xoffset, x.x(), x.y(), width);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_tex_sub_image_2d(groups::texture_target target, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTexSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function CopyTexSubImage2D"));
    }
    glCopyTexSubImage2D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_textures(span_const_u32 const& textures)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteTextures)
                Throw(undefined_behavior(
                    "unloaded function DeleteTextures"));
    }
    glDeleteTextures(textures.size(), reinterpret_cast<const GLuint *>(textures.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 1>>)
STATICINLINE void draw_arrays(groups::primitive_type mode, i32 first, i32 count)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawArrays)
                Throw(undefined_behavior(
                    "unloaded function DrawArrays"));
    }
    glDrawArrays(static_cast<GLenum>(mode), first, count);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 1>>)
STATICINLINE void draw_elements(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElements)
                Throw(undefined_behavior(
                    "unloaded function DrawElements"));
    }
    glDrawElements(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_textures(span_u32 textures)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenTextures)
                Throw(undefined_behavior(
                    "unloaded function GenTextures"));
    }
    glGenTextures(textures.size(), reinterpret_cast<GLuint *>(textures.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 1>>)
STATICINLINE GLboolean is_texture(u32 texture)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsTexture)
                Throw(undefined_behavior(
                    "unloaded function IsTexture"));
        glIsTexture(texture);
    }
    auto out = glIsTexture(texture);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 1.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 1>>)
STATICINLINE void polygon_offset(f32 factor, f32 units)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPolygonOffset)
                Throw(undefined_behavior(
                    "unloaded function PolygonOffset"));
    }
    glPolygonOffset(factor, units);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_sub_image_1d(groups::texture_target target, i32 level, i32 xoffset, i32 width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexSubImage1D)
                Throw(undefined_behavior(
                    "unloaded function TexSubImage1D"));
    }
    glTexSubImage1D(static_cast<GLenum>(target), level, xoffset, width, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.1 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_sub_image_2d(groups::texture_target target, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function TexSubImage2D"));
    }
    glTexSubImage2D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), width[0], width[1], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_1_1
#ifdef GL_VERSION_1_2
/* Introduced in GL core 1.2 */
template<
    class vec_2_i32,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<1, 2>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_tex_sub_image_3d(groups::texture_target target, i32 level, vec_3_i32 const& xoffset, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTexSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function CopyTexSubImage3D"));
    }
    glCopyTexSubImage3D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), xoffset.z(), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.2 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<1, 2>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void draw_range_elements(groups::primitive_type mode, u32 start, u32 end, i32 count, groups::draw_elements_type type, span_const_byte const& indices)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawRangeElements)
                Throw(undefined_behavior(
                    "unloaded function DrawRangeElements"));
    }
    glDrawRangeElements(static_cast<GLenum>(mode), start, end, count, static_cast<GLenum>(type), reinterpret_cast<const void *>(indices.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.2 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<1, 2>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_image_3d(groups::texture_target target, i32 level, i32 internalformat, vec_3_i32 const& width, i32 border, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexImage3D)
                Throw(undefined_behavior(
                    "unloaded function TexImage3D"));
    }
    glTexImage3D(static_cast<GLenum>(target), level, internalformat, width[0], width[1], width[2], border, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.2 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<1, 2>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_sub_image_3d(groups::texture_target target, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function TexSubImage3D"));
    }
    glTexSubImage3D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_1_2
#ifdef GL_VERSION_1_3
/* Introduced in GL core 1.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 3>>)
STATICINLINE void active_texture(groups::texture_unit texture)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glActiveTexture)
                Throw(undefined_behavior(
                    "unloaded function ActiveTexture"));
    }
    glActiveTexture(static_cast<GLenum>(texture));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.3 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<1, 3>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_image_1d(groups::texture_target target, i32 level, groups::internal_format internalformat, i32 width, i32 border, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexImage1D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexImage1D"));
    }
    glCompressedTexImage1D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalformat), width, border, data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.3 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 3>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_image_2d(groups::texture_target target, i32 level, groups::internal_format internalformat, vec_2_i32 const& width, i32 border, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexImage2D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexImage2D"));
    }
    glCompressedTexImage2D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalformat), width[0], width[1], border, data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.3 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<1, 3>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_image_3d(groups::texture_target target, i32 level, groups::internal_format internalformat, vec_3_i32 const& width, i32 border, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexImage3D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexImage3D"));
    }
    glCompressedTexImage3D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalformat), width[0], width[1], width[2], border, data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.3 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<1, 3>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_sub_image_1d(groups::texture_target target, i32 level, i32 xoffset, i32 width, groups::pixel_format format, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexSubImage1D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexSubImage1D"));
    }
    glCompressedTexSubImage1D(static_cast<GLenum>(target), level, xoffset, width, static_cast<GLenum>(format), data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.3 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<1, 3>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_sub_image_2d(groups::texture_target target, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& width, groups::pixel_format format, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexSubImage2D"));
    }
    glCompressedTexSubImage2D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), width[0], width[1], static_cast<GLenum>(format), data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.3 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<1, 3>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_sub_image_3d(groups::texture_target target, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, groups::pixel_format format, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexSubImage3D"));
    }
    glCompressedTexSubImage3D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], static_cast<GLenum>(format), data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.3 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<1, 3>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_compressed_tex_image(groups::texture_target target, i32 level, span_byte img)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetCompressedTexImage)
                Throw(undefined_behavior(
                    "unloaded function GetCompressedTexImage"));
    }
    glGetCompressedTexImage(static_cast<GLenum>(target), level, reinterpret_cast<void *>(img.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 3>>)
STATICINLINE void sample_coverage(f32 value, bool invert)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSampleCoverage)
                Throw(undefined_behavior(
                    "unloaded function SampleCoverage"));
    }
    glSampleCoverage(value, invert);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_1_3
#ifdef GL_VERSION_1_4
/* Introduced in GL core 1.4 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 4>>)
STATICINLINE void blend_func_separate(groups::blending_factor sfactorRGB, groups::blending_factor dfactorRGB, groups::blending_factor sfactorAlpha, groups::blending_factor dfactorAlpha)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendFuncSeparate)
                Throw(undefined_behavior(
                    "unloaded function BlendFuncSeparate"));
    }
    glBlendFuncSeparate(static_cast<GLenum>(sfactorRGB), static_cast<GLenum>(dfactorRGB), static_cast<GLenum>(sfactorAlpha), static_cast<GLenum>(dfactorAlpha));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.4 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<1, 4>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void multi_draw_arrays(groups::primitive_type mode, span_const_i32 first, span_const_i32 count)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiDrawArrays)
                Throw(undefined_behavior(
                    "unloaded function MultiDrawArrays"));
    }
    GLsizei drawcount = first.size();
    detail::assert_equal(first.size(), drawcount);
    detail::assert_equal(count.size(), drawcount);
    glMultiDrawArrays(static_cast<GLenum>(mode), first.data(), count.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.4 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<1, 4>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void multi_draw_elements(groups::primitive_type mode, span_const_i32 count, groups::draw_elements_type type, ptroff indices)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiDrawElements)
                Throw(undefined_behavior(
                    "unloaded function MultiDrawElements"));
    }
    GLsizei drawcount = count.size();
    detail::assert_equal(count.size(), drawcount);
    glMultiDrawElements(static_cast<GLenum>(mode), count.data(), static_cast<GLenum>(type), reinterpret_cast<const void*>(indices));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.4 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 4>>)
STATICINLINE void point_parameter(groups::point_parameter_name_arb pname, f32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPointParameterf)
                Throw(undefined_behavior(
                    "unloaded function PointParameterf"));
    }
    glPointParameterf(static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.4 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<1, 4>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void point_parameter(groups::point_parameter_name_arb pname, span_const_f32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPointParameterfv)
                Throw(undefined_behavior(
                    "unloaded function PointParameterfv"));
    }
    glPointParameterfv(static_cast<GLenum>(pname), reinterpret_cast<const GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.4 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 4>>)
STATICINLINE void point_parameter(groups::point_parameter_name_arb pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPointParameteri)
                Throw(undefined_behavior(
                    "unloaded function PointParameteri"));
    }
    glPointParameteri(static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.4 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<1, 4>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void point_parameter(groups::point_parameter_name_arb pname, span_const_i32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPointParameteriv)
                Throw(undefined_behavior(
                    "unloaded function PointParameteriv"));
    }
    glPointParameteriv(static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.4 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<1, 4>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void blend_color(vec_4_f32 const& red)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendColor)
                Throw(undefined_behavior(
                    "unloaded function BlendColor"));
    }
    glBlendColor(red.x(), red.y(), red.z(), red.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.4 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 4>>)
STATICINLINE void blend_equation(groups::blend_equation_mode_ext mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendEquation)
                Throw(undefined_behavior(
                    "unloaded function BlendEquation"));
    }
    glBlendEquation(static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_1_4
#ifdef GL_VERSION_1_5
/* Introduced in GL core 1.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 5>>)
STATICINLINE void begin_query(groups::query_target target, u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBeginQuery)
                Throw(undefined_behavior(
                    "unloaded function BeginQuery"));
    }
    glBeginQuery(static_cast<GLenum>(target), id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 5>>)
STATICINLINE void bind_buffer(groups::buffer_target_arb target, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindBuffer)
                Throw(undefined_behavior(
                    "unloaded function BindBuffer"));
        glIsBuffer(buffer);
    }
    glBindBuffer(static_cast<GLenum>(target), buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void buffer_data(groups::buffer_target_arb target, span_const_byte const& data, groups::buffer_usage_arb usage)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBufferData)
                Throw(undefined_behavior(
                    "unloaded function BufferData"));
    }
    glBufferData(static_cast<GLenum>(target), data.size(), reinterpret_cast<const void *>(data.data()), static_cast<GLenum>(usage));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void buffer_sub_data(groups::buffer_target_arb target, GLintptr offset, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function BufferSubData"));
    }
    glBufferSubData(static_cast<GLenum>(target), offset, data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_buffers(span_const_u32 const& buffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteBuffers)
                Throw(undefined_behavior(
                    "unloaded function DeleteBuffers"));
    }
    glDeleteBuffers(buffers.size(), reinterpret_cast<const GLuint *>(buffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_queries(span_const_u32 const& ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteQueries)
                Throw(undefined_behavior(
                    "unloaded function DeleteQueries"));
    }
    glDeleteQueries(ids.size(), reinterpret_cast<const GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 5>>)
STATICINLINE void end_query(groups::query_target target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEndQuery)
                Throw(undefined_behavior(
                    "unloaded function EndQuery"));
    }
    glEndQuery(static_cast<GLenum>(target));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_buffers(span_u32 buffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenBuffers)
                Throw(undefined_behavior(
                    "unloaded function GenBuffers"));
    }
    glGenBuffers(buffers.size(), reinterpret_cast<GLuint *>(buffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_queries(span_u32 ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenQueries)
                Throw(undefined_behavior(
                    "unloaded function GenQueries"));
    }
    glGenQueries(ids.size(), reinterpret_cast<GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_buffer_parameter(groups::buffer_target_arb target, groups::buffer_prop_arb pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetBufferParameteriv"));
    }
    glGetBufferParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_buffer_pointerv(groups::buffer_target_arb target, groups::buffer_pointer_name_arb pname, span_byte params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBufferPointerv)
                Throw(undefined_behavior(
                    "unloaded function GetBufferPointerv"));
    }
    glGetBufferPointerv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<void **>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_buffer_sub_data(groups::buffer_target_arb target, GLintptr offset, span_byte data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function GetBufferSubData"));
    }
    glGetBufferSubData(static_cast<GLenum>(target), offset, data.size(), reinterpret_cast<void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_query_objectiv(u32 id, groups::query_object_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryObjectiv)
                Throw(undefined_behavior(
                    "unloaded function GetQueryObjectiv"));
    }
    glGetQueryObjectiv(id, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_query_objectuiv(u32 id, groups::query_object_parameter_name pname, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryObjectuiv)
                Throw(undefined_behavior(
                    "unloaded function GetQueryObjectuiv"));
    }
    glGetQueryObjectuiv(id, static_cast<GLenum>(pname), reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<1, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_queryiv(groups::query_target target, groups::query_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryiv)
                Throw(undefined_behavior(
                    "unloaded function GetQueryiv"));
    }
    glGetQueryiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 1.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 5>>)
STATICINLINE GLboolean is_buffer(u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsBuffer)
                Throw(undefined_behavior(
                    "unloaded function IsBuffer"));
        glIsBuffer(buffer);
    }
    auto out = glIsBuffer(buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 1.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 5>>)
STATICINLINE GLboolean is_query(u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsQuery)
                Throw(undefined_behavior(
                    "unloaded function IsQuery"));
    }
    auto out = glIsQuery(id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 1.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 5>>)
STATICINLINE void * map_buffer(groups::buffer_target_arb target, groups::buffer_access_arb access)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMapBuffer)
                Throw(undefined_behavior(
                    "unloaded function MapBuffer"));
    }
    auto out = glMapBuffer(static_cast<GLenum>(target), static_cast<GLenum>(access));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 1.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<1, 5>>)
STATICINLINE GLboolean unmap_buffer(groups::buffer_target_arb target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUnmapBuffer)
                Throw(undefined_behavior(
                    "unloaded function UnmapBuffer"));
    }
    auto out = glUnmapBuffer(static_cast<GLenum>(target));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

#endif // GL_VERSION_1_5
#ifdef GL_VERSION_2_0
/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void attach_shader(u32 program, u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glAttachShader)
                Throw(undefined_behavior(
                    "unloaded function AttachShader"));
        glIsProgram(program);
        glIsShader(shader);
    }
    glAttachShader(program, shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void bind_attrib_location(u32 program, u32 index, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindAttribLocation)
                Throw(undefined_behavior(
                    "unloaded function BindAttribLocation"));
        glIsProgram(program);
    }
    glBindAttribLocation(program, index, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void blend_equation_separate(groups::blend_equation_mode_ext modeRGB, groups::blend_equation_mode_ext modeAlpha)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendEquationSeparate)
                Throw(undefined_behavior(
                    "unloaded function BlendEquationSeparate"));
    }
    glBlendEquationSeparate(static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void compile_shader(u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompileShader)
                Throw(undefined_behavior(
                    "unloaded function CompileShader"));
        glIsShader(shader);
    }
    glCompileShader(shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLuint create_program()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateProgram)
                Throw(undefined_behavior(
                    "unloaded function CreateProgram"));
    }
    auto out = glCreateProgram();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLuint create_shader(GLenum type)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateShader)
                Throw(undefined_behavior(
                    "unloaded function CreateShader"));
    }
    auto out = glCreateShader(type);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void delete_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteProgram)
                Throw(undefined_behavior(
                    "unloaded function DeleteProgram"));
        glIsProgram(program);
    }
    glDeleteProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void delete_shader(u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteShader)
                Throw(undefined_behavior(
                    "unloaded function DeleteShader"));
        glIsShader(shader);
    }
    glDeleteShader(shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void detach_shader(u32 program, u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDetachShader)
                Throw(undefined_behavior(
                    "unloaded function DetachShader"));
        glIsProgram(program);
        glIsShader(shader);
    }
    glDetachShader(program, shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void disable_vertex_attrib_array(u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDisableVertexAttribArray)
                Throw(undefined_behavior(
                    "unloaded function DisableVertexAttribArray"));
    }
    glDisableVertexAttribArray(index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_GLenum
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum>)
STATICINLINE void draw_buffers(span_const_GLenum const& bufs)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawBuffers)
                Throw(undefined_behavior(
                    "unloaded function DrawBuffers"));
    }
    glDrawBuffers(bufs.size(), reinterpret_cast<const GLenum *>(bufs.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void enable_vertex_attrib_array(u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEnableVertexAttribArray)
                Throw(undefined_behavior(
                    "unloaded function EnableVertexAttribArray"));
    }
    glEnableVertexAttribArray(index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_attrib(u32 program, u32 index, i32& length, i32& size, GLenum& type, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveAttrib)
                Throw(undefined_behavior(
                    "unloaded function GetActiveAttrib"));
        glIsProgram(program);
    }
    glGetActiveAttrib(program, index, name.size(), &length, &size, &type, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_uniform(u32 program, u32 index, i32& length, i32& size, GLenum& type, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniform)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniform"));
        glIsProgram(program);
    }
    glGetActiveUniform(program, index, name.size(), &length, &size, &type, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_attached_shaders(u32 program, i32& count, span_u32 shaders)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetAttachedShaders)
                Throw(undefined_behavior(
                    "unloaded function GetAttachedShaders"));
        glIsProgram(program);
    }
    glGetAttachedShaders(program, shaders.size(), &count, reinterpret_cast<GLuint *>(shaders.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLint get_attrib_location(u32 program, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetAttribLocation)
                Throw(undefined_behavior(
                    "unloaded function GetAttribLocation"));
        glIsProgram(program);
    }
    auto out = glGetAttribLocation(program, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_program_info_log(u32 program, i32& length, span_GLchar infoLog)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramInfoLog)
                Throw(undefined_behavior(
                    "unloaded function GetProgramInfoLog"));
        glIsProgram(program);
    }
    glGetProgramInfoLog(program, infoLog.size(), &length, infoLog.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_programiv(u32 program, groups::program_property_arb pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramiv"));
        glIsProgram(program);
    }
    glGetProgramiv(program, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_shader_info_log(u32 shader, i32& length, span_GLchar infoLog)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetShaderInfoLog)
                Throw(undefined_behavior(
                    "unloaded function GetShaderInfoLog"));
        glIsShader(shader);
    }
    glGetShaderInfoLog(shader, infoLog.size(), &length, infoLog.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_shader_source(u32 shader, i32& length, span_GLchar source)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetShaderSource)
                Throw(undefined_behavior(
                    "unloaded function GetShaderSource"));
        glIsShader(shader);
    }
    glGetShaderSource(shader, source.size(), &length, source.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_shaderiv(u32 shader, groups::shader_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetShaderiv)
                Throw(undefined_behavior(
                    "unloaded function GetShaderiv"));
        glIsShader(shader);
    }
    glGetShaderiv(shader, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLint get_uniform_location(u32 program, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformLocation)
                Throw(undefined_behavior(
                    "unloaded function GetUniformLocation"));
        glIsProgram(program);
    }
    auto out = glGetUniformLocation(program, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 2.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_uniformfv(u32 program, i32 location, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformfv)
                Throw(undefined_behavior(
                    "unloaded function GetUniformfv"));
        glIsProgram(program);
    }
    glGetUniformfv(program, location, reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_uniformiv(u32 program, i32 location, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformiv)
                Throw(undefined_behavior(
                    "unloaded function GetUniformiv"));
        glIsProgram(program);
    }
    glGetUniformiv(program, location, reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_vertex_attrib_pointerv(u32 index, groups::vertex_attrib_pointer_property_arb pname, span_byte pointer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribPointerv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribPointerv"));
    }
    glGetVertexAttribPointerv(index, static_cast<GLenum>(pname), reinterpret_cast<void **>(pointer.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_f64
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<std::decay_t<typename span_f64::value_type>, f64>)
STATICINLINE void get_vertex_attribdv(u32 index, groups::vertex_attrib_property_arb pname, span_f64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribdv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribdv"));
    }
    glGetVertexAttribdv(index, static_cast<GLenum>(pname), reinterpret_cast<GLdouble *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_vertex_attribfv(u32 index, groups::vertex_attrib_property_arb pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribfv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribfv"));
    }
    glGetVertexAttribfv(index, static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_vertex_attribiv(u32 index, groups::vertex_attrib_property_arb pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribiv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribiv"));
    }
    glGetVertexAttribiv(index, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsProgram)
                Throw(undefined_behavior(
                    "unloaded function IsProgram"));
        glIsProgram(program);
    }
    auto out = glIsProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_shader(u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsShader)
                Throw(undefined_behavior(
                    "unloaded function IsShader"));
        glIsShader(shader);
    }
    auto out = glIsShader(shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void link_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glLinkProgram)
                Throw(undefined_behavior(
                    "unloaded function LinkProgram"));
        glIsProgram(program);
    }
    glLinkProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void shader_source(u32 shader, std::vector<std::string_view> string, span_const_i32 const& length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glShaderSource)
                Throw(undefined_behavior(
                    "unloaded function ShaderSource"));
        glIsShader(shader);
    }
    auto [string_lens, string_cstr, string_store] = detail::transform_strings(string);
    glShaderSource(shader, string_cstr.size(), string_cstr.data(), reinterpret_cast<const GLint *>(length.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_func_separate(groups::stencil_face_direction face, groups::stencil_function func, i32 ref, u32 mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilFuncSeparate)
                Throw(undefined_behavior(
                    "unloaded function StencilFuncSeparate"));
    }
    glStencilFuncSeparate(static_cast<GLenum>(face), static_cast<GLenum>(func), ref, mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_mask_separate(groups::stencil_face_direction face, u32 mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilMaskSeparate)
                Throw(undefined_behavior(
                    "unloaded function StencilMaskSeparate"));
    }
    glStencilMaskSeparate(static_cast<GLenum>(face), mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_op_separate(groups::stencil_face_direction face, groups::stencil_op sfail, groups::stencil_op dpfail, groups::stencil_op dppass)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilOpSeparate)
                Throw(undefined_behavior(
                    "unloaded function StencilOpSeparate"));
    }
    glStencilOpSeparate(static_cast<GLenum>(face), static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLenum>(dppass));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void uniform(i32 location, f32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1f)
                Throw(undefined_behavior(
                    "unloaded function Uniform1f"));
    }
    glUniform1f(location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void uniform(i32 location, i32 count, span_const_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1fv)
                Throw(undefined_behavior(
                    "unloaded function Uniform1fv"));
    }
    glUniform1fv(location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void uniform(i32 location, i32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1i)
                Throw(undefined_behavior(
                    "unloaded function Uniform1i"));
    }
    glUniform1i(location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void uniform(i32 location, i32 count, span_const_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1iv)
                Throw(undefined_behavior(
                    "unloaded function Uniform1iv"));
    }
    glUniform1iv(location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_f32, f32, 2>)
STATICINLINE void uniform(i32 location, vec_2_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2f)
                Throw(undefined_behavior(
                    "unloaded function Uniform2f"));
    }
    glUniform2f(location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_2_f32> &&
    semantic::concepts::Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2fv)
                Throw(undefined_behavior(
                    "unloaded function Uniform2fv"));
    }
    glUniform2fv(location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void uniform(i32 location, vec_2_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2i)
                Throw(undefined_behavior(
                    "unloaded function Uniform2i"));
    }
    glUniform2i(location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_2_i32> &&
    semantic::concepts::Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2iv)
                Throw(undefined_behavior(
                    "unloaded function Uniform2iv"));
    }
    glUniform2iv(location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_3_f32, f32, 3>)
STATICINLINE void uniform(i32 location, vec_3_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3f)
                Throw(undefined_behavior(
                    "unloaded function Uniform3f"));
    }
    glUniform3f(location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_3_f32> &&
    semantic::concepts::Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3fv)
                Throw(undefined_behavior(
                    "unloaded function Uniform3fv"));
    }
    glUniform3fv(location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void uniform(i32 location, vec_3_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3i)
                Throw(undefined_behavior(
                    "unloaded function Uniform3i"));
    }
    glUniform3i(location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_3_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_3_i32> &&
    semantic::concepts::Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3iv)
                Throw(undefined_behavior(
                    "unloaded function Uniform3iv"));
    }
    glUniform3iv(location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void uniform(i32 location, vec_4_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4f)
                Throw(undefined_behavior(
                    "unloaded function Uniform4f"));
    }
    glUniform4f(location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_f32> &&
    semantic::concepts::Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4fv)
                Throw(undefined_behavior(
                    "unloaded function Uniform4fv"));
    }
    glUniform4fv(location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_4_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_i32, i32, 4>)
STATICINLINE void uniform(i32 location, vec_4_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4i)
                Throw(undefined_behavior(
                    "unloaded function Uniform4i"));
    }
    glUniform4i(location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_4_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_i32> &&
    semantic::concepts::Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4iv)
                Throw(undefined_behavior(
                    "unloaded function Uniform4iv"));
    }
    glUniform4iv(location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_mat_2x2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_mat_2x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2fv"));
    }
    glUniformMatrix2fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_mat_3x3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_mat_3x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3fv"));
    }
    glUniformMatrix3fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_mat_4x4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_mat_4x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4fv"));
    }
    glUniformMatrix4fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void use_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUseProgram)
                Throw(undefined_behavior(
                    "unloaded function UseProgram"));
        glIsProgram(program);
    }
    glUseProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void validate_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glValidateProgram)
                Throw(undefined_behavior(
                    "unloaded function ValidateProgram"));
        glIsProgram(program);
    }
    glValidateProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void vertex_attrib1d(u32 index, f64 x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib1d)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib1d"));
    }
    glVertexAttrib1d(index, x);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_f64
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_f64> &&
    std::is_same_v<std::decay_t<typename span_const_f64::value_type>, f64>)
STATICINLINE void vertex_attrib1dv(u32 index, span_const_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib1dv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib1dv"));
    }
    glVertexAttrib1dv(index, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void vertex_attrib1f(u32 index, f32 x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib1f)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib1f"));
    }
    glVertexAttrib1f(index, x);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void vertex_attrib1fv(u32 index, span_const_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib1fv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib1fv"));
    }
    glVertexAttrib1fv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void vertex_attrib1s(u32 index, i16 x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib1s)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib1s"));
    }
    glVertexAttrib1s(index, x);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_i16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i16> &&
    std::is_same_v<std::decay_t<typename span_const_i16::value_type>, i16>)
STATICINLINE void vertex_attrib1sv(u32 index, span_const_i16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib1sv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib1sv"));
    }
    glVertexAttrib1sv(index, reinterpret_cast<const GLshort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_2_f64
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_f64, f64, 2>)
STATICINLINE void vertex_attrib2d(u32 index, vec_2_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib2d)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib2d"));
    }
    glVertexAttrib2d(index, x.x(), x.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_2_f64
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_2_f64> &&
    semantic::concepts::Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
STATICINLINE void vertex_attrib2dv(u32 index, span_const_vec_2_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib2dv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib2dv"));
    }
    glVertexAttrib2dv(index, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_f32, f32, 2>)
STATICINLINE void vertex_attrib2f(u32 index, vec_2_f32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib2f)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib2f"));
    }
    glVertexAttrib2f(index, x.x(), x.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_2_f32> &&
    semantic::concepts::Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
STATICINLINE void vertex_attrib2fv(u32 index, span_const_vec_2_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib2fv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib2fv"));
    }
    glVertexAttrib2fv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_2_i16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i16, i16, 2>)
STATICINLINE void vertex_attrib2s(u32 index, vec_2_i16 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib2s)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib2s"));
    }
    glVertexAttrib2s(index, x.x(), x.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_2_i16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_2_i16> &&
    semantic::concepts::Vector<typename span_const_vec_2_i16::value_type, i16, 2>)
STATICINLINE void vertex_attrib2sv(u32 index, span_const_vec_2_i16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib2sv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib2sv"));
    }
    glVertexAttrib2sv(index, reinterpret_cast<const GLshort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_3_f64
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_3_f64, f64, 3>)
STATICINLINE void vertex_attrib3d(u32 index, vec_3_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib3d)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib3d"));
    }
    glVertexAttrib3d(index, x.x(), x.y(), x.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_3_f64
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_3_f64> &&
    semantic::concepts::Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
STATICINLINE void vertex_attrib3dv(u32 index, span_const_vec_3_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib3dv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib3dv"));
    }
    glVertexAttrib3dv(index, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_3_f32, f32, 3>)
STATICINLINE void vertex_attrib3f(u32 index, vec_3_f32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib3f)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib3f"));
    }
    glVertexAttrib3f(index, x.x(), x.y(), x.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_3_f32> &&
    semantic::concepts::Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
STATICINLINE void vertex_attrib3fv(u32 index, span_const_vec_3_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib3fv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib3fv"));
    }
    glVertexAttrib3fv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_3_i16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_3_i16, i16, 3>)
STATICINLINE void vertex_attrib3s(u32 index, vec_3_i16 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib3s)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib3s"));
    }
    glVertexAttrib3s(index, x.x(), x.y(), x.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_3_i16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_3_i16> &&
    semantic::concepts::Vector<typename span_const_vec_3_i16::value_type, i16, 3>)
STATICINLINE void vertex_attrib3sv(u32 index, span_const_vec_3_i16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib3sv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib3sv"));
    }
    glVertexAttrib3sv(index, reinterpret_cast<const GLshort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_i8
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i8> &&
    std::is_same_v<std::decay_t<typename span_const_i8::value_type>, i8>)
STATICINLINE void vertex_attrib4_nbv(u32 index, span_const_i8 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4Nbv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4Nbv"));
    }
    glVertexAttrib4Nbv(index, reinterpret_cast<const GLbyte *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void vertex_attrib4_niv(u32 index, span_const_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4Niv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4Niv"));
    }
    glVertexAttrib4Niv(index, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_i16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i16> &&
    std::is_same_v<std::decay_t<typename span_const_i16::value_type>, i16>)
STATICINLINE void vertex_attrib4_nsv(u32 index, span_const_i16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4Nsv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4Nsv"));
    }
    glVertexAttrib4Nsv(index, reinterpret_cast<const GLshort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_4_u8
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_u8, u8, 4>)
STATICINLINE void vertex_attrib4_nub(u32 index, vec_4_u8 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4Nub)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4Nub"));
    }
    glVertexAttrib4Nub(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_u8
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u8> &&
    std::is_same_v<std::decay_t<typename span_const_u8::value_type>, u8>)
STATICINLINE void vertex_attrib4_nubv(u32 index, span_const_u8 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4Nubv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4Nubv"));
    }
    glVertexAttrib4Nubv(index, reinterpret_cast<const GLubyte *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void vertex_attrib4_nuiv(u32 index, span_const_u32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4Nuiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4Nuiv"));
    }
    glVertexAttrib4Nuiv(index, reinterpret_cast<const GLuint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_u16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u16> &&
    std::is_same_v<std::decay_t<typename span_const_u16::value_type>, u16>)
STATICINLINE void vertex_attrib4_nusv(u32 index, span_const_u16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4Nusv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4Nusv"));
    }
    glVertexAttrib4Nusv(index, reinterpret_cast<const GLushort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_i8
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i8> &&
    std::is_same_v<std::decay_t<typename span_const_i8::value_type>, i8>)
STATICINLINE void vertex_attrib4bv(u32 index, span_const_i8 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4bv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4bv"));
    }
    glVertexAttrib4bv(index, reinterpret_cast<const GLbyte *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_4_f64
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_f64, f64, 4>)
STATICINLINE void vertex_attrib4d(u32 index, vec_4_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4d)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4d"));
    }
    glVertexAttrib4d(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_4_f64
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_f64> &&
    semantic::concepts::Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
STATICINLINE void vertex_attrib4dv(u32 index, span_const_vec_4_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4dv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4dv"));
    }
    glVertexAttrib4dv(index, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void vertex_attrib4f(u32 index, vec_4_f32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4f)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4f"));
    }
    glVertexAttrib4f(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_f32> &&
    semantic::concepts::Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
STATICINLINE void vertex_attrib4fv(u32 index, span_const_vec_4_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4fv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4fv"));
    }
    glVertexAttrib4fv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_4_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_i32> &&
    semantic::concepts::Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
STATICINLINE void vertex_attrib4iv(u32 index, span_const_vec_4_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4iv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4iv"));
    }
    glVertexAttrib4iv(index, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class vec_4_i16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_i16, i16, 4>)
STATICINLINE void vertex_attrib4s(u32 index, vec_4_i16 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4s)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4s"));
    }
    glVertexAttrib4s(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_4_i16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_i16> &&
    semantic::concepts::Vector<typename span_const_vec_4_i16::value_type, i16, 4>)
STATICINLINE void vertex_attrib4sv(u32 index, span_const_vec_4_i16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4sv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4sv"));
    }
    glVertexAttrib4sv(index, reinterpret_cast<const GLshort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_u8
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u8> &&
    std::is_same_v<std::decay_t<typename span_const_u8::value_type>, u8>)
STATICINLINE void vertex_attrib4ubv(u32 index, span_const_u8 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4ubv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4ubv"));
    }
    glVertexAttrib4ubv(index, reinterpret_cast<const GLubyte *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void vertex_attrib4uiv(u32 index, span_const_vec_4_u32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4uiv"));
    }
    glVertexAttrib4uiv(index, reinterpret_cast<const GLuint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_vec_4_u16
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_u16> &&
    semantic::concepts::Vector<typename span_const_vec_4_u16::value_type, u16, 4>)
STATICINLINE void vertex_attrib4usv(u32 index, span_const_vec_4_u16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4usv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4usv"));
    }
    glVertexAttrib4usv(index, reinterpret_cast<const GLushort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void vertex_attrib_pointer(u32 index, i32 size, groups::vertex_attrib_pointer_type type, bool normalized, i32 stride, span_const_byte const& pointer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribPointer)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribPointer"));
    }
    glVertexAttribPointer(index, size, static_cast<GLenum>(type), normalized, stride, reinterpret_cast<const void *>(pointer.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_2_0
#ifdef GL_VERSION_2_1
/* Introduced in GL core 2.1 */
template<
    class span_const_mat_2x3_f32
>
requires (
    MinimumVersion<Current, Version<2, 1>> &&
    semantic::concepts::Span<span_const_mat_2x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2x3fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2x3fv"));
    }
    glUniformMatrix2x3fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.1 */
template<
    class span_const_mat_2x4_f32
>
requires (
    MinimumVersion<Current, Version<2, 1>> &&
    semantic::concepts::Span<span_const_mat_2x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2x4fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2x4fv"));
    }
    glUniformMatrix2x4fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.1 */
template<
    class span_const_mat_3x2_f32
>
requires (
    MinimumVersion<Current, Version<2, 1>> &&
    semantic::concepts::Span<span_const_mat_3x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3x2fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3x2fv"));
    }
    glUniformMatrix3x2fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.1 */
template<
    class span_const_mat_3x4_f32
>
requires (
    MinimumVersion<Current, Version<2, 1>> &&
    semantic::concepts::Span<span_const_mat_3x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3x4fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3x4fv"));
    }
    glUniformMatrix3x4fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.1 */
template<
    class span_const_mat_4x2_f32
>
requires (
    MinimumVersion<Current, Version<2, 1>> &&
    semantic::concepts::Span<span_const_mat_4x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4x2fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4x2fv"));
    }
    glUniformMatrix4x2fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 2.1 */
template<
    class span_const_mat_4x3_f32
>
requires (
    MinimumVersion<Current, Version<2, 1>> &&
    semantic::concepts::Span<span_const_mat_4x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4x3fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4x3fv"));
    }
    glUniformMatrix4x3fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_2_1
#ifdef GL_VERSION_3_0
/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void begin_conditional_render(u32 id, groups::conditional_render_mode mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBeginConditionalRender)
                Throw(undefined_behavior(
                    "unloaded function BeginConditionalRender"));
    }
    glBeginConditionalRender(id, static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void begin_transform_feedback(groups::primitive_type primitiveMode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBeginTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function BeginTransformFeedback"));
    }
    glBeginTransformFeedback(static_cast<GLenum>(primitiveMode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_buffer_base(groups::buffer_target_arb target, u32 index, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindBufferBase)
                Throw(undefined_behavior(
                    "unloaded function BindBufferBase"));
        glIsBuffer(buffer);
    }
    glBindBufferBase(static_cast<GLenum>(target), index, buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_buffer_range(groups::buffer_target_arb target, u32 index, u32 buffer, GLintptr offset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindBufferRange)
                Throw(undefined_behavior(
                    "unloaded function BindBufferRange"));
        glIsBuffer(buffer);
    }
    glBindBufferRange(static_cast<GLenum>(target), index, buffer, offset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_frag_data_location(u32 program, u32 color, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindFragDataLocation)
                Throw(undefined_behavior(
                    "unloaded function BindFragDataLocation"));
        glIsProgram(program);
    }
    glBindFragDataLocation(program, color, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void clamp_color(groups::clamp_color_target_arb target, groups::clamp_color_mode_arb clamp)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClampColor)
                Throw(undefined_behavior(
                    "unloaded function ClampColor"));
    }
    glClampColor(static_cast<GLenum>(target), static_cast<GLenum>(clamp));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void clear_bufferfi(groups::buffer buffer, i32 drawbuffer, f32 depth, i32 stencil)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferfi)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferfi"));
    }
    glClearBufferfi(static_cast<GLenum>(buffer), drawbuffer, depth, stencil);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void clear_bufferfv(groups::buffer buffer, i32 drawbuffer, span_const_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferfv)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferfv"));
    }
    glClearBufferfv(static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void clear_bufferiv(groups::buffer buffer, i32 drawbuffer, span_const_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferiv)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferiv"));
    }
    glClearBufferiv(static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void clear_bufferuiv(groups::buffer buffer, i32 drawbuffer, span_const_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferuiv)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferuiv"));
    }
    glClearBufferuiv(static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void color_maski(u32 index, bool r, bool g, bool b, bool a)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glColorMaski)
                Throw(undefined_behavior(
                    "unloaded function ColorMaski"));
    }
    glColorMaski(index, r, g, b, a);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void disablei(groups::enable_cap target, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDisablei)
                Throw(undefined_behavior(
                    "unloaded function Disablei"));
    }
    glDisablei(static_cast<GLenum>(target), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void enablei(groups::enable_cap target, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEnablei)
                Throw(undefined_behavior(
                    "unloaded function Enablei"));
    }
    glEnablei(static_cast<GLenum>(target), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void end_conditional_render()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEndConditionalRender)
                Throw(undefined_behavior(
                    "unloaded function EndConditionalRender"));
    }
    glEndConditionalRender();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void end_transform_feedback()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEndTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function EndTransformFeedback"));
    }
    glEndTransformFeedback();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_bool
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_bool> &&
    std::is_same_v<std::decay_t<typename span_bool::value_type>, bool>)
STATICINLINE void get_booleani_v(groups::buffer_target_arb target, u32 index, span_bool data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBooleani_v)
                Throw(undefined_behavior(
                    "unloaded function GetBooleani_v"));
    }
    glGetBooleani_v(static_cast<GLenum>(target), index, reinterpret_cast<GLboolean *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLint get_frag_data_location(u32 program, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFragDataLocation)
                Throw(undefined_behavior(
                    "unloaded function GetFragDataLocation"));
        glIsProgram(program);
    }
    auto out = glGetFragDataLocation(program, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_integeri_v(groups::get_prop target, u32 index, span_i32 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetIntegeri_v)
                Throw(undefined_behavior(
                    "unloaded function GetIntegeri_v"));
    }
    glGetIntegeri_v(static_cast<GLenum>(target), index, reinterpret_cast<GLint *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE const GLubyte * get_stringi(groups::string_name name, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetStringi)
                Throw(undefined_behavior(
                    "unloaded function GetStringi"));
    }
    auto out = glGetStringi(static_cast<GLenum>(name), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_tex_parameter_iiv(groups::texture_target target, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function GetTexParameterIiv"));
    }
    glGetTexParameterIiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_tex_parameter_iuiv(groups::texture_target target, groups::get_texture_parameter pname, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function GetTexParameterIuiv"));
    }
    glGetTexParameterIuiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_transform_feedback_varying(u32 program, u32 index, i32& length, i32& size, GLenum& type, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTransformFeedbackVarying)
                Throw(undefined_behavior(
                    "unloaded function GetTransformFeedbackVarying"));
        glIsProgram(program);
    }
    glGetTransformFeedbackVarying(program, index, name.size(), &length, &size, &type, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_uniformuiv(u32 program, i32 location, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformuiv)
                Throw(undefined_behavior(
                    "unloaded function GetUniformuiv"));
        glIsProgram(program);
    }
    glGetUniformuiv(program, location, reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void get_vertex_attrib_iiv(u32 index, groups::vertex_attrib_enum pname, i32& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribIiv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribIiv"));
    }
    glGetVertexAttribIiv(index, static_cast<GLenum>(pname), &params);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void get_vertex_attrib_iuiv(u32 index, groups::vertex_attrib_enum pname, u32& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribIuiv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribIuiv"));
    }
    glGetVertexAttribIuiv(index, static_cast<GLenum>(pname), &params);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_enabledi(groups::enable_cap target, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsEnabledi)
                Throw(undefined_behavior(
                    "unloaded function IsEnabledi"));
    }
    auto out = glIsEnabledi(static_cast<GLenum>(target), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void tex_parameter_iiv(groups::texture_target target, groups::texture_parameter_name pname, span_const_i32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function TexParameterIiv"));
    }
    glTexParameterIiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void tex_parameter_iuiv(groups::texture_target target, groups::texture_parameter_name pname, span_const_u32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function TexParameterIuiv"));
    }
    glTexParameterIuiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<const GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void transform_feedback_varyings(u32 program, std::vector<std::string_view> varyings, groups::transform_feedback_buffer_mode bufferMode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTransformFeedbackVaryings)
                Throw(undefined_behavior(
                    "unloaded function TransformFeedbackVaryings"));
        glIsProgram(program);
    }
    auto [varyings_lens, varyings_cstr, varyings_store] = detail::transform_strings(varyings);
    glTransformFeedbackVaryings(program, varyings_cstr.size(), varyings_cstr.data(), static_cast<GLenum>(bufferMode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void uniform(i32 location, u32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1ui)
                Throw(undefined_behavior(
                    "unloaded function Uniform1ui"));
    }
    glUniform1ui(location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void uniform(i32 location, i32 count, span_const_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1uiv)
                Throw(undefined_behavior(
                    "unloaded function Uniform1uiv"));
    }
    glUniform1uiv(location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_2_u32, u32, 2>)
STATICINLINE void uniform(i32 location, vec_2_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2ui)
                Throw(undefined_behavior(
                    "unloaded function Uniform2ui"));
    }
    glUniform2ui(location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2uiv)
                Throw(undefined_behavior(
                    "unloaded function Uniform2uiv"));
    }
    glUniform2uiv(location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_u32, u32, 3>)
STATICINLINE void uniform(i32 location, vec_3_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3ui)
                Throw(undefined_behavior(
                    "unloaded function Uniform3ui"));
    }
    glUniform3ui(location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3uiv)
                Throw(undefined_behavior(
                    "unloaded function Uniform3uiv"));
    }
    glUniform3uiv(location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_4_u32, u32, 4>)
STATICINLINE void uniform(i32 location, vec_4_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4ui)
                Throw(undefined_behavior(
                    "unloaded function Uniform4ui"));
    }
    glUniform4ui(location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4uiv)
                Throw(undefined_behavior(
                    "unloaded function Uniform4uiv"));
    }
    glUniform4uiv(location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void vertex_attrib_i1i(u32 index, i32 x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI1i)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI1i"));
    }
    glVertexAttribI1i(index, x);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void vertex_attrib_i1iv(u32 index, span_const_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI1iv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI1iv"));
    }
    glVertexAttribI1iv(index, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void vertex_attrib_i1ui(u32 index, u32 x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI1ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI1ui"));
    }
    glVertexAttribI1ui(index, x);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void vertex_attrib_i1uiv(u32 index, span_const_u32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI1uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI1uiv"));
    }
    glVertexAttribI1uiv(index, reinterpret_cast<const GLuint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void vertex_attrib_i2i(u32 index, vec_2_i32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI2i)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI2i"));
    }
    glVertexAttribI2i(index, x.x(), x.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_2_i32> &&
    semantic::concepts::Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
STATICINLINE void vertex_attrib_i2iv(u32 index, span_const_vec_2_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI2iv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI2iv"));
    }
    glVertexAttribI2iv(index, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_2_u32, u32, 2>)
STATICINLINE void vertex_attrib_i2ui(u32 index, vec_2_u32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI2ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI2ui"));
    }
    glVertexAttribI2ui(index, x.x(), x.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void vertex_attrib_i2uiv(u32 index, span_const_vec_2_u32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI2uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI2uiv"));
    }
    glVertexAttribI2uiv(index, reinterpret_cast<const GLuint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void vertex_attrib_i3i(u32 index, vec_3_i32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI3i)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI3i"));
    }
    glVertexAttribI3i(index, x.x(), x.y(), x.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_3_i32> &&
    semantic::concepts::Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
STATICINLINE void vertex_attrib_i3iv(u32 index, span_const_vec_3_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI3iv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI3iv"));
    }
    glVertexAttribI3iv(index, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_u32, u32, 3>)
STATICINLINE void vertex_attrib_i3ui(u32 index, vec_3_u32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI3ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI3ui"));
    }
    glVertexAttribI3ui(index, x.x(), x.y(), x.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void vertex_attrib_i3uiv(u32 index, span_const_vec_3_u32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI3uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI3uiv"));
    }
    glVertexAttribI3uiv(index, reinterpret_cast<const GLuint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_i8
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_i8> &&
    std::is_same_v<std::decay_t<typename span_const_i8::value_type>, i8>)
STATICINLINE void vertex_attrib_i4bv(u32 index, span_const_i8 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4bv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4bv"));
    }
    glVertexAttribI4bv(index, reinterpret_cast<const GLbyte *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_4_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_4_i32, i32, 4>)
STATICINLINE void vertex_attrib_i4i(u32 index, vec_4_i32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4i)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4i"));
    }
    glVertexAttribI4i(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_4_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_4_i32> &&
    semantic::concepts::Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
STATICINLINE void vertex_attrib_i4iv(u32 index, span_const_vec_4_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4iv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4iv"));
    }
    glVertexAttribI4iv(index, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_4_i16
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_4_i16> &&
    semantic::concepts::Vector<typename span_const_vec_4_i16::value_type, i16, 4>)
STATICINLINE void vertex_attrib_i4sv(u32 index, span_const_vec_4_i16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4sv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4sv"));
    }
    glVertexAttribI4sv(index, reinterpret_cast<const GLshort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_u8
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u8> &&
    std::is_same_v<std::decay_t<typename span_const_u8::value_type>, u8>)
STATICINLINE void vertex_attrib_i4ubv(u32 index, span_const_u8 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4ubv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4ubv"));
    }
    glVertexAttribI4ubv(index, reinterpret_cast<const GLubyte *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_4_u32, u32, 4>)
STATICINLINE void vertex_attrib_i4ui(u32 index, vec_4_u32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4ui"));
    }
    glVertexAttribI4ui(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void vertex_attrib_i4uiv(u32 index, span_const_vec_4_u32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4uiv"));
    }
    glVertexAttribI4uiv(index, reinterpret_cast<const GLuint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_vec_4_u16
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_4_u16> &&
    semantic::concepts::Vector<typename span_const_vec_4_u16::value_type, u16, 4>)
STATICINLINE void vertex_attrib_i4usv(u32 index, span_const_vec_4_u16 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4usv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4usv"));
    }
    glVertexAttribI4usv(index, reinterpret_cast<const GLushort *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void vertex_attrib_i_pointer(u32 index, i32 size, groups::vertex_attrib_int type, i32 stride, span_const_byte const& pointer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribIPointer)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribIPointer"));
    }
    glVertexAttribIPointer(index, size, static_cast<GLenum>(type), stride, reinterpret_cast<const void *>(pointer.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_framebuffer(groups::framebuffer_target target, u32 framebuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function BindFramebuffer"));
        glIsFramebuffer(framebuffer);
    }
    glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_renderbuffer(groups::renderbuffer_target target, u32 renderbuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindRenderbuffer)
                Throw(undefined_behavior(
                    "unloaded function BindRenderbuffer"));
        glIsRenderbuffer(renderbuffer);
    }
    glBindRenderbuffer(static_cast<GLenum>(target), renderbuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void blit_framebuffer(i32 srcX0, i32 srcY0, i32 srcX1, i32 srcY1, i32 dstX0, i32 dstY0, i32 dstX1, i32 dstY1, groups::clear_buffer_mask mask, groups::blit_framebuffer_filter filter)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlitFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function BlitFramebuffer"));
    }
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, static_cast<GLenum>(mask), static_cast<GLenum>(filter));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLenum check_framebuffer_status(groups::framebuffer_target target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCheckFramebufferStatus)
                Throw(undefined_behavior(
                    "unloaded function CheckFramebufferStatus"));
    }
    auto out = glCheckFramebufferStatus(static_cast<GLenum>(target));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_framebuffers(span_const_u32 const& framebuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteFramebuffers)
                Throw(undefined_behavior(
                    "unloaded function DeleteFramebuffers"));
    }
    glDeleteFramebuffers(framebuffers.size(), reinterpret_cast<const GLuint *>(framebuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_renderbuffers(span_const_u32 const& renderbuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteRenderbuffers)
                Throw(undefined_behavior(
                    "unloaded function DeleteRenderbuffers"));
    }
    glDeleteRenderbuffers(renderbuffers.size(), reinterpret_cast<const GLuint *>(renderbuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void framebuffer_renderbuffer(groups::framebuffer_target target, groups::framebuffer_attachment attachment, groups::renderbuffer_target renderbuffertarget, u32 renderbuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferRenderbuffer)
                Throw(undefined_behavior(
                    "unloaded function FramebufferRenderbuffer"));
        glIsRenderbuffer(renderbuffer);
    }
    glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(attachment), static_cast<GLenum>(renderbuffertarget), renderbuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void framebuffer_texture_1d(groups::framebuffer_target target, groups::framebuffer_attachment attachment, groups::texture_target textarget, u32 texture, i32 level)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferTexture1D)
                Throw(undefined_behavior(
                    "unloaded function FramebufferTexture1D"));
        glIsTexture(texture);
    }
    glFramebufferTexture1D(static_cast<GLenum>(target), static_cast<GLenum>(attachment), static_cast<GLenum>(textarget), texture, level);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void framebuffer_texture_2d(groups::framebuffer_target target, groups::framebuffer_attachment attachment, groups::texture_target textarget, u32 texture, i32 level)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferTexture2D)
                Throw(undefined_behavior(
                    "unloaded function FramebufferTexture2D"));
        glIsTexture(texture);
    }
    glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(attachment), static_cast<GLenum>(textarget), texture, level);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void framebuffer_texture_3d(groups::framebuffer_target target, groups::framebuffer_attachment attachment, groups::texture_target textarget, u32 texture, i32 level, i32 zoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferTexture3D)
                Throw(undefined_behavior(
                    "unloaded function FramebufferTexture3D"));
        glIsTexture(texture);
    }
    glFramebufferTexture3D(static_cast<GLenum>(target), static_cast<GLenum>(attachment), static_cast<GLenum>(textarget), texture, level, zoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void framebuffer_texture_layer(groups::framebuffer_target target, groups::framebuffer_attachment attachment, u32 texture, i32 level, i32 layer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferTextureLayer)
                Throw(undefined_behavior(
                    "unloaded function FramebufferTextureLayer"));
        glIsTexture(texture);
    }
    glFramebufferTextureLayer(static_cast<GLenum>(target), static_cast<GLenum>(attachment), texture, level, layer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_framebuffers(span_u32 framebuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenFramebuffers)
                Throw(undefined_behavior(
                    "unloaded function GenFramebuffers"));
    }
    glGenFramebuffers(framebuffers.size(), reinterpret_cast<GLuint *>(framebuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_renderbuffers(span_u32 renderbuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenRenderbuffers)
                Throw(undefined_behavior(
                    "unloaded function GenRenderbuffers"));
    }
    glGenRenderbuffers(renderbuffers.size(), reinterpret_cast<GLuint *>(renderbuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void generate_mipmap(GLenum target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenerateMipmap)
                Throw(undefined_behavior(
                    "unloaded function GenerateMipmap"));
    }
    glGenerateMipmap(target);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_framebuffer_attachment_parameter(groups::framebuffer_target target, groups::framebuffer_attachment attachment, groups::framebuffer_attachment_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFramebufferAttachmentParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetFramebufferAttachmentParameteriv"));
    }
    glGetFramebufferAttachmentParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(attachment), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_renderbuffer_parameter(groups::renderbuffer_target target, groups::renderbuffer_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetRenderbufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetRenderbufferParameteriv"));
    }
    glGetRenderbufferParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_framebuffer(u32 framebuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function IsFramebuffer"));
        glIsFramebuffer(framebuffer);
    }
    auto out = glIsFramebuffer(framebuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_renderbuffer(u32 renderbuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsRenderbuffer)
                Throw(undefined_behavior(
                    "unloaded function IsRenderbuffer"));
        glIsRenderbuffer(renderbuffer);
    }
    auto out = glIsRenderbuffer(renderbuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void renderbuffer_storage(groups::renderbuffer_target target, groups::internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glRenderbufferStorage)
                Throw(undefined_behavior(
                    "unloaded function RenderbufferStorage"));
    }
    glRenderbufferStorage(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void renderbuffer_storage_multisample(groups::renderbuffer_target target, i32 samples, groups::internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glRenderbufferStorageMultisample)
                Throw(undefined_behavior(
                    "unloaded function RenderbufferStorageMultisample"));
    }
    glRenderbufferStorageMultisample(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void flush_mapped_buffer_range(groups::buffer_target_arb target, GLintptr offset, GLsizeiptr length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFlushMappedBufferRange)
                Throw(undefined_behavior(
                    "unloaded function FlushMappedBufferRange"));
    }
    glFlushMappedBufferRange(static_cast<GLenum>(target), offset, length);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void * map_buffer_range(groups::buffer_target_arb target, GLintptr offset, GLsizeiptr length, groups::map_buffer_access_mask access)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMapBufferRange)
                Throw(undefined_behavior(
                    "unloaded function MapBufferRange"));
    }
    auto out = glMapBufferRange(static_cast<GLenum>(target), offset, length, static_cast<GLenum>(access));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_vertex_array(u32 array)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindVertexArray)
                Throw(undefined_behavior(
                    "unloaded function BindVertexArray"));
        glIsVertexArray(array);
    }
    glBindVertexArray(array);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_vertex_arrays(span_const_u32 const& arrays)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteVertexArrays)
                Throw(undefined_behavior(
                    "unloaded function DeleteVertexArrays"));
    }
    glDeleteVertexArrays(arrays.size(), reinterpret_cast<const GLuint *>(arrays.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_vertex_arrays(span_u32 arrays)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenVertexArrays)
                Throw(undefined_behavior(
                    "unloaded function GenVertexArrays"));
    }
    glGenVertexArrays(arrays.size(), reinterpret_cast<GLuint *>(arrays.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_vertex_array(u32 array)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsVertexArray)
                Throw(undefined_behavior(
                    "unloaded function IsVertexArray"));
        glIsVertexArray(array);
    }
    auto out = glIsVertexArray(array);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

#endif // GL_VERSION_3_0
#ifdef GL_VERSION_3_1
/* Introduced in GL core 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void draw_arrays_instanced(groups::primitive_type mode, i32 first, i32 count, i32 instancecount)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawArraysInstanced)
                Throw(undefined_behavior(
                    "unloaded function DrawArraysInstanced"));
    }
    glDrawArraysInstanced(static_cast<GLenum>(mode), first, count, instancecount);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void draw_elements_instanced(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices, i32 instancecount)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsInstanced)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsInstanced"));
    }
    glDrawElementsInstanced(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), instancecount);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void primitive_restart_index(u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPrimitiveRestartIndex)
                Throw(undefined_behavior(
                    "unloaded function PrimitiveRestartIndex"));
    }
    glPrimitiveRestartIndex(index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void tex_buffer(groups::texture_target target, groups::sized_internal_format internalformat, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexBuffer)
                Throw(undefined_behavior(
                    "unloaded function TexBuffer"));
        glIsBuffer(buffer);
    }
    glTexBuffer(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void copy_buffer_sub_data(groups::copy_buffer_sub_data_target readTarget, groups::copy_buffer_sub_data_target writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function CopyBufferSubData"));
    }
    glCopyBufferSubData(static_cast<GLenum>(readTarget), static_cast<GLenum>(writeTarget), readOffset, writeOffset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_uniform_block_name(u32 program, u32 uniformBlockIndex, i32& length, span_GLchar uniformBlockName)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniformBlockName)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniformBlockName"));
        glIsProgram(program);
    }
    glGetActiveUniformBlockName(program, uniformBlockIndex, uniformBlockName.size(), &length, uniformBlockName.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_active_uniform_blockiv(u32 program, u32 uniformBlockIndex, groups::uniform_block_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniformBlockiv)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniformBlockiv"));
        glIsProgram(program);
    }
    glGetActiveUniformBlockiv(program, uniformBlockIndex, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_uniform_name(u32 program, u32 uniformIndex, i32& length, span_GLchar uniformName)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniformName)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniformName"));
        glIsProgram(program);
    }
    glGetActiveUniformName(program, uniformIndex, uniformName.size(), &length, uniformName.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    class span_const_u32,
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_active_uniformsiv(u32 program, span_const_u32 const& uniformIndices, groups::uniform_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniformsiv)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniformsiv"));
        glIsProgram(program);
    }
    glGetActiveUniformsiv(program, uniformIndices.size(), reinterpret_cast<const GLuint *>(uniformIndices.data()), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE GLuint get_uniform_block_index(u32 program, std::string_view const& uniformBlockName)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformBlockIndex)
                Throw(undefined_behavior(
                    "unloaded function GetUniformBlockIndex"));
        glIsProgram(program);
    }
    auto out = glGetUniformBlockIndex(program, uniformBlockName.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.1 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_uniform_indices(u32 program, i32 uniformCount, std::vector<std::string_view> uniformNames, span_u32 uniformIndices)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformIndices)
                Throw(undefined_behavior(
                    "unloaded function GetUniformIndices"));
        glIsProgram(program);
    }
    auto [uniformNames_lens, uniformNames_cstr, uniformNames_store] = detail::transform_strings(uniformNames);
    glGetUniformIndices(program, uniformCount, uniformNames_cstr.data(), reinterpret_cast<GLuint *>(uniformIndices.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void uniform_block_binding(u32 program, u32 uniformBlockIndex, u32 uniformBlockBinding)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformBlockBinding)
                Throw(undefined_behavior(
                    "unloaded function UniformBlockBinding"));
        glIsProgram(program);
    }
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_3_1
#ifdef GL_VERSION_3_2
/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void draw_elements_base_vertex(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices, i32 basevertex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsBaseVertex)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsBaseVertex"));
    }
    glDrawElementsBaseVertex(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), basevertex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void draw_elements_instanced_base_vertex(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices, i32 instancecount, i32 basevertex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsInstancedBaseVertex)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsInstancedBaseVertex"));
    }
    glDrawElementsInstancedBaseVertex(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), instancecount, basevertex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void draw_range_elements_base_vertex(groups::primitive_type mode, u32 start, u32 end, i32 count, groups::draw_elements_type type, span_const_byte const& indices, i32 basevertex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawRangeElementsBaseVertex)
                Throw(undefined_behavior(
                    "unloaded function DrawRangeElementsBaseVertex"));
    }
    glDrawRangeElementsBaseVertex(static_cast<GLenum>(mode), start, end, count, static_cast<GLenum>(type), reinterpret_cast<const void *>(indices.data()), basevertex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void multi_draw_elements_base_vertex(groups::primitive_type mode, span_const_i32 count, groups::draw_elements_type type, ptroff indices, span_const_i32 basevertex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiDrawElementsBaseVertex)
                Throw(undefined_behavior(
                    "unloaded function MultiDrawElementsBaseVertex"));
    }
    GLsizei drawcount = count.size();
    detail::assert_equal(count.size(), drawcount);
    detail::assert_equal(basevertex.size(), drawcount);
    glMultiDrawElementsBaseVertex(static_cast<GLenum>(mode), count.data(), static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), basevertex.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void provoking_vertex(groups::vertex_provoking_mode mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProvokingVertex)
                Throw(undefined_behavior(
                    "unloaded function ProvokingVertex"));
    }
    glProvokingVertex(static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE GLenum client_wait_sync(GLsync sync, groups::sync_object_mask flags, u64 timeout)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClientWaitSync)
                Throw(undefined_behavior(
                    "unloaded function ClientWaitSync"));
    }
    auto out = glClientWaitSync(sync, static_cast<GLenum>(flags), timeout);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void delete_sync(GLsync sync)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteSync)
                Throw(undefined_behavior(
                    "unloaded function DeleteSync"));
    }
    glDeleteSync(sync);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE GLsync fence_sync(groups::sync_condition condition, groups::sync_behavior_flags flags)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFenceSync)
                Throw(undefined_behavior(
                    "unloaded function FenceSync"));
    }
    auto out = glFenceSync(static_cast<GLenum>(condition), static_cast<GLenum>(flags));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.2 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_integer64v(groups::get_prop pname, span_i64 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetInteger64v)
                Throw(undefined_behavior(
                    "unloaded function GetInteger64v"));
    }
    glGetInteger64v(static_cast<GLenum>(pname), reinterpret_cast<GLint64 *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_synciv(GLsync sync, groups::sync_parameter_name pname, i32& length, span_i32 values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSynciv)
                Throw(undefined_behavior(
                    "unloaded function GetSynciv"));
    }
    glGetSynciv(sync, static_cast<GLenum>(pname), values.size(), &length, reinterpret_cast<GLint *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE GLboolean is_sync(GLsync sync)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsSync)
                Throw(undefined_behavior(
                    "unloaded function IsSync"));
    }
    auto out = glIsSync(sync);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void wait_sync(GLsync sync, groups::sync_behavior_flags flags, u64 timeout)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glWaitSync)
                Throw(undefined_behavior(
                    "unloaded function WaitSync"));
    }
    glWaitSync(sync, static_cast<GLenum>(flags), timeout);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void framebuffer_texture(groups::framebuffer_target target, groups::framebuffer_attachment attachment, u32 texture, i32 level)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferTexture)
                Throw(undefined_behavior(
                    "unloaded function FramebufferTexture"));
        glIsTexture(texture);
    }
    glFramebufferTexture(static_cast<GLenum>(target), static_cast<GLenum>(attachment), texture, level);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_buffer_parameteri64v(groups::buffer_target_arb target, groups::buffer_prop_arb pname, span_i64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBufferParameteri64v)
                Throw(undefined_behavior(
                    "unloaded function GetBufferParameteri64v"));
    }
    glGetBufferParameteri64v(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint64 *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_integer64i_v(groups::get_prop target, u32 index, span_i64 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetInteger64i_v)
                Throw(undefined_behavior(
                    "unloaded function GetInteger64i_v"));
    }
    glGetInteger64i_v(static_cast<GLenum>(target), index, reinterpret_cast<GLint64 *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_multisamplefv(groups::get_multisample_prop_nv pname, u32 index, span_f32 val)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetMultisamplefv)
                Throw(undefined_behavior(
                    "unloaded function GetMultisamplefv"));
    }
    glGetMultisamplefv(static_cast<GLenum>(pname), index, reinterpret_cast<GLfloat *>(val.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void sample_maski(u32 maskNumber, GLbitfield mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSampleMaski)
                Throw(undefined_behavior(
                    "unloaded function SampleMaski"));
    }
    glSampleMaski(maskNumber, mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void tex_image_2d_multisample(groups::texture_target target, i32 samples, groups::internal_format internalformat, vec_2_i32 const& width, bool fixedsamplelocations)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexImage2DMultisample)
                Throw(undefined_behavior(
                    "unloaded function TexImage2DMultisample"));
    }
    glTexImage2DMultisample(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width[0], width[1], fixedsamplelocations);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.2 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void tex_image_3d_multisample(groups::texture_target target, i32 samples, groups::internal_format internalformat, vec_3_i32 const& width, bool fixedsamplelocations)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexImage3DMultisample)
                Throw(undefined_behavior(
                    "unloaded function TexImage3DMultisample"));
    }
    glTexImage3DMultisample(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width[0], width[1], width[2], fixedsamplelocations);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_3_2
#ifdef GL_VERSION_3_3
/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void bind_frag_data_location_indexed(u32 program, u32 colorNumber, u32 index, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindFragDataLocationIndexed)
                Throw(undefined_behavior(
                    "unloaded function BindFragDataLocationIndexed"));
        glIsProgram(program);
    }
    glBindFragDataLocationIndexed(program, colorNumber, index, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE GLint get_frag_data_index(u32 program, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFragDataIndex)
                Throw(undefined_behavior(
                    "unloaded function GetFragDataIndex"));
        glIsProgram(program);
    }
    auto out = glGetFragDataIndex(program, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void bind_sampler(u32 unit, u32 sampler)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindSampler)
                Throw(undefined_behavior(
                    "unloaded function BindSampler"));
        glIsSampler(sampler);
    }
    glBindSampler(unit, sampler);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_samplers(span_const_u32 const& samplers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteSamplers)
                Throw(undefined_behavior(
                    "unloaded function DeleteSamplers"));
    }
    glDeleteSamplers(samplers.size(), reinterpret_cast<const GLuint *>(samplers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_samplers(span_u32 samplers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenSamplers)
                Throw(undefined_behavior(
                    "unloaded function GenSamplers"));
    }
    glGenSamplers(samplers.size(), reinterpret_cast<GLuint *>(samplers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_sampler_parameter_iiv(u32 sampler, groups::sampler_parameter_i pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSamplerParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function GetSamplerParameterIiv"));
        glIsSampler(sampler);
    }
    glGetSamplerParameterIiv(sampler, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_sampler_parameter_iuiv(u32 sampler, groups::sampler_parameter_i pname, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSamplerParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function GetSamplerParameterIuiv"));
        glIsSampler(sampler);
    }
    glGetSamplerParameterIuiv(sampler, static_cast<GLenum>(pname), reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_sampler_parameter(u32 sampler, groups::sampler_parameter_f pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSamplerParameterfv)
                Throw(undefined_behavior(
                    "unloaded function GetSamplerParameterfv"));
        glIsSampler(sampler);
    }
    glGetSamplerParameterfv(sampler, static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_sampler_parameter(u32 sampler, groups::sampler_parameter_i pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSamplerParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetSamplerParameteriv"));
        glIsSampler(sampler);
    }
    glGetSamplerParameteriv(sampler, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE GLboolean is_sampler(u32 sampler)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsSampler)
                Throw(undefined_behavior(
                    "unloaded function IsSampler"));
        glIsSampler(sampler);
    }
    auto out = glIsSampler(sampler);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 3.3 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void sampler_parameter_iiv(u32 sampler, groups::sampler_parameter_i pname, span_const_i32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameterIiv"));
        glIsSampler(sampler);
    }
    glSamplerParameterIiv(sampler, static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void sampler_parameter_iuiv(u32 sampler, groups::sampler_parameter_i pname, span_const_u32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameterIuiv"));
        glIsSampler(sampler);
    }
    glSamplerParameterIuiv(sampler, static_cast<GLenum>(pname), reinterpret_cast<const GLuint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void sampler_parameter(u32 sampler, groups::sampler_parameter_f pname, f32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameterf)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameterf"));
        glIsSampler(sampler);
    }
    glSamplerParameterf(sampler, static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void sampler_parameter(u32 sampler, groups::sampler_parameter_f pname, span_const_f32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameterfv)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameterfv"));
        glIsSampler(sampler);
    }
    glSamplerParameterfv(sampler, static_cast<GLenum>(pname), reinterpret_cast<const GLfloat *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void sampler_parameter(u32 sampler, groups::sampler_parameter_i pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameteri)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameteri"));
        glIsSampler(sampler);
    }
    glSamplerParameteri(sampler, static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void sampler_parameter(u32 sampler, groups::sampler_parameter_i pname, span_const_i32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameteriv)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameteriv"));
        glIsSampler(sampler);
    }
    glSamplerParameteriv(sampler, static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_query_objecti64v(u32 id, groups::query_object_parameter_name pname, span_i64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryObjecti64v)
                Throw(undefined_behavior(
                    "unloaded function GetQueryObjecti64v"));
    }
    glGetQueryObjecti64v(id, static_cast<GLenum>(pname), reinterpret_cast<GLint64 *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_u64
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_u64> &&
    std::is_same_v<std::decay_t<typename span_u64::value_type>, u64>)
STATICINLINE void get_query_objectui64v(u32 id, groups::query_object_parameter_name pname, span_u64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryObjectui64v)
                Throw(undefined_behavior(
                    "unloaded function GetQueryObjectui64v"));
    }
    glGetQueryObjectui64v(id, static_cast<GLenum>(pname), reinterpret_cast<GLuint64 *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void query_counter(u32 id, groups::query_counter_target target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glQueryCounter)
                Throw(undefined_behavior(
                    "unloaded function QueryCounter"));
    }
    glQueryCounter(id, static_cast<GLenum>(target));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void vertex_attrib_divisor(u32 index, u32 divisor)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribDivisor)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribDivisor"));
    }
    glVertexAttribDivisor(index, divisor);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void vertex_attrib_p1ui(u32 index, groups::vertex_attrib_pointer_type type, bool normalized, u32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribP1ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribP1ui"));
    }
    glVertexAttribP1ui(index, static_cast<GLenum>(type), normalized, value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void vertex_attrib_p1uiv(u32 index, groups::vertex_attrib_pointer_type type, bool normalized, span_const_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribP1uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribP1uiv"));
    }
    glVertexAttribP1uiv(index, static_cast<GLenum>(type), normalized, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void vertex_attrib_p2ui(u32 index, groups::vertex_attrib_pointer_type type, bool normalized, u32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribP2ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribP2ui"));
    }
    glVertexAttribP2ui(index, static_cast<GLenum>(type), normalized, value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void vertex_attrib_p2uiv(u32 index, groups::vertex_attrib_pointer_type type, bool normalized, span_const_vec_2_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribP2uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribP2uiv"));
    }
    glVertexAttribP2uiv(index, static_cast<GLenum>(type), normalized, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void vertex_attrib_p3ui(u32 index, groups::vertex_attrib_pointer_type type, bool normalized, u32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribP3ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribP3ui"));
    }
    glVertexAttribP3ui(index, static_cast<GLenum>(type), normalized, value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void vertex_attrib_p3uiv(u32 index, groups::vertex_attrib_pointer_type type, bool normalized, span_const_vec_3_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribP3uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribP3uiv"));
    }
    glVertexAttribP3uiv(index, static_cast<GLenum>(type), normalized, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void vertex_attrib_p4ui(u32 index, groups::vertex_attrib_pointer_type type, bool normalized, u32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribP4ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribP4ui"));
    }
    glVertexAttribP4ui(index, static_cast<GLenum>(type), normalized, value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void vertex_attrib_p4uiv(u32 index, groups::vertex_attrib_pointer_type type, bool normalized, span_const_vec_4_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribP4uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribP4uiv"));
    }
    glVertexAttribP4uiv(index, static_cast<GLenum>(type), normalized, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void color_p3ui(groups::color_pointer_type type, u32 color)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glColorP3ui)
                Throw(undefined_behavior(
                    "unloaded function ColorP3ui"));
    }
    glColorP3ui(static_cast<GLenum>(type), color);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void color_p3uiv(groups::color_pointer_type type, span_const_vec_3_u32 const& color)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glColorP3uiv)
                Throw(undefined_behavior(
                    "unloaded function ColorP3uiv"));
    }
    glColorP3uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(color.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void color_p4ui(groups::color_pointer_type type, u32 color)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glColorP4ui)
                Throw(undefined_behavior(
                    "unloaded function ColorP4ui"));
    }
    glColorP4ui(static_cast<GLenum>(type), color);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void color_p4uiv(groups::color_pointer_type type, span_const_vec_4_u32 const& color)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glColorP4uiv)
                Throw(undefined_behavior(
                    "unloaded function ColorP4uiv"));
    }
    glColorP4uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(color.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void multi_tex_coord_p1ui(groups::texture_unit texture, groups::tex_coord_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiTexCoordP1ui)
                Throw(undefined_behavior(
                    "unloaded function MultiTexCoordP1ui"));
    }
    glMultiTexCoordP1ui(static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void multi_tex_coord_p1uiv(groups::texture_unit texture, groups::tex_coord_pointer_type type, span_const_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiTexCoordP1uiv)
                Throw(undefined_behavior(
                    "unloaded function MultiTexCoordP1uiv"));
    }
    glMultiTexCoordP1uiv(static_cast<GLenum>(texture), static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void multi_tex_coord_p2ui(groups::texture_unit texture, groups::tex_coord_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiTexCoordP2ui)
                Throw(undefined_behavior(
                    "unloaded function MultiTexCoordP2ui"));
    }
    glMultiTexCoordP2ui(static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void multi_tex_coord_p2uiv(groups::texture_unit texture, groups::tex_coord_pointer_type type, span_const_vec_2_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiTexCoordP2uiv)
                Throw(undefined_behavior(
                    "unloaded function MultiTexCoordP2uiv"));
    }
    glMultiTexCoordP2uiv(static_cast<GLenum>(texture), static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void multi_tex_coord_p3ui(groups::texture_unit texture, groups::tex_coord_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiTexCoordP3ui)
                Throw(undefined_behavior(
                    "unloaded function MultiTexCoordP3ui"));
    }
    glMultiTexCoordP3ui(static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void multi_tex_coord_p3uiv(groups::texture_unit texture, groups::tex_coord_pointer_type type, span_const_vec_3_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiTexCoordP3uiv)
                Throw(undefined_behavior(
                    "unloaded function MultiTexCoordP3uiv"));
    }
    glMultiTexCoordP3uiv(static_cast<GLenum>(texture), static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void multi_tex_coord_p4ui(groups::texture_unit texture, groups::tex_coord_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiTexCoordP4ui)
                Throw(undefined_behavior(
                    "unloaded function MultiTexCoordP4ui"));
    }
    glMultiTexCoordP4ui(static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void multi_tex_coord_p4uiv(groups::texture_unit texture, groups::tex_coord_pointer_type type, span_const_vec_4_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiTexCoordP4uiv)
                Throw(undefined_behavior(
                    "unloaded function MultiTexCoordP4uiv"));
    }
    glMultiTexCoordP4uiv(static_cast<GLenum>(texture), static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void normal_p3ui(groups::normal_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNormalP3ui)
                Throw(undefined_behavior(
                    "unloaded function NormalP3ui"));
    }
    glNormalP3ui(static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void normal_p3uiv(groups::normal_pointer_type type, span_const_vec_3_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNormalP3uiv)
                Throw(undefined_behavior(
                    "unloaded function NormalP3uiv"));
    }
    glNormalP3uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void secondary_color_p3ui(groups::color_pointer_type type, u32 color)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSecondaryColorP3ui)
                Throw(undefined_behavior(
                    "unloaded function SecondaryColorP3ui"));
    }
    glSecondaryColorP3ui(static_cast<GLenum>(type), color);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void secondary_color_p3uiv(groups::color_pointer_type type, span_const_vec_3_u32 const& color)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSecondaryColorP3uiv)
                Throw(undefined_behavior(
                    "unloaded function SecondaryColorP3uiv"));
    }
    glSecondaryColorP3uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(color.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void tex_coord_p1ui(groups::tex_coord_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexCoordP1ui)
                Throw(undefined_behavior(
                    "unloaded function TexCoordP1ui"));
    }
    glTexCoordP1ui(static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void tex_coord_p1uiv(groups::tex_coord_pointer_type type, span_const_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexCoordP1uiv)
                Throw(undefined_behavior(
                    "unloaded function TexCoordP1uiv"));
    }
    glTexCoordP1uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void tex_coord_p2ui(groups::tex_coord_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexCoordP2ui)
                Throw(undefined_behavior(
                    "unloaded function TexCoordP2ui"));
    }
    glTexCoordP2ui(static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void tex_coord_p2uiv(groups::tex_coord_pointer_type type, span_const_vec_2_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexCoordP2uiv)
                Throw(undefined_behavior(
                    "unloaded function TexCoordP2uiv"));
    }
    glTexCoordP2uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void tex_coord_p3ui(groups::tex_coord_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexCoordP3ui)
                Throw(undefined_behavior(
                    "unloaded function TexCoordP3ui"));
    }
    glTexCoordP3ui(static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void tex_coord_p3uiv(groups::tex_coord_pointer_type type, span_const_vec_3_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexCoordP3uiv)
                Throw(undefined_behavior(
                    "unloaded function TexCoordP3uiv"));
    }
    glTexCoordP3uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void tex_coord_p4ui(groups::tex_coord_pointer_type type, u32 coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexCoordP4ui)
                Throw(undefined_behavior(
                    "unloaded function TexCoordP4ui"));
    }
    glTexCoordP4ui(static_cast<GLenum>(type), coords);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void tex_coord_p4uiv(groups::tex_coord_pointer_type type, span_const_vec_4_u32 const& coords)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexCoordP4uiv)
                Throw(undefined_behavior(
                    "unloaded function TexCoordP4uiv"));
    }
    glTexCoordP4uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(coords.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void vertex_p2ui(groups::vertex_pointer_type type, u32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexP2ui)
                Throw(undefined_behavior(
                    "unloaded function VertexP2ui"));
    }
    glVertexP2ui(static_cast<GLenum>(type), value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void vertex_p2uiv(groups::vertex_pointer_type type, span_const_vec_2_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexP2uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexP2uiv"));
    }
    glVertexP2uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void vertex_p3ui(groups::vertex_pointer_type type, u32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexP3ui)
                Throw(undefined_behavior(
                    "unloaded function VertexP3ui"));
    }
    glVertexP3ui(static_cast<GLenum>(type), value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void vertex_p3uiv(groups::vertex_pointer_type type, span_const_vec_3_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexP3uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexP3uiv"));
    }
    glVertexP3uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 3>>)
STATICINLINE void vertex_p4ui(groups::vertex_pointer_type type, u32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexP4ui)
                Throw(undefined_behavior(
                    "unloaded function VertexP4ui"));
    }
    glVertexP4ui(static_cast<GLenum>(type), value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 3.3 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void vertex_p4uiv(groups::vertex_pointer_type type, span_const_vec_4_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexP4uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexP4uiv"));
    }
    glVertexP4uiv(static_cast<GLenum>(type), reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_3_3
#ifdef GL_VERSION_4_0
/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void blend_equation_separatei(u32 buf, groups::blend_equation_mode_ext modeRGB, groups::blend_equation_mode_ext modeAlpha)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendEquationSeparatei)
                Throw(undefined_behavior(
                    "unloaded function BlendEquationSeparatei"));
    }
    glBlendEquationSeparatei(buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void blend_equationi(u32 buf, groups::blend_equation_mode_ext mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendEquationi)
                Throw(undefined_behavior(
                    "unloaded function BlendEquationi"));
    }
    glBlendEquationi(buf, static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void blend_func_separatei(u32 buf, groups::blending_factor srcRGB, groups::blending_factor dstRGB, groups::blending_factor srcAlpha, groups::blending_factor dstAlpha)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendFuncSeparatei)
                Throw(undefined_behavior(
                    "unloaded function BlendFuncSeparatei"));
    }
    glBlendFuncSeparatei(buf, static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB), static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void blend_funci(u32 buf, groups::blending_factor src, groups::blending_factor dst)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendFunci)
                Throw(undefined_behavior(
                    "unloaded function BlendFunci"));
    }
    glBlendFunci(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void min_sample_shading(f32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMinSampleShading)
                Throw(undefined_behavior(
                    "unloaded function MinSampleShading"));
    }
    glMinSampleShading(value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void draw_arrays_indirect(groups::primitive_type mode, ptroff indirect)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawArraysIndirect)
                Throw(undefined_behavior(
                    "unloaded function DrawArraysIndirect"));
    }
    glDrawArraysIndirect(static_cast<GLenum>(mode), reinterpret_cast<const void*>(indirect));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void draw_elements_indirect(groups::primitive_type mode, groups::draw_elements_type type, ptroff indirect)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsIndirect)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsIndirect"));
    }
    glDrawElementsIndirect(static_cast<GLenum>(mode), static_cast<GLenum>(type), reinterpret_cast<const void*>(indirect));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<std::decay_t<typename span_f64::value_type>, f64>)
STATICINLINE void get_uniformdv(u32 program, i32 location, span_f64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformdv)
                Throw(undefined_behavior(
                    "unloaded function GetUniformdv"));
        glIsProgram(program);
    }
    glGetUniformdv(program, location, reinterpret_cast<GLdouble *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void uniform(i32 location, f64 x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1d)
                Throw(undefined_behavior(
                    "unloaded function Uniform1d"));
    }
    glUniform1d(location, x);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_f64> &&
    std::is_same_v<std::decay_t<typename span_const_f64::value_type>, f64>)
STATICINLINE void uniform(i32 location, i32 count, span_const_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1dv)
                Throw(undefined_behavior(
                    "unloaded function Uniform1dv"));
    }
    glUniform1dv(location, count, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class vec_2_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Vector<vec_2_f64, f64, 2>)
STATICINLINE void uniform(i32 location, vec_2_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2d)
                Throw(undefined_behavior(
                    "unloaded function Uniform2d"));
    }
    glUniform2d(location, x.x(), x.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_vec_2_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_vec_2_f64> &&
    semantic::concepts::Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_2_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2dv)
                Throw(undefined_behavior(
                    "unloaded function Uniform2dv"));
    }
    glUniform2dv(location, count, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class vec_3_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Vector<vec_3_f64, f64, 3>)
STATICINLINE void uniform(i32 location, vec_3_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3d)
                Throw(undefined_behavior(
                    "unloaded function Uniform3d"));
    }
    glUniform3d(location, x.x(), x.y(), x.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_vec_3_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_vec_3_f64> &&
    semantic::concepts::Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_3_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3dv)
                Throw(undefined_behavior(
                    "unloaded function Uniform3dv"));
    }
    glUniform3dv(location, count, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class vec_4_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Vector<vec_4_f64, f64, 4>)
STATICINLINE void uniform(i32 location, vec_4_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4d)
                Throw(undefined_behavior(
                    "unloaded function Uniform4d"));
    }
    glUniform4d(location, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_vec_4_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_vec_4_f64> &&
    semantic::concepts::Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_4_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4dv)
                Throw(undefined_behavior(
                    "unloaded function Uniform4dv"));
    }
    glUniform4dv(location, count, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_2x2_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_2x2_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x2_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2dv"));
    }
    glUniformMatrix2dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_2x3_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_2x3_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x3_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2x3dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2x3dv"));
    }
    glUniformMatrix2x3dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_2x4_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_2x4_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x4_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2x4dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2x4dv"));
    }
    glUniformMatrix2x4dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_3x3_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_3x3_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x3_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3dv"));
    }
    glUniformMatrix3dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_3x2_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_3x2_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x2_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3x2dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3x2dv"));
    }
    glUniformMatrix3x2dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_3x4_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_3x4_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x4_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3x4dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3x4dv"));
    }
    glUniformMatrix3x4dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_4x4_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_4x4_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x4_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4dv"));
    }
    glUniformMatrix4dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_4x2_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_4x2_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x2_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4x2dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4x2dv"));
    }
    glUniformMatrix4x2dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_mat_4x3_f64
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_mat_4x3_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x3_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4x3dv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4x3dv"));
    }
    glUniformMatrix4x3dv(location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_subroutine_name(u32 program, groups::shader_type shadertype, u32 index, i32& length, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveSubroutineName)
                Throw(undefined_behavior(
                    "unloaded function GetActiveSubroutineName"));
        glIsProgram(program);
    }
    glGetActiveSubroutineName(program, static_cast<GLenum>(shadertype), index, name.size(), &length, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_subroutine_uniform_name(u32 program, groups::shader_type shadertype, u32 index, i32& length, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveSubroutineUniformName)
                Throw(undefined_behavior(
                    "unloaded function GetActiveSubroutineUniformName"));
        glIsProgram(program);
    }
    glGetActiveSubroutineUniformName(program, static_cast<GLenum>(shadertype), index, name.size(), &length, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_active_subroutine_uniformiv(u32 program, groups::shader_type shadertype, u32 index, groups::subroutine_parameter_name pname, span_i32 values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveSubroutineUniformiv)
                Throw(undefined_behavior(
                    "unloaded function GetActiveSubroutineUniformiv"));
        glIsProgram(program);
    }
    glGetActiveSubroutineUniformiv(program, static_cast<GLenum>(shadertype), index, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void get_program_stageiv(u32 program, groups::shader_type shadertype, groups::program_stage_prop pname, i32& values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramStageiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramStageiv"));
        glIsProgram(program);
    }
    glGetProgramStageiv(program, static_cast<GLenum>(shadertype), static_cast<GLenum>(pname), &values);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE GLuint get_subroutine_index(u32 program, groups::shader_type shadertype, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSubroutineIndex)
                Throw(undefined_behavior(
                    "unloaded function GetSubroutineIndex"));
        glIsProgram(program);
    }
    auto out = glGetSubroutineIndex(program, static_cast<GLenum>(shadertype), name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE GLint get_subroutine_uniform_location(u32 program, groups::shader_type shadertype, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSubroutineUniformLocation)
                Throw(undefined_behavior(
                    "unloaded function GetSubroutineUniformLocation"));
        glIsProgram(program);
    }
    auto out = glGetSubroutineUniformLocation(program, static_cast<GLenum>(shadertype), name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void get_uniform_subroutineuiv(groups::shader_type shadertype, i32 location, u32& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformSubroutineuiv)
                Throw(undefined_behavior(
                    "unloaded function GetUniformSubroutineuiv"));
    }
    glGetUniformSubroutineuiv(static_cast<GLenum>(shadertype), location, &params);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void uniform_subroutines(groups::shader_type shadertype, span_const_u32 const& indices)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformSubroutinesuiv)
                Throw(undefined_behavior(
                    "unloaded function UniformSubroutinesuiv"));
    }
    glUniformSubroutinesuiv(static_cast<GLenum>(shadertype), indices.size(), reinterpret_cast<const GLuint *>(indices.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void patch_parameter(groups::patch_parameter_name pname, span_const_f32 const& values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPatchParameterfv)
                Throw(undefined_behavior(
                    "unloaded function PatchParameterfv"));
    }
    glPatchParameterfv(static_cast<GLenum>(pname), reinterpret_cast<const GLfloat *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void patch_parameter(groups::patch_parameter_name pname, i32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPatchParameteri)
                Throw(undefined_behavior(
                    "unloaded function PatchParameteri"));
    }
    glPatchParameteri(static_cast<GLenum>(pname), value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void bind_transform_feedback(groups::bind_transform_feedback_target target, u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function BindTransformFeedback"));
    }
    glBindTransformFeedback(static_cast<GLenum>(target), id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_transform_feedbacks(span_const_u32 const& ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteTransformFeedbacks)
                Throw(undefined_behavior(
                    "unloaded function DeleteTransformFeedbacks"));
    }
    glDeleteTransformFeedbacks(ids.size(), reinterpret_cast<const GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void draw_transform_feedback(groups::primitive_type mode, u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function DrawTransformFeedback"));
    }
    glDrawTransformFeedback(static_cast<GLenum>(mode), id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_transform_feedbacks(span_u32 ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenTransformFeedbacks)
                Throw(undefined_behavior(
                    "unloaded function GenTransformFeedbacks"));
    }
    glGenTransformFeedbacks(ids.size(), reinterpret_cast<GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE GLboolean is_transform_feedback(u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function IsTransformFeedback"));
    }
    auto out = glIsTransformFeedback(id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void pause_transform_feedback()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPauseTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function PauseTransformFeedback"));
    }
    glPauseTransformFeedback();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void resume_transform_feedback()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glResumeTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function ResumeTransformFeedback"));
    }
    glResumeTransformFeedback();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void begin_query_indexed(groups::query_target target, u32 index, u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBeginQueryIndexed)
                Throw(undefined_behavior(
                    "unloaded function BeginQueryIndexed"));
    }
    glBeginQueryIndexed(static_cast<GLenum>(target), index, id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void draw_transform_feedback_stream(groups::primitive_type mode, u32 id, u32 stream)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawTransformFeedbackStream)
                Throw(undefined_behavior(
                    "unloaded function DrawTransformFeedbackStream"));
    }
    glDrawTransformFeedbackStream(static_cast<GLenum>(mode), id, stream);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 0>>)
STATICINLINE void end_query_indexed(groups::query_target target, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEndQueryIndexed)
                Throw(undefined_behavior(
                    "unloaded function EndQueryIndexed"));
    }
    glEndQueryIndexed(static_cast<GLenum>(target), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_query_indexediv(groups::query_target target, u32 index, groups::query_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryIndexediv)
                Throw(undefined_behavior(
                    "unloaded function GetQueryIndexediv"));
    }
    glGetQueryIndexediv(static_cast<GLenum>(target), index, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_4_0
#ifdef GL_VERSION_4_1
/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void clear_depthf(f32 d)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearDepthf)
                Throw(undefined_behavior(
                    "unloaded function ClearDepthf"));
    }
    glClearDepthf(d);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void depth_rangef(f32 n, f32 f)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthRangef)
                Throw(undefined_behavior(
                    "unloaded function DepthRangef"));
    }
    glDepthRangef(n, f);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_shader_precision_format(groups::shader_type shadertype, groups::precision_type precisiontype, span_i32 range, i32& precision)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetShaderPrecisionFormat)
                Throw(undefined_behavior(
                    "unloaded function GetShaderPrecisionFormat"));
    }
    glGetShaderPrecisionFormat(static_cast<GLenum>(shadertype), static_cast<GLenum>(precisiontype), reinterpret_cast<GLint *>(range.data()), &precision);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void release_shader_compiler()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glReleaseShaderCompiler)
                Throw(undefined_behavior(
                    "unloaded function ReleaseShaderCompiler"));
    }
    glReleaseShaderCompiler();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_byte,
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void shader_binary(span_const_u32 const& shaders, groups::shader_binary_format binaryFormat, span_const_byte const& binary, i32 length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glShaderBinary)
                Throw(undefined_behavior(
                    "unloaded function ShaderBinary"));
    }
    glShaderBinary(shaders.size(), reinterpret_cast<const GLuint *>(shaders.data()), static_cast<GLenum>(binaryFormat), reinterpret_cast<const void *>(binary.data()), length);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_program_binary(u32 program, i32& length, GLenum& binaryFormat, span_byte binary)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramBinary)
                Throw(undefined_behavior(
                    "unloaded function GetProgramBinary"));
        glIsProgram(program);
    }
    glGetProgramBinary(program, binary.size(), &length, &binaryFormat, reinterpret_cast<void *>(binary.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void program_binary(u32 program, GLenum binaryFormat, span_const_byte const& binary, i32 length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramBinary)
                Throw(undefined_behavior(
                    "unloaded function ProgramBinary"));
        glIsProgram(program);
    }
    glProgramBinary(program, binaryFormat, reinterpret_cast<const void *>(binary.data()), length);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void program_parameter(u32 program, groups::program_parameter_prop pname, i32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramParameteri)
                Throw(undefined_behavior(
                    "unloaded function ProgramParameteri"));
        glIsProgram(program);
    }
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void active_shader_program(u32 pipeline, u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glActiveShaderProgram)
                Throw(undefined_behavior(
                    "unloaded function ActiveShaderProgram"));
        glIsProgramPipeline(pipeline);
        glIsProgram(program);
    }
    glActiveShaderProgram(pipeline, program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void bind_program_pipeline(u32 pipeline)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindProgramPipeline)
                Throw(undefined_behavior(
                    "unloaded function BindProgramPipeline"));
        glIsProgramPipeline(pipeline);
    }
    glBindProgramPipeline(pipeline);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE GLuint create_shader_programv(GLenum type, std::vector<std::string_view> strings)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateShaderProgramv)
                Throw(undefined_behavior(
                    "unloaded function CreateShaderProgramv"));
    }
    auto [strings_lens, strings_cstr, strings_store] = detail::transform_strings(strings);
    auto out = glCreateShaderProgramv(type, strings_cstr.size(), strings_cstr.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.1 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_program_pipelines(span_const_u32 const& pipelines)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteProgramPipelines)
                Throw(undefined_behavior(
                    "unloaded function DeleteProgramPipelines"));
    }
    glDeleteProgramPipelines(pipelines.size(), reinterpret_cast<const GLuint *>(pipelines.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_program_pipelines(span_u32 pipelines)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenProgramPipelines)
                Throw(undefined_behavior(
                    "unloaded function GenProgramPipelines"));
    }
    glGenProgramPipelines(pipelines.size(), reinterpret_cast<GLuint *>(pipelines.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_program_pipeline_info_log(u32 pipeline, i32& length, span_GLchar infoLog)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramPipelineInfoLog)
                Throw(undefined_behavior(
                    "unloaded function GetProgramPipelineInfoLog"));
        glIsProgramPipeline(pipeline);
    }
    glGetProgramPipelineInfoLog(pipeline, infoLog.size(), &length, infoLog.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_program_pipelineiv(u32 pipeline, groups::pipeline_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramPipelineiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramPipelineiv"));
        glIsProgramPipeline(pipeline);
    }
    glGetProgramPipelineiv(pipeline, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE GLboolean is_program_pipeline(u32 pipeline)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsProgramPipeline)
                Throw(undefined_behavior(
                    "unloaded function IsProgramPipeline"));
        glIsProgramPipeline(pipeline);
    }
    auto out = glIsProgramPipeline(pipeline);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void program_uniform(u32 program, i32 location, f64 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1d)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1d"));
        glIsProgram(program);
    }
    glProgramUniform1d(program, location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_f64> &&
    std::is_same_v<std::decay_t<typename span_const_f64::value_type>, f64>)
STATICINLINE void program_uniform(u32 program, i32 location, span_const_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1dv"));
        glIsProgram(program);
    }
    glProgramUniform1dv(program, location, value.size(), reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void program_uniform(u32 program, i32 location, f32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1f)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1f"));
        glIsProgram(program);
    }
    glProgramUniform1f(program, location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void program_uniform(u32 program, i32 location, span_const_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1fv"));
        glIsProgram(program);
    }
    glProgramUniform1fv(program, location, value.size(), reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1i)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1i"));
        glIsProgram(program);
    }
    glProgramUniform1i(program, location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void program_uniform(u32 program, i32 location, span_const_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1iv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1iv"));
        glIsProgram(program);
    }
    glProgramUniform1iv(program, location, value.size(), reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void program_uniform(u32 program, i32 location, u32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1ui)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1ui"));
        glIsProgram(program);
    }
    glProgramUniform1ui(program, location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void program_uniform(u32 program, i32 location, span_const_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1uiv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1uiv"));
        glIsProgram(program);
    }
    glProgramUniform1uiv(program, location, value.size(), reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_2_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_2_f64, f64, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_f64 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2d)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2d"));
        glIsProgram(program);
    }
    glProgramUniform2d(program, location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_2_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_2_f64> &&
    semantic::concepts::Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_2_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2dv"));
        glIsProgram(program);
    }
    glProgramUniform2dv(program, location, count, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_2_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_2_f32, f32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2f)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2f"));
        glIsProgram(program);
    }
    glProgramUniform2f(program, location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_2_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_2_f32> &&
    semantic::concepts::Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2fv"));
        glIsProgram(program);
    }
    glProgramUniform2fv(program, location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2i)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2i"));
        glIsProgram(program);
    }
    glProgramUniform2i(program, location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_2_i32> &&
    semantic::concepts::Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2iv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2iv"));
        glIsProgram(program);
    }
    glProgramUniform2iv(program, location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_2_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_2_u32, u32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2ui)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2ui"));
        glIsProgram(program);
    }
    glProgramUniform2ui(program, location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2uiv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2uiv"));
        glIsProgram(program);
    }
    glProgramUniform2uiv(program, location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_3_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_3_f64, f64, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_f64 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3d)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3d"));
        glIsProgram(program);
    }
    glProgramUniform3d(program, location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_3_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_3_f64> &&
    semantic::concepts::Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_3_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3dv"));
        glIsProgram(program);
    }
    glProgramUniform3dv(program, location, count, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_3_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_3_f32, f32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3f)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3f"));
        glIsProgram(program);
    }
    glProgramUniform3f(program, location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_3_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_3_f32> &&
    semantic::concepts::Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3fv"));
        glIsProgram(program);
    }
    glProgramUniform3fv(program, location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3i)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3i"));
        glIsProgram(program);
    }
    glProgramUniform3i(program, location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_3_i32> &&
    semantic::concepts::Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3iv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3iv"));
        glIsProgram(program);
    }
    glProgramUniform3iv(program, location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_3_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_3_u32, u32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3ui)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3ui"));
        glIsProgram(program);
    }
    glProgramUniform3ui(program, location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3uiv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3uiv"));
        glIsProgram(program);
    }
    glProgramUniform3uiv(program, location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_4_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_4_f64, f64, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_f64 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4d)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4d"));
        glIsProgram(program);
    }
    glProgramUniform4d(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_4_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_4_f64> &&
    semantic::concepts::Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_4_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4dv"));
        glIsProgram(program);
    }
    glProgramUniform4dv(program, location, count, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4f)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4f"));
        glIsProgram(program);
    }
    glProgramUniform4f(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_4_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_4_f32> &&
    semantic::concepts::Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4fv"));
        glIsProgram(program);
    }
    glProgramUniform4fv(program, location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_4_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_4_i32, i32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4i)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4i"));
        glIsProgram(program);
    }
    glProgramUniform4i(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_4_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_4_i32> &&
    semantic::concepts::Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4iv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4iv"));
        glIsProgram(program);
    }
    glProgramUniform4iv(program, location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_4_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_4_u32, u32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4ui)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4ui"));
        glIsProgram(program);
    }
    glProgramUniform4ui(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4uiv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4uiv"));
        glIsProgram(program);
    }
    glProgramUniform4uiv(program, location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_2x2_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_2x2_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x2_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_2x2_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_2x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_2x3_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_2x3_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x3_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2x3dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2x3dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2x3dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_2x3_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_2x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2x3fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2x3fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2x3fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_2x4_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_2x4_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x4_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2x4dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2x4dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2x4dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_2x4_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_2x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2x4fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2x4fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2x4fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_3x3_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_3x3_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x3_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_3x3_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_3x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_3x2_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_3x2_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x2_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3x2dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3x2dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3x2dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_3x2_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_3x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3x2fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3x2fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3x2fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_3x4_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_3x4_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x4_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3x4dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3x4dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3x4dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_3x4_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_3x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3x4fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3x4fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3x4fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_4x4_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_4x4_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x4_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_4x4_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_4x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_4x2_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_4x2_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x2_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4x2dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4x2dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4x2dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_4x2_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_4x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4x2fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4x2fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4x2fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_4x3_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_4x3_f64> &&
    semantic::concepts::Matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x3_f64 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4x3dv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4x3dv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4x3dv(program, location, count, transpose, reinterpret_cast<const GLdouble *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_mat_4x3_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_mat_4x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4x3fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4x3fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4x3fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void use_program_stages(u32 pipeline, groups::use_program_stage_mask stages, u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUseProgramStages)
                Throw(undefined_behavior(
                    "unloaded function UseProgramStages"));
        glIsProgramPipeline(pipeline);
        glIsProgram(program);
    }
    glUseProgramStages(pipeline, static_cast<GLenum>(stages), program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void validate_program_pipeline(u32 pipeline)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glValidateProgramPipeline)
                Throw(undefined_behavior(
                    "unloaded function ValidateProgramPipeline"));
        glIsProgramPipeline(pipeline);
    }
    glValidateProgramPipeline(pipeline);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<std::decay_t<typename span_f64::value_type>, f64>)
STATICINLINE void get_vertex_attrib_ldv(u32 index, groups::vertex_attrib_enum pname, span_f64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribLdv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribLdv"));
    }
    glGetVertexAttribLdv(index, static_cast<GLenum>(pname), reinterpret_cast<GLdouble *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void vertex_attrib_l1d(u32 index, f64 x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribL1d)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribL1d"));
    }
    glVertexAttribL1d(index, x);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_f64> &&
    std::is_same_v<std::decay_t<typename span_const_f64::value_type>, f64>)
STATICINLINE void vertex_attrib_l1dv(u32 index, span_const_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribL1dv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribL1dv"));
    }
    glVertexAttribL1dv(index, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_2_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_2_f64, f64, 2>)
STATICINLINE void vertex_attrib_l2d(u32 index, vec_2_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribL2d)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribL2d"));
    }
    glVertexAttribL2d(index, x.x(), x.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_2_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_2_f64> &&
    semantic::concepts::Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
STATICINLINE void vertex_attrib_l2dv(u32 index, span_const_vec_2_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribL2dv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribL2dv"));
    }
    glVertexAttribL2dv(index, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_3_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_3_f64, f64, 3>)
STATICINLINE void vertex_attrib_l3d(u32 index, vec_3_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribL3d)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribL3d"));
    }
    glVertexAttribL3d(index, x.x(), x.y(), x.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_3_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_3_f64> &&
    semantic::concepts::Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
STATICINLINE void vertex_attrib_l3dv(u32 index, span_const_vec_3_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribL3dv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribL3dv"));
    }
    glVertexAttribL3dv(index, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_4_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_4_f64, f64, 4>)
STATICINLINE void vertex_attrib_l4d(u32 index, vec_4_f64 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribL4d)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribL4d"));
    }
    glVertexAttribL4d(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_vec_4_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_vec_4_f64> &&
    semantic::concepts::Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
STATICINLINE void vertex_attrib_l4dv(u32 index, span_const_vec_4_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribL4dv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribL4dv"));
    }
    glVertexAttribL4dv(index, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void vertex_attrib_l_pointer(u32 index, groups::vertex_attrib_long type, i32 stride, span_const_byte const& pointer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribLPointer)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribLPointer"));
    }
    glVertexAttribLPointer(index, pointer.size(), static_cast<GLenum>(type), stride, reinterpret_cast<const void *>(pointer.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_f64> &&
    std::is_same_v<std::decay_t<typename span_const_f64::value_type>, f64>)
STATICINLINE void depth_range_arrayv(u32 first, i32 count, span_const_f64 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthRangeArrayv)
                Throw(undefined_behavior(
                    "unloaded function DepthRangeArrayv"));
    }
    glDepthRangeArrayv(first, count, reinterpret_cast<const GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 1>>)
STATICINLINE void depth_range_indexed(u32 index, f64 n, f64 f)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthRangeIndexed)
                Throw(undefined_behavior(
                    "unloaded function DepthRangeIndexed"));
    }
    glDepthRangeIndexed(index, n, f);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_f64
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<std::decay_t<typename span_f64::value_type>, f64>)
STATICINLINE void get_doublei_v(groups::get_prop target, u32 index, span_f64 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetDoublei_v)
                Throw(undefined_behavior(
                    "unloaded function GetDoublei_v"));
    }
    glGetDoublei_v(static_cast<GLenum>(target), index, reinterpret_cast<GLdouble *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_floati_v(groups::get_prop target, u32 index, span_f32 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFloati_v)
                Throw(undefined_behavior(
                    "unloaded function GetFloati_v"));
    }
    glGetFloati_v(static_cast<GLenum>(target), index, reinterpret_cast<GLfloat *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void scissor_arrayv(u32 first, i32 count, span_const_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glScissorArrayv)
                Throw(undefined_behavior(
                    "unloaded function ScissorArrayv"));
    }
    glScissorArrayv(first, count, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void scissor_indexed(u32 index, i32 left, i32 bottom, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glScissorIndexed)
                Throw(undefined_behavior(
                    "unloaded function ScissorIndexed"));
    }
    glScissorIndexed(index, left, bottom, width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void scissor_indexedv(u32 index, span_const_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glScissorIndexedv)
                Throw(undefined_behavior(
                    "unloaded function ScissorIndexedv"));
    }
    glScissorIndexedv(index, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void viewport_arrayv(u32 first, i32 count, span_const_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glViewportArrayv)
                Throw(undefined_behavior(
                    "unloaded function ViewportArrayv"));
    }
    glViewportArrayv(first, count, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class vec_2_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Vector<vec_2_f32, f32, 2>)
STATICINLINE void viewport_indexedf(u32 index, vec_2_f32 const& x, vec_2_f32 const& w)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glViewportIndexedf)
                Throw(undefined_behavior(
                    "unloaded function ViewportIndexedf"));
    }
    glViewportIndexedf(index, x.x(), x.y(), w[0], w[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.1 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<4, 1>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void viewport_indexedfv(u32 index, span_const_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glViewportIndexedfv)
                Throw(undefined_behavior(
                    "unloaded function ViewportIndexedfv"));
    }
    glViewportIndexedfv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_4_1
#ifdef GL_VERSION_4_2
/* Introduced in GL core 4.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 2>>)
STATICINLINE void draw_arrays_instanced_base_instance(groups::primitive_type mode, i32 first, i32 count, i32 instancecount, u32 baseinstance)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawArraysInstancedBaseInstance)
                Throw(undefined_behavior(
                    "unloaded function DrawArraysInstancedBaseInstance"));
    }
    glDrawArraysInstancedBaseInstance(static_cast<GLenum>(mode), first, count, instancecount, baseinstance);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 2>>)
STATICINLINE void draw_elements_instanced_base_instance(groups::primitive_type mode, i32 count, groups::primitive_type type, ptroff indices, i32 instancecount, u32 baseinstance)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsInstancedBaseInstance)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsInstancedBaseInstance"));
    }
    glDrawElementsInstancedBaseInstance(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), instancecount, baseinstance);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 2>>)
STATICINLINE void draw_elements_instanced_base_vertex_base_instance(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices, i32 instancecount, i32 basevertex, u32 baseinstance)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsInstancedBaseVertexBaseInstance)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsInstancedBaseVertexBaseInstance"));
    }
    glDrawElementsInstancedBaseVertexBaseInstance(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), instancecount, basevertex, baseinstance);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 2>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_internalformativ(groups::texture_target target, groups::internal_format internalformat, groups::internal_format_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetInternalformativ)
                Throw(undefined_behavior(
                    "unloaded function GetInternalformativ"));
    }
    glGetInternalformativ(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), static_cast<GLenum>(pname), params.size(), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 2>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_active_atomic_counter_bufferiv(u32 program, u32 bufferIndex, groups::atomic_counter_buffer_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveAtomicCounterBufferiv)
                Throw(undefined_behavior(
                    "unloaded function GetActiveAtomicCounterBufferiv"));
        glIsProgram(program);
    }
    glGetActiveAtomicCounterBufferiv(program, bufferIndex, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 2>>)
STATICINLINE void bind_image_texture(u32 unit, u32 texture, i32 level, bool layered, i32 layer, groups::buffer_access_arb access, groups::internal_format format)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindImageTexture)
                Throw(undefined_behavior(
                    "unloaded function BindImageTexture"));
        glIsTexture(texture);
    }
    glBindImageTexture(unit, texture, level, layered, layer, static_cast<GLenum>(access), static_cast<GLenum>(format));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 2>>)
STATICINLINE void memory_barrier(groups::memory_barrier_mask barriers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMemoryBarrier)
                Throw(undefined_behavior(
                    "unloaded function MemoryBarrier"));
    }
    glMemoryBarrier(static_cast<GLenum>(barriers));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 2>>)
STATICINLINE void tex_storage_1d(groups::texture_target target, i32 levels, groups::sized_internal_format internalformat, i32 width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage1D)
                Throw(undefined_behavior(
                    "unloaded function TexStorage1D"));
    }
    glTexStorage1D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internalformat), width);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 2>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void tex_storage_2d(groups::texture_target target, i32 levels, groups::sized_internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage2D)
                Throw(undefined_behavior(
                    "unloaded function TexStorage2D"));
    }
    glTexStorage2D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 2>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void tex_storage_3d(groups::texture_target target, i32 levels, groups::sized_internal_format internalformat, vec_3_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage3D)
                Throw(undefined_behavior(
                    "unloaded function TexStorage3D"));
    }
    glTexStorage3D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internalformat), width[0], width[1], width[2]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 2>>)
STATICINLINE void draw_transform_feedback_instanced(groups::primitive_type mode, u32 id, i32 instancecount)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawTransformFeedbackInstanced)
                Throw(undefined_behavior(
                    "unloaded function DrawTransformFeedbackInstanced"));
    }
    glDrawTransformFeedbackInstanced(static_cast<GLenum>(mode), id, instancecount);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 2>>)
STATICINLINE void draw_transform_feedback_stream_instanced(groups::primitive_type mode, u32 id, u32 stream, i32 instancecount)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawTransformFeedbackStreamInstanced)
                Throw(undefined_behavior(
                    "unloaded function DrawTransformFeedbackStreamInstanced"));
    }
    glDrawTransformFeedbackStreamInstanced(static_cast<GLenum>(mode), id, stream, instancecount);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_4_2
#ifdef GL_VERSION_4_3
/* Introduced in GL core 4.3 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void clear_buffer_data(groups::buffer_storage_target target, groups::sized_internal_format internalformat, groups::pixel_format format, groups::pixel_type type, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferData)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferData"));
    }
    glClearBufferData(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void clear_buffer_sub_data(groups::buffer_target_arb target, groups::sized_internal_format internalformat, GLintptr offset, GLsizeiptr size, groups::pixel_format format, groups::pixel_type type, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferSubData"));
    }
    glClearBufferSubData(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), offset, size, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void dispatch_compute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDispatchCompute)
                Throw(undefined_behavior(
                    "unloaded function DispatchCompute"));
    }
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void dispatch_compute_indirect(GLintptr indirect)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDispatchComputeIndirect)
                Throw(undefined_behavior(
                    "unloaded function DispatchComputeIndirect"));
    }
    glDispatchComputeIndirect(indirect);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void copy_image_sub_data(u32 srcName, groups::copy_image_sub_data_target srcTarget, i32 srcLevel, i32 srcX, i32 srcY, i32 srcZ, u32 dstName, groups::copy_image_sub_data_target dstTarget, i32 dstLevel, i32 dstX, i32 dstY, i32 dstZ, i32 srcWidth, i32 srcHeight, i32 srcDepth)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyImageSubData)
                Throw(undefined_behavior(
                    "unloaded function CopyImageSubData"));
    }
    glCopyImageSubData(srcName, static_cast<GLenum>(srcTarget), srcLevel, srcX, srcY, srcZ, dstName, static_cast<GLenum>(dstTarget), dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void framebuffer_parameter(groups::framebuffer_target target, groups::framebuffer_parameter_name pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferParameteri)
                Throw(undefined_behavior(
                    "unloaded function FramebufferParameteri"));
    }
    glFramebufferParameteri(static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_framebuffer_parameter(groups::framebuffer_target target, groups::framebuffer_attachment_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFramebufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetFramebufferParameteriv"));
    }
    glGetFramebufferParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_internalformati64v(groups::texture_target target, groups::internal_format internalformat, groups::internal_format_prop pname, span_i64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetInternalformati64v)
                Throw(undefined_behavior(
                    "unloaded function GetInternalformati64v"));
    }
    glGetInternalformati64v(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), static_cast<GLenum>(pname), params.size(), reinterpret_cast<GLint64 *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void invalidate_buffer_data(u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateBufferData)
                Throw(undefined_behavior(
                    "unloaded function InvalidateBufferData"));
        glIsBuffer(buffer);
    }
    glInvalidateBufferData(buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void invalidate_buffer_sub_data(u32 buffer, GLintptr offset, GLsizeiptr length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function InvalidateBufferSubData"));
        glIsBuffer(buffer);
    }
    glInvalidateBufferSubData(buffer, offset, length);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_const_GLenum
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum>)
STATICINLINE void invalidate_framebuffer(groups::framebuffer_target target, span_const_GLenum const& attachments)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function InvalidateFramebuffer"));
    }
    glInvalidateFramebuffer(static_cast<GLenum>(target), attachments.size(), reinterpret_cast<const GLenum *>(attachments.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_const_GLenum,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void invalidate_sub_framebuffer(groups::framebuffer_target target, span_const_GLenum const& attachments, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateSubFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function InvalidateSubFramebuffer"));
    }
    glInvalidateSubFramebuffer(static_cast<GLenum>(target), attachments.size(), reinterpret_cast<const GLenum *>(attachments.data()), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void invalidate_tex_image(u32 texture, i32 level)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateTexImage)
                Throw(undefined_behavior(
                    "unloaded function InvalidateTexImage"));
        glIsTexture(texture);
    }
    glInvalidateTexImage(texture, level);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void invalidate_tex_sub_image(u32 texture, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateTexSubImage)
                Throw(undefined_behavior(
                    "unloaded function InvalidateTexSubImage"));
        glIsTexture(texture);
    }
    glInvalidateTexSubImage(texture, level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void multi_draw_arrays_indirect(groups::primitive_type mode, ptroff indirect, i32 drawcount, i32 stride)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiDrawArraysIndirect)
                Throw(undefined_behavior(
                    "unloaded function MultiDrawArraysIndirect"));
    }
    glMultiDrawArraysIndirect(static_cast<GLenum>(mode), reinterpret_cast<const void*>(indirect), drawcount, stride);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void multi_draw_elements_indirect(groups::primitive_type mode, groups::draw_elements_type type, ptroff indirect, i32 drawcount, i32 stride)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiDrawElementsIndirect)
                Throw(undefined_behavior(
                    "unloaded function MultiDrawElementsIndirect"));
    }
    glMultiDrawElementsIndirect(static_cast<GLenum>(mode), static_cast<GLenum>(type), reinterpret_cast<const void*>(indirect), drawcount, stride);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_program_interfaceiv(u32 program, groups::program_interface programInterface, groups::program_interface_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramInterfaceiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramInterfaceiv"));
        glIsProgram(program);
    }
    glGetProgramInterfaceiv(program, static_cast<GLenum>(programInterface), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE GLuint get_program_resource_index(u32 program, groups::program_interface programInterface, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceIndex)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceIndex"));
        glIsProgram(program);
    }
    auto out = glGetProgramResourceIndex(program, static_cast<GLenum>(programInterface), name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE GLint get_program_resource_location(u32 program, groups::program_interface programInterface, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceLocation)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceLocation"));
        glIsProgram(program);
    }
    auto out = glGetProgramResourceLocation(program, static_cast<GLenum>(programInterface), name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE GLint get_program_resource_location_index(u32 program, groups::program_interface programInterface, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceLocationIndex)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceLocationIndex"));
        glIsProgram(program);
    }
    auto out = glGetProgramResourceLocationIndex(program, static_cast<GLenum>(programInterface), name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.3 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_program_resource_name(u32 program, groups::program_interface programInterface, u32 index, i32& length, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceName)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceName"));
        glIsProgram(program);
    }
    glGetProgramResourceName(program, static_cast<GLenum>(programInterface), index, name.size(), &length, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_const_GLenum,
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_program_resourceiv(u32 program, groups::program_interface programInterface, u32 index, span_const_GLenum const& props, i32& length, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceiv"));
        glIsProgram(program);
    }
    glGetProgramResourceiv(program, static_cast<GLenum>(programInterface), index, props.size(), reinterpret_cast<const GLenum *>(props.data()), params.size(), &length, reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void shader_storage_block_binding(u32 program, u32 storageBlockIndex, u32 storageBlockBinding)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glShaderStorageBlockBinding)
                Throw(undefined_behavior(
                    "unloaded function ShaderStorageBlockBinding"));
        glIsProgram(program);
    }
    glShaderStorageBlockBinding(program, storageBlockIndex, storageBlockBinding);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void tex_buffer_range(groups::texture_target target, groups::sized_internal_format internalformat, u32 buffer, GLintptr offset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexBufferRange)
                Throw(undefined_behavior(
                    "unloaded function TexBufferRange"));
        glIsBuffer(buffer);
    }
    glTexBufferRange(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), buffer, offset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void tex_storage_2d_multisample(groups::texture_target target, i32 samples, groups::sized_internal_format internalformat, vec_2_i32 const& width, bool fixedsamplelocations)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage2DMultisample)
                Throw(undefined_behavior(
                    "unloaded function TexStorage2DMultisample"));
    }
    glTexStorage2DMultisample(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width[0], width[1], fixedsamplelocations);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void tex_storage_3d_multisample(groups::texture_target target, i32 samples, groups::sized_internal_format internalformat, vec_3_i32 const& width, bool fixedsamplelocations)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage3DMultisample)
                Throw(undefined_behavior(
                    "unloaded function TexStorage3DMultisample"));
    }
    glTexStorage3DMultisample(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width[0], width[1], width[2], fixedsamplelocations);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void texture_view(u32 texture, groups::texture_target target, u32 origtexture, groups::sized_internal_format internalformat, u32 minlevel, u32 numlevels, u32 minlayer, u32 numlayers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureView)
                Throw(undefined_behavior(
                    "unloaded function TextureView"));
        glIsTexture(texture);
    }
    glTextureView(texture, static_cast<GLenum>(target), origtexture, static_cast<GLenum>(internalformat), minlevel, numlevels, minlayer, numlayers);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void bind_vertex_buffer(u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindVertexBuffer)
                Throw(undefined_behavior(
                    "unloaded function BindVertexBuffer"));
        glIsBuffer(buffer);
    }
    glBindVertexBuffer(bindingindex, buffer, offset, stride);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void vertex_attrib_binding(u32 attribindex, u32 bindingindex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribBinding)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribBinding"));
    }
    glVertexAttribBinding(attribindex, bindingindex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void vertex_attrib_format(u32 attribindex, i32 size, groups::vertex_attrib_type type, bool normalized, u32 relativeoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribFormat)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribFormat"));
    }
    glVertexAttribFormat(attribindex, size, static_cast<GLenum>(type), normalized, relativeoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void vertex_attrib_i_format(u32 attribindex, i32 size, groups::vertex_attrib_int type, u32 relativeoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribIFormat)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribIFormat"));
    }
    glVertexAttribIFormat(attribindex, size, static_cast<GLenum>(type), relativeoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void vertex_attrib_l_format(u32 attribindex, i32 size, groups::vertex_attrib_long type, u32 relativeoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribLFormat)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribLFormat"));
    }
    glVertexAttribLFormat(attribindex, size, static_cast<GLenum>(type), relativeoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void vertex_binding_divisor(u32 bindingindex, u32 divisor)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexBindingDivisor)
                Throw(undefined_behavior(
                    "unloaded function VertexBindingDivisor"));
    }
    glVertexBindingDivisor(bindingindex, divisor);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void debug_message_callback(GLDEBUGPROC callback, span_const_byte const& userParam)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDebugMessageCallback)
                Throw(undefined_behavior(
                    "unloaded function DebugMessageCallback"));
    }
    glDebugMessageCallback(callback, reinterpret_cast<const void *>(userParam.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void debug_message_control(groups::debug_source source, groups::debug_type type, groups::debug_severity severity, span_const_u32 const& ids, bool enabled)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDebugMessageControl)
                Throw(undefined_behavior(
                    "unloaded function DebugMessageControl"));
    }
    glDebugMessageControl(static_cast<GLenum>(source), static_cast<GLenum>(type), static_cast<GLenum>(severity), ids.size(), reinterpret_cast<const GLuint *>(ids.data()), enabled);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void debug_message_insert(groups::debug_source source, groups::debug_type type, u32 id, groups::debug_severity severity, i32 length, std::string_view const& buf)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDebugMessageInsert)
                Throw(undefined_behavior(
                    "unloaded function DebugMessageInsert"));
    }
    glDebugMessageInsert(static_cast<GLenum>(source), static_cast<GLenum>(type), id, static_cast<GLenum>(severity), length, buf.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_GLchar,
    class span_GLenum,
    class span_i32,
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_GLenum> &&
    std::is_same_v<std::decay_t<typename span_GLenum::value_type>, GLenum> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE GLuint get_debug_message_log(span_GLenum sources, span_GLenum types, span_u32 ids, span_GLenum severities, span_i32 lengths, span_GLchar messageLog)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetDebugMessageLog)
                Throw(undefined_behavior(
                    "unloaded function GetDebugMessageLog"));
    }
    auto out = glGetDebugMessageLog(sources.size(), messageLog.size(), reinterpret_cast<GLenum *>(sources.data()), reinterpret_cast<GLenum *>(types.data()), reinterpret_cast<GLuint *>(ids.data()), reinterpret_cast<GLenum *>(severities.data()), reinterpret_cast<GLsizei *>(lengths.data()), messageLog.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.3 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_object_label(groups::object_identifier identifier, u32 name, i32& length, span_GLchar label)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetObjectLabel)
                Throw(undefined_behavior(
                    "unloaded function GetObjectLabel"));
    }
    glGetObjectLabel(static_cast<GLenum>(identifier), name, label.size(), &length, label.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_GLchar,
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_object_ptr_label(span_const_byte const& ptr, i32& length, span_GLchar label)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetObjectPtrLabel)
                Throw(undefined_behavior(
                    "unloaded function GetObjectPtrLabel"));
    }
    glGetObjectPtrLabel(reinterpret_cast<const void *>(ptr.data()), label.size(), &length, label.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void object_label(groups::object_identifier identifier, u32 name, i32 length, std::string_view const& label)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glObjectLabel)
                Throw(undefined_behavior(
                    "unloaded function ObjectLabel"));
    }
    glObjectLabel(static_cast<GLenum>(identifier), name, length, label.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 3>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void object_ptr_label(span_const_byte const& ptr, i32 length, std::string_view const& label)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glObjectPtrLabel)
                Throw(undefined_behavior(
                    "unloaded function ObjectPtrLabel"));
    }
    glObjectPtrLabel(reinterpret_cast<const void *>(ptr.data()), length, label.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void pop_debug_group()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPopDebugGroup)
                Throw(undefined_behavior(
                    "unloaded function PopDebugGroup"));
    }
    glPopDebugGroup();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.3 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 3>>)
STATICINLINE void push_debug_group(groups::debug_source source, u32 id, i32 length, std::string_view const& message)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPushDebugGroup)
                Throw(undefined_behavior(
                    "unloaded function PushDebugGroup"));
    }
    glPushDebugGroup(static_cast<GLenum>(source), id, length, message.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_4_3
#ifdef GL_VERSION_4_4
/* Introduced in GL core 4.4 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void buffer_storage(groups::buffer_storage_target target, span_const_byte const& data, groups::buffer_storage_mask flags)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBufferStorage)
                Throw(undefined_behavior(
                    "unloaded function BufferStorage"));
    }
    glBufferStorage(static_cast<GLenum>(target), data.size(), reinterpret_cast<const void *>(data.data()), static_cast<GLenum>(flags));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.4 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void clear_tex_image(u32 texture, i32 level, groups::pixel_format format, groups::pixel_type type, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearTexImage)
                Throw(undefined_behavior(
                    "unloaded function ClearTexImage"));
        glIsTexture(texture);
    }
    glClearTexImage(texture, level, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.4 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void clear_tex_sub_image(u32 texture, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearTexSubImage)
                Throw(undefined_behavior(
                    "unloaded function ClearTexSubImage"));
        glIsTexture(texture);
    }
    glClearTexSubImage(texture, level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.4 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void bind_buffers_base(groups::buffer_target_arb target, u32 first, span_const_u32 const& buffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindBuffersBase)
                Throw(undefined_behavior(
                    "unloaded function BindBuffersBase"));
    }
    glBindBuffersBase(static_cast<GLenum>(target), first, buffers.size(), reinterpret_cast<const GLuint *>(buffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.4 */
template<
    class span_const_GLintptr,
    class span_const_GLsizeiptr,
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32> &&
    semantic::concepts::Span<span_const_GLintptr> &&
    std::is_same_v<std::decay_t<typename span_const_GLintptr::value_type>, GLintptr> &&
    semantic::concepts::Span<span_const_GLsizeiptr> &&
    std::is_same_v<std::decay_t<typename span_const_GLsizeiptr::value_type>, GLsizeiptr>)
STATICINLINE void bind_buffers_range(groups::buffer_target_arb target, u32 first, span_const_u32 const& buffers, span_const_GLintptr const& offsets, span_const_GLsizeiptr const& sizes)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindBuffersRange)
                Throw(undefined_behavior(
                    "unloaded function BindBuffersRange"));
    }
    glBindBuffersRange(static_cast<GLenum>(target), first, buffers.size(), reinterpret_cast<const GLuint *>(buffers.data()), reinterpret_cast<const GLintptr *>(offsets.data()), reinterpret_cast<const GLsizeiptr *>(sizes.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.4 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void bind_image_textures(u32 first, span_const_u32 const& textures)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindImageTextures)
                Throw(undefined_behavior(
                    "unloaded function BindImageTextures"));
    }
    glBindImageTextures(first, textures.size(), reinterpret_cast<const GLuint *>(textures.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.4 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void bind_samplers(u32 first, span_const_u32 const& samplers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindSamplers)
                Throw(undefined_behavior(
                    "unloaded function BindSamplers"));
    }
    glBindSamplers(first, samplers.size(), reinterpret_cast<const GLuint *>(samplers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.4 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void bind_textures(u32 first, span_const_u32 const& textures)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindTextures)
                Throw(undefined_behavior(
                    "unloaded function BindTextures"));
    }
    glBindTextures(first, textures.size(), reinterpret_cast<const GLuint *>(textures.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.4 */
template<
    class span_const_GLintptr,
    class span_const_i32,
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 4>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32> &&
    semantic::concepts::Span<span_const_GLintptr> &&
    std::is_same_v<std::decay_t<typename span_const_GLintptr::value_type>, GLintptr> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void bind_vertex_buffers(u32 first, span_const_u32 const& buffers, span_const_GLintptr const& offsets, span_const_i32 const& strides)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindVertexBuffers)
                Throw(undefined_behavior(
                    "unloaded function BindVertexBuffers"));
    }
    glBindVertexBuffers(first, buffers.size(), reinterpret_cast<const GLuint *>(buffers.data()), reinterpret_cast<const GLintptr *>(offsets.data()), reinterpret_cast<const GLsizei *>(strides.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_4_4
#ifdef GL_VERSION_4_5
/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void clip_control(groups::clip_control_origin origin, groups::clip_control_depth depth)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClipControl)
                Throw(undefined_behavior(
                    "unloaded function ClipControl"));
    }
    glClipControl(static_cast<GLenum>(origin), static_cast<GLenum>(depth));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void bind_texture_unit(u32 unit, u32 texture)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindTextureUnit)
                Throw(undefined_behavior(
                    "unloaded function BindTextureUnit"));
        glIsTexture(texture);
    }
    glBindTextureUnit(unit, texture);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void blit_named_framebuffer(u32 readFramebuffer, u32 drawFramebuffer, i32 srcX0, i32 srcY0, i32 srcX1, i32 srcY1, i32 dstX0, i32 dstY0, i32 dstX1, i32 dstY1, groups::clear_buffer_mask mask, groups::blit_framebuffer_filter filter)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlitNamedFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function BlitNamedFramebuffer"));
    }
    glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, static_cast<GLenum>(mask), static_cast<GLenum>(filter));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE GLenum check_named_framebuffer_status(u32 framebuffer, groups::framebuffer_target target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCheckNamedFramebufferStatus)
                Throw(undefined_behavior(
                    "unloaded function CheckNamedFramebufferStatus"));
        glIsFramebuffer(framebuffer);
    }
    auto out = glCheckNamedFramebufferStatus(framebuffer, static_cast<GLenum>(target));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void clear_named_buffer_data(u32 buffer, groups::sized_internal_format internalformat, groups::pixel_format format, groups::pixel_type type, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearNamedBufferData)
                Throw(undefined_behavior(
                    "unloaded function ClearNamedBufferData"));
        glIsBuffer(buffer);
    }
    glClearNamedBufferData(buffer, static_cast<GLenum>(internalformat), static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void clear_named_buffer_sub_data(u32 buffer, groups::sized_internal_format internalformat, GLintptr offset, GLsizeiptr size, groups::pixel_format format, groups::pixel_type type, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearNamedBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function ClearNamedBufferSubData"));
        glIsBuffer(buffer);
    }
    glClearNamedBufferSubData(buffer, static_cast<GLenum>(internalformat), offset, size, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void clear_named_framebufferfi(u32 framebuffer, groups::buffer buffer, i32 drawbuffer, f32 depth, i32 stencil)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearNamedFramebufferfi)
                Throw(undefined_behavior(
                    "unloaded function ClearNamedFramebufferfi"));
        glIsFramebuffer(framebuffer);
    }
    glClearNamedFramebufferfi(framebuffer, static_cast<GLenum>(buffer), drawbuffer, depth, stencil);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void clear_named_framebufferfv(u32 framebuffer, groups::buffer buffer, i32 drawbuffer, span_const_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearNamedFramebufferfv)
                Throw(undefined_behavior(
                    "unloaded function ClearNamedFramebufferfv"));
        glIsFramebuffer(framebuffer);
    }
    glClearNamedFramebufferfv(framebuffer, static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void clear_named_framebufferiv(u32 framebuffer, groups::buffer buffer, i32 drawbuffer, span_const_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearNamedFramebufferiv)
                Throw(undefined_behavior(
                    "unloaded function ClearNamedFramebufferiv"));
        glIsFramebuffer(framebuffer);
    }
    glClearNamedFramebufferiv(framebuffer, static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void clear_named_framebufferuiv(u32 framebuffer, groups::buffer buffer, i32 drawbuffer, span_const_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearNamedFramebufferuiv)
                Throw(undefined_behavior(
                    "unloaded function ClearNamedFramebufferuiv"));
        glIsFramebuffer(framebuffer);
    }
    glClearNamedFramebufferuiv(framebuffer, static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_texture_sub_image_1d(u32 texture, i32 level, i32 xoffset, i32 width, groups::pixel_format format, i32 imageSize, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTextureSubImage1D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTextureSubImage1D"));
        glIsTexture(texture);
    }
    glCompressedTextureSubImage1D(texture, level, xoffset, width, static_cast<GLenum>(format), imageSize, reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_texture_sub_image_2d(u32 texture, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& width, groups::pixel_format format, i32 imageSize, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTextureSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTextureSubImage2D"));
        glIsTexture(texture);
    }
    glCompressedTextureSubImage2D(texture, level, xoffset.x(), xoffset.y(), width[0], width[1], static_cast<GLenum>(format), imageSize, reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_texture_sub_image_3d(u32 texture, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, groups::pixel_format format, i32 imageSize, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTextureSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTextureSubImage3D"));
        glIsTexture(texture);
    }
    glCompressedTextureSubImage3D(texture, level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], static_cast<GLenum>(format), imageSize, reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void copy_named_buffer_sub_data(u32 readBuffer, u32 writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyNamedBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function CopyNamedBufferSubData"));
    }
    glCopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_texture_sub_image_1d(u32 texture, i32 level, i32 xoffset, vec_2_i32 const& x, i32 width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTextureSubImage1D)
                Throw(undefined_behavior(
                    "unloaded function CopyTextureSubImage1D"));
        glIsTexture(texture);
    }
    glCopyTextureSubImage1D(texture, level, xoffset, x.x(), x.y(), width);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_texture_sub_image_2d(u32 texture, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTextureSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function CopyTextureSubImage2D"));
        glIsTexture(texture);
    }
    glCopyTextureSubImage2D(texture, level, xoffset.x(), xoffset.y(), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_2_i32,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_texture_sub_image_3d(u32 texture, i32 level, vec_3_i32 const& xoffset, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTextureSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function CopyTextureSubImage3D"));
        glIsTexture(texture);
    }
    glCopyTextureSubImage3D(texture, level, xoffset.x(), xoffset.y(), xoffset.z(), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_buffers(span_u32 buffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateBuffers)
                Throw(undefined_behavior(
                    "unloaded function CreateBuffers"));
    }
    glCreateBuffers(buffers.size(), reinterpret_cast<GLuint *>(buffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_framebuffers(span_u32 framebuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateFramebuffers)
                Throw(undefined_behavior(
                    "unloaded function CreateFramebuffers"));
    }
    glCreateFramebuffers(framebuffers.size(), reinterpret_cast<GLuint *>(framebuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_program_pipelines(span_u32 pipelines)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateProgramPipelines)
                Throw(undefined_behavior(
                    "unloaded function CreateProgramPipelines"));
    }
    glCreateProgramPipelines(pipelines.size(), reinterpret_cast<GLuint *>(pipelines.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_queries(GLenum target, span_u32 ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateQueries)
                Throw(undefined_behavior(
                    "unloaded function CreateQueries"));
    }
    glCreateQueries(target, ids.size(), reinterpret_cast<GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_renderbuffers(span_u32 renderbuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateRenderbuffers)
                Throw(undefined_behavior(
                    "unloaded function CreateRenderbuffers"));
    }
    glCreateRenderbuffers(renderbuffers.size(), reinterpret_cast<GLuint *>(renderbuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_samplers(span_u32 samplers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateSamplers)
                Throw(undefined_behavior(
                    "unloaded function CreateSamplers"));
    }
    glCreateSamplers(samplers.size(), reinterpret_cast<GLuint *>(samplers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_textures(GLenum target, span_u32 textures)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateTextures)
                Throw(undefined_behavior(
                    "unloaded function CreateTextures"));
    }
    glCreateTextures(target, textures.size(), reinterpret_cast<GLuint *>(textures.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_transform_feedbacks(span_u32 ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateTransformFeedbacks)
                Throw(undefined_behavior(
                    "unloaded function CreateTransformFeedbacks"));
    }
    glCreateTransformFeedbacks(ids.size(), reinterpret_cast<GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void create_vertex_arrays(span_u32 arrays)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateVertexArrays)
                Throw(undefined_behavior(
                    "unloaded function CreateVertexArrays"));
    }
    glCreateVertexArrays(arrays.size(), reinterpret_cast<GLuint *>(arrays.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void disable_vertex_array_attrib(u32 vaobj, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDisableVertexArrayAttrib)
                Throw(undefined_behavior(
                    "unloaded function DisableVertexArrayAttrib"));
        glIsVertexArray(vaobj);
    }
    glDisableVertexArrayAttrib(vaobj, index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void enable_vertex_array_attrib(u32 vaobj, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEnableVertexArrayAttrib)
                Throw(undefined_behavior(
                    "unloaded function EnableVertexArrayAttrib"));
        glIsVertexArray(vaobj);
    }
    glEnableVertexArrayAttrib(vaobj, index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void flush_mapped_named_buffer_range(u32 buffer, GLintptr offset, GLsizeiptr length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFlushMappedNamedBufferRange)
                Throw(undefined_behavior(
                    "unloaded function FlushMappedNamedBufferRange"));
        glIsBuffer(buffer);
    }
    glFlushMappedNamedBufferRange(buffer, offset, length);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void generate_texture_mipmap(u32 texture)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenerateTextureMipmap)
                Throw(undefined_behavior(
                    "unloaded function GenerateTextureMipmap"));
        glIsTexture(texture);
    }
    glGenerateTextureMipmap(texture);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_compressed_texture_image(u32 texture, i32 level, i32 bufSize, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetCompressedTextureImage)
                Throw(undefined_behavior(
                    "unloaded function GetCompressedTextureImage"));
        glIsTexture(texture);
    }
    glGetCompressedTextureImage(texture, level, bufSize, reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_named_buffer_parameteri64v(u32 buffer, groups::buffer_prop_arb pname, span_i64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetNamedBufferParameteri64v)
                Throw(undefined_behavior(
                    "unloaded function GetNamedBufferParameteri64v"));
        glIsBuffer(buffer);
    }
    glGetNamedBufferParameteri64v(buffer, static_cast<GLenum>(pname), reinterpret_cast<GLint64 *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_named_buffer_parameter(u32 buffer, groups::buffer_prop_arb pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetNamedBufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetNamedBufferParameteriv"));
        glIsBuffer(buffer);
    }
    glGetNamedBufferParameteriv(buffer, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_named_buffer_pointerv(u32 buffer, groups::buffer_pointer_name_arb pname, span_byte params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetNamedBufferPointerv)
                Throw(undefined_behavior(
                    "unloaded function GetNamedBufferPointerv"));
        glIsBuffer(buffer);
    }
    glGetNamedBufferPointerv(buffer, static_cast<GLenum>(pname), reinterpret_cast<void **>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_named_buffer_sub_data(u32 buffer, GLintptr offset, GLsizeiptr size, span_byte data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetNamedBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function GetNamedBufferSubData"));
        glIsBuffer(buffer);
    }
    glGetNamedBufferSubData(buffer, offset, size, reinterpret_cast<void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_named_framebuffer_attachment_parameter(u32 framebuffer, groups::framebuffer_attachment attachment, groups::framebuffer_attachment_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetNamedFramebufferAttachmentParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetNamedFramebufferAttachmentParameteriv"));
        glIsFramebuffer(framebuffer);
    }
    glGetNamedFramebufferAttachmentParameteriv(framebuffer, static_cast<GLenum>(attachment), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_named_framebuffer_parameter(u32 framebuffer, groups::get_framebuffer_parameter pname, span_i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetNamedFramebufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetNamedFramebufferParameteriv"));
        glIsFramebuffer(framebuffer);
    }
    glGetNamedFramebufferParameteriv(framebuffer, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_named_renderbuffer_parameter(u32 renderbuffer, groups::renderbuffer_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetNamedRenderbufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetNamedRenderbufferParameteriv"));
        glIsRenderbuffer(renderbuffer);
    }
    glGetNamedRenderbufferParameteriv(renderbuffer, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void get_query_buffer_objecti64v(u32 id, u32 buffer, groups::query_object_parameter_name pname, GLintptr offset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryBufferObjecti64v)
                Throw(undefined_behavior(
                    "unloaded function GetQueryBufferObjecti64v"));
        glIsBuffer(buffer);
    }
    glGetQueryBufferObjecti64v(id, buffer, static_cast<GLenum>(pname), offset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void get_query_buffer_objectiv(u32 id, u32 buffer, groups::query_object_parameter_name pname, GLintptr offset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryBufferObjectiv)
                Throw(undefined_behavior(
                    "unloaded function GetQueryBufferObjectiv"));
        glIsBuffer(buffer);
    }
    glGetQueryBufferObjectiv(id, buffer, static_cast<GLenum>(pname), offset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void get_query_buffer_objectui64v(u32 id, u32 buffer, groups::query_object_parameter_name pname, GLintptr offset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryBufferObjectui64v)
                Throw(undefined_behavior(
                    "unloaded function GetQueryBufferObjectui64v"));
        glIsBuffer(buffer);
    }
    glGetQueryBufferObjectui64v(id, buffer, static_cast<GLenum>(pname), offset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void get_query_buffer_objectuiv(u32 id, u32 buffer, groups::query_object_parameter_name pname, GLintptr offset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryBufferObjectuiv)
                Throw(undefined_behavior(
                    "unloaded function GetQueryBufferObjectuiv"));
        glIsBuffer(buffer);
    }
    glGetQueryBufferObjectuiv(id, buffer, static_cast<GLenum>(pname), offset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_texture_image(u32 texture, i32 level, groups::pixel_format format, groups::pixel_type type, i32 bufSize, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTextureImage)
                Throw(undefined_behavior(
                    "unloaded function GetTextureImage"));
        glIsTexture(texture);
    }
    glGetTextureImage(texture, level, static_cast<GLenum>(format), static_cast<GLenum>(type), bufSize, reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_texture_level_parameter(u32 texture, i32 level, groups::get_texture_parameter pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTextureLevelParameterfv)
                Throw(undefined_behavior(
                    "unloaded function GetTextureLevelParameterfv"));
        glIsTexture(texture);
    }
    glGetTextureLevelParameterfv(texture, level, static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_texture_level_parameter(u32 texture, i32 level, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTextureLevelParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetTextureLevelParameteriv"));
        glIsTexture(texture);
    }
    glGetTextureLevelParameteriv(texture, level, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_texture_parameter_iiv(u32 texture, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTextureParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function GetTextureParameterIiv"));
        glIsTexture(texture);
    }
    glGetTextureParameterIiv(texture, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_texture_parameter_iuiv(u32 texture, groups::get_texture_parameter pname, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTextureParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function GetTextureParameterIuiv"));
        glIsTexture(texture);
    }
    glGetTextureParameterIuiv(texture, static_cast<GLenum>(pname), reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_texture_parameter(u32 texture, groups::get_texture_parameter pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTextureParameterfv)
                Throw(undefined_behavior(
                    "unloaded function GetTextureParameterfv"));
        glIsTexture(texture);
    }
    glGetTextureParameterfv(texture, static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_texture_parameter(u32 texture, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTextureParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetTextureParameteriv"));
        glIsTexture(texture);
    }
    glGetTextureParameteriv(texture, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_transform_feedbacki64_v(u32 xfb, groups::transform_feedback_prop pname, u32 index, span_i64 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTransformFeedbacki64_v)
                Throw(undefined_behavior(
                    "unloaded function GetTransformFeedbacki64_v"));
        glIsTransformFeedback(xfb);
    }
    glGetTransformFeedbacki64_v(xfb, static_cast<GLenum>(pname), index, reinterpret_cast<GLint64 *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_transform_feedbacki_v(u32 xfb, groups::transform_feedback_prop pname, u32 index, span_i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTransformFeedbacki_v)
                Throw(undefined_behavior(
                    "unloaded function GetTransformFeedbacki_v"));
        glIsTransformFeedback(xfb);
    }
    glGetTransformFeedbacki_v(xfb, static_cast<GLenum>(pname), index, reinterpret_cast<GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_transform_feedbackiv(u32 xfb, groups::transform_feedback_prop pname, span_i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTransformFeedbackiv)
                Throw(undefined_behavior(
                    "unloaded function GetTransformFeedbackiv"));
        glIsTransformFeedback(xfb);
    }
    glGetTransformFeedbackiv(xfb, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_vertex_array_indexed64iv(u32 vaobj, u32 index, groups::vertex_array_prop pname, span_i64 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexArrayIndexed64iv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexArrayIndexed64iv"));
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayIndexed64iv(vaobj, index, static_cast<GLenum>(pname), reinterpret_cast<GLint64 *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_vertex_array_indexediv(u32 vaobj, u32 index, groups::vertex_array_prop pname, span_i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexArrayIndexediv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexArrayIndexediv"));
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayIndexediv(vaobj, index, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_vertex_arrayiv(u32 vaobj, groups::vertex_array_prop pname, span_i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexArrayiv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexArrayiv"));
        glIsVertexArray(vaobj);
    }
    glGetVertexArrayiv(vaobj, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_GLenum
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum>)
STATICINLINE void invalidate_named_framebuffer_data(u32 framebuffer, i32 numAttachments, span_const_GLenum const& attachments)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateNamedFramebufferData)
                Throw(undefined_behavior(
                    "unloaded function InvalidateNamedFramebufferData"));
        glIsFramebuffer(framebuffer);
    }
    glInvalidateNamedFramebufferData(framebuffer, numAttachments, reinterpret_cast<const GLenum *>(attachments.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_GLenum,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void invalidate_named_framebuffer_sub_data(u32 framebuffer, i32 numAttachments, span_const_GLenum const& attachments, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateNamedFramebufferSubData)
                Throw(undefined_behavior(
                    "unloaded function InvalidateNamedFramebufferSubData"));
        glIsFramebuffer(framebuffer);
    }
    glInvalidateNamedFramebufferSubData(framebuffer, numAttachments, reinterpret_cast<const GLenum *>(attachments.data()), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void * map_named_buffer(u32 buffer, groups::buffer_access_arb access)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMapNamedBuffer)
                Throw(undefined_behavior(
                    "unloaded function MapNamedBuffer"));
        glIsBuffer(buffer);
    }
    auto out = glMapNamedBuffer(buffer, static_cast<GLenum>(access));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void * map_named_buffer_range(u32 buffer, GLintptr offset, GLsizeiptr length, groups::map_buffer_access_mask access)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMapNamedBufferRange)
                Throw(undefined_behavior(
                    "unloaded function MapNamedBufferRange"));
        glIsBuffer(buffer);
    }
    auto out = glMapNamedBufferRange(buffer, offset, length, static_cast<GLenum>(access));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void named_buffer_data(u32 buffer, GLsizeiptr size, span_const_byte const& data, groups::vertex_buffer_object_usage usage)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedBufferData)
                Throw(undefined_behavior(
                    "unloaded function NamedBufferData"));
        glIsBuffer(buffer);
    }
    glNamedBufferData(buffer, size, reinterpret_cast<const void *>(data.data()), static_cast<GLenum>(usage));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void named_buffer_storage(u32 buffer, span_const_byte const& data, groups::buffer_storage_mask flags)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedBufferStorage)
                Throw(undefined_behavior(
                    "unloaded function NamedBufferStorage"));
        glIsBuffer(buffer);
    }
    glNamedBufferStorage(buffer, data.size(), reinterpret_cast<const void *>(data.data()), static_cast<GLenum>(flags));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void named_buffer_sub_data(u32 buffer, GLintptr offset, GLsizeiptr size, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function NamedBufferSubData"));
        glIsBuffer(buffer);
    }
    glNamedBufferSubData(buffer, offset, size, reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void named_framebuffer_draw_buffer(u32 framebuffer, groups::color_buffer buf)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedFramebufferDrawBuffer)
                Throw(undefined_behavior(
                    "unloaded function NamedFramebufferDrawBuffer"));
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferDrawBuffer(framebuffer, static_cast<GLenum>(buf));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_GLenum
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum>)
STATICINLINE void named_framebuffer_draw_buffers(u32 framebuffer, i32 n, span_const_GLenum const& bufs)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedFramebufferDrawBuffers)
                Throw(undefined_behavior(
                    "unloaded function NamedFramebufferDrawBuffers"));
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferDrawBuffers(framebuffer, n, reinterpret_cast<const GLenum *>(bufs.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void named_framebuffer_parameter(u32 framebuffer, groups::framebuffer_parameter_name pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedFramebufferParameteri)
                Throw(undefined_behavior(
                    "unloaded function NamedFramebufferParameteri"));
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferParameteri(framebuffer, static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void named_framebuffer_read_buffer(u32 framebuffer, groups::color_buffer src)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedFramebufferReadBuffer)
                Throw(undefined_behavior(
                    "unloaded function NamedFramebufferReadBuffer"));
        glIsFramebuffer(framebuffer);
    }
    glNamedFramebufferReadBuffer(framebuffer, static_cast<GLenum>(src));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void named_framebuffer_renderbuffer(u32 framebuffer, groups::framebuffer_attachment attachment, groups::renderbuffer_target renderbuffertarget, u32 renderbuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedFramebufferRenderbuffer)
                Throw(undefined_behavior(
                    "unloaded function NamedFramebufferRenderbuffer"));
        glIsFramebuffer(framebuffer);
        glIsRenderbuffer(renderbuffer);
    }
    glNamedFramebufferRenderbuffer(framebuffer, static_cast<GLenum>(attachment), static_cast<GLenum>(renderbuffertarget), renderbuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void named_framebuffer_texture(u32 framebuffer, groups::framebuffer_attachment attachment, u32 texture, i32 level)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedFramebufferTexture)
                Throw(undefined_behavior(
                    "unloaded function NamedFramebufferTexture"));
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTexture(framebuffer, static_cast<GLenum>(attachment), texture, level);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void named_framebuffer_texture_layer(u32 framebuffer, groups::framebuffer_attachment attachment, u32 texture, i32 level, i32 layer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedFramebufferTextureLayer)
                Throw(undefined_behavior(
                    "unloaded function NamedFramebufferTextureLayer"));
        glIsFramebuffer(framebuffer);
        glIsTexture(texture);
    }
    glNamedFramebufferTextureLayer(framebuffer, static_cast<GLenum>(attachment), texture, level, layer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void named_renderbuffer_storage(u32 renderbuffer, groups::internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedRenderbufferStorage)
                Throw(undefined_behavior(
                    "unloaded function NamedRenderbufferStorage"));
        glIsRenderbuffer(renderbuffer);
    }
    glNamedRenderbufferStorage(renderbuffer, static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void named_renderbuffer_storage_multisample(u32 renderbuffer, i32 samples, groups::internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glNamedRenderbufferStorageMultisample)
                Throw(undefined_behavior(
                    "unloaded function NamedRenderbufferStorageMultisample"));
        glIsRenderbuffer(renderbuffer);
    }
    glNamedRenderbufferStorageMultisample(renderbuffer, samples, static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void texture_buffer(u32 texture, groups::sized_internal_format internalformat, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureBuffer)
                Throw(undefined_behavior(
                    "unloaded function TextureBuffer"));
        glIsTexture(texture);
        glIsBuffer(buffer);
    }
    glTextureBuffer(texture, static_cast<GLenum>(internalformat), buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void texture_buffer_range(u32 texture, groups::sized_internal_format internalformat, u32 buffer, GLintptr offset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureBufferRange)
                Throw(undefined_behavior(
                    "unloaded function TextureBufferRange"));
        glIsTexture(texture);
        glIsBuffer(buffer);
    }
    glTextureBufferRange(texture, static_cast<GLenum>(internalformat), buffer, offset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void texture_parameter_iiv(u32 texture, groups::texture_parameter_name pname, span_const_i32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function TextureParameterIiv"));
        glIsTexture(texture);
    }
    glTextureParameterIiv(texture, static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void texture_parameter_iuiv(u32 texture, groups::texture_parameter_name pname, span_const_u32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function TextureParameterIuiv"));
        glIsTexture(texture);
    }
    glTextureParameterIuiv(texture, static_cast<GLenum>(pname), reinterpret_cast<const GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void texture_parameter(u32 texture, groups::texture_parameter_name pname, f32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureParameterf)
                Throw(undefined_behavior(
                    "unloaded function TextureParameterf"));
        glIsTexture(texture);
    }
    glTextureParameterf(texture, static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void texture_parameter(u32 texture, groups::texture_parameter_name pname, span_const_f32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureParameterfv)
                Throw(undefined_behavior(
                    "unloaded function TextureParameterfv"));
        glIsTexture(texture);
    }
    glTextureParameterfv(texture, static_cast<GLenum>(pname), reinterpret_cast<const GLfloat *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void texture_parameter(u32 texture, groups::texture_parameter_name pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureParameteri)
                Throw(undefined_behavior(
                    "unloaded function TextureParameteri"));
        glIsTexture(texture);
    }
    glTextureParameteri(texture, static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void texture_parameter(u32 texture, groups::texture_parameter_name pname, span_const_i32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureParameteriv)
                Throw(undefined_behavior(
                    "unloaded function TextureParameteriv"));
        glIsTexture(texture);
    }
    glTextureParameteriv(texture, static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void texture_storage_1d(u32 texture, i32 levels, groups::sized_internal_format internalformat, i32 width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureStorage1D)
                Throw(undefined_behavior(
                    "unloaded function TextureStorage1D"));
        glIsTexture(texture);
    }
    glTextureStorage1D(texture, levels, static_cast<GLenum>(internalformat), width);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void texture_storage_2d(u32 texture, i32 levels, groups::sized_internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureStorage2D)
                Throw(undefined_behavior(
                    "unloaded function TextureStorage2D"));
        glIsTexture(texture);
    }
    glTextureStorage2D(texture, levels, static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void texture_storage_2d_multisample(u32 texture, i32 samples, groups::sized_internal_format internalformat, vec_2_i32 const& width, bool fixedsamplelocations)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureStorage2DMultisample)
                Throw(undefined_behavior(
                    "unloaded function TextureStorage2DMultisample"));
        glIsTexture(texture);
    }
    glTextureStorage2DMultisample(texture, samples, static_cast<GLenum>(internalformat), width[0], width[1], fixedsamplelocations);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void texture_storage_3d(u32 texture, i32 levels, groups::sized_internal_format internalformat, vec_3_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureStorage3D)
                Throw(undefined_behavior(
                    "unloaded function TextureStorage3D"));
        glIsTexture(texture);
    }
    glTextureStorage3D(texture, levels, static_cast<GLenum>(internalformat), width[0], width[1], width[2]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void texture_storage_3d_multisample(u32 texture, i32 samples, groups::sized_internal_format internalformat, vec_3_i32 const& width, bool fixedsamplelocations)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureStorage3DMultisample)
                Throw(undefined_behavior(
                    "unloaded function TextureStorage3DMultisample"));
        glIsTexture(texture);
    }
    glTextureStorage3DMultisample(texture, samples, static_cast<GLenum>(internalformat), width[0], width[1], width[2], fixedsamplelocations);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void texture_sub_image_1d(u32 texture, i32 level, i32 xoffset, i32 width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureSubImage1D)
                Throw(undefined_behavior(
                    "unloaded function TextureSubImage1D"));
        glIsTexture(texture);
    }
    glTextureSubImage1D(texture, level, xoffset, width, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void texture_sub_image_2d(u32 texture, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function TextureSubImage2D"));
        glIsTexture(texture);
    }
    glTextureSubImage2D(texture, level, xoffset.x(), xoffset.y(), width[0], width[1], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void texture_sub_image_3d(u32 texture, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function TextureSubImage3D"));
        glIsTexture(texture);
    }
    glTextureSubImage3D(texture, level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void transform_feedback_buffer_base(u32 xfb, u32 index, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTransformFeedbackBufferBase)
                Throw(undefined_behavior(
                    "unloaded function TransformFeedbackBufferBase"));
        glIsTransformFeedback(xfb);
        glIsBuffer(buffer);
    }
    glTransformFeedbackBufferBase(xfb, index, buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void transform_feedback_buffer_range(u32 xfb, u32 index, u32 buffer, GLintptr offset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTransformFeedbackBufferRange)
                Throw(undefined_behavior(
                    "unloaded function TransformFeedbackBufferRange"));
        glIsTransformFeedback(xfb);
        glIsBuffer(buffer);
    }
    glTransformFeedbackBufferRange(xfb, index, buffer, offset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE GLboolean unmap_named_buffer(u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUnmapNamedBuffer)
                Throw(undefined_behavior(
                    "unloaded function UnmapNamedBuffer"));
        glIsBuffer(buffer);
    }
    auto out = glUnmapNamedBuffer(buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void vertex_array_attrib_binding(u32 vaobj, u32 attribindex, u32 bindingindex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexArrayAttribBinding)
                Throw(undefined_behavior(
                    "unloaded function VertexArrayAttribBinding"));
        glIsVertexArray(vaobj);
    }
    glVertexArrayAttribBinding(vaobj, attribindex, bindingindex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void vertex_array_attrib_format(u32 vaobj, u32 attribindex, i32 size, groups::vertex_attrib_type type, bool normalized, u32 relativeoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexArrayAttribFormat)
                Throw(undefined_behavior(
                    "unloaded function VertexArrayAttribFormat"));
        glIsVertexArray(vaobj);
    }
    glVertexArrayAttribFormat(vaobj, attribindex, size, static_cast<GLenum>(type), normalized, relativeoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void vertex_array_attrib_i_format(u32 vaobj, u32 attribindex, i32 size, groups::vertex_attrib_int type, u32 relativeoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexArrayAttribIFormat)
                Throw(undefined_behavior(
                    "unloaded function VertexArrayAttribIFormat"));
        glIsVertexArray(vaobj);
    }
    glVertexArrayAttribIFormat(vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void vertex_array_attrib_l_format(u32 vaobj, u32 attribindex, i32 size, groups::vertex_attrib_long type, u32 relativeoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexArrayAttribLFormat)
                Throw(undefined_behavior(
                    "unloaded function VertexArrayAttribLFormat"));
        glIsVertexArray(vaobj);
    }
    glVertexArrayAttribLFormat(vaobj, attribindex, size, static_cast<GLenum>(type), relativeoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void vertex_array_binding_divisor(u32 vaobj, u32 bindingindex, u32 divisor)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexArrayBindingDivisor)
                Throw(undefined_behavior(
                    "unloaded function VertexArrayBindingDivisor"));
        glIsVertexArray(vaobj);
    }
    glVertexArrayBindingDivisor(vaobj, bindingindex, divisor);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void vertex_array_element_buffer(u32 vaobj, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexArrayElementBuffer)
                Throw(undefined_behavior(
                    "unloaded function VertexArrayElementBuffer"));
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayElementBuffer(vaobj, buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void vertex_array_vertex_buffer(u32 vaobj, u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexArrayVertexBuffer)
                Throw(undefined_behavior(
                    "unloaded function VertexArrayVertexBuffer"));
        glIsVertexArray(vaobj);
        glIsBuffer(buffer);
    }
    glVertexArrayVertexBuffer(vaobj, bindingindex, buffer, offset, stride);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_const_GLintptr,
    class span_const_i32,
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32> &&
    semantic::concepts::Span<span_const_GLintptr> &&
    std::is_same_v<std::decay_t<typename span_const_GLintptr::value_type>, GLintptr> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void vertex_array_vertex_buffers(u32 vaobj, u32 first, i32 count, span_const_u32 const& buffers, span_const_GLintptr const& offsets, span_const_i32 const& strides)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexArrayVertexBuffers)
                Throw(undefined_behavior(
                    "unloaded function VertexArrayVertexBuffers"));
        glIsVertexArray(vaobj);
    }
    glVertexArrayVertexBuffers(vaobj, first, count, reinterpret_cast<const GLuint *>(buffers.data()), reinterpret_cast<const GLintptr *>(offsets.data()), reinterpret_cast<const GLsizei *>(strides.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void memory_barrier_by_region(groups::memory_barrier_mask barriers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMemoryBarrierByRegion)
                Throw(undefined_behavior(
                    "unloaded function MemoryBarrierByRegion"));
    }
    glMemoryBarrierByRegion(static_cast<GLenum>(barriers));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_compressed_texture_sub_image(u32 texture, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, i32 bufSize, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetCompressedTextureSubImage)
                Throw(undefined_behavior(
                    "unloaded function GetCompressedTextureSubImage"));
        glIsTexture(texture);
    }
    glGetCompressedTextureSubImage(texture, level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], bufSize, reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_texture_sub_image(u32 texture, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, groups::pixel_format format, groups::pixel_type type, i32 bufSize, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTextureSubImage)
                Throw(undefined_behavior(
                    "unloaded function GetTextureSubImage"));
        glIsTexture(texture);
    }
    glGetTextureSubImage(texture, level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], static_cast<GLenum>(format), static_cast<GLenum>(type), bufSize, reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE GLenum get_graphics_reset_status()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetGraphicsResetStatus)
                Throw(undefined_behavior(
                    "unloaded function GetGraphicsResetStatus"));
    }
    auto out = glGetGraphicsResetStatus();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void getn_compressed_tex_image(groups::texture_target target, i32 lod, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnCompressedTexImage)
                Throw(undefined_behavior(
                    "unloaded function GetnCompressedTexImage"));
    }
    glGetnCompressedTexImage(static_cast<GLenum>(target), lod, pixels.size(), reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void getn_tex_image(groups::texture_target target, i32 level, groups::pixel_format format, groups::pixel_type type, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnTexImage)
                Throw(undefined_behavior(
                    "unloaded function GetnTexImage"));
    }
    glGetnTexImage(static_cast<GLenum>(target), level, static_cast<GLenum>(format), static_cast<GLenum>(type), pixels.size(), reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_f64
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<std::decay_t<typename span_f64::value_type>, f64>)
STATICINLINE void getn_uniformdv(u32 program, i32 location, i32 bufSize, span_f64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnUniformdv)
                Throw(undefined_behavior(
                    "unloaded function GetnUniformdv"));
        glIsProgram(program);
    }
    glGetnUniformdv(program, location, bufSize, reinterpret_cast<GLdouble *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnUniformfv)
                Throw(undefined_behavior(
                    "unloaded function GetnUniformfv"));
        glIsProgram(program);
    }
    glGetnUniformfv(program, location, bufSize, reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnUniformiv)
                Throw(undefined_behavior(
                    "unloaded function GetnUniformiv"));
        glIsProgram(program);
    }
    glGetnUniformiv(program, location, bufSize, reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void getn_uniformuiv(u32 program, i32 location, i32 bufSize, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnUniformuiv)
                Throw(undefined_behavior(
                    "unloaded function GetnUniformuiv"));
        glIsProgram(program);
    }
    glGetnUniformuiv(program, location, bufSize, reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void readn_pixels(vec_2_i32 const& x, vec_2_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_byte data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glReadnPixels)
                Throw(undefined_behavior(
                    "unloaded function ReadnPixels"));
    }
    glReadnPixels(x.x(), x.y(), width[0], width[1], static_cast<GLenum>(format), static_cast<GLenum>(type), data.size(), reinterpret_cast<void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void getn_color_table(groups::color_table_target target, groups::pixel_format format, groups::pixel_type type, span_byte table)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnColorTable)
                Throw(undefined_behavior(
                    "unloaded function GetnColorTable"));
    }
    glGetnColorTable(static_cast<GLenum>(target), static_cast<GLenum>(format), static_cast<GLenum>(type), table.size(), reinterpret_cast<void *>(table.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void getn_convolution_filter(groups::convolution_target target, groups::pixel_format format, groups::pixel_type type, span_byte image)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnConvolutionFilter)
                Throw(undefined_behavior(
                    "unloaded function GetnConvolutionFilter"));
    }
    glGetnConvolutionFilter(static_cast<GLenum>(target), static_cast<GLenum>(format), static_cast<GLenum>(type), image.size(), reinterpret_cast<void *>(image.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void getn_histogram(groups::histogram_target target, bool reset, groups::pixel_format format, groups::pixel_type type, span_byte values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnHistogram)
                Throw(undefined_behavior(
                    "unloaded function GetnHistogram"));
    }
    glGetnHistogram(static_cast<GLenum>(target), reset, static_cast<GLenum>(format), static_cast<GLenum>(type), values.size(), reinterpret_cast<void *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_f64
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<std::decay_t<typename span_f64::value_type>, f64>)
STATICINLINE void getn_mapdv(groups::map_target target, groups::map_query query, i32 bufSize, span_f64 v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnMapdv)
                Throw(undefined_behavior(
                    "unloaded function GetnMapdv"));
    }
    glGetnMapdv(static_cast<GLenum>(target), static_cast<GLenum>(query), bufSize, reinterpret_cast<GLdouble *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void getn_mapfv(groups::map_target target, groups::map_query query, i32 bufSize, span_f32 v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnMapfv)
                Throw(undefined_behavior(
                    "unloaded function GetnMapfv"));
    }
    glGetnMapfv(static_cast<GLenum>(target), static_cast<GLenum>(query), bufSize, reinterpret_cast<GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void getn_mapiv(groups::map_target target, groups::map_query query, i32 bufSize, span_i32 v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnMapiv)
                Throw(undefined_behavior(
                    "unloaded function GetnMapiv"));
    }
    glGetnMapiv(static_cast<GLenum>(target), static_cast<GLenum>(query), bufSize, reinterpret_cast<GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void getn_minmax(groups::minmax_target target, bool reset, groups::pixel_format format, groups::pixel_type type, span_byte values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnMinmax)
                Throw(undefined_behavior(
                    "unloaded function GetnMinmax"));
    }
    glGetnMinmax(static_cast<GLenum>(target), reset, static_cast<GLenum>(format), static_cast<GLenum>(type), values.size(), reinterpret_cast<void *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void getn_pixel_mapfv(groups::pixel_map map, i32 bufSize, span_f32 values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnPixelMapfv)
                Throw(undefined_behavior(
                    "unloaded function GetnPixelMapfv"));
    }
    glGetnPixelMapfv(static_cast<GLenum>(map), bufSize, reinterpret_cast<GLfloat *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void getn_pixel_mapuiv(groups::pixel_map map, i32 bufSize, span_u32 values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnPixelMapuiv)
                Throw(undefined_behavior(
                    "unloaded function GetnPixelMapuiv"));
    }
    glGetnPixelMapuiv(static_cast<GLenum>(map), bufSize, reinterpret_cast<GLuint *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u16
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u16> &&
    std::is_same_v<std::decay_t<typename span_u16::value_type>, u16>)
STATICINLINE void getn_pixel_mapusv(groups::pixel_map map, i32 bufSize, span_u16 values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnPixelMapusv)
                Throw(undefined_behavior(
                    "unloaded function GetnPixelMapusv"));
    }
    glGetnPixelMapusv(static_cast<GLenum>(map), bufSize, reinterpret_cast<GLushort *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_u8
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_u8> &&
    std::is_same_v<std::decay_t<typename span_u8::value_type>, u8>)
STATICINLINE void getn_polygon_stipple(span_u8 pattern)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnPolygonStipple)
                Throw(undefined_behavior(
                    "unloaded function GetnPolygonStipple"));
    }
    glGetnPolygonStipple(pattern.size(), reinterpret_cast<GLubyte *>(pattern.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<4, 5>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void getn_separable_filter(groups::separable_target target, groups::pixel_format format, groups::pixel_type type, span_byte row, span_byte column, span_byte span)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnSeparableFilter)
                Throw(undefined_behavior(
                    "unloaded function GetnSeparableFilter"));
    }
    glGetnSeparableFilter(static_cast<GLenum>(target), static_cast<GLenum>(format), static_cast<GLenum>(type), row.size(), reinterpret_cast<void *>(row.data()), column.size(), reinterpret_cast<void *>(column.data()), reinterpret_cast<void *>(span.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.5 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 5>>)
STATICINLINE void texture_barrier()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTextureBarrier)
                Throw(undefined_behavior(
                    "unloaded function TextureBarrier"));
    }
    glTextureBarrier();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_4_5
#ifdef GL_VERSION_4_6
/* Introduced in GL core 4.6 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<4, 6>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void specialize_shader(u32 shader, std::string_view const& pEntryPoint, u32 numSpecializationConstants, span_const_u32 const& pConstantIndex, span_const_u32 const& pConstantValue)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSpecializeShader)
                Throw(undefined_behavior(
                    "unloaded function SpecializeShader"));
        glIsShader(shader);
    }
    glSpecializeShader(shader, pEntryPoint.data(), numSpecializationConstants, reinterpret_cast<const GLuint *>(pConstantIndex.data()), reinterpret_cast<const GLuint *>(pConstantValue.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.6 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 6>>)
STATICINLINE void multi_draw_arrays_indirect_count(groups::primitive_type mode, ptroff indirect, GLintptr drawcount, i32 maxdrawcount, i32 stride)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiDrawArraysIndirectCount)
                Throw(undefined_behavior(
                    "unloaded function MultiDrawArraysIndirectCount"));
    }
    glMultiDrawArraysIndirectCount(static_cast<GLenum>(mode), reinterpret_cast<const void*>(indirect), drawcount, maxdrawcount, stride);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.6 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 6>>)
STATICINLINE void multi_draw_elements_indirect_count(groups::primitive_type mode, groups::draw_elements_type type, ptroff indirect, GLintptr drawcount, i32 maxdrawcount, i32 stride)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMultiDrawElementsIndirectCount)
                Throw(undefined_behavior(
                    "unloaded function MultiDrawElementsIndirectCount"));
    }
    glMultiDrawElementsIndirectCount(static_cast<GLenum>(mode), static_cast<GLenum>(type), reinterpret_cast<const void*>(indirect), drawcount, maxdrawcount, stride);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL core 4.6 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<4, 6>>)
STATICINLINE void polygon_offset_clamp(f32 factor, f32 units, f32 clamp)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPolygonOffsetClamp)
                Throw(undefined_behavior(
                    "unloaded function PolygonOffsetClamp"));
    }
    glPolygonOffsetClamp(factor, units, clamp);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_VERSION_4_6

}; // struct core

template<typename Current>
struct es
{
#ifdef GL_ES_VERSION_2_0
/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void active_texture(groups::texture_unit texture)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glActiveTexture)
                Throw(undefined_behavior(
                    "unloaded function ActiveTexture"));
    }
    glActiveTexture(static_cast<GLenum>(texture));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void attach_shader(u32 program, u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glAttachShader)
                Throw(undefined_behavior(
                    "unloaded function AttachShader"));
        glIsProgram(program);
        glIsShader(shader);
    }
    glAttachShader(program, shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void bind_attrib_location(u32 program, u32 index, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindAttribLocation)
                Throw(undefined_behavior(
                    "unloaded function BindAttribLocation"));
        glIsProgram(program);
    }
    glBindAttribLocation(program, index, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void bind_buffer(groups::buffer_target_arb target, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindBuffer)
                Throw(undefined_behavior(
                    "unloaded function BindBuffer"));
        glIsBuffer(buffer);
    }
    glBindBuffer(static_cast<GLenum>(target), buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void bind_framebuffer(groups::framebuffer_target target, u32 framebuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function BindFramebuffer"));
        glIsFramebuffer(framebuffer);
    }
    glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void bind_renderbuffer(groups::renderbuffer_target target, u32 renderbuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindRenderbuffer)
                Throw(undefined_behavior(
                    "unloaded function BindRenderbuffer"));
        glIsRenderbuffer(renderbuffer);
    }
    glBindRenderbuffer(static_cast<GLenum>(target), renderbuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void bind_texture(groups::texture_target target, u32 texture)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindTexture)
                Throw(undefined_behavior(
                    "unloaded function BindTexture"));
        glIsTexture(texture);
    }
    glBindTexture(static_cast<GLenum>(target), texture);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void blend_color(vec_4_f32 const& red)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendColor)
                Throw(undefined_behavior(
                    "unloaded function BlendColor"));
    }
    glBlendColor(red.x(), red.y(), red.z(), red.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void blend_equation(groups::blend_equation_mode_ext mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendEquation)
                Throw(undefined_behavior(
                    "unloaded function BlendEquation"));
    }
    glBlendEquation(static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void blend_equation_separate(groups::blend_equation_mode_ext modeRGB, groups::blend_equation_mode_ext modeAlpha)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendEquationSeparate)
                Throw(undefined_behavior(
                    "unloaded function BlendEquationSeparate"));
    }
    glBlendEquationSeparate(static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void blend_func(groups::blending_factor sfactor, groups::blending_factor dfactor)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendFunc)
                Throw(undefined_behavior(
                    "unloaded function BlendFunc"));
    }
    glBlendFunc(static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void blend_func_separate(groups::blending_factor sfactorRGB, groups::blending_factor dfactorRGB, groups::blending_factor sfactorAlpha, groups::blending_factor dfactorAlpha)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendFuncSeparate)
                Throw(undefined_behavior(
                    "unloaded function BlendFuncSeparate"));
    }
    glBlendFuncSeparate(static_cast<GLenum>(sfactorRGB), static_cast<GLenum>(dfactorRGB), static_cast<GLenum>(sfactorAlpha), static_cast<GLenum>(dfactorAlpha));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void buffer_data(groups::buffer_target_arb target, span_const_byte const& data, groups::buffer_usage_arb usage)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBufferData)
                Throw(undefined_behavior(
                    "unloaded function BufferData"));
    }
    glBufferData(static_cast<GLenum>(target), data.size(), reinterpret_cast<const void *>(data.data()), static_cast<GLenum>(usage));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void buffer_sub_data(groups::buffer_target_arb target, GLintptr offset, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function BufferSubData"));
    }
    glBufferSubData(static_cast<GLenum>(target), offset, data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLenum check_framebuffer_status(groups::framebuffer_target target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCheckFramebufferStatus)
                Throw(undefined_behavior(
                    "unloaded function CheckFramebufferStatus"));
    }
    auto out = glCheckFramebufferStatus(static_cast<GLenum>(target));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void clear(groups::clear_buffer_mask mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClear)
                Throw(undefined_behavior(
                    "unloaded function Clear"));
    }
    glClear(static_cast<GLenum>(mask));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void clear_color(vec_4_f32 const& red)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearColor)
                Throw(undefined_behavior(
                    "unloaded function ClearColor"));
    }
    glClearColor(red.x(), red.y(), red.z(), red.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void clear_depthf(f32 d)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearDepthf)
                Throw(undefined_behavior(
                    "unloaded function ClearDepthf"));
    }
    glClearDepthf(d);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void clear_stencil(i32 s)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearStencil)
                Throw(undefined_behavior(
                    "unloaded function ClearStencil"));
    }
    glClearStencil(s);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_4_bool
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_bool, bool, 4>)
STATICINLINE void color_mask(vec_4_bool const& red)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glColorMask)
                Throw(undefined_behavior(
                    "unloaded function ColorMask"));
    }
    glColorMask(red.x(), red.y(), red.z(), red.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void compile_shader(u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompileShader)
                Throw(undefined_behavior(
                    "unloaded function CompileShader"));
        glIsShader(shader);
    }
    glCompileShader(shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_image_2d(groups::texture_target target, i32 level, groups::internal_format internalformat, vec_2_i32 const& width, i32 border, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexImage2D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexImage2D"));
    }
    glCompressedTexImage2D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalformat), width[0], width[1], border, data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_sub_image_2d(groups::texture_target target, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& width, groups::pixel_format format, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexSubImage2D"));
    }
    glCompressedTexSubImage2D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), width[0], width[1], static_cast<GLenum>(format), data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_tex_image_2d(groups::texture_target target, i32 level, groups::internal_format internalformat, vec_2_i32 const& x, vec_2_i32 const& width, i32 border)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTexImage2D)
                Throw(undefined_behavior(
                    "unloaded function CopyTexImage2D"));
    }
    glCopyTexImage2D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalformat), x.x(), x.y(), width[0], width[1], border);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_tex_sub_image_2d(groups::texture_target target, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTexSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function CopyTexSubImage2D"));
    }
    glCopyTexSubImage2D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLuint create_program()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateProgram)
                Throw(undefined_behavior(
                    "unloaded function CreateProgram"));
    }
    auto out = glCreateProgram();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLuint create_shader(GLenum type)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateShader)
                Throw(undefined_behavior(
                    "unloaded function CreateShader"));
    }
    auto out = glCreateShader(type);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void cull_face(groups::cull_face_mode mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCullFace)
                Throw(undefined_behavior(
                    "unloaded function CullFace"));
    }
    glCullFace(static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_buffers(span_const_u32 const& buffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteBuffers)
                Throw(undefined_behavior(
                    "unloaded function DeleteBuffers"));
    }
    glDeleteBuffers(buffers.size(), reinterpret_cast<const GLuint *>(buffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_framebuffers(span_const_u32 const& framebuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteFramebuffers)
                Throw(undefined_behavior(
                    "unloaded function DeleteFramebuffers"));
    }
    glDeleteFramebuffers(framebuffers.size(), reinterpret_cast<const GLuint *>(framebuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void delete_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteProgram)
                Throw(undefined_behavior(
                    "unloaded function DeleteProgram"));
        glIsProgram(program);
    }
    glDeleteProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_renderbuffers(span_const_u32 const& renderbuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteRenderbuffers)
                Throw(undefined_behavior(
                    "unloaded function DeleteRenderbuffers"));
    }
    glDeleteRenderbuffers(renderbuffers.size(), reinterpret_cast<const GLuint *>(renderbuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void delete_shader(u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteShader)
                Throw(undefined_behavior(
                    "unloaded function DeleteShader"));
        glIsShader(shader);
    }
    glDeleteShader(shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_textures(span_const_u32 const& textures)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteTextures)
                Throw(undefined_behavior(
                    "unloaded function DeleteTextures"));
    }
    glDeleteTextures(textures.size(), reinterpret_cast<const GLuint *>(textures.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void depth_func(groups::depth_function func)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthFunc)
                Throw(undefined_behavior(
                    "unloaded function DepthFunc"));
    }
    glDepthFunc(static_cast<GLenum>(func));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void depth_mask(bool flag)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthMask)
                Throw(undefined_behavior(
                    "unloaded function DepthMask"));
    }
    glDepthMask(flag);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void depth_rangef(f32 n, f32 f)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDepthRangef)
                Throw(undefined_behavior(
                    "unloaded function DepthRangef"));
    }
    glDepthRangef(n, f);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void detach_shader(u32 program, u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDetachShader)
                Throw(undefined_behavior(
                    "unloaded function DetachShader"));
        glIsProgram(program);
        glIsShader(shader);
    }
    glDetachShader(program, shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void disable(groups::enable_cap cap)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDisable)
                Throw(undefined_behavior(
                    "unloaded function Disable"));
    }
    glDisable(static_cast<GLenum>(cap));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void disable_vertex_attrib_array(u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDisableVertexAttribArray)
                Throw(undefined_behavior(
                    "unloaded function DisableVertexAttribArray"));
    }
    glDisableVertexAttribArray(index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void draw_arrays(groups::primitive_type mode, i32 first, i32 count)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawArrays)
                Throw(undefined_behavior(
                    "unloaded function DrawArrays"));
    }
    glDrawArrays(static_cast<GLenum>(mode), first, count);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void draw_elements(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElements)
                Throw(undefined_behavior(
                    "unloaded function DrawElements"));
    }
    glDrawElements(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void enable(groups::enable_cap cap)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEnable)
                Throw(undefined_behavior(
                    "unloaded function Enable"));
    }
    glEnable(static_cast<GLenum>(cap));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void enable_vertex_attrib_array(u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEnableVertexAttribArray)
                Throw(undefined_behavior(
                    "unloaded function EnableVertexAttribArray"));
    }
    glEnableVertexAttribArray(index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void finish()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFinish)
                Throw(undefined_behavior(
                    "unloaded function Finish"));
    }
    glFinish();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void flush()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFlush)
                Throw(undefined_behavior(
                    "unloaded function Flush"));
    }
    glFlush();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void framebuffer_renderbuffer(groups::framebuffer_target target, groups::framebuffer_attachment attachment, groups::renderbuffer_target renderbuffertarget, u32 renderbuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferRenderbuffer)
                Throw(undefined_behavior(
                    "unloaded function FramebufferRenderbuffer"));
        glIsRenderbuffer(renderbuffer);
    }
    glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(attachment), static_cast<GLenum>(renderbuffertarget), renderbuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void framebuffer_texture_2d(groups::framebuffer_target target, groups::framebuffer_attachment attachment, groups::texture_target textarget, u32 texture, i32 level)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferTexture2D)
                Throw(undefined_behavior(
                    "unloaded function FramebufferTexture2D"));
        glIsTexture(texture);
    }
    glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(attachment), static_cast<GLenum>(textarget), texture, level);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void front_face(groups::front_face_direction mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFrontFace)
                Throw(undefined_behavior(
                    "unloaded function FrontFace"));
    }
    glFrontFace(static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_buffers(span_u32 buffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenBuffers)
                Throw(undefined_behavior(
                    "unloaded function GenBuffers"));
    }
    glGenBuffers(buffers.size(), reinterpret_cast<GLuint *>(buffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_framebuffers(span_u32 framebuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenFramebuffers)
                Throw(undefined_behavior(
                    "unloaded function GenFramebuffers"));
    }
    glGenFramebuffers(framebuffers.size(), reinterpret_cast<GLuint *>(framebuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_renderbuffers(span_u32 renderbuffers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenRenderbuffers)
                Throw(undefined_behavior(
                    "unloaded function GenRenderbuffers"));
    }
    glGenRenderbuffers(renderbuffers.size(), reinterpret_cast<GLuint *>(renderbuffers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_textures(span_u32 textures)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenTextures)
                Throw(undefined_behavior(
                    "unloaded function GenTextures"));
    }
    glGenTextures(textures.size(), reinterpret_cast<GLuint *>(textures.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void generate_mipmap(GLenum target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenerateMipmap)
                Throw(undefined_behavior(
                    "unloaded function GenerateMipmap"));
    }
    glGenerateMipmap(target);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_attrib(u32 program, u32 index, i32& length, i32& size, GLenum& type, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveAttrib)
                Throw(undefined_behavior(
                    "unloaded function GetActiveAttrib"));
        glIsProgram(program);
    }
    glGetActiveAttrib(program, index, name.size(), &length, &size, &type, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_uniform(u32 program, u32 index, i32& length, i32& size, GLenum& type, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniform)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniform"));
        glIsProgram(program);
    }
    glGetActiveUniform(program, index, name.size(), &length, &size, &type, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_attached_shaders(u32 program, i32& count, span_u32 shaders)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetAttachedShaders)
                Throw(undefined_behavior(
                    "unloaded function GetAttachedShaders"));
        glIsProgram(program);
    }
    glGetAttachedShaders(program, shaders.size(), &count, reinterpret_cast<GLuint *>(shaders.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLint get_attrib_location(u32 program, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetAttribLocation)
                Throw(undefined_behavior(
                    "unloaded function GetAttribLocation"));
        glIsProgram(program);
    }
    auto out = glGetAttribLocation(program, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    class span_bool
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_bool> &&
    std::is_same_v<std::decay_t<typename span_bool::value_type>, bool>)
STATICINLINE void get_booleanv(groups::get_prop pname, span_bool data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBooleanv)
                Throw(undefined_behavior(
                    "unloaded function GetBooleanv"));
    }
    glGetBooleanv(static_cast<GLenum>(pname), reinterpret_cast<GLboolean *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_buffer_parameter(groups::buffer_target_arb target, groups::buffer_prop_arb pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetBufferParameteriv"));
    }
    glGetBufferParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLenum get_error()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetError)
                Throw(undefined_behavior(
                    "unloaded function GetError"));
    }
    auto out = glGetError();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_floatv(groups::get_prop pname, span_f32 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFloatv)
                Throw(undefined_behavior(
                    "unloaded function GetFloatv"));
    }
    glGetFloatv(static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_framebuffer_attachment_parameter(groups::framebuffer_target target, groups::framebuffer_attachment attachment, groups::framebuffer_attachment_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFramebufferAttachmentParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetFramebufferAttachmentParameteriv"));
    }
    glGetFramebufferAttachmentParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(attachment), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_integerv(groups::get_prop pname, span_i32 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetIntegerv)
                Throw(undefined_behavior(
                    "unloaded function GetIntegerv"));
    }
    glGetIntegerv(static_cast<GLenum>(pname), reinterpret_cast<GLint *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_program_info_log(u32 program, i32& length, span_GLchar infoLog)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramInfoLog)
                Throw(undefined_behavior(
                    "unloaded function GetProgramInfoLog"));
        glIsProgram(program);
    }
    glGetProgramInfoLog(program, infoLog.size(), &length, infoLog.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_programiv(u32 program, groups::program_property_arb pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramiv"));
        glIsProgram(program);
    }
    glGetProgramiv(program, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_renderbuffer_parameter(groups::renderbuffer_target target, groups::renderbuffer_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetRenderbufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetRenderbufferParameteriv"));
    }
    glGetRenderbufferParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_shader_info_log(u32 shader, i32& length, span_GLchar infoLog)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetShaderInfoLog)
                Throw(undefined_behavior(
                    "unloaded function GetShaderInfoLog"));
        glIsShader(shader);
    }
    glGetShaderInfoLog(shader, infoLog.size(), &length, infoLog.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_shader_precision_format(groups::shader_type shadertype, groups::precision_type precisiontype, span_i32 range, i32& precision)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetShaderPrecisionFormat)
                Throw(undefined_behavior(
                    "unloaded function GetShaderPrecisionFormat"));
    }
    glGetShaderPrecisionFormat(static_cast<GLenum>(shadertype), static_cast<GLenum>(precisiontype), reinterpret_cast<GLint *>(range.data()), &precision);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_shader_source(u32 shader, i32& length, span_GLchar source)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetShaderSource)
                Throw(undefined_behavior(
                    "unloaded function GetShaderSource"));
        glIsShader(shader);
    }
    glGetShaderSource(shader, source.size(), &length, source.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_shaderiv(u32 shader, groups::shader_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetShaderiv)
                Throw(undefined_behavior(
                    "unloaded function GetShaderiv"));
        glIsShader(shader);
    }
    glGetShaderiv(shader, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE const GLubyte * get_string(groups::string_name name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetString)
                Throw(undefined_behavior(
                    "unloaded function GetString"));
    }
    auto out = glGetString(static_cast<GLenum>(name));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_tex_parameter(groups::texture_target target, groups::get_texture_parameter pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexParameterfv)
                Throw(undefined_behavior(
                    "unloaded function GetTexParameterfv"));
    }
    glGetTexParameterfv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_tex_parameter(groups::texture_target target, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetTexParameteriv"));
    }
    glGetTexParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLint get_uniform_location(u32 program, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformLocation)
                Throw(undefined_behavior(
                    "unloaded function GetUniformLocation"));
        glIsProgram(program);
    }
    auto out = glGetUniformLocation(program, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_uniformfv(u32 program, i32 location, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformfv)
                Throw(undefined_behavior(
                    "unloaded function GetUniformfv"));
        glIsProgram(program);
    }
    glGetUniformfv(program, location, reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_uniformiv(u32 program, i32 location, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformiv)
                Throw(undefined_behavior(
                    "unloaded function GetUniformiv"));
        glIsProgram(program);
    }
    glGetUniformiv(program, location, reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_vertex_attrib_pointerv(u32 index, groups::vertex_attrib_pointer_property_arb pname, span_byte pointer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribPointerv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribPointerv"));
    }
    glGetVertexAttribPointerv(index, static_cast<GLenum>(pname), reinterpret_cast<void **>(pointer.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_vertex_attribfv(u32 index, groups::vertex_attrib_property_arb pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribfv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribfv"));
    }
    glGetVertexAttribfv(index, static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_vertex_attribiv(u32 index, groups::vertex_attrib_property_arb pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribiv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribiv"));
    }
    glGetVertexAttribiv(index, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void hint(groups::hint_target target, groups::hint_mode mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glHint)
                Throw(undefined_behavior(
                    "unloaded function Hint"));
    }
    glHint(static_cast<GLenum>(target), static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_buffer(u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsBuffer)
                Throw(undefined_behavior(
                    "unloaded function IsBuffer"));
        glIsBuffer(buffer);
    }
    auto out = glIsBuffer(buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_enabled(groups::enable_cap cap)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsEnabled)
                Throw(undefined_behavior(
                    "unloaded function IsEnabled"));
    }
    auto out = glIsEnabled(static_cast<GLenum>(cap));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_framebuffer(u32 framebuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function IsFramebuffer"));
        glIsFramebuffer(framebuffer);
    }
    auto out = glIsFramebuffer(framebuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsProgram)
                Throw(undefined_behavior(
                    "unloaded function IsProgram"));
        glIsProgram(program);
    }
    auto out = glIsProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_renderbuffer(u32 renderbuffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsRenderbuffer)
                Throw(undefined_behavior(
                    "unloaded function IsRenderbuffer"));
        glIsRenderbuffer(renderbuffer);
    }
    auto out = glIsRenderbuffer(renderbuffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_shader(u32 shader)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsShader)
                Throw(undefined_behavior(
                    "unloaded function IsShader"));
        glIsShader(shader);
    }
    auto out = glIsShader(shader);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE GLboolean is_texture(u32 texture)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsTexture)
                Throw(undefined_behavior(
                    "unloaded function IsTexture"));
        glIsTexture(texture);
    }
    auto out = glIsTexture(texture);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void line_width(f32 width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glLineWidth)
                Throw(undefined_behavior(
                    "unloaded function LineWidth"));
    }
    glLineWidth(width);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void link_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glLinkProgram)
                Throw(undefined_behavior(
                    "unloaded function LinkProgram"));
        glIsProgram(program);
    }
    glLinkProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void pixel_storei(groups::pixel_store_parameter pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPixelStorei)
                Throw(undefined_behavior(
                    "unloaded function PixelStorei"));
    }
    glPixelStorei(static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void polygon_offset(f32 factor, f32 units)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPolygonOffset)
                Throw(undefined_behavior(
                    "unloaded function PolygonOffset"));
    }
    glPolygonOffset(factor, units);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void read_pixels(vec_2_i32 const& x, vec_2_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_byte pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glReadPixels)
                Throw(undefined_behavior(
                    "unloaded function ReadPixels"));
    }
    glReadPixels(x.x(), x.y(), width[0], width[1], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void release_shader_compiler()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glReleaseShaderCompiler)
                Throw(undefined_behavior(
                    "unloaded function ReleaseShaderCompiler"));
    }
    glReleaseShaderCompiler();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void renderbuffer_storage(groups::renderbuffer_target target, groups::internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glRenderbufferStorage)
                Throw(undefined_behavior(
                    "unloaded function RenderbufferStorage"));
    }
    glRenderbufferStorage(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void sample_coverage(f32 value, bool invert)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSampleCoverage)
                Throw(undefined_behavior(
                    "unloaded function SampleCoverage"));
    }
    glSampleCoverage(value, invert);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void scissor(vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glScissor)
                Throw(undefined_behavior(
                    "unloaded function Scissor"));
    }
    glScissor(x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_byte,
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void shader_binary(span_const_u32 const& shaders, groups::shader_binary_format binaryFormat, span_const_byte const& binary, i32 length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glShaderBinary)
                Throw(undefined_behavior(
                    "unloaded function ShaderBinary"));
    }
    glShaderBinary(shaders.size(), reinterpret_cast<const GLuint *>(shaders.data()), static_cast<GLenum>(binaryFormat), reinterpret_cast<const void *>(binary.data()), length);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void shader_source(u32 shader, std::vector<std::string_view> string, span_const_i32 const& length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glShaderSource)
                Throw(undefined_behavior(
                    "unloaded function ShaderSource"));
        glIsShader(shader);
    }
    auto [string_lens, string_cstr, string_store] = detail::transform_strings(string);
    glShaderSource(shader, string_cstr.size(), string_cstr.data(), reinterpret_cast<const GLint *>(length.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_func(groups::stencil_function func, i32 ref, u32 mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilFunc)
                Throw(undefined_behavior(
                    "unloaded function StencilFunc"));
    }
    glStencilFunc(static_cast<GLenum>(func), ref, mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_func_separate(groups::stencil_face_direction face, groups::stencil_function func, i32 ref, u32 mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilFuncSeparate)
                Throw(undefined_behavior(
                    "unloaded function StencilFuncSeparate"));
    }
    glStencilFuncSeparate(static_cast<GLenum>(face), static_cast<GLenum>(func), ref, mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_mask(u32 mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilMask)
                Throw(undefined_behavior(
                    "unloaded function StencilMask"));
    }
    glStencilMask(mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_mask_separate(groups::stencil_face_direction face, u32 mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilMaskSeparate)
                Throw(undefined_behavior(
                    "unloaded function StencilMaskSeparate"));
    }
    glStencilMaskSeparate(static_cast<GLenum>(face), mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_op(groups::stencil_op fail, groups::stencil_op zfail, groups::stencil_op zpass)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilOp)
                Throw(undefined_behavior(
                    "unloaded function StencilOp"));
    }
    glStencilOp(static_cast<GLenum>(fail), static_cast<GLenum>(zfail), static_cast<GLenum>(zpass));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void stencil_op_separate(groups::stencil_face_direction face, groups::stencil_op sfail, groups::stencil_op dpfail, groups::stencil_op dppass)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glStencilOpSeparate)
                Throw(undefined_behavior(
                    "unloaded function StencilOpSeparate"));
    }
    glStencilOpSeparate(static_cast<GLenum>(face), static_cast<GLenum>(sfail), static_cast<GLenum>(dpfail), static_cast<GLenum>(dppass));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_image_2d(groups::texture_target target, i32 level, i32 internalformat, vec_2_i32 const& width, i32 border, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexImage2D)
                Throw(undefined_behavior(
                    "unloaded function TexImage2D"));
    }
    glTexImage2D(static_cast<GLenum>(target), level, internalformat, width[0], width[1], border, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void tex_parameter(groups::texture_target target, groups::texture_parameter_name pname, f32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameterf)
                Throw(undefined_behavior(
                    "unloaded function TexParameterf"));
    }
    glTexParameterf(static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void tex_parameter(groups::texture_target target, groups::texture_parameter_name pname, span_const_f32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameterfv)
                Throw(undefined_behavior(
                    "unloaded function TexParameterfv"));
    }
    glTexParameterfv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<const GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void tex_parameter(groups::texture_target target, groups::texture_parameter_name pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameteri)
                Throw(undefined_behavior(
                    "unloaded function TexParameteri"));
    }
    glTexParameteri(static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void tex_parameter(groups::texture_target target, groups::texture_parameter_name pname, span_const_i32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameteriv)
                Throw(undefined_behavior(
                    "unloaded function TexParameteriv"));
    }
    glTexParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_sub_image_2d(groups::texture_target target, i32 level, vec_2_i32 const& xoffset, vec_2_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexSubImage2D)
                Throw(undefined_behavior(
                    "unloaded function TexSubImage2D"));
    }
    glTexSubImage2D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), width[0], width[1], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void uniform(i32 location, f32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1f)
                Throw(undefined_behavior(
                    "unloaded function Uniform1f"));
    }
    glUniform1f(location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void uniform(i32 location, i32 count, span_const_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1fv)
                Throw(undefined_behavior(
                    "unloaded function Uniform1fv"));
    }
    glUniform1fv(location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void uniform(i32 location, i32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1i)
                Throw(undefined_behavior(
                    "unloaded function Uniform1i"));
    }
    glUniform1i(location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void uniform(i32 location, i32 count, span_const_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1iv)
                Throw(undefined_behavior(
                    "unloaded function Uniform1iv"));
    }
    glUniform1iv(location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_f32, f32, 2>)
STATICINLINE void uniform(i32 location, vec_2_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2f)
                Throw(undefined_behavior(
                    "unloaded function Uniform2f"));
    }
    glUniform2f(location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_2_f32> &&
    semantic::concepts::Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2fv)
                Throw(undefined_behavior(
                    "unloaded function Uniform2fv"));
    }
    glUniform2fv(location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void uniform(i32 location, vec_2_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2i)
                Throw(undefined_behavior(
                    "unloaded function Uniform2i"));
    }
    glUniform2i(location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_2_i32> &&
    semantic::concepts::Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2iv)
                Throw(undefined_behavior(
                    "unloaded function Uniform2iv"));
    }
    glUniform2iv(location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_3_f32, f32, 3>)
STATICINLINE void uniform(i32 location, vec_3_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3f)
                Throw(undefined_behavior(
                    "unloaded function Uniform3f"));
    }
    glUniform3f(location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_3_f32> &&
    semantic::concepts::Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3fv)
                Throw(undefined_behavior(
                    "unloaded function Uniform3fv"));
    }
    glUniform3fv(location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void uniform(i32 location, vec_3_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3i)
                Throw(undefined_behavior(
                    "unloaded function Uniform3i"));
    }
    glUniform3i(location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_3_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_3_i32> &&
    semantic::concepts::Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3iv)
                Throw(undefined_behavior(
                    "unloaded function Uniform3iv"));
    }
    glUniform3iv(location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void uniform(i32 location, vec_4_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4f)
                Throw(undefined_behavior(
                    "unloaded function Uniform4f"));
    }
    glUniform4f(location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_f32> &&
    semantic::concepts::Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4fv)
                Throw(undefined_behavior(
                    "unloaded function Uniform4fv"));
    }
    glUniform4fv(location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_4_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_i32, i32, 4>)
STATICINLINE void uniform(i32 location, vec_4_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4i)
                Throw(undefined_behavior(
                    "unloaded function Uniform4i"));
    }
    glUniform4i(location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_4_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_i32> &&
    semantic::concepts::Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4iv)
                Throw(undefined_behavior(
                    "unloaded function Uniform4iv"));
    }
    glUniform4iv(location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_mat_2x2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_mat_2x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2fv"));
    }
    glUniformMatrix2fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_mat_3x3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_mat_3x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3fv"));
    }
    glUniformMatrix3fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_mat_4x4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_mat_4x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4fv"));
    }
    glUniformMatrix4fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void use_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUseProgram)
                Throw(undefined_behavior(
                    "unloaded function UseProgram"));
        glIsProgram(program);
    }
    glUseProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void validate_program(u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glValidateProgram)
                Throw(undefined_behavior(
                    "unloaded function ValidateProgram"));
        glIsProgram(program);
    }
    glValidateProgram(program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<2, 0>>)
STATICINLINE void vertex_attrib1f(u32 index, f32 x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib1f)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib1f"));
    }
    glVertexAttrib1f(index, x);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void vertex_attrib1fv(u32 index, span_const_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib1fv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib1fv"));
    }
    glVertexAttrib1fv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_f32, f32, 2>)
STATICINLINE void vertex_attrib2f(u32 index, vec_2_f32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib2f)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib2f"));
    }
    glVertexAttrib2f(index, x.x(), x.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_2_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_2_f32> &&
    semantic::concepts::Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
STATICINLINE void vertex_attrib2fv(u32 index, span_const_vec_2_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib2fv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib2fv"));
    }
    glVertexAttrib2fv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_3_f32, f32, 3>)
STATICINLINE void vertex_attrib3f(u32 index, vec_3_f32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib3f)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib3f"));
    }
    glVertexAttrib3f(index, x.x(), x.y(), x.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_3_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_3_f32> &&
    semantic::concepts::Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
STATICINLINE void vertex_attrib3fv(u32 index, span_const_vec_3_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib3fv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib3fv"));
    }
    glVertexAttrib3fv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void vertex_attrib4f(u32 index, vec_4_f32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4f)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4f"));
    }
    glVertexAttrib4f(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_vec_4_f32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_vec_4_f32> &&
    semantic::concepts::Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
STATICINLINE void vertex_attrib4fv(u32 index, span_const_vec_4_f32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttrib4fv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttrib4fv"));
    }
    glVertexAttrib4fv(index, reinterpret_cast<const GLfloat *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void vertex_attrib_pointer(u32 index, i32 size, groups::vertex_attrib_pointer_type type, bool normalized, i32 stride, span_const_byte const& pointer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribPointer)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribPointer"));
    }
    glVertexAttribPointer(index, size, static_cast<GLenum>(type), normalized, stride, reinterpret_cast<const void *>(pointer.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 2.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<2, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void viewport(vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glViewport)
                Throw(undefined_behavior(
                    "unloaded function Viewport"));
    }
    glViewport(x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_ES_VERSION_2_0
#ifdef GL_ES_VERSION_3_0
/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void begin_query(groups::query_target target, u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBeginQuery)
                Throw(undefined_behavior(
                    "unloaded function BeginQuery"));
    }
    glBeginQuery(static_cast<GLenum>(target), id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void begin_transform_feedback(groups::primitive_type primitiveMode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBeginTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function BeginTransformFeedback"));
    }
    glBeginTransformFeedback(static_cast<GLenum>(primitiveMode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_buffer_base(groups::buffer_target_arb target, u32 index, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindBufferBase)
                Throw(undefined_behavior(
                    "unloaded function BindBufferBase"));
        glIsBuffer(buffer);
    }
    glBindBufferBase(static_cast<GLenum>(target), index, buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_buffer_range(groups::buffer_target_arb target, u32 index, u32 buffer, GLintptr offset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindBufferRange)
                Throw(undefined_behavior(
                    "unloaded function BindBufferRange"));
        glIsBuffer(buffer);
    }
    glBindBufferRange(static_cast<GLenum>(target), index, buffer, offset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_sampler(u32 unit, u32 sampler)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindSampler)
                Throw(undefined_behavior(
                    "unloaded function BindSampler"));
        glIsSampler(sampler);
    }
    glBindSampler(unit, sampler);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_transform_feedback(groups::bind_transform_feedback_target target, u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function BindTransformFeedback"));
    }
    glBindTransformFeedback(static_cast<GLenum>(target), id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void bind_vertex_array(u32 array)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindVertexArray)
                Throw(undefined_behavior(
                    "unloaded function BindVertexArray"));
        glIsVertexArray(array);
    }
    glBindVertexArray(array);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void blit_framebuffer(i32 srcX0, i32 srcY0, i32 srcX1, i32 srcY1, i32 dstX0, i32 dstY0, i32 dstX1, i32 dstY1, groups::clear_buffer_mask mask, groups::blit_framebuffer_filter filter)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlitFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function BlitFramebuffer"));
    }
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, static_cast<GLenum>(mask), static_cast<GLenum>(filter));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void clear_bufferfi(groups::buffer buffer, i32 drawbuffer, f32 depth, i32 stencil)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferfi)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferfi"));
    }
    glClearBufferfi(static_cast<GLenum>(buffer), drawbuffer, depth, stencil);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void clear_bufferfv(groups::buffer buffer, i32 drawbuffer, span_const_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferfv)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferfv"));
    }
    glClearBufferfv(static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void clear_bufferiv(groups::buffer buffer, i32 drawbuffer, span_const_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferiv)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferiv"));
    }
    glClearBufferiv(static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void clear_bufferuiv(groups::buffer buffer, i32 drawbuffer, span_const_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClearBufferuiv)
                Throw(undefined_behavior(
                    "unloaded function ClearBufferuiv"));
    }
    glClearBufferuiv(static_cast<GLenum>(buffer), drawbuffer, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLenum client_wait_sync(GLsync sync, groups::sync_object_mask flags, u64 timeout)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glClientWaitSync)
                Throw(undefined_behavior(
                    "unloaded function ClientWaitSync"));
    }
    auto out = glClientWaitSync(sync, static_cast<GLenum>(flags), timeout);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_image_3d(groups::texture_target target, i32 level, groups::internal_format internalformat, vec_3_i32 const& width, i32 border, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexImage3D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexImage3D"));
    }
    glCompressedTexImage3D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalformat), width[0], width[1], width[2], border, data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void compressed_tex_sub_image_3d(groups::texture_target target, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, groups::pixel_format format, span_const_byte const& data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCompressedTexSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function CompressedTexSubImage3D"));
    }
    glCompressedTexSubImage3D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], static_cast<GLenum>(format), data.size(), reinterpret_cast<const void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void copy_buffer_sub_data(groups::copy_buffer_sub_data_target readTarget, groups::copy_buffer_sub_data_target writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyBufferSubData)
                Throw(undefined_behavior(
                    "unloaded function CopyBufferSubData"));
    }
    glCopyBufferSubData(static_cast<GLenum>(readTarget), static_cast<GLenum>(writeTarget), readOffset, writeOffset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_2_i32,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void copy_tex_sub_image_3d(groups::texture_target target, i32 level, vec_3_i32 const& xoffset, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyTexSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function CopyTexSubImage3D"));
    }
    glCopyTexSubImage3D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), xoffset.z(), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_queries(span_const_u32 const& ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteQueries)
                Throw(undefined_behavior(
                    "unloaded function DeleteQueries"));
    }
    glDeleteQueries(ids.size(), reinterpret_cast<const GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_samplers(span_const_u32 const& samplers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteSamplers)
                Throw(undefined_behavior(
                    "unloaded function DeleteSamplers"));
    }
    glDeleteSamplers(samplers.size(), reinterpret_cast<const GLuint *>(samplers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void delete_sync(GLsync sync)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteSync)
                Throw(undefined_behavior(
                    "unloaded function DeleteSync"));
    }
    glDeleteSync(sync);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_transform_feedbacks(span_const_u32 const& ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteTransformFeedbacks)
                Throw(undefined_behavior(
                    "unloaded function DeleteTransformFeedbacks"));
    }
    glDeleteTransformFeedbacks(ids.size(), reinterpret_cast<const GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_vertex_arrays(span_const_u32 const& arrays)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteVertexArrays)
                Throw(undefined_behavior(
                    "unloaded function DeleteVertexArrays"));
    }
    glDeleteVertexArrays(arrays.size(), reinterpret_cast<const GLuint *>(arrays.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void draw_arrays_instanced(groups::primitive_type mode, i32 first, i32 count, i32 instancecount)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawArraysInstanced)
                Throw(undefined_behavior(
                    "unloaded function DrawArraysInstanced"));
    }
    glDrawArraysInstanced(static_cast<GLenum>(mode), first, count, instancecount);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_GLenum
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum>)
STATICINLINE void draw_buffers(span_const_GLenum const& bufs)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawBuffers)
                Throw(undefined_behavior(
                    "unloaded function DrawBuffers"));
    }
    glDrawBuffers(bufs.size(), reinterpret_cast<const GLenum *>(bufs.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void draw_elements_instanced(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices, i32 instancecount)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsInstanced)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsInstanced"));
    }
    glDrawElementsInstanced(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), instancecount);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void draw_range_elements(groups::primitive_type mode, u32 start, u32 end, i32 count, groups::draw_elements_type type, span_const_byte const& indices)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawRangeElements)
                Throw(undefined_behavior(
                    "unloaded function DrawRangeElements"));
    }
    glDrawRangeElements(static_cast<GLenum>(mode), start, end, count, static_cast<GLenum>(type), reinterpret_cast<const void *>(indices.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void end_query(groups::query_target target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEndQuery)
                Throw(undefined_behavior(
                    "unloaded function EndQuery"));
    }
    glEndQuery(static_cast<GLenum>(target));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void end_transform_feedback()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEndTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function EndTransformFeedback"));
    }
    glEndTransformFeedback();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLsync fence_sync(groups::sync_condition condition, groups::sync_behavior_flags flags)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFenceSync)
                Throw(undefined_behavior(
                    "unloaded function FenceSync"));
    }
    auto out = glFenceSync(static_cast<GLenum>(condition), static_cast<GLenum>(flags));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void flush_mapped_buffer_range(groups::buffer_target_arb target, GLintptr offset, GLsizeiptr length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFlushMappedBufferRange)
                Throw(undefined_behavior(
                    "unloaded function FlushMappedBufferRange"));
    }
    glFlushMappedBufferRange(static_cast<GLenum>(target), offset, length);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void framebuffer_texture_layer(groups::framebuffer_target target, groups::framebuffer_attachment attachment, u32 texture, i32 level, i32 layer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferTextureLayer)
                Throw(undefined_behavior(
                    "unloaded function FramebufferTextureLayer"));
        glIsTexture(texture);
    }
    glFramebufferTextureLayer(static_cast<GLenum>(target), static_cast<GLenum>(attachment), texture, level, layer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_queries(span_u32 ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenQueries)
                Throw(undefined_behavior(
                    "unloaded function GenQueries"));
    }
    glGenQueries(ids.size(), reinterpret_cast<GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_samplers(span_u32 samplers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenSamplers)
                Throw(undefined_behavior(
                    "unloaded function GenSamplers"));
    }
    glGenSamplers(samplers.size(), reinterpret_cast<GLuint *>(samplers.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_transform_feedbacks(span_u32 ids)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenTransformFeedbacks)
                Throw(undefined_behavior(
                    "unloaded function GenTransformFeedbacks"));
    }
    glGenTransformFeedbacks(ids.size(), reinterpret_cast<GLuint *>(ids.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_vertex_arrays(span_u32 arrays)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenVertexArrays)
                Throw(undefined_behavior(
                    "unloaded function GenVertexArrays"));
    }
    glGenVertexArrays(arrays.size(), reinterpret_cast<GLuint *>(arrays.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_active_uniform_block_name(u32 program, u32 uniformBlockIndex, i32& length, span_GLchar uniformBlockName)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniformBlockName)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniformBlockName"));
        glIsProgram(program);
    }
    glGetActiveUniformBlockName(program, uniformBlockIndex, uniformBlockName.size(), &length, uniformBlockName.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_active_uniform_blockiv(u32 program, u32 uniformBlockIndex, groups::uniform_block_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniformBlockiv)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniformBlockiv"));
        glIsProgram(program);
    }
    glGetActiveUniformBlockiv(program, uniformBlockIndex, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_u32,
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_active_uniformsiv(u32 program, span_const_u32 const& uniformIndices, groups::uniform_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetActiveUniformsiv)
                Throw(undefined_behavior(
                    "unloaded function GetActiveUniformsiv"));
        glIsProgram(program);
    }
    glGetActiveUniformsiv(program, uniformIndices.size(), reinterpret_cast<const GLuint *>(uniformIndices.data()), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_buffer_parameteri64v(groups::buffer_target_arb target, groups::buffer_prop_arb pname, span_i64 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBufferParameteri64v)
                Throw(undefined_behavior(
                    "unloaded function GetBufferParameteri64v"));
    }
    glGetBufferParameteri64v(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint64 *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_buffer_pointerv(groups::buffer_target_arb target, groups::buffer_pointer_name_arb pname, span_byte params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBufferPointerv)
                Throw(undefined_behavior(
                    "unloaded function GetBufferPointerv"));
    }
    glGetBufferPointerv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<void **>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLint get_frag_data_location(u32 program, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFragDataLocation)
                Throw(undefined_behavior(
                    "unloaded function GetFragDataLocation"));
        glIsProgram(program);
    }
    auto out = glGetFragDataLocation(program, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_integer64i_v(groups::get_prop target, u32 index, span_i64 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetInteger64i_v)
                Throw(undefined_behavior(
                    "unloaded function GetInteger64i_v"));
    }
    glGetInteger64i_v(static_cast<GLenum>(target), index, reinterpret_cast<GLint64 *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_i64
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<std::decay_t<typename span_i64::value_type>, i64>)
STATICINLINE void get_integer64v(groups::get_prop pname, span_i64 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetInteger64v)
                Throw(undefined_behavior(
                    "unloaded function GetInteger64v"));
    }
    glGetInteger64v(static_cast<GLenum>(pname), reinterpret_cast<GLint64 *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_integeri_v(groups::get_prop target, u32 index, span_i32 data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetIntegeri_v)
                Throw(undefined_behavior(
                    "unloaded function GetIntegeri_v"));
    }
    glGetIntegeri_v(static_cast<GLenum>(target), index, reinterpret_cast<GLint *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_internalformativ(groups::texture_target target, groups::internal_format internalformat, groups::internal_format_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetInternalformativ)
                Throw(undefined_behavior(
                    "unloaded function GetInternalformativ"));
    }
    glGetInternalformativ(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), static_cast<GLenum>(pname), params.size(), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_program_binary(u32 program, i32& length, GLenum& binaryFormat, span_byte binary)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramBinary)
                Throw(undefined_behavior(
                    "unloaded function GetProgramBinary"));
        glIsProgram(program);
    }
    glGetProgramBinary(program, binary.size(), &length, &binaryFormat, reinterpret_cast<void *>(binary.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_query_objectuiv(u32 id, groups::query_object_parameter_name pname, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryObjectuiv)
                Throw(undefined_behavior(
                    "unloaded function GetQueryObjectuiv"));
    }
    glGetQueryObjectuiv(id, static_cast<GLenum>(pname), reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_queryiv(groups::query_target target, groups::query_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetQueryiv)
                Throw(undefined_behavior(
                    "unloaded function GetQueryiv"));
    }
    glGetQueryiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_sampler_parameter(u32 sampler, groups::sampler_parameter_f pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSamplerParameterfv)
                Throw(undefined_behavior(
                    "unloaded function GetSamplerParameterfv"));
        glIsSampler(sampler);
    }
    glGetSamplerParameterfv(sampler, static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_sampler_parameter(u32 sampler, groups::sampler_parameter_i pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSamplerParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetSamplerParameteriv"));
        glIsSampler(sampler);
    }
    glGetSamplerParameteriv(sampler, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE const GLubyte * get_stringi(groups::string_name name, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetStringi)
                Throw(undefined_behavior(
                    "unloaded function GetStringi"));
    }
    auto out = glGetStringi(static_cast<GLenum>(name), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_synciv(GLsync sync, groups::sync_parameter_name pname, i32& length, span_i32 values)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSynciv)
                Throw(undefined_behavior(
                    "unloaded function GetSynciv"));
    }
    glGetSynciv(sync, static_cast<GLenum>(pname), values.size(), &length, reinterpret_cast<GLint *>(values.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_transform_feedback_varying(u32 program, u32 index, i32& length, i32& size, GLenum& type, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTransformFeedbackVarying)
                Throw(undefined_behavior(
                    "unloaded function GetTransformFeedbackVarying"));
        glIsProgram(program);
    }
    glGetTransformFeedbackVarying(program, index, name.size(), &length, &size, &type, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLuint get_uniform_block_index(u32 program, std::string_view const& uniformBlockName)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformBlockIndex)
                Throw(undefined_behavior(
                    "unloaded function GetUniformBlockIndex"));
        glIsProgram(program);
    }
    auto out = glGetUniformBlockIndex(program, uniformBlockName.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_uniform_indices(u32 program, i32 uniformCount, std::vector<std::string_view> uniformNames, span_u32 uniformIndices)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformIndices)
                Throw(undefined_behavior(
                    "unloaded function GetUniformIndices"));
        glIsProgram(program);
    }
    auto [uniformNames_lens, uniformNames_cstr, uniformNames_store] = detail::transform_strings(uniformNames);
    glGetUniformIndices(program, uniformCount, uniformNames_cstr.data(), reinterpret_cast<GLuint *>(uniformIndices.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_uniformuiv(u32 program, i32 location, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetUniformuiv)
                Throw(undefined_behavior(
                    "unloaded function GetUniformuiv"));
        glIsProgram(program);
    }
    glGetUniformuiv(program, location, reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void get_vertex_attrib_iiv(u32 index, groups::vertex_attrib_enum pname, i32& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribIiv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribIiv"));
    }
    glGetVertexAttribIiv(index, static_cast<GLenum>(pname), &params);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void get_vertex_attrib_iuiv(u32 index, groups::vertex_attrib_enum pname, u32& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetVertexAttribIuiv)
                Throw(undefined_behavior(
                    "unloaded function GetVertexAttribIuiv"));
    }
    glGetVertexAttribIuiv(index, static_cast<GLenum>(pname), &params);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_GLenum
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum>)
STATICINLINE void invalidate_framebuffer(groups::framebuffer_target target, span_const_GLenum const& attachments)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function InvalidateFramebuffer"));
    }
    glInvalidateFramebuffer(static_cast<GLenum>(target), attachments.size(), reinterpret_cast<const GLenum *>(attachments.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_GLenum,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void invalidate_sub_framebuffer(groups::framebuffer_target target, span_const_GLenum const& attachments, vec_2_i32 const& x, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glInvalidateSubFramebuffer)
                Throw(undefined_behavior(
                    "unloaded function InvalidateSubFramebuffer"));
    }
    glInvalidateSubFramebuffer(static_cast<GLenum>(target), attachments.size(), reinterpret_cast<const GLenum *>(attachments.data()), x.x(), x.y(), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_query(u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsQuery)
                Throw(undefined_behavior(
                    "unloaded function IsQuery"));
    }
    auto out = glIsQuery(id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_sampler(u32 sampler)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsSampler)
                Throw(undefined_behavior(
                    "unloaded function IsSampler"));
        glIsSampler(sampler);
    }
    auto out = glIsSampler(sampler);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_sync(GLsync sync)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsSync)
                Throw(undefined_behavior(
                    "unloaded function IsSync"));
    }
    auto out = glIsSync(sync);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_transform_feedback(u32 id)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function IsTransformFeedback"));
    }
    auto out = glIsTransformFeedback(id);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean is_vertex_array(u32 array)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsVertexArray)
                Throw(undefined_behavior(
                    "unloaded function IsVertexArray"));
        glIsVertexArray(array);
    }
    auto out = glIsVertexArray(array);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void * map_buffer_range(groups::buffer_target_arb target, GLintptr offset, GLsizeiptr length, groups::map_buffer_access_mask access)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMapBufferRange)
                Throw(undefined_behavior(
                    "unloaded function MapBufferRange"));
    }
    auto out = glMapBufferRange(static_cast<GLenum>(target), offset, length, static_cast<GLenum>(access));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void pause_transform_feedback()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPauseTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function PauseTransformFeedback"));
    }
    glPauseTransformFeedback();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void program_binary(u32 program, GLenum binaryFormat, span_const_byte const& binary, i32 length)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramBinary)
                Throw(undefined_behavior(
                    "unloaded function ProgramBinary"));
        glIsProgram(program);
    }
    glProgramBinary(program, binaryFormat, reinterpret_cast<const void *>(binary.data()), length);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void program_parameter(u32 program, groups::program_parameter_prop pname, i32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramParameteri)
                Throw(undefined_behavior(
                    "unloaded function ProgramParameteri"));
        glIsProgram(program);
    }
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void read_buffer(groups::read_buffer_mode src)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glReadBuffer)
                Throw(undefined_behavior(
                    "unloaded function ReadBuffer"));
    }
    glReadBuffer(static_cast<GLenum>(src));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void renderbuffer_storage_multisample(groups::renderbuffer_target target, i32 samples, groups::internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glRenderbufferStorageMultisample)
                Throw(undefined_behavior(
                    "unloaded function RenderbufferStorageMultisample"));
    }
    glRenderbufferStorageMultisample(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void resume_transform_feedback()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glResumeTransformFeedback)
                Throw(undefined_behavior(
                    "unloaded function ResumeTransformFeedback"));
    }
    glResumeTransformFeedback();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void sampler_parameter(u32 sampler, groups::sampler_parameter_f pname, f32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameterf)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameterf"));
        glIsSampler(sampler);
    }
    glSamplerParameterf(sampler, static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void sampler_parameter(u32 sampler, groups::sampler_parameter_f pname, span_const_f32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameterfv)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameterfv"));
        glIsSampler(sampler);
    }
    glSamplerParameterfv(sampler, static_cast<GLenum>(pname), reinterpret_cast<const GLfloat *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void sampler_parameter(u32 sampler, groups::sampler_parameter_i pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameteri)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameteri"));
        glIsSampler(sampler);
    }
    glSamplerParameteri(sampler, static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void sampler_parameter(u32 sampler, groups::sampler_parameter_i pname, span_const_i32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameteriv)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameteriv"));
        glIsSampler(sampler);
    }
    glSamplerParameteriv(sampler, static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_image_3d(groups::texture_target target, i32 level, i32 internalformat, vec_3_i32 const& width, i32 border, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexImage3D)
                Throw(undefined_behavior(
                    "unloaded function TexImage3D"));
    }
    glTexImage3D(static_cast<GLenum>(target), level, internalformat, width[0], width[1], width[2], border, static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void tex_storage_2d(groups::texture_target target, i32 levels, groups::sized_internal_format internalformat, vec_2_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage2D)
                Throw(undefined_behavior(
                    "unloaded function TexStorage2D"));
    }
    glTexStorage2D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internalformat), width[0], width[1]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void tex_storage_3d(groups::texture_target target, i32 levels, groups::sized_internal_format internalformat, vec_3_i32 const& width)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage3D)
                Throw(undefined_behavior(
                    "unloaded function TexStorage3D"));
    }
    glTexStorage3D(static_cast<GLenum>(target), levels, static_cast<GLenum>(internalformat), width[0], width[1], width[2]);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_byte,
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void tex_sub_image_3d(groups::texture_target target, i32 level, vec_3_i32 const& xoffset, vec_3_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_const_byte const& pixels)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexSubImage3D)
                Throw(undefined_behavior(
                    "unloaded function TexSubImage3D"));
    }
    glTexSubImage3D(static_cast<GLenum>(target), level, xoffset.x(), xoffset.y(), xoffset.z(), width[0], width[1], width[2], static_cast<GLenum>(format), static_cast<GLenum>(type), reinterpret_cast<const void *>(pixels.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void transform_feedback_varyings(u32 program, std::vector<std::string_view> varyings, groups::transform_feedback_buffer_mode bufferMode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTransformFeedbackVaryings)
                Throw(undefined_behavior(
                    "unloaded function TransformFeedbackVaryings"));
        glIsProgram(program);
    }
    auto [varyings_lens, varyings_cstr, varyings_store] = detail::transform_strings(varyings);
    glTransformFeedbackVaryings(program, varyings_cstr.size(), varyings_cstr.data(), static_cast<GLenum>(bufferMode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void uniform(i32 location, u32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1ui)
                Throw(undefined_behavior(
                    "unloaded function Uniform1ui"));
    }
    glUniform1ui(location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void uniform(i32 location, i32 count, span_const_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform1uiv)
                Throw(undefined_behavior(
                    "unloaded function Uniform1uiv"));
    }
    glUniform1uiv(location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_2_u32, u32, 2>)
STATICINLINE void uniform(i32 location, vec_2_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2ui)
                Throw(undefined_behavior(
                    "unloaded function Uniform2ui"));
    }
    glUniform2ui(location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform2uiv)
                Throw(undefined_behavior(
                    "unloaded function Uniform2uiv"));
    }
    glUniform2uiv(location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_3_u32, u32, 3>)
STATICINLINE void uniform(i32 location, vec_3_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3ui)
                Throw(undefined_behavior(
                    "unloaded function Uniform3ui"));
    }
    glUniform3ui(location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform3uiv)
                Throw(undefined_behavior(
                    "unloaded function Uniform3uiv"));
    }
    glUniform3uiv(location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_4_u32, u32, 4>)
STATICINLINE void uniform(i32 location, vec_4_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4ui)
                Throw(undefined_behavior(
                    "unloaded function Uniform4ui"));
    }
    glUniform4ui(location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void uniform(i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniform4uiv)
                Throw(undefined_behavior(
                    "unloaded function Uniform4uiv"));
    }
    glUniform4uiv(location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void uniform_block_binding(u32 program, u32 uniformBlockIndex, u32 uniformBlockBinding)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformBlockBinding)
                Throw(undefined_behavior(
                    "unloaded function UniformBlockBinding"));
        glIsProgram(program);
    }
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_mat_2x3_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_mat_2x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2x3fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2x3fv"));
    }
    glUniformMatrix2x3fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_mat_2x4_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_mat_2x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_2x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix2x4fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix2x4fv"));
    }
    glUniformMatrix2x4fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_mat_3x2_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_mat_3x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3x2fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3x2fv"));
    }
    glUniformMatrix3x2fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_mat_3x4_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_mat_3x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_3x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix3x4fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix3x4fv"));
    }
    glUniformMatrix3x4fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_mat_4x2_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_mat_4x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4x2fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4x2fv"));
    }
    glUniformMatrix4x2fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_mat_4x3_f32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_mat_4x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
STATICINLINE void uniform(i32 location, i32 count, bool transpose, span_const_mat_4x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUniformMatrix4x3fv)
                Throw(undefined_behavior(
                    "unloaded function UniformMatrix4x3fv"));
    }
    glUniformMatrix4x3fv(location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE GLboolean unmap_buffer(groups::buffer_target_arb target)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUnmapBuffer)
                Throw(undefined_behavior(
                    "unloaded function UnmapBuffer"));
    }
    auto out = glUnmapBuffer(static_cast<GLenum>(target));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void vertex_attrib_divisor(u32 index, u32 divisor)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribDivisor)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribDivisor"));
    }
    glVertexAttribDivisor(index, divisor);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_4_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_4_i32, i32, 4>)
STATICINLINE void vertex_attrib_i4i(u32 index, vec_4_i32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4i)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4i"));
    }
    glVertexAttribI4i(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_vec_4_i32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_4_i32> &&
    semantic::concepts::Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
STATICINLINE void vertex_attrib_i4iv(u32 index, span_const_vec_4_i32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4iv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4iv"));
    }
    glVertexAttribI4iv(index, reinterpret_cast<const GLint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Vector<vec_4_u32, u32, 4>)
STATICINLINE void vertex_attrib_i4ui(u32 index, vec_4_u32 const& x)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4ui)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4ui"));
    }
    glVertexAttribI4ui(index, x.x(), x.y(), x.z(), x.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void vertex_attrib_i4uiv(u32 index, span_const_vec_4_u32 const& v)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribI4uiv)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribI4uiv"));
    }
    glVertexAttribI4uiv(index, reinterpret_cast<const GLuint *>(v.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 0>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void vertex_attrib_i_pointer(u32 index, i32 size, groups::vertex_attrib_int type, i32 stride, span_const_byte const& pointer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribIPointer)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribIPointer"));
    }
    glVertexAttribIPointer(index, size, static_cast<GLenum>(type), stride, reinterpret_cast<const void *>(pointer.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.0 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 0>>)
STATICINLINE void wait_sync(GLsync sync, groups::sync_behavior_flags flags, u64 timeout)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glWaitSync)
                Throw(undefined_behavior(
                    "unloaded function WaitSync"));
    }
    glWaitSync(sync, static_cast<GLenum>(flags), timeout);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_ES_VERSION_3_0
#ifdef GL_ES_VERSION_3_1
/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void dispatch_compute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDispatchCompute)
                Throw(undefined_behavior(
                    "unloaded function DispatchCompute"));
    }
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void dispatch_compute_indirect(GLintptr indirect)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDispatchComputeIndirect)
                Throw(undefined_behavior(
                    "unloaded function DispatchComputeIndirect"));
    }
    glDispatchComputeIndirect(indirect);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void draw_arrays_indirect(groups::primitive_type mode, ptroff indirect)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawArraysIndirect)
                Throw(undefined_behavior(
                    "unloaded function DrawArraysIndirect"));
    }
    glDrawArraysIndirect(static_cast<GLenum>(mode), reinterpret_cast<const void*>(indirect));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void draw_elements_indirect(groups::primitive_type mode, groups::draw_elements_type type, ptroff indirect)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsIndirect)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsIndirect"));
    }
    glDrawElementsIndirect(static_cast<GLenum>(mode), static_cast<GLenum>(type), reinterpret_cast<const void*>(indirect));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void framebuffer_parameter(groups::framebuffer_target target, groups::framebuffer_parameter_name pname, i32 param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferParameteri)
                Throw(undefined_behavior(
                    "unloaded function FramebufferParameteri"));
    }
    glFramebufferParameteri(static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_framebuffer_parameter(groups::framebuffer_target target, groups::framebuffer_attachment_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetFramebufferParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetFramebufferParameteriv"));
    }
    glGetFramebufferParameteriv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_program_interfaceiv(u32 program, groups::program_interface programInterface, groups::program_interface_prop pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramInterfaceiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramInterfaceiv"));
        glIsProgram(program);
    }
    glGetProgramInterfaceiv(program, static_cast<GLenum>(programInterface), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE GLuint get_program_resource_index(u32 program, groups::program_interface programInterface, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceIndex)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceIndex"));
        glIsProgram(program);
    }
    auto out = glGetProgramResourceIndex(program, static_cast<GLenum>(programInterface), name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE GLint get_program_resource_location(u32 program, groups::program_interface programInterface, std::string_view const& name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceLocation)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceLocation"));
        glIsProgram(program);
    }
    auto out = glGetProgramResourceLocation(program, static_cast<GLenum>(programInterface), name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.1 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_program_resource_name(u32 program, groups::program_interface programInterface, u32 index, i32& length, span_GLchar name)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceName)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceName"));
        glIsProgram(program);
    }
    glGetProgramResourceName(program, static_cast<GLenum>(programInterface), index, name.size(), &length, name.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_GLenum,
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_GLenum> &&
    std::is_same_v<std::decay_t<typename span_const_GLenum::value_type>, GLenum> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_program_resourceiv(u32 program, groups::program_interface programInterface, u32 index, span_const_GLenum const& props, i32& length, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramResourceiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramResourceiv"));
        glIsProgram(program);
    }
    glGetProgramResourceiv(program, static_cast<GLenum>(programInterface), index, props.size(), reinterpret_cast<const GLenum *>(props.data()), params.size(), &length, reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void active_shader_program(u32 pipeline, u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glActiveShaderProgram)
                Throw(undefined_behavior(
                    "unloaded function ActiveShaderProgram"));
        glIsProgramPipeline(pipeline);
        glIsProgram(program);
    }
    glActiveShaderProgram(pipeline, program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void bind_program_pipeline(u32 pipeline)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindProgramPipeline)
                Throw(undefined_behavior(
                    "unloaded function BindProgramPipeline"));
        glIsProgramPipeline(pipeline);
    }
    glBindProgramPipeline(pipeline);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE GLuint create_shader_programv(GLenum type, std::vector<std::string_view> strings)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCreateShaderProgramv)
                Throw(undefined_behavior(
                    "unloaded function CreateShaderProgramv"));
    }
    auto [strings_lens, strings_cstr, strings_store] = detail::transform_strings(strings);
    auto out = glCreateShaderProgramv(type, strings_cstr.size(), strings_cstr.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.1 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void delete_program_pipelines(span_const_u32 const& pipelines)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDeleteProgramPipelines)
                Throw(undefined_behavior(
                    "unloaded function DeleteProgramPipelines"));
    }
    glDeleteProgramPipelines(pipelines.size(), reinterpret_cast<const GLuint *>(pipelines.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void gen_program_pipelines(span_u32 pipelines)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGenProgramPipelines)
                Throw(undefined_behavior(
                    "unloaded function GenProgramPipelines"));
    }
    glGenProgramPipelines(pipelines.size(), reinterpret_cast<GLuint *>(pipelines.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_program_pipeline_info_log(u32 pipeline, i32& length, span_GLchar infoLog)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramPipelineInfoLog)
                Throw(undefined_behavior(
                    "unloaded function GetProgramPipelineInfoLog"));
        glIsProgramPipeline(pipeline);
    }
    glGetProgramPipelineInfoLog(pipeline, infoLog.size(), &length, infoLog.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_program_pipelineiv(u32 pipeline, groups::pipeline_parameter_name pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetProgramPipelineiv)
                Throw(undefined_behavior(
                    "unloaded function GetProgramPipelineiv"));
        glIsProgramPipeline(pipeline);
    }
    glGetProgramPipelineiv(pipeline, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE GLboolean is_program_pipeline(u32 pipeline)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsProgramPipeline)
                Throw(undefined_behavior(
                    "unloaded function IsProgramPipeline"));
        glIsProgramPipeline(pipeline);
    }
    auto out = glIsProgramPipeline(pipeline);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void program_uniform(u32 program, i32 location, f32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1f)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1f"));
        glIsProgram(program);
    }
    glProgramUniform1f(program, location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<std::decay_t<typename span_const_f32::value_type>, f32>)
STATICINLINE void program_uniform(u32 program, i32 location, span_const_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1fv"));
        glIsProgram(program);
    }
    glProgramUniform1fv(program, location, value.size(), reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1i)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1i"));
        glIsProgram(program);
    }
    glProgramUniform1i(program, location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void program_uniform(u32 program, i32 location, span_const_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1iv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1iv"));
        glIsProgram(program);
    }
    glProgramUniform1iv(program, location, value.size(), reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void program_uniform(u32 program, i32 location, u32 v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1ui)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1ui"));
        glIsProgram(program);
    }
    glProgramUniform1ui(program, location, v0);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void program_uniform(u32 program, i32 location, span_const_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform1uiv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform1uiv"));
        glIsProgram(program);
    }
    glProgramUniform1uiv(program, location, value.size(), reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_2_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_2_f32, f32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2f)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2f"));
        glIsProgram(program);
    }
    glProgramUniform2f(program, location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_2_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_2_f32> &&
    semantic::concepts::Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2fv"));
        glIsProgram(program);
    }
    glProgramUniform2fv(program, location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2i)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2i"));
        glIsProgram(program);
    }
    glProgramUniform2i(program, location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_2_i32> &&
    semantic::concepts::Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2iv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2iv"));
        glIsProgram(program);
    }
    glProgramUniform2iv(program, location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_2_u32, u32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2ui)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2ui"));
        glIsProgram(program);
    }
    glProgramUniform2ui(program, location, v0.x(), v0.y());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_2_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform2uiv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform2uiv"));
        glIsProgram(program);
    }
    glProgramUniform2uiv(program, location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_3_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_3_f32, f32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3f)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3f"));
        glIsProgram(program);
    }
    glProgramUniform3f(program, location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_3_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_3_f32> &&
    semantic::concepts::Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3fv"));
        glIsProgram(program);
    }
    glProgramUniform3fv(program, location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3i)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3i"));
        glIsProgram(program);
    }
    glProgramUniform3i(program, location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_3_i32> &&
    semantic::concepts::Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3iv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3iv"));
        glIsProgram(program);
    }
    glProgramUniform3iv(program, location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_3_u32, u32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3ui)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3ui"));
        glIsProgram(program);
    }
    glProgramUniform3ui(program, location, v0.x(), v0.y(), v0.z());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_3_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform3uiv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform3uiv"));
        glIsProgram(program);
    }
    glProgramUniform3uiv(program, location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_4_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_4_f32, f32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_f32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4f)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4f"));
        glIsProgram(program);
    }
    glProgramUniform4f(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_4_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_4_f32> &&
    semantic::concepts::Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4fv"));
        glIsProgram(program);
    }
    glProgramUniform4fv(program, location, count, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_4_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_4_i32, i32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_i32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4i)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4i"));
        glIsProgram(program);
    }
    glProgramUniform4i(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_4_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_4_i32> &&
    semantic::concepts::Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4iv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4iv"));
        glIsProgram(program);
    }
    glProgramUniform4iv(program, location, count, reinterpret_cast<const GLint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_4_u32, u32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_u32 const& v0)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4ui)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4ui"));
        glIsProgram(program);
    }
    glProgramUniform4ui(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_vec_4_u32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniform4uiv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniform4uiv"));
        glIsProgram(program);
    }
    glProgramUniform4uiv(program, location, count, reinterpret_cast<const GLuint *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_2x2_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_2x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_2x3_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_2x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2x3fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2x3fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2x3fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_2x4_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_2x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_2x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix2x4fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix2x4fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix2x4fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_3x3_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_3x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_3x2_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_3x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3x2fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3x2fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3x2fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_3x4_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_3x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_3x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix3x4fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix3x4fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix3x4fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_4x4_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_4x4_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x4_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_4x2_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_4x2_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x2_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4x2fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4x2fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4x2fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_const_mat_4x3_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_const_mat_4x3_f32> &&
    semantic::concepts::Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
STATICINLINE void program_uniform(u32 program, i32 location, i32 count, bool transpose, span_const_mat_4x3_f32 const& value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glProgramUniformMatrix4x3fv)
                Throw(undefined_behavior(
                    "unloaded function ProgramUniformMatrix4x3fv"));
        glIsProgram(program);
    }
    glProgramUniformMatrix4x3fv(program, location, count, transpose, reinterpret_cast<const GLfloat *>(value.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void use_program_stages(u32 pipeline, groups::use_program_stage_mask stages, u32 program)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glUseProgramStages)
                Throw(undefined_behavior(
                    "unloaded function UseProgramStages"));
        glIsProgramPipeline(pipeline);
        glIsProgram(program);
    }
    glUseProgramStages(pipeline, static_cast<GLenum>(stages), program);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void validate_program_pipeline(u32 pipeline)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glValidateProgramPipeline)
                Throw(undefined_behavior(
                    "unloaded function ValidateProgramPipeline"));
        glIsProgramPipeline(pipeline);
    }
    glValidateProgramPipeline(pipeline);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void bind_image_texture(u32 unit, u32 texture, i32 level, bool layered, i32 layer, groups::buffer_access_arb access, groups::internal_format format)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindImageTexture)
                Throw(undefined_behavior(
                    "unloaded function BindImageTexture"));
        glIsTexture(texture);
    }
    glBindImageTexture(unit, texture, level, layered, layer, static_cast<GLenum>(access), static_cast<GLenum>(format));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_bool
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_bool> &&
    std::is_same_v<std::decay_t<typename span_bool::value_type>, bool>)
STATICINLINE void get_booleani_v(groups::buffer_target_arb target, u32 index, span_bool data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetBooleani_v)
                Throw(undefined_behavior(
                    "unloaded function GetBooleani_v"));
    }
    glGetBooleani_v(static_cast<GLenum>(target), index, reinterpret_cast<GLboolean *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void memory_barrier(groups::memory_barrier_mask barriers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMemoryBarrier)
                Throw(undefined_behavior(
                    "unloaded function MemoryBarrier"));
    }
    glMemoryBarrier(static_cast<GLenum>(barriers));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void memory_barrier_by_region(groups::memory_barrier_mask barriers)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMemoryBarrierByRegion)
                Throw(undefined_behavior(
                    "unloaded function MemoryBarrierByRegion"));
    }
    glMemoryBarrierByRegion(static_cast<GLenum>(barriers));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_multisamplefv(groups::get_multisample_prop_nv pname, u32 index, span_f32 val)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetMultisamplefv)
                Throw(undefined_behavior(
                    "unloaded function GetMultisamplefv"));
    }
    glGetMultisamplefv(static_cast<GLenum>(pname), index, reinterpret_cast<GLfloat *>(val.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void get_tex_level_parameter(groups::texture_target target, i32 level, groups::get_texture_parameter pname, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexLevelParameterfv)
                Throw(undefined_behavior(
                    "unloaded function GetTexLevelParameterfv"));
    }
    glGetTexLevelParameterfv(static_cast<GLenum>(target), level, static_cast<GLenum>(pname), reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_tex_level_parameter(groups::texture_target target, i32 level, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexLevelParameteriv)
                Throw(undefined_behavior(
                    "unloaded function GetTexLevelParameteriv"));
    }
    glGetTexLevelParameteriv(static_cast<GLenum>(target), level, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void sample_maski(u32 maskNumber, GLbitfield mask)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSampleMaski)
                Throw(undefined_behavior(
                    "unloaded function SampleMaski"));
    }
    glSampleMaski(maskNumber, mask);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 1>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2>)
STATICINLINE void tex_storage_2d_multisample(groups::texture_target target, i32 samples, groups::sized_internal_format internalformat, vec_2_i32 const& width, bool fixedsamplelocations)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage2DMultisample)
                Throw(undefined_behavior(
                    "unloaded function TexStorage2DMultisample"));
    }
    glTexStorage2DMultisample(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width[0], width[1], fixedsamplelocations);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void bind_vertex_buffer(u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBindVertexBuffer)
                Throw(undefined_behavior(
                    "unloaded function BindVertexBuffer"));
        glIsBuffer(buffer);
    }
    glBindVertexBuffer(bindingindex, buffer, offset, stride);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void vertex_attrib_binding(u32 attribindex, u32 bindingindex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribBinding)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribBinding"));
    }
    glVertexAttribBinding(attribindex, bindingindex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void vertex_attrib_format(u32 attribindex, i32 size, groups::vertex_attrib_type type, bool normalized, u32 relativeoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribFormat)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribFormat"));
    }
    glVertexAttribFormat(attribindex, size, static_cast<GLenum>(type), normalized, relativeoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void vertex_attrib_i_format(u32 attribindex, i32 size, groups::vertex_attrib_int type, u32 relativeoffset)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexAttribIFormat)
                Throw(undefined_behavior(
                    "unloaded function VertexAttribIFormat"));
    }
    glVertexAttribIFormat(attribindex, size, static_cast<GLenum>(type), relativeoffset);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.1 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 1>>)
STATICINLINE void vertex_binding_divisor(u32 bindingindex, u32 divisor)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glVertexBindingDivisor)
                Throw(undefined_behavior(
                    "unloaded function VertexBindingDivisor"));
    }
    glVertexBindingDivisor(bindingindex, divisor);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_ES_VERSION_3_1
#ifdef GL_ES_VERSION_3_2
/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void blend_barrier()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendBarrier)
                Throw(undefined_behavior(
                    "unloaded function BlendBarrier"));
    }
    glBlendBarrier();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void copy_image_sub_data(u32 srcName, groups::copy_image_sub_data_target srcTarget, i32 srcLevel, i32 srcX, i32 srcY, i32 srcZ, u32 dstName, groups::copy_image_sub_data_target dstTarget, i32 dstLevel, i32 dstX, i32 dstY, i32 dstZ, i32 srcWidth, i32 srcHeight, i32 srcDepth)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glCopyImageSubData)
                Throw(undefined_behavior(
                    "unloaded function CopyImageSubData"));
    }
    glCopyImageSubData(srcName, static_cast<GLenum>(srcTarget), srcLevel, srcX, srcY, srcZ, dstName, static_cast<GLenum>(dstTarget), dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void debug_message_callback(GLDEBUGPROC callback, span_const_byte const& userParam)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDebugMessageCallback)
                Throw(undefined_behavior(
                    "unloaded function DebugMessageCallback"));
    }
    glDebugMessageCallback(callback, reinterpret_cast<const void *>(userParam.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void debug_message_control(groups::debug_source source, groups::debug_type type, groups::debug_severity severity, span_const_u32 const& ids, bool enabled)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDebugMessageControl)
                Throw(undefined_behavior(
                    "unloaded function DebugMessageControl"));
    }
    glDebugMessageControl(static_cast<GLenum>(source), static_cast<GLenum>(type), static_cast<GLenum>(severity), ids.size(), reinterpret_cast<const GLuint *>(ids.data()), enabled);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void debug_message_insert(groups::debug_source source, groups::debug_type type, u32 id, groups::debug_severity severity, i32 length, std::string_view const& buf)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDebugMessageInsert)
                Throw(undefined_behavior(
                    "unloaded function DebugMessageInsert"));
    }
    glDebugMessageInsert(static_cast<GLenum>(source), static_cast<GLenum>(type), id, static_cast<GLenum>(severity), length, buf.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_GLchar,
    class span_GLenum,
    class span_i32,
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_GLenum> &&
    std::is_same_v<std::decay_t<typename span_GLenum::value_type>, GLenum> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE GLuint get_debug_message_log(span_GLenum sources, span_GLenum types, span_u32 ids, span_GLenum severities, span_i32 lengths, span_GLchar messageLog)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetDebugMessageLog)
                Throw(undefined_behavior(
                    "unloaded function GetDebugMessageLog"));
    }
    auto out = glGetDebugMessageLog(sources.size(), messageLog.size(), reinterpret_cast<GLenum *>(sources.data()), reinterpret_cast<GLenum *>(types.data()), reinterpret_cast<GLuint *>(ids.data()), reinterpret_cast<GLenum *>(severities.data()), reinterpret_cast<GLsizei *>(lengths.data()), messageLog.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.2 */
template<
    class span_GLchar
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_object_label(groups::object_identifier identifier, u32 name, i32& length, span_GLchar label)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetObjectLabel)
                Throw(undefined_behavior(
                    "unloaded function GetObjectLabel"));
    }
    glGetObjectLabel(static_cast<GLenum>(identifier), name, label.size(), &length, label.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_GLchar,
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte> &&
    semantic::concepts::Span<span_GLchar> &&
    std::is_same_v<std::decay_t<typename span_GLchar::value_type>, GLchar>)
STATICINLINE void get_object_ptr_label(span_const_byte const& ptr, i32& length, span_GLchar label)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetObjectPtrLabel)
                Throw(undefined_behavior(
                    "unloaded function GetObjectPtrLabel"));
    }
    glGetObjectPtrLabel(reinterpret_cast<const void *>(ptr.data()), label.size(), &length, label.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_byte
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void get_pointerv(groups::get_pointerv_prop pname, span_byte params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetPointerv)
                Throw(undefined_behavior(
                    "unloaded function GetPointerv"));
    }
    glGetPointerv(static_cast<GLenum>(pname), reinterpret_cast<void **>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void object_label(groups::object_identifier identifier, u32 name, i32 length, std::string_view const& label)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glObjectLabel)
                Throw(undefined_behavior(
                    "unloaded function ObjectLabel"));
    }
    glObjectLabel(static_cast<GLenum>(identifier), name, length, label.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void object_ptr_label(span_const_byte const& ptr, i32 length, std::string_view const& label)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glObjectPtrLabel)
                Throw(undefined_behavior(
                    "unloaded function ObjectPtrLabel"));
    }
    glObjectPtrLabel(reinterpret_cast<const void *>(ptr.data()), length, label.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void pop_debug_group()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPopDebugGroup)
                Throw(undefined_behavior(
                    "unloaded function PopDebugGroup"));
    }
    glPopDebugGroup();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void push_debug_group(groups::debug_source source, u32 id, i32 length, std::string_view const& message)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPushDebugGroup)
                Throw(undefined_behavior(
                    "unloaded function PushDebugGroup"));
    }
    glPushDebugGroup(static_cast<GLenum>(source), id, length, message.data());
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void blend_equation_separatei(u32 buf, groups::blend_equation_mode_ext modeRGB, groups::blend_equation_mode_ext modeAlpha)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendEquationSeparatei)
                Throw(undefined_behavior(
                    "unloaded function BlendEquationSeparatei"));
    }
    glBlendEquationSeparatei(buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void blend_equationi(u32 buf, groups::blend_equation_mode_ext mode)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendEquationi)
                Throw(undefined_behavior(
                    "unloaded function BlendEquationi"));
    }
    glBlendEquationi(buf, static_cast<GLenum>(mode));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void blend_func_separatei(u32 buf, groups::blending_factor srcRGB, groups::blending_factor dstRGB, groups::blending_factor srcAlpha, groups::blending_factor dstAlpha)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendFuncSeparatei)
                Throw(undefined_behavior(
                    "unloaded function BlendFuncSeparatei"));
    }
    glBlendFuncSeparatei(buf, static_cast<GLenum>(srcRGB), static_cast<GLenum>(dstRGB), static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void blend_funci(u32 buf, groups::blending_factor src, groups::blending_factor dst)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glBlendFunci)
                Throw(undefined_behavior(
                    "unloaded function BlendFunci"));
    }
    glBlendFunci(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void color_maski(u32 index, bool r, bool g, bool b, bool a)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glColorMaski)
                Throw(undefined_behavior(
                    "unloaded function ColorMaski"));
    }
    glColorMaski(index, r, g, b, a);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void disablei(groups::enable_cap target, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDisablei)
                Throw(undefined_behavior(
                    "unloaded function Disablei"));
    }
    glDisablei(static_cast<GLenum>(target), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void enablei(groups::enable_cap target, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glEnablei)
                Throw(undefined_behavior(
                    "unloaded function Enablei"));
    }
    glEnablei(static_cast<GLenum>(target), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE GLboolean is_enabledi(groups::enable_cap target, u32 index)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glIsEnabledi)
                Throw(undefined_behavior(
                    "unloaded function IsEnabledi"));
    }
    auto out = glIsEnabledi(static_cast<GLenum>(target), index);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void draw_elements_base_vertex(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices, i32 basevertex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsBaseVertex)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsBaseVertex"));
    }
    glDrawElementsBaseVertex(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), basevertex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void draw_elements_instanced_base_vertex(groups::primitive_type mode, i32 count, groups::draw_elements_type type, ptroff indices, i32 instancecount, i32 basevertex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawElementsInstancedBaseVertex)
                Throw(undefined_behavior(
                    "unloaded function DrawElementsInstancedBaseVertex"));
    }
    glDrawElementsInstancedBaseVertex(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), reinterpret_cast<const void*>(indices), instancecount, basevertex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_const_byte
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_byte> &&
    std::is_same_v<std::decay_t<typename span_const_byte::value_type>, const std::byte>)
STATICINLINE void draw_range_elements_base_vertex(groups::primitive_type mode, u32 start, u32 end, i32 count, groups::draw_elements_type type, span_const_byte const& indices, i32 basevertex)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glDrawRangeElementsBaseVertex)
                Throw(undefined_behavior(
                    "unloaded function DrawRangeElementsBaseVertex"));
    }
    glDrawRangeElementsBaseVertex(static_cast<GLenum>(mode), start, end, count, static_cast<GLenum>(type), reinterpret_cast<const void *>(indices.data()), basevertex);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void framebuffer_texture(groups::framebuffer_target target, groups::framebuffer_attachment attachment, u32 texture, i32 level)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glFramebufferTexture)
                Throw(undefined_behavior(
                    "unloaded function FramebufferTexture"));
        glIsTexture(texture);
    }
    glFramebufferTexture(static_cast<GLenum>(target), static_cast<GLenum>(attachment), texture, level);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void primitive_bounding_box(f32 minX, f32 minY, f32 minZ, f32 minW, f32 maxX, f32 maxY, f32 maxZ, f32 maxW)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPrimitiveBoundingBox)
                Throw(undefined_behavior(
                    "unloaded function PrimitiveBoundingBox"));
    }
    glPrimitiveBoundingBox(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE GLenum get_graphics_reset_status()
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetGraphicsResetStatus)
                Throw(undefined_behavior(
                    "unloaded function GetGraphicsResetStatus"));
    }
    auto out = glGetGraphicsResetStatus();
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
    return out;
}

/* Introduced in GL es 3.2 */
template<
    class span_f32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<std::decay_t<typename span_f32::value_type>, f32>)
STATICINLINE void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnUniformfv)
                Throw(undefined_behavior(
                    "unloaded function GetnUniformfv"));
        glIsProgram(program);
    }
    glGetnUniformfv(program, location, bufSize, reinterpret_cast<GLfloat *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnUniformiv)
                Throw(undefined_behavior(
                    "unloaded function GetnUniformiv"));
        glIsProgram(program);
    }
    glGetnUniformiv(program, location, bufSize, reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void getn_uniformuiv(u32 program, i32 location, i32 bufSize, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetnUniformuiv)
                Throw(undefined_behavior(
                    "unloaded function GetnUniformuiv"));
        glIsProgram(program);
    }
    glGetnUniformuiv(program, location, bufSize, reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_byte,
    class vec_2_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Span<span_byte> &&
    std::is_same_v<std::decay_t<typename span_byte::value_type>, std::byte>)
STATICINLINE void readn_pixels(vec_2_i32 const& x, vec_2_i32 const& width, groups::pixel_format format, groups::pixel_type type, span_byte data)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glReadnPixels)
                Throw(undefined_behavior(
                    "unloaded function ReadnPixels"));
    }
    glReadnPixels(x.x(), x.y(), width[0], width[1], static_cast<GLenum>(format), static_cast<GLenum>(type), data.size(), reinterpret_cast<void *>(data.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void min_sample_shading(f32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glMinSampleShading)
                Throw(undefined_behavior(
                    "unloaded function MinSampleShading"));
    }
    glMinSampleShading(value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void patch_parameter(groups::patch_parameter_name pname, i32 value)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glPatchParameteri)
                Throw(undefined_behavior(
                    "unloaded function PatchParameteri"));
    }
    glPatchParameteri(static_cast<GLenum>(pname), value);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_sampler_parameter_iiv(u32 sampler, groups::sampler_parameter_i pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSamplerParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function GetSamplerParameterIiv"));
        glIsSampler(sampler);
    }
    glGetSamplerParameterIiv(sampler, static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_sampler_parameter_iuiv(u32 sampler, groups::sampler_parameter_i pname, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetSamplerParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function GetSamplerParameterIuiv"));
        glIsSampler(sampler);
    }
    glGetSamplerParameterIuiv(sampler, static_cast<GLenum>(pname), reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<std::decay_t<typename span_i32::value_type>, i32>)
STATICINLINE void get_tex_parameter_iiv(groups::texture_target target, groups::get_texture_parameter pname, span_i32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function GetTexParameterIiv"));
    }
    glGetTexParameterIiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_u32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<std::decay_t<typename span_u32::value_type>, u32>)
STATICINLINE void get_tex_parameter_iuiv(groups::texture_target target, groups::get_texture_parameter pname, span_u32 params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glGetTexParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function GetTexParameterIuiv"));
    }
    glGetTexParameterIuiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void sampler_parameter_iiv(u32 sampler, groups::sampler_parameter_i pname, span_const_i32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameterIiv"));
        glIsSampler(sampler);
    }
    glSamplerParameterIiv(sampler, static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void sampler_parameter_iuiv(u32 sampler, groups::sampler_parameter_i pname, span_const_u32 const& param)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glSamplerParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function SamplerParameterIuiv"));
        glIsSampler(sampler);
    }
    glSamplerParameterIuiv(sampler, static_cast<GLenum>(pname), reinterpret_cast<const GLuint *>(param.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_const_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<std::decay_t<typename span_const_i32::value_type>, i32>)
STATICINLINE void tex_parameter_iiv(groups::texture_target target, groups::texture_parameter_name pname, span_const_i32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameterIiv)
                Throw(undefined_behavior(
                    "unloaded function TexParameterIiv"));
    }
    glTexParameterIiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<const GLint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class span_const_u32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<std::decay_t<typename span_const_u32::value_type>, u32>)
STATICINLINE void tex_parameter_iuiv(groups::texture_target target, groups::texture_parameter_name pname, span_const_u32 const& params)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexParameterIuiv)
                Throw(undefined_behavior(
                    "unloaded function TexParameterIuiv"));
    }
    glTexParameterIuiv(static_cast<GLenum>(target), static_cast<GLenum>(pname), reinterpret_cast<const GLuint *>(params.data()));
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void tex_buffer(groups::texture_target target, groups::sized_internal_format internalformat, u32 buffer)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexBuffer)
                Throw(undefined_behavior(
                    "unloaded function TexBuffer"));
        glIsBuffer(buffer);
    }
    glTexBuffer(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), buffer);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    typename Dummy = void
>
requires (
    MinimumVersion<Current, Version<3, 2>>)
STATICINLINE void tex_buffer_range(groups::texture_target target, groups::sized_internal_format internalformat, u32 buffer, GLintptr offset, GLsizeiptr size)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexBufferRange)
                Throw(undefined_behavior(
                    "unloaded function TexBufferRange"));
        glIsBuffer(buffer);
    }
    glTexBufferRange(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), buffer, offset, size);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

/* Introduced in GL es 3.2 */
template<
    class vec_3_i32
>
requires (
    MinimumVersion<Current, Version<3, 2>> &&
    semantic::concepts::Vector<vec_3_i32, i32, 3>)
STATICINLINE void tex_storage_3d_multisample(groups::texture_target target, i32 samples, groups::sized_internal_format internalformat, vec_3_i32 const& width, bool fixedsamplelocations)
{
    if constexpr(compile_info::debug_mode)
    {
        if constexpr(gl::is_linked)
            if(!glTexStorage3DMultisample)
                Throw(undefined_behavior(
                    "unloaded function TexStorage3DMultisample"));
    }
    glTexStorage3DMultisample(static_cast<GLenum>(target), samples, static_cast<GLenum>(internalformat), width[0], width[1], width[2], fixedsamplelocations);
    if constexpr(compile_info::debug_mode)
    {
        if(auto error = glGetError(); error != GL_NO_ERROR)
            Throw(undefined_behavior("GL error occurred: " + std::to_string(error)));
    }
}

#endif // GL_ES_VERSION_3_2

}; // struct es

} // gl::impl
