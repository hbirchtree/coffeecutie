#include <CEAGL/eagl.h>

#include "EGLView.h"


EGLDisplay eglGetDisplay(EGLNativeDisplayType nativeDisplay)
{
    return [EGLView createView];
}

EGLboolean eglInitialize(EGLDisplay display, void*, void*)
{
    if(display)
    {
        EGLView* view = (EGLView*)display;
        [view createContext: EGL_OPENGL_ES2_BIT];
        return EGL_TRUE;
    }
    return EGL_FALSE;
}

EGLboolean eglQuerySurface(EGLDisplay display, EGLSurface surface,
                           EGLint attr, EGLint* value)
{
    EGLView* view = (EGLView*)display;
    switch(attr)
    {
    case EGL_WIDTH:
        *value = (int)[[view getView] drawableWidth];
        break;
    case EGL_HEIGHT:
        *value = (int)[[view getView] drawableHeight];
        break;
    case EGL_MAX_SWAP_INTERVAL:
        *value = 1;
        break;
    case EGL_MIN_SWAP_INTERVAL:
        *value = 1;
        break;
    }
    return EGL_TRUE;
}

const char* eglQueryString(EGLDisplay display, EGLint attr)
{
    //EGLView* view = (EGLView*)display;
    switch(attr)
    {
    case EGL_VENDOR:
        return "Apple EAGL";
    case EGL_VERSION:
        return "1.2 Coffee EAGL-to-EGL layer";
    case EGL_EXTENSIONS:
        return "";
    case EGL_CLIENT_APIS:
        return "OpenGL_ES";
    }
    return nullptr;
}

static inline int ColorFormatToChannels(GLKViewDrawableColorFormat fmt,
                                        int color)
{
    switch(fmt)
    {
    case GLKViewDrawableColorFormatSRGBA8888:
    case GLKViewDrawableColorFormatRGBA8888:
        return 8;
    case GLKViewDrawableColorFormatRGB565:
        {
            switch(color)
            {
            case 0:
                return 5;
            case 1:
                return 6;
            case 2:
                return 5;
            case 3:
                return 0;
            }
        }
    default:
        return 0;
    }
}

static inline int DepthFormatToInt(GLKViewDrawableDepthFormat fmt)
{
    switch(fmt)
    {
    case GLKViewDrawableDepthFormat16:
        return 16;
    case GLKViewDrawableDepthFormat24:
        return 24;
    default:
        return 0;
    }
}

static inline int StencilFormatToInt(GLKViewDrawableStencilFormat fmt)
{
    switch(fmt)
    {
    case GLKViewDrawableStencilFormat8:
        return 8;
    default:
        return 0;
    }
}

static inline int MultisampleToInt(GLKViewDrawableMultisample fmt)
{
    switch(fmt)
    {
    case GLKViewDrawableMultisample4X:
        return 4;
    default:
        return 1;
    }
}

EGLboolean eglGetConfigAttrib(EGLDisplay display, EGLConfig cfg,
                              EGLint attr,
                              EGLint* value)
{
    EGLView* view = (EGLView*)display;
    
    GLKViewDrawableColorFormat c_fmt = [[view getView] drawableColorFormat];
    
    switch(attr)
    {
    case EGL_RED_SIZE:
        *value = ColorFormatToChannels(c_fmt, 0);
        break;
    case EGL_GREEN_SIZE:
        *value = ColorFormatToChannels(c_fmt, 1);
        break;
    case EGL_BLUE_SIZE:
        *value = ColorFormatToChannels(c_fmt, 2);
        break;
    case EGL_ALPHA_SIZE:
        *value = ColorFormatToChannels(c_fmt, 3);
        break;
    
    
    case EGL_DEPTH_SIZE:
        *value = DepthFormatToInt([[view getView] drawableDepthFormat]);
        break;
    case EGL_STENCIL_SIZE:
        *value = StencilFormatToInt([[view getView] drawableStencilFormat]);
        break;
        
    case EGL_BUFFER_SIZE:
        *value = ColorFormatToChannels(c_fmt, 0)
            + ColorFormatToChannels(c_fmt, 1)
            + ColorFormatToChannels(c_fmt, 2);
        break;
        
    case EGL_SAMPLES:
        *value = MultisampleToInt([[view getView] drawableMultisample]);
        break;
    
    default:
        return EGL_FALSE;
    }
    return EGL_TRUE;
}

EGLboolean eglChooseConfig(EGLDisplay disp, EGLint const* preferred,
                           EGLConfig* config, EGLint numConfigs,
                           EGLint* pnumConfigs)
{
    *pnumConfigs = 1;
    return EGL_TRUE;
}

EGLContext eglCreateContext(EGLDisplay display, EGLConfig cfg,
                            EGLContext share, EGLint const* cfg_pref)
{
    if(display)
    {
        EGLView* view = (EGLView*)display;
        return [view getContext];
    }
    return EGL_NO_CONTEXT;
}
EGLSurface eglCreateWindowSurface(EGLDisplay display, EGLConfig cfg,
                                  EGLNativeWindowType window,
                                  EGLint const* attrs)
{
    if(display)
    {
        EGLView* view = (EGLView*)display;
        [view createView];
        
        return [view getView];
    }
    return EGL_NO_SURFACE;
}


EGLboolean eglSwapBuffers(EGLDisplay display, EGLSurface surface)
{
    // Swapping is done outside our control

    if(display)
        return EGL_TRUE;
    return EGL_FALSE;
}

EGLboolean eglSwapInterval(EGLDisplay display, EGLint interval)
{
    return EGL_TRUE;
}

EGLboolean eglMakeCurrent(EGLDisplay display, EGLSurface surfaceDraw,
                          EGLSurface surfaceRead, EGLContext context)
{
    if(context)
    {
        EAGLContext* ctxt = (EAGLContext*)context;
        return [EAGLContext setCurrentContext:ctxt];
    }
    return EGL_FALSE;
}


void eglDestroySurface(EGLDisplay, EGLSurface)
{
}
void eglDestroyContext(EGLDisplay, EGLContext)
{
}

EGLboolean eglTerminate(EGLDisplay display)
{
    EGLView* view = (EGLView*)display;
    
    if([EAGLContext currentContext])
    {
        EAGLContext* ctx = [EAGLContext currentContext];
        [EAGLContext setCurrentContext:nil];
        [ctx dealloc];
    }
    
    [view dealloc];
    
    return EGL_TRUE;
}

EGLint eglGetError(void)
{
    return 0;
}
