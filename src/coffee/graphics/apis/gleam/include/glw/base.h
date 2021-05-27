#pragma once

#include <peripherals/identify/compiler/variable_attributes.h>
#include <peripherals/libc/types.h>

#define GL_BASE_ES_MASK 0x1000
#define GL_BASE_VERSION_MASK 0x00FF

#if defined(GLEAM_USE_CORE)

#define GL_BASE_ES_VERSION 0x000

#include <glad/KHR/khrplatform.h>
#include <glad/glad.h>

#define GL_BASE_CORE_VERSION 0x460

#elif defined(GLEAM_USE_ES)

#define GL_BASE_CORE_VERSION 0x000

#if defined(GLEAM_USE_LINKED)

#if C_HAS_INCLUDE(<GLES3 / glext3.h>)
#include <GLES3/glext3.h>
#endif
#if C_HAS_INCLUDE(<GLES2 / glext2.h>)
#include <GLES2/glext2.h>
#endif

#if C_HAS_INCLUDE(<GLES3 / gl32.h>)
#include <GLES3/gl32.h>
#define GL_BASE_ES_VERSION 0x320
#elif C_HAS_INCLUDE(<GLES3 / gl31.h>)
#include <GLES3/gl31.h>
#define GL_BASE_ES_VERSION 0x310
#elif C_HAS_INCLUDE(<GLES3 / gl3.h>)
#include <GLES3/gl3.h>
#define GL_BASE_ES_VERSION 0x300
#elif C_HAS_INCLUDE(<GLES2 / gl2.h>)
#include <GLES2/gl2.h>
#define GL_BASE_ES_VERSION 0x200
#else
#error Configured for linked GLES headers, but none found
#endif
#else

#include <glad_es/glad.h>
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

using ::libc_types::ptroff;

} // namespace gl
