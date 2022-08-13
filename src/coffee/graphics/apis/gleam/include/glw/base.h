#pragma once

#include <peripherals/identify/compiler/variable_attributes.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/string_ops.h>

#define GL_BASE_ES_MASK 0x1000
#define GL_BASE_VERSION_MASK 0x00FF

#if defined(GLEAM_USE_CORE)

#define GL_BASE_ES_VERSION 0x000

#include <glad/KHR/khrplatform.h>
#include <glad/glad.h>

#define GL_BASE_CORE_VERSION 0x460

#elif defined(GLEAM_USE_ES)

#ifndef GLEAM_RESTRICT_ES
#define GLEAM_RESTRICT_ES 0x400
#endif

#define GL_BASE_CORE_VERSION 0x000

#if defined(GLEAM_USE_LINKED)

#if C_HAS_INCLUDE(<GLES3/gl32.h>) && GLEAM_RESTRICT_ES >= 0x320
#include <GLES3/gl32.h>
#define GL_BASE_ES_VERSION 0x320
#elif C_HAS_INCLUDE(<GLES3/gl31.h>) && GLEAM_RESTRICT_ES >= 0x310
#include <GLES3/gl31.h>
#define GL_BASE_ES_VERSION 0x310
#elif C_HAS_INCLUDE(<GLES3/gl3.h>) && GLEAM_RESTRICT_ES >= 0x300
#include <GLES3/gl3.h>
#define GL_BASE_ES_VERSION 0x300
#elif C_HAS_INCLUDE(<OpenGLES/ES3/gl.h>)
#include <OpenGLES/ES3/gl.h>
#define GL_BASE_ES_VERSION 0x300
#elif C_HAS_INCLUDE(<GLES2/gl2.h>) && GLEAM_RESTRICT_ES >= 0x200
#include <GLES2/gl2.h>
#define GL_BASE_ES_VERSION 0x200
#else
#error Configured for linked GLES headers, but none found
#endif

#if C_HAS_INCLUDE(<GLES3 / gl3ext.h>) && GLEAM_RESTRICT_ES >= 0x300
#include <GLES3/gl3ext.h>
#endif
#if C_HAS_INCLUDE(<GLES2 / gl2ext.h>) && GLEAM_RESTRICT_ES == 0x200
#include <GLES2/gl2ext.h>
#endif

#else

#include <glad/glad.h>
#define GL_BASE_VERSION 0x320

#endif
#endif

#define GL_VERSION_VERIFY(CORE_VER, ES_VER) \
    GL_BASE_CORE_VERSION >= CORE_VER || GL_BASE_ES_VERSION >= ES_VER

namespace gl {

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
concept MinimumVersion = Current::major >= Required::major &&
                         (Current::major > Required::major ||
                          Current::minor >= Required::minor);

template<class Current, class Maximum>
concept MaximumVersion = Current::major < Maximum::major ||
                         (Current::major == Maximum::major &&
                          Current::minor < Maximum::minor);

using ::libc_types::ptroff;

namespace detail {

inline std::string error_to_hex(auto error)
{
    return stl_types::str::print::pointerify(error);
}

void error_check(std::string_view cmd_name);

} // namespace detail
} // namespace gl
