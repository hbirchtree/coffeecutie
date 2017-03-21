#pragma once

#include <coffee/core/coffee_mem_macros.h>

#ifdef COFFEE_GLEAM_DESKTOP

#include <glad/glad.h>
#include <glad/KHR/khrplatform.h>

#else

#if defined(COFFEE_LINKED_GLES)
#include <GLES2/gl2.h>
#if defined(COFFEE_LINKED_GLES30)
#include <GLES3/gl3.h>
#endif
#if defined(COFFEE_LINKED_GLES31)
#include <GLES3/gl31.h>
#endif
#if defined(COFFEE_LINKED_GLES32)
#include <GLES3/gl32.h>
#endif
#else
#include <glad_es/glad.h>
#include <glad_es/KHR/khrplatform.h>
#endif

#endif
