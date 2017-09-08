#pragma once

/*
 * If you've come here, then I welcome you to hell!
 * This is a wrapper around Apple's EAGL API.
 *
 */

#include <stdint.h>
#include "eagl_types.h"

typedef void* EGLDisplay;
typedef void* EGLConfig;
typedef void* EGLSurface;
typedef void* EGLContext;

typedef void* EGLNativeDisplayType;
typedef void* EGLNativeWindowType;

typedef int32_t EGLint;
typedef uint32_t EGLuint;
typedef EGLint EGLboolean;

// Setup
EGLDisplay eglGetDisplay(EGLNativeDisplayType nativeDisplay);
EGLboolean eglInitialize(EGLDisplay display, void*, void*);

// Queries
EGLboolean eglQuerySurface(EGLDisplay display, EGLSurface surface,
                           EGLint attr, EGLint* value);
const char* eglQueryString(EGLDisplay display, EGLint attr);

// Setup
EGLboolean eglGetConfigAttrib(EGLDisplay display, EGLConfig cfg,
                              EGLint attr,
                              EGLint* value);
EGLboolean eglChooseConfig(EGLDisplay disp, EGLint const* preferred,
                           EGLConfig* config, EGLint numConfigs,
                           EGLint* pnumConfigs);

// Constructors
EGLContext eglCreateContext(EGLDisplay display, EGLConfig cfg,
                            EGLContext share, EGLint const* cfg_pref);
EGLSurface eglCreateWindowSurface(EGLDisplay display, EGLConfig cfg,
                                  EGLNativeWindowType window,
                                  EGLint const* attrs);

// Processing
EGLboolean eglSwapBuffers(EGLDisplay display, EGLSurface surface);
EGLboolean eglSwapInterval(EGLDisplay display, EGLint interval);

EGLboolean eglMakeCurrent(EGLDisplay display, EGLSurface surfaceDraw,
                          EGLSurface surfaceRead, EGLContext context);


// Destructors
void eglDestroySurface(EGLDisplay display, EGLSurface surface);
void eglDestroyContext(EGLDisplay display, EGLContext context);

EGLboolean eglTerminate(EGLDisplay display);

EGLint eglGetError(void);


#define EGL_NO_CONTEXT nullptr
#define EGL_NO_SURFACE nullptr
#define EGL_NO_DISPLAY nullptr

#define EGL_DEFAULT_DISPLAY nullptr
