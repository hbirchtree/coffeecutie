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

// Desktop GL functions, up to OpenGL 4.5
#include <glad/glad.h>
#include <glad/KHR/khrplatform.h>

#define GL_VERSION_BASE_CORE 0xFFFF
#define GL_VERSION_BASE_ES   0x0

#else

#define GL_VERSION_BASE_CORE 0x0

#if defined(COFFEE_LINKED_GLES)

#if defined(COFFEE_WINDOWS)
// I believe ANGLE could choke on a glass of water.
#define GL_GLEXT_PROTOTYPES
#endif

#if defined(COFFEE_APPLE)
// Apple loads the functions from a framework
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define GL_VERSION_BASE_ES 0x200

#else

#if defined(COFFEE_LINKED_GLES32)
#include <GLES3/gl32.h>
#ifndef GL_VERSION_BASE_ES
#define GL_VERSION_BASE_ES 0x320
#endif
#endif
#if defined(COFFEE_LINKED_GLES31)
#include <GLES3/gl31.h>
#ifndef GL_VERSION_BASE_ES
#define GL_VERSION_BASE_ES 0x310
#endif
#endif
#if defined(COFFEE_LINKED_GLES30)
#include <GLES3/gl3.h>
#ifndef GL_VERSION_BASE_ES
#define GL_VERSION_BASE_ES 0x300
#endif
#endif

// Standard headers for GLES functions
#include <GLES2/gl2.h>

#ifndef GL_VERSION_BASE_ES
#define GL_VERSION_BASE_ES 0x200
#endif

// GLES3.x is optional

#endif

#else
// Runtime-loaded OpenGL functions
#include <glad_es/glad.h>
#include <glad_es/KHR/khrplatform.h>

#define GL_VERSION_BASE_ES 0xFFFF

#endif

#endif
