#pragma once
#include <EGL/egl.h>

#include <coffee/core/types/tdef/integertypes.h>

using namespace Coffee;

struct Coffee_EGLContext;
struct Coffee_EGLVisual;

int32 Coffee_EGLInit(Coffee_EGLContext* ctxt, Coffee_EGLVisual const* request);

int32 Coffee_EGLConfig(Coffee_EGLContext* ctxt, EGLNativeWindowType win,
                       Coffee_EGLVisual* current, EGLContext share);

EGLContext Coffee_EGLCreateContext(Coffee_EGLContext* ctxt, EGLContext share);

bool Coffee_EGLConfigureSurface(Coffee_EGLContext* ctxt, Coffee_EGLVisual* current);

int32 Coffee_EGLTerminate(Coffee_EGLContext* ctxt);