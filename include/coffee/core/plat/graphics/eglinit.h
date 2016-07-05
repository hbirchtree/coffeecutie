#pragma once
#include <EGL/egl.h>

#include <coffee/core/types/tdef/integertypes.h>

using namespace Coffee;

struct Coffee_EGLConfigurations
{
    EGLConfig* configs;
    EGLint numConfigurations;
};

struct Coffee_EGLDisplay
{
    EGLDisplay display;
    EGLint major,minor;
};

struct Coffee_EGLContext
{
    EGLSurface surface;
    EGLContext context;
    Coffee_EGLConfigurations cfg;
};

struct Coffee_EGLVisual
{
    EGLint renderable,surface;
    EGLint r,g,b,d;
};

namespace Coffee{
namespace CEGL{
    extern int32 Init(Coffee_EGLDisplay* ctxt);

    extern int32 GetConfigs(Coffee_EGLDisplay* dpy, Coffee_EGLContext* ctx,
                            Coffee_EGLVisual const* request);

    extern int32 CreateSurface(Coffee_EGLDisplay* disp, Coffee_EGLContext* ctxt,
                               EGLNativeWindowType win, Coffee_EGLVisual* current);

    extern EGLContext CreateContext(Coffee_EGLDisplay* dpy,
                                    EGLConfig cfg, EGLContext share);

    extern bool ConfigureSurface(Coffee_EGLDisplay* dpy, Coffee_EGLContext* ctxt);

    extern void DestroyContext(Coffee_EGLDisplay* dpy, Coffee_EGLContext* ctxt);

    extern int32 Terminate(Coffee_EGLDisplay* ctxt);
}
}
