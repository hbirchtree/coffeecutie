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

struct EGL_GL_Context : public CGL::CGL_Context, public CObject
{
private:
    EGL_Data& data;
    ThreadId m_threadId;
public:
    EGL_GL_Context(EGLRenderer& renderer):
        CObject(nullptr),
        data(*renderer.m_eglData)
    {
    }

    virtual bool acquireContext()
    {
        EGL_ERRORCHECK_N(eglMakeCurrent(data.display, data.surface, data.surface, data.context));
        new (&m_threadId) ThreadId;
        return true;
    }
    virtual bool releaseContext()
    {
        EGL_ERRORCHECK_N(eglMakeCurrent(data.display, nullptr, nullptr, nullptr));
    }
    virtual ThreadId const& currentThread()
    {
        return m_threadId;
    }
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
    m_eglData->surface = nullptr;
    m_eglData->context = nullptr;
    m_eglData->config = nullptr;

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

            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,

            EGL_NONE
        };

        eglChooseConfig(m_eglData->display, &config_preferred[0], nullptr, 0, &config_num);

        cVerbose(6, "Number of FB configs: {0}", config_num);
        if(config_num < 1)
            return false;

        Vector<EGLConfig> configurations;
        configurations.resize(config_num);

        EGL_ERRORCHECK(eglChooseConfig(m_eglData->display, &config_preferred[0],
                       &configurations[0], config_num, &config_num));

        m_eglData->config = configurations[3];
        cVerbose(7, "Picked EGLConfig: {0}", (u64)m_eglData->config);
        CDContextBits bits;
        for(EGLConfig const& cfg : configurations)
        {
            egl_config_to_bits(m_eglData->display, cfg, bits);
            cVerbose(8, "EGLConfig information:{1}: {0}", bits, (u64)cfg);
        }
    }

    {
        Vector<EGLint> config_preferred = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE,
        };

        m_eglData->context = eglCreateContext(m_eglData->display, m_eglData->config,
                                              EGL_NO_CONTEXT, &config_preferred[0]);

        EGL_NULLCHECK(m_eglData->context, "Failed to create EGL context");
    }

    /* And now we perform voodoo-magic.
     * We need a valid X window manager handle for our window.
     * Where do we get it? The moon.
     */
    SDL2Window* its_a_me_mario = C_DCAST<SDL2Window>(this);
    SDL_SysWMinfo wminfo;
    cVerbose(8, "Window pointer {0}", (u64)its_a_me_mario->getSDL2Context()->window);
    SDL_VERSION(&wminfo.version);
    if(SDL_GetWindowWMInfo(its_a_me_mario->getSDL2Context()->window, &wminfo) == SDL_FALSE)
    {
        cDebug("Failed to retrieve window information");
        return false;
    }

    cVerbose(8, "WM variant: {0}", (u32)wminfo.subsystem);
    cVerbose(8, "X11 window handle: {0}", (u64)wminfo.info.x11.window);
    cVerbose(8, "Config for surface: {0}", (u64)m_eglData->config);

    cVerbose(8, "Calling eglCreateWindowSurface...");
    m_eglData->surface = eglCreateWindowSurface(m_eglData->display, m_eglData->config,
                                                wminfo.info.x11.window, nullptr);
    cVerbose(8, "eglCreateWindowSurface returned!");
    EGL_NULLCHECK(m_eglData->surface, "Failed to create surface");

    cVerbose(8, "Null check succeeded");

    return glContext()->acquireContext();
}

bool EGLRenderer::contextPostInit(const GLProperties &props, CString *err)
{
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
    CDContextBits bits;
    egl_config_to_bits(m_eglData->display, m_eglData->config, bits);
    return bits;
}

ThreadId EGLRenderer::contextThread()
{
    return ThreadId();
}

CGL::CGL_Context *EGLRenderer::glContext()
{
    static EGL_GL_Context ctxt(*this);
    return &ctxt;
}

CGL::CGL_ScopedContext EGLRenderer::scopedContext()
{
    return CGL::CGL_ScopedContext(glContext());
}

}
}
#endif
