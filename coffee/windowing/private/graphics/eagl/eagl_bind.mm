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
    return EGL_TRUE;
}

const char* eglQueryString(EGLDisplay display, EGLint attr)
{
    return nullptr;
}

EGLboolean eglGetConfigAttrib(EGLDisplay display, EGLConfig cfg,
                              EGLint attr,
                              EGLint* value)
{
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
    if(display)
    {
        EGLView* view = (EGLView*)display;
        // Do something to swap the buffers
        return EGL_TRUE;
    }
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


void eglDestroySurface(EGLDisplay display, EGLSurface surface)
{
}
void eglDestroyContext(EGLDisplay display, EGLContext context)
{
}

EGLboolean eglTerminate(EGLDisplay display)
{
    return EGL_TRUE;
}

EGLint eglGetError(void)
{
    return 0;
}
