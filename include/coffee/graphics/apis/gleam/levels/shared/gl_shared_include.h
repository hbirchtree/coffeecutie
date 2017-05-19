#pragma once

/*!
 * \file gl_shared_include.h
 * \brief
 * GLES comes in two forms:
 *  - Header-defined functions + linked functions (Maemo, Android, NaCL, Emscripten)
 *  - Dynamically loaded functions from the OS (Android, Linux, Mac OSX, Windows)
 *
 * Dynamically loaded will occupy more symbols and code.
 * It will probably be slower compared to the linked option.
 * Dynamically loaded will be more flexible on platforms with multiple versions
 *  available with a single, statically linked binary.
 * Linked alternative works well on restricted or otherwise limited platforms.
 *
 * Android can do both, but we use dynamic loading to avoid separate GLESv2 and GLESv3 loading.
 *
 * OpenGL on desktop only dynamically loads functions, as this works everywhere.
 */

#include <coffee/core/coffee_mem_macros.h>

#ifdef COFFEE_GLEAM_DESKTOP

#include <glad/glad.h>
#include <glad/KHR/khrplatform.h>

#else


#if defined(COFFEE_LINKED_GLES)
#if defined(COFFEE_WINDOWS)
#define GL_GLEXT_PROTOTYPES // I believe ANGLE could choke on a glass of water.
#endif
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
