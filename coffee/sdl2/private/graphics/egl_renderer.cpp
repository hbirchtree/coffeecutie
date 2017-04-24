#include <coffee/sdl2/graphics/egl_renderer.h>

#if defined(COFFEE_USE_MAEMO_EGL)

#include <coffee/core/plat/windowmanager/plat_windowtype.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#include <coffee/core/CDebug>
#include <coffee/core/string_casting.h>

#include <iostream>

const constexpr static int EGL_MIN_VERB = 5;

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#define EGL_ERRORCHECK(result) \
    if(result != EGL_TRUE) \
    { \
        EGLint error = eglGetError(); \
        cVerbose(5, "@" #result ": " "EGL error: {0}", StrUtil::hexify(error)); \
        if(err) \
            *err = cast_pod<>(error); \
        return false; \
    }

#define EGL_ERRORCHECK_N(result) \
    if(result != EGL_TRUE) \
    { \
        EGLint error = eglGetError(); \
        cVerbose(5, "@" #result ": " "EGL error: {0}", StrUtil::hexify(error)); \
        return false; \
    }

#define EGL_ERRORCHECK_NOBOOL(result) \
    if(result != EGL_TRUE) \
    { \
        EGLint error = eglGetError(); \
        cVerbose(5, "@" #result ": " "EGL error: {0}", StrUtil::hexify(error)); \
    }

#define EGL_NULLCHECK(result, error_text) \
    if(!result) \
    { \
        cVerbose(5, "@" #result ": " error_text); \
        if(err) \
            *err = error_text; \
        return false; \
    }

#define EGL_VERIFYCONTEXT() \
    if(!m_eglData \
            || m_eglData->display == EGL_NO_DISPLAY \
            || m_eglData->surface == EGL_NO_SURFACE)

#if C_SYSTEM_BITNESS == 32
#define EGL_PRINT(id) cast_pod<u32>((u32)(id))
#else
#define EGL_PRINT(id) cast_pod<u64>((u64)(id))
#endif

namespace Coffee{
namespace Display{

#define BITS_ASSIGN(var, property) \
    EGL_ERRORCHECK_N(eglGetConfigAttrib(disp, cfg, property, &prop)); \
    bits.var = C_CAST<uint8>(prop);

#define EGL_PRINT_VALUE(property) \
    EGL_ERRORCHECK_N(eglGetConfigAttrib(disp, cfg, property, &prop)); \
    cVerbose(EGL_MIN_VERB, "Config {0}: EGL property" #property ": {1}", EGL_PRINT(cfg), prop);

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

    BITS_ASSIGN(buffer_size, EGL_BUFFER_SIZE);

//    EGL_PRINT_VALUE(EGL_RED_SIZE);
//    EGL_PRINT_VALUE(EGL_GREEN_SIZE);
//    EGL_PRINT_VALUE(EGL_BLUE_SIZE);
//    EGL_PRINT_VALUE(EGL_ALPHA_SIZE);
//    EGL_PRINT_VALUE(EGL_ALPHA_MASK_SIZE);
//    EGL_PRINT_VALUE(EGL_BUFFER_SIZE);
//    EGL_PRINT_VALUE(EGL_COLOR_BUFFER_TYPE);
//    EGL_PRINT_VALUE(EGL_CONFIG_CAVEAT);
//    EGL_PRINT_VALUE(EGL_CONFIG_ID);
//    EGL_PRINT_VALUE(EGL_CONFORMANT);
//    EGL_PRINT_VALUE(EGL_DEPTH_SIZE);
//    EGL_PRINT_VALUE(EGL_LEVEL);
//    EGL_PRINT_VALUE(EGL_LUMINANCE_SIZE);
//    EGL_PRINT_VALUE(EGL_MAX_PBUFFER_HEIGHT);
//    EGL_PRINT_VALUE(EGL_MAX_PBUFFER_WIDTH);
//    EGL_PRINT_VALUE(EGL_MAX_PBUFFER_PIXELS);
//    EGL_PRINT_VALUE(EGL_MAX_SWAP_INTERVAL);
//    EGL_PRINT_VALUE(EGL_MIN_SWAP_INTERVAL);
//    EGL_PRINT_VALUE(EGL_NATIVE_RENDERABLE);
//    EGL_PRINT_VALUE(EGL_NATIVE_VISUAL_ID);
//    EGL_PRINT_VALUE(EGL_NATIVE_VISUAL_TYPE);
//    EGL_PRINT_VALUE(EGL_RENDERABLE_TYPE);
//    EGL_PRINT_VALUE(EGL_SAMPLES);
//    EGL_PRINT_VALUE(EGL_STENCIL_SIZE);
//    EGL_PRINT_VALUE(EGL_SURFACE_TYPE);
//    EGL_PRINT_VALUE(EGL_TRANSPARENT_TYPE);
//    EGL_PRINT_VALUE(EGL_TRANSPARENT_RED_VALUE);
//    EGL_PRINT_VALUE(EGL_TRANSPARENT_GREEN_VALUE);
//    EGL_PRINT_VALUE(EGL_TRANSPARENT_BLUE_VALUE);

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

static bool egl_create_context(EGLRenderer* renderer,
                               EGL_Data* m_eglData,
                               CString* err)
{
    {
        Vector<EGLint> config_preferred = {
    #if !defined(COFFEE_RASPBERRYPI)
            EGL_CONTEXT_CLIENT_VERSION, 2,
    #endif
            EGL_NONE,
        };

        m_eglData->context = eglCreateContext(m_eglData->display, m_eglData->config,
                                              EGL_NO_CONTEXT, &config_preferred[0]);

        EGL_NULLCHECK(m_eglData->context, "Failed to create EGL context");
    }

#if defined(SDL_VIDEO_DRIVER_X11) || defined(COFFEE_USE_MAEMO_X11)
    /* And now we perform voodoo-magic.
     * We need a valid X window manager handle for our window.
     * Where do we get it?
     * We ask the other side of the class that we are part of. This requires traversing the class hierarchy up and down. It's really weird.
     */
    ::Display* x_disp = nullptr;
    ::Window x_win = 0;
#elif defined(COFFEE_RASPBERRY_DMX)
    ::EGL_DISPMANX_WINDOW_T dmx_win = {};
#endif

    WindowManagerClient* wm_client = C_DCAST<WindowManagerClient>(renderer);
    CDWindow* win = wm_client->window();
#if defined(SDL_VIDEO_DRIVER_X11) || defined(COFFEE_USE_MAEMO_X11)
    x_disp = win->wininfo.x11.display;
    x_win = win->wininfo.x11.window;
#elif defined(COFFEE_RASPBERRY_DMX)
    dmx_win = win->wininfo.dmx.window;
#endif
    delete win;

#if defined(SDL_VIDEO_DRIVER_X11) || defined(COFFEE_USE_MAEMO_X11)
    cVerbose(8, "X11 display connection from SDL: {0}", EGL_PRINT(x_disp));
    cVerbose(8, "X11 display connection from EGL: {0}", EGL_PRINT(eglGetDisplay(x_disp)));
    cVerbose(8, "X11 window handle: {0}", EGL_PRINT(x_win));
#elif defined(COFFEE_RASPBERRY_DMX)
    cVerbose(8, "DISPMANX window handle: {0}", EGL_PRINT(dmx_win.element));
#endif
    cVerbose(8, "Config for surface: {0}", EGL_PRINT(m_eglData->config));

    cVerbose(8, "Calling eglCreateWindowSurface...");

#if defined(SDL_VIDEO_DRIVER_X11) || defined(COFFEE_USE_MAEMO_X11)
    m_eglData->surface = eglCreateWindowSurface(m_eglData->display, m_eglData->config,
                                                (EGLNativeWindowType)x_win, nullptr);
#elif defined(COFFEE_RASPBERRY_DMX)
    m_eglData->surface = eglCreateWindowSurface(m_eglData->display, m_eglData->config,
                                                &dmx_win, nullptr);
#endif

    cVerbose(8, "eglCreateWindowSurface returned: {0}", eglGetError());
    EGL_NULLCHECK(m_eglData->surface, "Failed to create surface");

    cVerbose(8, "Null check succeeded");

    cVerbose(8, "EGL vendor: {0}", eglQueryString(m_eglData->display, EGL_VENDOR));
    cVerbose(8, "EGL extensions: {0}", eglQueryString(m_eglData->display, EGL_EXTENSIONS));
    cVerbose(8, "EGL version: {0}", eglQueryString(m_eglData->display, EGL_VERSION));
    cVerbose(8, "EGL client APIs: {0}", eglQueryString(m_eglData->display, EGL_CLIENT_APIS));

    return true;
}

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
        return true;
    }
    virtual ThreadId const& currentThread()
    {
        return m_threadId;
    }
};

struct EGL_GL_WorkerContext : public CGL::CGL_WorkerContext
{
private:
    EGL_Data data;
    ThreadId m_threadId;
public:
    EGL_GL_WorkerContext(EGLRenderer& renderer):
        data()
    {
        data.display = renderer.m_eglData->display;
        data.config = renderer.m_eglData->config;

        egl_create_context(&renderer, &data, nullptr);
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
        return true;
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
    C_UNUSED(err);

    return true;
}

bool EGLRenderer::contextPostInit(const GLProperties &props, CString *err)
{
    C_UNUSED(props);

    m_eglData = UqPtr<EGL_Data>(new EGL_Data);
    m_eglData->surface = nullptr;
    m_eglData->context = nullptr;
    m_eglData->config = nullptr;

    WindowManagerClient* wm_client = C_DCAST<WindowManagerClient>(this);
    CDWindow* win = wm_client->window();
#if defined(SDL_VIDEO_DRIVER_X11) || defined(COFFEE_USE_MAEMO_X11)
    ::Display* x_disp = win->wininfo.x11.display;
#endif
    delete win;

#if defined(SDL_VIDEO_DRIVER_X11) || defined(COFFEE_USE_MAEMO_X11)
    m_eglData->display = eglGetDisplay((EGLNativeDisplayType)x_disp);
#else
    m_eglData->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif

    EGL_NULLCHECK(m_eglData->display, "Failed to retrieve EGL display");

    EGL_ERRORCHECK(eglInitialize(m_eglData->display, nullptr, nullptr));

    EGLint config_num = 0;

    {
        /* Set up preferred configuration */
        Vector<EGLint> config_preferred = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    #if !defined(COFFEE_RASPBERRYPI)
            EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
    #endif

    #if !defined(COFFEE_RASPBERRYPI)
            EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
            EGL_CONFIG_CAVEAT, EGL_NONE,
    #endif
//            EGL_NATIVE_RENDERABLE, EGL_TRUE,

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

        cVerbose(5, "Number of FB configs: {0}", config_num);
        if(config_num < 1)
        {
            *err = "No framebuffer configurations found";
            return false;
        }

        Vector<EGLConfig> configurations;
        configurations.resize(config_num);

        EGL_ERRORCHECK(eglChooseConfig(m_eglData->display, &config_preferred[0],
                       &configurations[0], config_num, &config_num));

        m_eglData->config = nullptr;
        CDContextBits bits;
        for(EGLConfig const& cfg : configurations)
        {
            egl_config_to_bits(m_eglData->display, cfg, bits);
            cVerbose(5, "EGLConfig information:{1}: {0}", bits, EGL_PRINT(cfg));
            if(bits.samples == props.bits.samples &&
                    bits.red == props.bits.red &&
                    bits.green == props.bits.green &&
                    bits.blue == props.bits.blue)
            {
                m_eglData->config = cfg;
                break;
            }
        }
        if(!m_eglData->config)
            m_eglData->config = configurations[0];
        cVerbose(7, "Picked EGLConfig: {0}", EGL_PRINT(m_eglData->config));
    }

    if(!egl_create_context(this, m_eglData.get(), err))
        return false;

    if(!glContext()->acquireContext())
        return false;

    glViewport(0,0,1920,1080);
    glClearColor(1.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    if(props.flags & GLProperties::GLVSync)
        setSwapInterval(1);
    else
        setSwapInterval(0);

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
    EGL_VERIFYCONTEXT()
        return;

    EGL_ERRORCHECK_NOBOOL(eglSwapBuffers(m_eglData->display, m_eglData->surface));
}

CSize EGLRenderer::framebufferSize() const
{
    CSize size = {};

    EGL_VERIFYCONTEXT()
        return size;

    EGL_ERRORCHECK_NOBOOL(eglQuerySurface(m_eglData->display, m_eglData->surface, EGL_WIDTH, &size.w));
    EGL_ERRORCHECK_NOBOOL(eglQuerySurface(m_eglData->display, m_eglData->surface, EGL_HEIGHT, &size.h));

    return size;
}

int EGLRenderer::swapInterval() const
{
    return 0;
}

void EGLRenderer::setSwapInterval(const int &i)
{
    EGL_VERIFYCONTEXT()
            return;

    EGLint max_swap = 0;
    EGLint min_swap = 0;
    eglGetConfigAttrib(m_eglData->display, m_eglData->config, EGL_MAX_SWAP_INTERVAL, &max_swap);
    eglGetConfigAttrib(m_eglData->display, m_eglData->config, EGL_MIN_SWAP_INTERVAL, &min_swap);
    if(i < min_swap || i > max_swap)
    {
        cWarning("Invalid swap interval, limits: [{0}, {1}], got {2}",
                 min_swap, max_swap, i);
        return;
    }
    EGL_ERRORCHECK_NOBOOL(eglSwapInterval(m_eglData->display, i));
}

CDContextBits EGLRenderer::context()
{
    CDContextBits bits;

    EGL_VERIFYCONTEXT()
            return bits;

    egl_config_to_bits(m_eglData->display, m_eglData->config, bits);
    return bits;
}

ThreadId EGLRenderer::contextThread()
{
    return ThreadId();
}

CGL::CGL_Context *EGLRenderer::glContext()
{
    EGL_VERIFYCONTEXT()
            return nullptr;

    static EGL_GL_Context ctxt(*this);
    return &ctxt;
}

CGL::CGL_ScopedContext EGLRenderer::scopedContext()
{
    return CGL::CGL_ScopedContext(glContext());
}

CGL::CGL_WorkerContext *EGLRenderer::workerContext()
{
    EGL_VERIFYCONTEXT()
            return nullptr;

    return new EGL_GL_WorkerContext(*this);
}

}
}
#endif
