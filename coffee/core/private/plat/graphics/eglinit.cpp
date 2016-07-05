#include <coffee/core/plat/graphics/eglinit.h>
#include <coffee/core/CDebug>

#include <coffee/core/plat/plat_memory.h>
//#include "egltypes.h"

namespace Coffee {

int32 CEGL::Init(Coffee_EGLDisplay *ctxt) {
    *ctxt = {};

    ctxt->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (ctxt->display == EGL_NO_DISPLAY) {
        return EGL_BAD_DISPLAY;
    }

    if (eglInitialize(ctxt->display, &ctxt->major, &ctxt->minor) == EGL_FALSE) {
        return EGL_BAD_CONFIG;
    }

    return EGL_TRUE;
}

int32 CEGL::GetConfigs(Coffee_EGLDisplay *dpy, Coffee_EGLContext *ctx,
                           Coffee_EGLVisual const *request) {
    const EGLint confAttrib[] = {
            EGL_RENDERABLE_TYPE, request->renderable,
            EGL_SURFACE_TYPE, request->surface,
            EGL_BLUE_SIZE, request->b,
            EGL_GREEN_SIZE, request->g,
            EGL_RED_SIZE, request->r,
            EGL_NONE
    };

    EGLint numCfgs;

    {
        eglChooseConfig(dpy->display, confAttrib, nullptr, 0, &numCfgs);
        auto configs = new EGLConfig[numCfgs];
        if (!configs)
            return EGL_BAD_CONFIG;
        ctx->cfg.configs = configs;
        ctx->cfg.numConfigurations = numCfgs;
    }
    return EGL_TRUE;
}

int32 CEGL::CreateSurface(Coffee_EGLDisplay *disp, Coffee_EGLContext *ctxt,
                              EGLNativeWindowType win, Coffee_EGLVisual *current) {
    EGLDisplay dpy = disp->display;
    EGLint i;
    EGLConfig *select = nullptr;
    cDebug("Surface configurations: {0}",ctxt->cfg.numConfigurations);

    if(ctxt->cfg.numConfigurations > 0)
    {
        i = 0;
        select = ctxt->cfg.configs;
    }

//    for (i = 0; i < ctxt->cfg.numConfigurations; i++) {
//        auto &cfg = ctxt->cfg.configs[i];
//        EGLint w = 0, h = 0;
//        eglGetConfigAttrib(dpy, cfg, EGL_WIDTH, &w);
//        eglGetConfigAttrib(dpy, cfg, EGL_HEIGHT, &h);
//        cDebug("Resolution: {0}x{1}", w, h);
//        EGLint r = 0, g = 0, b = 0;
//        if (eglGetConfigAttrib(dpy, cfg, EGL_RED_SIZE, &r) &&
//            eglGetConfigAttrib(dpy, cfg, EGL_GREEN_SIZE, &g) &&
//            eglGetConfigAttrib(dpy, cfg, EGL_BLUE_SIZE, &b) &&
//            r == current->r && g == current->g && b == current->b) {
//            select = &cfg;
//            break;
//        }else{
//            cDebug("Mismatch: R{0}G{1}B{2}",r,g,b);
//        }
//    }
//    if (!select) {
//        return EGL_BAD_CONFIG;
//    }

    cDebug("Configurations: {0}, i={1}", ctxt->cfg.numConfigurations, i);

    {
        EGLConfig *old = ctxt->cfg.configs;

        ctxt->cfg.numConfigurations = 1;
        ctxt->cfg.configs = new EGLConfig;

        Coffee::MemCpy(ctxt->cfg.configs, &old[i], sizeof(EGLConfig));
        delete[] old;
    }

    ctxt->surface = eglCreateWindowSurface(dpy, *select, win, nullptr);

    if (!ctxt->surface)
        return EGL_BAD_SURFACE;

    return EGL_TRUE;
}

EGLContext CEGL::CreateContext(Coffee_EGLDisplay *dpy, EGLConfig cfg, EGLContext share) {
    const EGLint ctxAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE,
    };

    return eglCreateContext(dpy->display, cfg, share, ctxAttribs);
}

bool CEGL::ConfigureSurface(Coffee_EGLDisplay *dpy, Coffee_EGLContext *ctxt) {
    return eglMakeCurrent(dpy->display, ctxt->surface, ctxt->surface, ctxt->context) != EGL_FALSE;
}

void CEGL::DestroyContext(Coffee_EGLDisplay *dpy, Coffee_EGLContext *ctxt) {
    eglDestroyContext(dpy->display, ctxt->context);
    eglDestroySurface(dpy->display, ctxt->surface);
}

int32 CEGL::Terminate(Coffee_EGLDisplay *ctxt) {
    return eglTerminate(ctxt->display);
}
}