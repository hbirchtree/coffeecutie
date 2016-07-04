#include <coffee/core/plat/graphics/eglinit.h>

#include <coffee/core/plat/plat_memory.h>
#include "egltypes.h"

int32 Coffee_EGLInit(Coffee_EGLContext* ctxt, Coffee_EGLVisual const* request)
{
    ctxt = {};

    const EGLint attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_RENDERABLE_TYPE, request->renderable,
            EGL_SURFACE_TYPE, request->surface,
            EGL_BLUE_SIZE, request->b,
            EGL_GREEN_SIZE, request->g,
            EGL_RED_SIZE, request->r,
            EGL_DEPTH_SIZE, request->d,
            EGL_NONE
    };

    EGLint numCfgs;

    ctxt->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if(ctxt->display == EGL_NO_DISPLAY)
        return EGL_BAD_DISPLAY;

    if(eglInitialize(ctxt->display, &ctxt->major, &ctxt->minor) == EGL_FALSE)
        return EGL_BAD_CONFIG;

    {
        eglChooseConfig(ctxt->display, attribs, nullptr, 0, &numCfgs);
        auto configs = new EGLConfig[numCfgs];
        if(!configs)
            return EGL_BAD_CONFIG;
        ctxt->cfg.configs = configs;
        ctxt->cfg.numConfigurations = numCfgs;
    }

    return EGL_TRUE;
}

int32 Coffee_EGLConfig(Coffee_EGLContext* ctxt, EGLNativeWindowType win, Coffee_EGLVisual* current, EGLContext share)
{
    EGLDisplay dpy = ctxt->display;
    EGLint i;
    EGLConfig* select = nullptr;
    for(i=0;i<ctxt->cfg.numConfigurations;i++)
    {
        auto& cfg = ctxt->cfg.configs[i];
        EGLint r,g,b,d;
        if(eglGetConfigAttrib(dpy,cfg,EGL_RED_SIZE, &r) &&
                eglGetConfigAttrib(dpy,cfg,EGL_GREEN_SIZE, &g) &&
                eglGetConfigAttrib(dpy,cfg,EGL_BLUE_SIZE, &b) &&
                eglGetConfigAttrib(dpy,cfg,EGL_DEPTH_SIZE, &d) &&
                r == current->r && g == current->g && b == current->b && d == current->d)
        {
            select = &cfg;
            break;
        }
    }
    if(!select)
        return EGL_BAD_CONFIG;

    {
        EGLConfig* old = ctxt->cfg.configs;

        ctxt->cfg.numConfigurations = 1;
        ctxt->cfg.configs = new EGLConfig;

        Coffee::MemCpy(ctxt->cfg.configs,&old[i],sizeof(EGLConfig));
        delete[] old;
    }

    eglGetConfigAttrib(dpy,*select,EGL_NATIVE_VISUAL_ID,&current->fmt);
    ctxt->surface = eglCreateWindowSurface(dpy,*select,win,nullptr);

    if(!ctxt->surface)
        return EGL_BAD_SURFACE;

    return EGL_TRUE;
}

EGLContext Coffee_EGLCreateContext(Coffee_EGLContext* ctxt, EGLContext share)
{
    return eglCreateContext(ctxt->display,*ctxt->cfg.configs,share,nullptr);
}

bool Coffee_EGLConfigureSurface(Coffee_EGLContext* ctxt, Coffee_EGLVisual* current)
{
    if(eglMakeCurrent(ctxt->display, ctxt->surface, ctxt->surface, ctxt->context) == EGL_FALSE)
        return false;

    eglQuerySurface(ctxt->display,ctxt->surface, EGL_WIDTH, &current->w);
    eglQuerySurface(ctxt->display,ctxt->surface, EGL_HEIGHT, &current->h);

    return true;
}

int32 Coffee_EGLTerminate(Coffee_EGLContext* ctxt)
{
    return eglTerminate(ctxt->display);
}