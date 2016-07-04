#pragma once

#include <EGL/egl.h>

struct Coffee_EGLConfigurations
{
    EGLConfig* configs;
    EGLint numConfigurations;
};

struct Coffee_EGLContext
{
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    Coffee_EGLConfigurations cfg;

    EGLint major,minor;
};

struct Coffee_EGLVisual
{
    EGLint renderable,surface;
    EGLint r,g,b,d;
    EGLint w,h,fmt;
};