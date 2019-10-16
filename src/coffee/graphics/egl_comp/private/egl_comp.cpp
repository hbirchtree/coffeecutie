#include <coffee/egl/egl_comp.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#if defined(FEATURE_ENABLE_CEAGL)
#include <CEAGL/eagl.h>
#else
#include <EGL/egl.h>
#endif

#if !defined(EGL_VERSION_1_3)
#error EGL version 1.3 is required
#endif

namespace egl {
namespace detail {

struct EGLData
{
    EGLDisplay display;
};

void EGLDataDeleter::operator()(EGLData* data)
{
    delete data;
}

} // namespace detail

void DisplayHandle::load(entity_container& e, comp_app::app_error& ec)
{
    auto& config = comp_app::AppLoader::config<comp_app::GLConfig>(e);

    m_data = stl_types::MkUqDST<detail::EGLData, detail::EGLDataDeleter>();

    m_data->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if(!m_data->display)
    {
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::NoDisplay;
        return;
    }

    EGLint major = config.version.major, minor = config.version.minor;
    if(!eglInitialize(m_data->display, &major, &minor))
    {
        ec = comp_app::AppError::ContextNotAvailable;
        ec = stl_types::str::convert::hexify(eglGetError());
        return;
    }

#if defined(EGL_VERSION_1_2)
#if !defined(COFFEE_APPLE)
    if(config.profile & comp_app::GLConfig::Core)
    {
#if defined(EGL_VERSION_1_4)
        if(!eglBindAPI(EGL_OPENGL_API))
        {
            ec = "failed to bind EGL API OpenGL";
            ec = comp_app::AppError::ContextFailedBind;
            return;
        }
#else
        Throw(undefined_behavior("OpenGL is not supported on this target"));
#endif
    } else if(config.profile & comp_app::GLConfig::Embedded)
    {
        if(!eglBindAPI(EGL_OPENGL_ES_API))
        {
            ec = "failed to bind EGL API OpenGLES";
            ec = comp_app::AppError::ContextFailedBind;
            return;
        }
    }
#endif
#else
    static_assert(false, "unsupported EGL");
#endif
}

void DisplayHandle::unload(entity_container& e, comp_app::app_error& ec)
{
    eglTerminate(m_data->display);
}

detail::EGLData& DisplayHandle::context()
{
    return *m_data;
}

void GraphicsContext::load(entity_container& e, comp_app::app_error& ec)
{
    using Profile = comp_app::GLConfig::Profile;
    using namespace typing::pixels;

    auto& config  = comp_app::AppLoader::config<comp_app::GLConfig>(e);
    auto& handle  = *e.service<DisplayHandle>();
    auto  color   = properties::get<properties::layout>(config.framebufferFmt);
    auto  depth   = properties::get<properties::layout>(config.depthFmt);
    auto  display = handle.context().display;

    const stl_types::Vector<EGLint> surfaceConfig = {
        EGL_RENDERABLE_TYPE,
#if defined(EGL_VERSION_1_4)
        (config.profile & Profile::Core) ? EGL_OPENGL_BIT :
#endif
                                         EGL_OPENGL_ES2_BIT,

        EGL_CONFORMANT,
#if defined(EGL_VERSION_1_4)
        (config.profile & Profile::Core) ? EGL_OPENGL_BIT :
#endif
                                         EGL_OPENGL_ES2_BIT,

        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,

        EGL_COLOR_BUFFER_TYPE,
        EGL_RGB_BUFFER,

        EGL_RED_SIZE,
        color.r,
        EGL_GREEN_SIZE,
        color.g,
        EGL_BLUE_SIZE,
        color.b,
        EGL_ALPHA_SIZE,
        color.a,

        EGL_STENCIL_SIZE,
        depth.stencil,
        EGL_DEPTH_SIZE,
        depth.depth,

        EGL_NONE,
    };

    EGLint numConfig = 0;
    eglChooseConfig(display, surfaceConfig.data(), &m_config, 1, &numConfig);

    if(numConfig == 0)
    {
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::FramebufferMismatch;
        return;
    }

    stl_types::Vector<EGLint> attribs = {
#if defined(EGL_VERSION_1_5)
        EGL_CONTEXT_MAJOR_VERSION,
        config.version.major,
        EGL_CONTEXT_MINOR_VERSION,
        config.version.minor,
        EGL_CONTEXT_OPENGL_DEBUG,
        (config.profile & comp_app::GLConfig::Debug) ? EGL_TRUE : EGL_FALSE,
#elif defined(EGL_VERSION_1_3)
        EGL_CONTEXT_CLIENT_VERSION,
        2,
#endif
        EGL_NONE,
    };

    m_context =
        eglCreateContext(display, m_config, EGL_NO_CONTEXT, attribs.data());

    if(!m_context)
    {
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::NoContext;
        return;
    }

    m_container = &e;
}

void GraphicsContext::unload(entity_container& c, comp_app::app_error&)
{
    eglDestroyContext(c.service<DisplayHandle>()->context().display, m_context);
}

void GraphicsFramebuffer::swapBuffers(comp_app::app_error& ec)
{
    if(!eglSwapBuffers(
           m_container->service<DisplayHandle>()->context().display, m_surface))
    {
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::SwapBuffersFailed;
    }
}

void GraphicsFramebuffer::load(entity_container& e, comp_app::app_error& ec)
{
    m_container = &e;

    auto display = e.service<DisplayHandle>()->context().display;

    auto ptr_info = e.service<comp_app::PtrNativeWindowInfo>();

    m_surface = eglCreateWindowSurface(
        display,
        e.service<egl::GraphicsContext>()->m_config,
        C_RCAST<EGLNativeWindowType>(ptr_info->window),
        nullptr);

    if(!m_surface)
    {
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::FramebufferMismatch;
        return;
    }

    auto context = e.service<GraphicsContext>()->m_context;

    if(!eglMakeCurrent(display, m_surface, m_surface, context))
    {
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::ContextNotAvailable;
    }
}

void GraphicsFramebuffer::unload(entity_container& e, comp_app::app_error& ec)
{
    auto display = e.service<DisplayHandle>()->context().display;

    if(!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
    {
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::FramebufferTeardownFail;
        return;
    }

    if(!eglDestroySurface(display, m_surface))
    {
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::FramebufferTeardownFail;
    }
}

comp_app::size_2d_t GraphicsFramebuffer::size() const
{
    comp_app::size_2d_t out;
    auto display = m_container->service<DisplayHandle>()->context().display;
    eglQuerySurface(display, m_surface, EGL_WIDTH, &out.w);
    eglQuerySurface(display, m_surface, EGL_HEIGHT, &out.h);
    return out;
}

void GraphicsSwapControls::load(entity_container& p, comp_app::app_error&)
{
    m_container = &p;
}

libc_types::i32 GraphicsSwapControls::swapInterval() const
{
    return 0;
}

void GraphicsSwapControls::setSwapInterval(libc_types::i32 interval)
{
    auto display = m_container->service<DisplayHandle>()->context().display;

    if(!eglSwapInterval(display, interval))
    {
        comp_app::app_error ec;
        ec = stl_types::str::convert::hexify(eglGetError());
        ec = comp_app::AppError::InvalidSwapInterval;
    }
}

} // namespace egl
