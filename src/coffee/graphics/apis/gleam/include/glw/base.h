#pragma once

#include <peripherals/identify/compiler/variable_attributes.h>

#if defined(GLEAM_USE_CORE)

#include <glad/KHR/khrplatform.h>
#include <glad/glad.h>

#elif defined(GLEAM_USE_ES)
#if defined(GLEAM_USE_LINKED)

#if C_HAS_INCLUDE(<GLES3/glext3.h>)
#include <GLES3/glext3.h>
#endif
#if C_HAS_INCLUDE(<GLES2/glext2.h>)
#include <GLES2/glext2.h>
#endif

#if C_HAS_INCLUDE(<GLES3/gl32.h>)
#include <GLES3/gl32.h>
#elif C_HAS_INCLUDE(<GLES3/gl31.h>)
#include <GLES3/gl31.h>
#elif C_HAS_INCLUDE(<GLES3/gl3.h>)
#include <GLES3/gl3.h>
#elif C_HAS_INCLUDE(<GLES2/gl2.h>)
#include <GLES2/gl2.h>
#else
#error Configured for linked GLES headers, but none found
#endif
#endif

#include <glad_es/glad.h>

#endif

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
concept MinimumVersion =
    Current::major >= Required::major &&
    (Current::major > Required::major || Current::minor >= Required::minor);

}
