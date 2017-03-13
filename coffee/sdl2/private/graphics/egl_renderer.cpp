#include <coffee/sdl2/graphics/egl_renderer.h>

#include <coffee/sdl2/windowing/csdl2_window.h>
#include "../types/sdl2datatypes.h"

#include <coffee/core/CDebug>
#include <coffee/core/string_casting.h>

#if defined(COFFEE_USE_MAEMO_EGL)

#include <EGL/egl.h>

#define EGL_ERRORCHECK(result) \
    if(result != EGL_TRUE) \
    { \
        EGLint error = eglGetError(); \
        cVerbose(5, "@" #result ": " "EGL error: {0}", error); \
        *err = cast_pod<>(error); \
        return false; \
    }

#define EGL_ERRORCHECK_N(result) \
    if(result != EGL_TRUE) \
    { \
        EGLint error = eglGetError(); \
        cVerbose(5, "@" #result ": " "EGL error: {0}", error); \
        return false; \
    }

#define EGL_NULLCHECK(result, error_text) \
    if(!result) \
    { \
        cVerbose(5, "@" #result ": " error_text); \
        return false; \
    }

namespace Coffee{
namespace Display{

#define BITS_ASSIGN(var, property) \
    EGL_ERRORCHECK_N(eglGetConfigAttrib(disp, cfg, property, &prop)); \
    bits.var = C_CAST<uint8>(prop);

bool egl_config_to_bits(EGLDisplay disp, EGLConfig cfg, CDContextBits& bits)
{
    EGLint prop = 0;
    BITS_ASSIGN(red, EGL_RED_SIZE);
    BITS_ASSIGN(green, EGL_GREEN_SIZE);
    BITS_ASSIGN(blue, EGL_BLUE_SIZE);
    BITS_ASSIGN(alpha, EGL_ALPHA_SIZE);
    BITS_ASSIGN(depth, EGL_DEPTH_SIZE);
    BITS_ASSIGN(stencil, EGL_STENCIL_SIZE);
    BITS_ASSIGN(samples, EGL_SAMPLES);
    return true;
}

#undef BITS_ASSIGN

struct EGL_Data
{
    EGLDisplay display;
    EGLSurface surface;
    EGLConfig config;
    EGLContext context;
};

EGLRenderer::EGLRenderer()
{
}

EGLRenderer::~EGLRenderer()
{
}

bool EGLRenderer::contextPreInit(const GLProperties &props, CString *)
{
    C_UNUSED(props);
    return true;
}

bool EGLRenderer::contextInit(const GLProperties &props, CString *err)
{
    C_UNUSED(props);

    m_eglData = std::unique_ptr<EGL_Data>(new EGL_Data);
    m_eglData->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGL_NULLCHECK(m_eglData->display, "Failed to retrieve EGL display");

    EGL_ERRORCHECK(eglInitialize(m_eglData->display, nullptr, nullptr));

    EGLint config_num = 0;

    {
        /* Set up preferred configuration */
        Vector<EGLint> config_preferred = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,

            EGL_RED_SIZE, props.bits.red,
            EGL_GREEN_SIZE, props.bits.green,
            EGL_BLUE_SIZE, props.bits.blue,
            EGL_ALPHA_SIZE, props.bits.alpha,

            EGL_STENCIL_SIZE, props.bits.stencil,
            EGL_DEPTH_SIZE, props.bits.depth,

            EGL_SAMPLES, props.bits.samples,
            EGL_NONE
        };

        eglChooseConfig(m_eglData->display, &config_preferred[0], nullptr, 0, &config_num);

        cVerbose(6, "Number of FB configs: {0}", config_num);
        if(config_num < 1)
            return false;

        Vector<EGLConfig> configurations;
        configurations.resize(config_num);

        eglChooseConfig(m_eglData->display, &config_preferred[0],
                &configurations[0], config_num, &config_num);

        m_eglData->config = configurations[0];
        cVerbose(8, "Picked EGLConfig: {0}", (u64)m_eglData->config);
        CDContextBits bits;
        for(EGLConfig const& cfg : configurations)
        {
            egl_config_to_bits(m_eglData->display, m_eglData->config, bits);
            cVerbose(8, "EGLConfig information: {0}", bits);
        }
    }

    {
        Vector<EGLint> config_preferred = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE,
        };

        m_eglData->context = eglCreateContext(m_eglData->display, m_eglData->config,
                                              EGL_NO_CONTEXT, &config_preferred[0]);

        if(m_eglData->context == EGL_NO_CONTEXT)
        {
            cWarning("Failed to create EGL context: {0}", eglGetError());
            return false;
        }
    }

    return true;
}

bool EGLRenderer::contextPostInit(const GLProperties &props, CString *err)
{
    /* And now we perform voodoo-magic.
     * We need a valid X window manager handle for our window.
     * Where do we get it? The moon.
     */
    SDL2Window* its_a_me_mario = C_DCAST<SDL2Window>(this);
    SDL_SysWMinfo wminfo;
    if(SDL_GetWindowWMInfo(its_a_me_mario->getSDL2Context()->window, &wminfo) == SDL_FALSE)
    {
        cDebug("Failed to retrieve window information");
        return false;
    }

    cDebug("WM variant: {0}", (u32)wminfo.subsystem);
    cVerbose(8, "X11 window handle: {0}", (uint64_t const&)wminfo.info.x11.window);

    m_eglData->surface = eglCreateWindowSurface(m_eglData->display, m_eglData->config,
                                                wminfo.info.x11.window, nullptr);

    if(m_eglData->surface == EGL_NO_SURFACE)
    {
        cWarning("Failed to create surface: {0}", eglGetError());
        return false;
    }
    return true;
}

void EGLRenderer::contextTerminate()
{
    if(!m_eglData)
        return;

    if(!m_eglData->display)
    {
        m_eglData.reset(nullptr);
        return;
    }

    if(m_eglData->surface)
        eglDestroySurface(m_eglData->display, m_eglData->surface);

    if(m_eglData->context)
        eglDestroyContext(m_eglData->display, m_eglData->context);

    eglTerminate(m_eglData->display);

    m_eglData.reset(nullptr);
}

void EGLRenderer::swapBuffers()
{

}

CSize EGLRenderer::framebufferSize() const
{

}

int EGLRenderer::swapInterval() const
{

}

void EGLRenderer::setSwapInterval(const int &i)
{

}

CDContextBits EGLRenderer::context()
{
    return {};
}

ThreadId EGLRenderer::contextThread()
{
    return ThreadId();
}

CGL::CGL_Context *EGLRenderer::glContext()
{

}

CGL::CGL_ScopedContext EGLRenderer::scopedContext()
{

}

}
}
#endif
