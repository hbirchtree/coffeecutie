#pragma once

/*
 *
 * OpenGL/ES
 *
 */

#if defined(COFFEE_RASPBERRYPI) || defined(COFFEE_MAEMO)
#define COFFEE_LINKED_GLES
#endif

#if defined(COFFEE_RASPBERRYPI) || defined(COFFEE_MAEMO)
#define COFFEE_ONLY_GLES20
#endif

#if defined(COFFEE_APPLE_MOBILE) || defined(COFFEE_GLES20_MODE)
#define COFFEE_LINKED_GLES
#define COFFEE_ONLY_GLES20
#endif

#define COFFEE_GLES20_EXTENSIONS

#if defined(COFFEE_EMSCRIPTEN)
#define COFFEE_WEBGL
#define COFFEE_LINKED_GLES
#define COFFEE_LINKED_GLES20
#endif

/*
 *
 * Display stack
 *
 */

#if defined(COFFEE_RASPBERRYPI) || defined(COFFEE_MAEMO) || \
    defined(COFFEE_ANDROID)
#define COFFEE_DISABLE_SRGB_SUPPORT
#define COFFEE_USE_IMMERSIVE_VIEW
#define COFFEE_ALWAYS_VSYNC
#endif

#if defined(COFFEE_MAEMO)
/* This flag indicates that the platform
 * has a fixed resolution and color depth. */
#define COFFEE_FRAGILE_FRAMEBUFFER
/* Maemo 5 runs with a fixed 60Hz Vsync */
#define COFFEE_ALWAYS_VSYNC
/* There are a couple of calls for X specific to Hildon */
#define COFFEE_X11_HILDON
#endif

#if defined(COFFEE_WINDOWS) && defined(COFFEE_GLES20_MODE)
#define COFFEE_USE_MAEMO_EGL
#define COFFEE_USE_WINDOWS_ANGLE
#endif

#if defined(COFFEE_ANDROID)
#define COFFEE_USE_ANDROID_NATIVEWIN
#define COFFEE_USE_MAEMO_EGL
#endif

#if defined(COFFEE_APPLE_MOBILE)
#define COFFEE_USE_APPLE_GLKIT
#define COFFEE_USE_MAEMO_EGL
#endif

#if defined(COFFEE_LINUX_LIGHTWEIGHT_WM) || defined(COFFEE_MAEMO)
/* This is the super-fast X11 combination, SDL2 can't even
 *  be compared to this */
/* The window is guaranteed to show within 500ms */
#if !defined(COFFEE_GLEAM_DESKTOP)
/* You cannot load OpenGL (non-ES) with EGL :( */
#define COFFEE_USE_MAEMO_EGL
#else
#define COFFEE_USE_LINUX_GLX
#endif
#define COFFEE_USE_MAEMO_X11
#endif

/*
 *
 * SDL stuff, because it is default
 *
 */

#if !defined(COFFEE_RASPBERRY_DMX) && !defined(COFFEE_USE_LINUX_GLX) && \
    !defined(COFFEE_USE_MAEMO_EGL) && !defined(COFFEE_USE_MAEMO_X11) && \
    defined(COFFEE_USE_SDL2)
#define COFFEE_USE_SDL_GL
#endif

#if !defined(COFFEE_RASPBERRY_DMX) && !defined(COFFEE_USE_LINUX_GLX) && \
    !defined(COFFEE_USE_MAEMO_X11) && defined(COFFEE_USE_SDL2)
#define COFFEE_USE_SDL_WINDOW
#endif

#if !defined(COFFEE_RASPBERRY_DMX) && !defined(COFFEE_USE_MAEMO_X11) && \
    defined(COFFEE_USE_SDL2)
#define COFFEE_USE_SDL_EVENT
#endif
