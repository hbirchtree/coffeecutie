#pragma once

#include <peripherals/concepts/span.h>
#include <peripherals/concepts/vector.h>
#include <peripherals/identify/compiler/variable_attributes.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/string/hex.h>

#if defined(FEATURE_ENABLE_CoreJsonLogger) && 0
#include <fmt_extensions/format.h>
#include <platforms/pimpl_state.h>
#include <platforms/profiling/jsonprofile.h>
#include <type_traits>

namespace glw::trace::detail {
template<typename T>
requires(
    !std::is_enum_v<std::remove_cv_t<T>> &&
    !std::is_integral_v<std::remove_cv_t<T>> &&
    !std::is_floating_point_v<std::remove_cv_t<T>> && !std::is_pointer_v<T>)
inline std::string to_string(T const& arg)
{
    return "?";
}

template<typename T>
requires(
    !std::is_enum_v<std::remove_cv_t<T>> &&
    (std::is_integral_v<std::remove_cv_t<T>> ||
     std::is_floating_point_v<std::remove_cv_t<T>> || std::is_pointer_v<T>))
inline std::string to_string(T const& arg)
{
    if constexpr(std::is_pointer_v<T>)
        return fmt::format("{}", reinterpret_cast<const void*>(arg));
    else
        return fmt::format("{}", arg);
}

template<typename T>
requires std::is_enum_v<std::remove_cv_t<T>>
inline std::string to_string(T const& arg)
{
    return fmt::format("0x{:X}", static_cast<unsigned int>(arg));
}

template<typename... T>
inline void capture_gl_trace(std::string_view func, T... args)
{
    std::vector<std::string> args_;
    (args_.push_back(to_string(args)), ...);
    platform::profiling::json::CaptureTrace(
        *platform::state->GetProfilerTStore(), func, args_);
}

template<typename DataT, typename... T>
inline void capture_gl_trace_data(
    std::string_view func, gsl::span<DataT> const& data, T... args)
{
    std::vector<std::string> args_;
    (args_.push_back(to_string(args)), ...);
    platform::profiling::json::CaptureTrace(
        *platform::state->GetProfilerTStore(),
        func,
        args_,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size()));
}
} // namespace glw::trace::detail

#define GLW_FPTR_TRACE(func, ...) \
    glw::trace::detail::capture_gl_trace(#func, ##__VA_ARGS__)
#define GLW_FPTR_TRACE_DATA(func, data, ...) \
    glw::trace::detail::capture_gl_trace(#func, ##__VA_ARGS__)
#endif

// clang-format off

#define GL_BASE_ES_MASK 0x1000
#define GL_BASE_VERSION_MASK 0x00FF

#if defined(GLEAM_USE_CORE)

#define GL_BASE_ES_VERSION 0x000

#include <KHR/khrplatform.h>
#include <glad/gl.h>

#define GL_BASE_CORE_VERSION 0x460

#elif defined(GLEAM_USE_ES) // defined(GLEAM_USE_CORE)

#ifndef GLEAM_RESTRICT_ES
#define GLEAM_RESTRICT_ES 0x400
#endif

#define GL_BASE_CORE_VERSION 0x000

#if defined(GLEAM_USE_LINKED)
/* Profile functions */
#if C_HAS_INCLUDE(<GLES3/gl32.h>) && GLEAM_RESTRICT_ES >= 0x320
#include <GLES3/gl32.h>
#define GL_BASE_ES_VERSION 0x320
#elif C_HAS_INCLUDE(<GLES3/gl31.h>) && GLEAM_RESTRICT_ES >= 0x310
#include <GLES3/gl31.h>
#define GL_BASE_ES_VERSION 0x310
#elif C_HAS_INCLUDE(<GLES3/gl3.h>) && GLEAM_RESTRICT_ES >= 0x300
#include <GLES3/gl3.h>
#define GL_BASE_ES_VERSION 0x300
#elif C_HAS_INCLUDE(<OpenGLES/ES3/gl.h>) && GLEAM_RESTRICT_ES >= 0x300
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#define GL_BASE_ES_VERSION 0x300
#elif C_HAS_INCLUDE(<OpenGLES/ES2/gl.h>) && GLEAM_RESTRICT_ES >= 0x200
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#define GL_BASE_ES_VERSION 0x200
#elif C_HAS_INCLUDE(<GLES2/gl2.h>) && GLEAM_RESTRICT_ES >= 0x200
#include <GLES2/gl2.h>
#define GL_BASE_ES_VERSION 0x200
#else
#error Configured for linked GLES headers, but none found
#endif

/* Extension functions */
#if C_HAS_INCLUDE(<GLES3/gl3ext.h>)
#include <GLES3/gl3ext.h>
#elif C_HAS_INCLUDE(<GLES2/gl2ext.h>)
//#include <GLES2/gl2ext.h>
#endif // C_HAS_INCLUDE(<GLES3/gl3ext.h>)

#else // defined(GLEAM_USE_LINKED)

#include <glad/gles2.h>
#define GL_BASE_ES_VERSION 0x320

#endif // defined(GLEAM_USE_LINKED)
#endif // defined(GLEAM_USE_ES)

// clang-format on

#define GL_VERSION_VERIFY(CORE_VER, ES_VER) \
    GL_BASE_CORE_VERSION >= CORE_VER || GL_BASE_ES_VERSION >= ES_VER

namespace gl {
namespace concepts {

template<class T, typename E, size_t M, size_t N>
concept matrix = semantic::concepts::Matrix<T, E, M, N>;
template<class T, typename E>
concept size_2d = semantic::concepts::Size2D<T, E>;
template<class T, typename E>
concept size_3d = semantic::concepts::Size2D<T, E>;
template<class T, typename E, size_t N>
concept vector = semantic::concepts::Vector<T, E, N>;

template<class T>
concept span = semantic::concepts::Span<T>;

} // namespace concepts

using libc_types::f32;
using libc_types::f64;
using libc_types::i16;
using libc_types::i32;
using libc_types::i64;
using libc_types::i8;
using libc_types::u16;
using libc_types::u32;
using libc_types::u64;
using libc_types::u8;

using gsl::span;

constexpr bool is_linked =
#if defined(GLEAM_USE_LINKED)
    true
#else
    false
#endif
    ;

template<size_t Major, size_t Minor>
struct Version
{
    static constexpr size_t major = Major;
    static constexpr size_t minor = Minor;
};

template<class Current, class Required>
concept MinimumVersion =
    Current::major >= Required::major &&
    (Current::major > Required::major || Current::minor >= Required::minor);

template<class Current, class Maximum>
concept MaximumVersion =
    Current::major < Maximum::major ||
    (Current::major == Maximum::major && Current::minor <= Maximum::minor);

enum class error_check
{
    on,
    off,
};

namespace detail {

inline std::string error_to_hex(auto error)
{
    return stl_types::str::fmt::pointerify(error);
}

void error_check(std::string_view cmd_name, enum error_check check_errors);

} // namespace detail
} // namespace gl
