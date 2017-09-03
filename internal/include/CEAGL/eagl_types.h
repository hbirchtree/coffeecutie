#pragma once

enum EGLvalues {
    EGL_NONE = 0,
    
    EGL_FALSE = 0,
    EGL_TRUE = 1,
    
    // Configuration properties
    EGL_RED_SIZE = 0x100,
    EGL_GREEN_SIZE,
    EGL_BLUE_SIZE,
    EGL_ALPHA_SIZE,
    
    EGL_DEPTH_SIZE,
    EGL_STENCIL_SIZE,
    EGL_SAMPLES,
    EGL_BUFFER_SIZE,
    
    // Surface properties
    EGL_WIDTH = 0x140,
    EGL_HEIGHT,
    EGL_MAX_SWAP_INTERVAL,
    EGL_MIN_SWAP_INTERVAL,
    
    // Context properties
    EGL_CONTEXT_CLIENT_VERSION = 0x200,
    EGL_COLOR_BUFFER_TYPE,
    EGL_SURFACE_TYPE,
    EGL_RENDERABLE_TYPE,
    
    // EGL properties
    EGL_VENDOR = 0x400,
    EGL_VERSION,
    EGL_EXTENSIONS,
    EGL_CLIENT_APIS,
    
    // Context attribute settings
    EGL_OPENGL_ES2_BIT = 0x800,
    EGL_OPENGL_ES3_BIT,
    EGL_WINDOW_BIT,
    EGL_RGB_BUFFER,
    EGL_CONFIG_CAVEAT,
    EGL_CONFORMANT,
};
