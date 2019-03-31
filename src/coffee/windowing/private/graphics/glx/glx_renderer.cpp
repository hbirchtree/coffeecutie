#include <coffee/windowing/graphics/glx/glx_renderer.h>

#if defined(COFFEE_USE_LINUX_GLX)

#include "../../windowing/x11/x11_window_data.h"

#include <coffee/windowing/windowing/x11/x11_window.h>

#include <coffee/core/CDebug>

#include <coffee/core/plat/windowmanager/plat_windowtype.h>
#include <coffee/core/base/renderer/windowmanagerclient.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>

#ifndef None
#define None 0L
#endif

#include <GL/glx.h>
#include <GL/glxext.h>

/* Advanced use of GLX should allow transparent windows */
/* Right now it does not work */
//#define USE_ADVANCED_GLX

typedef GLXContext (*glXCreateContextAttribsARBProc)(::Display*, GLXFBConfig,GLXContext,Bool, const int*);

namespace Coffee{
namespace Display{

struct GLX_Context;

struct GLX_Data
{
    GLX_Data():
        display(nullptr),
        x_window(None),
        context(nullptr),
        window(None),
        config(nullptr),
        visual(nullptr)
    {
    }

    ::Display* display;
    ::Window x_window;

    GLXContext context;
    GLXWindow window;
    GLXFBConfig config;

    XVisualInfo* visual;
//    Atom del_atom;
//    XRenderPictFormat* pict_format;


    UqPtr<GLX_Context> c_context;
};

struct GLX_Context : CGL::CGL_Context
{
    ThreadId m_threadId;
    GLX_Data* m_data;

    // CGL_Context interface
public:
    GLX_Context(GLX_Data* data) :
        m_data(data)
    {
    }

    bool acquireContext()
    {
        new (&m_threadId) ThreadId;
        glXMakeContextCurrent(m_data->display, m_data->x_window, m_data->x_window, m_data->context);
        return true;
    }

    bool releaseContext()
    {
        glXMakeContextCurrent(m_data->display, 0, 0, nullptr);
        return true;
    }

    const ThreadId &currentThread()
    {
        return m_threadId;
    }
};

bool GLXRenderer::contextPreInit(const GLProperties &props, CString *)
{
    int visual_data[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, True,

//        GLX_RED_SIZE, props.bits.red,
//        GLX_GREEN_SIZE, props.bits.green,
//        GLX_BLUE_SIZE, props.bits.blue,
//        GLX_ALPHA_SIZE, props.bits.alpha,

        GLX_DEPTH_SIZE, props.bits.depth,
//        GLX_STENCIL_SIZE, props.bits.stencil,

//        GLX_SAMPLES, props.bits.samples,

        None,
    };

    X11Window* xwin = C_DCAST<X11Window>(this);

    if(!xwin->m_xData)
        return false;

    WindowManagerClient* wm = C_DCAST<WindowManagerClient>(this);
    CDWindow* window_handle = wm->window();

    ::Display* x_disp = window_handle->wininfo.x11.display;

    delete window_handle;

    m_gxData = MkUqDST<GLX_Data, GLX_Data_Deleter>();
    *m_gxData = {};

#ifdef USE_ADVANCED_GLX

    int numFbConfigs = 0;
    auto configs = glXChooseFBConfig(x_disp, DefaultScreen(x_disp),
                                     visual_data, &numFbConfigs);

    cDebug("Found {0} GLX FB configs", numFbConfigs);


    if(numFbConfigs <= 0)
    {
        cWarning("No framebuffer configurations found!");
        return false;
    }

    m_gxData->config = nullptr;

    XVisualInfo* visual = nullptr;

    for(int i=0;i<numFbConfigs;i++)
    {
        visual = glXGetVisualFromFBConfig(x_disp, configs[i]);

        if(!visual)
            continue;

        auto pict_format = XRenderFindVisualFormat(x_disp, visual->visual);
        if(!pict_format)
            continue;

        if(pict_format->direct.alphaMask > 0)
        {
            m_gxData->config = configs[i];
            break;
        }
    }

    if(!m_gxData->config)
    {
        cWarning("Failed to find matching GLX FB config!");
        return false;
    }
#else

    GLint attrs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    auto visual = glXChooseVisual(x_disp, 0, attrs);

#endif

    xwin->m_xData->visual = visual;

    m_gxData->visual = visual;
    m_gxData->display = x_disp;

    xwin->showWindow();

    return true;
}

/* Source:
 * https://github.com/datenwolf/codesamples/tree/master/samples/OpenGL/x11argb_opengl
 *
*/
static int isExtensionSupported(const char *extList, const char *extension)
{
    const char *start;
    const char *where, *terminator;

    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
    if ( where || *extension == '\0' )
        return 0;

    /* It takes a bit of care to be fool-proof about parsing the
         OpenGL extensions string. Don't be fooled by sub-strings,
         etc. */
    for ( start = extList; ; ) {
        where = strstr( start, extension );

        if ( !where )
            break;

        terminator = where + strlen( extension );

        if ( where == start || *(where - 1) == ' ' )
            if ( *terminator == ' ' || *terminator == '\0' )
                return 1;

        start = terminator;
    }
    return 0;
}

bool GLXRenderer::contextPostInit(const GLProperties &props, CString *)
{
    X11Window* xwin = C_DCAST<X11Window>(this);
    auto xdat = xwin->m_xData.get();

//    m_gxData->window = xdat->window;
//    m_gxData->x_window = xdat->window;
//    m_gxData->display = xdat->display;

    WindowManagerClient* wm = C_DCAST<WindowManagerClient>(this);
    CDWindow* window_handle = wm->window();

    m_gxData->x_window = window_handle->wininfo.x11.window;

//    m_gxData->window = glXCreateWindow(m_gxData->display, m_gxData->config,
//                                       window_handle->wininfo.x11.window, glx_attr);

    delete window_handle;

    if(!m_gxData->window)
    {
//        cWarning("Failed to create GLX window, using X window");
        m_gxData->window = m_gxData->x_window;
    }

    int dummy;
    if(!glXQueryExtension(m_gxData->display, &dummy, &dummy))
    {
        cWarning("GLX support not found!");
        return false;
    }

    int context_flags = 0;
    int profile_flags = 0;

    profile_flags |= (props.flags & GLProperties::GLCoreProfile)
            ? GLX_CONTEXT_CORE_PROFILE_BIT_ARB : 0;
    context_flags |= (props.flags & GLProperties::GLDebug)
            ? GLX_CONTEXT_DEBUG_BIT_ARB : 0;
//    context_flags |= (props.flags & GLProperties::GLES)
//            ? GLX_CONTEXT_ES2_PROFILE_BIT_EXT : 0;
#if defined(GLX_EXT_framebuffer_sRGB)
    context_flags |= (props.flags & GLProperties::GLSRGB)
            ? GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB : 0;
#endif

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, props.version.major,
        GLX_CONTEXT_MINOR_VERSION_ARB, props.version.minor,

        GLX_CONTEXT_PROFILE_MASK_ARB, profile_flags,
        GLX_CONTEXT_FLAGS_ARB, context_flags,

        None
    };

    GLXContext ctxt = nullptr;

#ifdef USE_ADVANCED_GLX
    if(isExtensionSupported(glXQueryExtensionsString(m_gxData->display,
                                                     DefaultScreen(m_gxData->display)),
                            "GLX_ARB_create_context"))
    {
        glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
                (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
                    (const GLubyte*)"glXCreateContextAttribsARB"
                    );
        if(glXCreateContextAttribsARB)
        {
            ctxt = glXCreateContextAttribsARB(m_gxData->display, m_gxData->config,
                                              0, True, context_attribs);

            XSync(m_gxData->display, False);
        }else
            cWarning("Failed to retrieve glXCreateContextAttribsARB");
    }else
        cWarning("GLX_ARB_create_context not supported");
#endif

    if(!ctxt)
        ctxt = glXCreateContext(xdat->display, xdat->visual,
                                nullptr, GL_TRUE);

    if(!ctxt)
    {
        cWarning("Failed to create GLX context, will be dying now");
        return false;
    }

    m_gxData->context = ctxt;
    m_gxData->c_context = MkUq<GLX_Context>(m_gxData.get());

    return true;
}

void GLXRenderer::contextTerminate()
{

}

void GLXRenderer::swapBuffers()
{
#if MODE_DEBUG
    CASSERT(C_OCAST<bool>(m_gxData));
    CASSERT(m_gxData->display);
    CASSERT(m_gxData->window);
#endif

    glXSwapBuffers(m_gxData->display, m_gxData->window);
}

CSize GLXRenderer::framebufferSize() const
{
    return {};
}

int GLXRenderer::swapInterval() const
{
    return 0;
}

void GLXRenderer::setSwapInterval(const int &i)
{

}

CDContextBits GLXRenderer::context()
{
    return {};
}

ThreadId GLXRenderer::contextThread()
{
    if(m_gxData && m_gxData->c_context)
        return m_gxData->c_context->currentThread();
    return ThreadId();
}

CGL::CGL_Context *GLXRenderer::glContext()
{
    if(m_gxData)
        return m_gxData->c_context.get();
    return nullptr;
}

CGL::CGL_ScopedContext GLXRenderer::scopedContext()
{
    return CGL::CGL_ScopedContext(glContext());
}

void GLX_Data_Deleter::operator()(GLX_Data *ptr)
{
    delete ptr;
}

}
}

#endif
