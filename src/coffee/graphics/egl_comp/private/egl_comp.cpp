#include <coffee/egl/egl_comp.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/core/CProfiling>
#include <peripherals/stl/string_casting.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <coffee/core/debug/logging.h>

#if defined(FEATURE_ENABLE_CEAGL)
#include <CEAGL/eagl.h>
#else
#include <EGL/egl.h>

#if defined(COFFEE_ANDROID)
#include <EGL/eglext.h>
#endif
#endif

#if !defined(EGL_VERSION_1_3)
#error EGL version 1.3 is required for OpenGL ES 2.0
#endif

#if defined(COFFEE_ANDROID)
namespace android_egl {
static PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC
                                            GetCompositorTimingSupported;
static PFNEGLGETCOMPOSITORTIMINGANDROIDPROC GetCompositorTiming;
} // namespace android_egl
#endif

namespace egl {

using stl_types::str::convert::hexify;

namespace detail {

struct EGLData
{
    EGLDisplay display;
};

void EGLDataDeleter::operator()(EGLData* data)
{
    delete data;
}

static void* gles2_dl{};

} // namespace detail

static std::string egl_to_error()
{
    using namespace std::string_view_literals;

    auto error = eglGetError();
#define ERROR_MAP(code) \
    if(error == code)   \
        return #code;

    ERROR_MAP(EGL_SUCCESS)
    ERROR_MAP(EGL_BAD_ACCESS)
    ERROR_MAP(EGL_BAD_ALLOC)
    ERROR_MAP(EGL_BAD_ATTRIBUTE)
    ERROR_MAP(EGL_BAD_CONFIG)
    ERROR_MAP(EGL_BAD_CONTEXT)
    ERROR_MAP(EGL_BAD_CURRENT_SURFACE)
    ERROR_MAP(EGL_BAD_DISPLAY)
    ERROR_MAP(EGL_BAD_MATCH)
    ERROR_MAP(EGL_BAD_NATIVE_PIXMAP)
    ERROR_MAP(EGL_BAD_NATIVE_WINDOW)
    ERROR_MAP(EGL_BAD_PARAMETER)
    ERROR_MAP(EGL_BAD_SURFACE)
    ERROR_MAP(EGL_NOT_INITIALIZED)

#if defined(EGL_CONTEXT_LOST)
    ERROR_MAP(EGL_CONTEXT_LOST) // IMG_power_management
#endif

#undef ERROR_MAP

    return "0x" + hexify(error);
}

void DisplayHandle::load(entity_container& e, comp_app::app_error& ec)
{
    auto& config  = comp_app::AppLoader::config<comp_app::GLConfig>(e);
    auto& appInfo = *e.service<comp_app::AppInfo>();

    m_data = stl_types::MkUqDST<detail::EGLData, detail::EGLDataDeleter>();

    m_data->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if(!m_data->display)
    {
        ec = "eglGetDisplay:" + egl_to_error();
        ec = comp_app::AppError::NoDisplay;
        return;
    }

    if(eglInitialize(m_data->display, &m_major, &m_minor) == EGL_FALSE)
    {
        ec = comp_app::AppError::ContextNotAvailable;
        ec = "eglInitialize:" + egl_to_error();
        return;
    }

    appInfo.add("egl:vendor", eglQueryString(m_data->display, EGL_VENDOR));
    appInfo.add("egl:version", eglQueryString(m_data->display, EGL_VERSION));

    if(auto extensions = eglQueryString(m_data->display, EGL_EXTENSIONS))
    {
        appInfo.add("egl:extensions", extensions);
    }

#if defined(EGL_VERSION_1_2)
#if !defined(COFFEE_APPLE)
    if(config.profile & comp_app::GLConfig::Core)
    {
#if defined(EGL_VERSION_1_4)
        if(eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
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
        if(eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
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

    /* Set up dynamic loader function for the binding */
    {
        auto& config = e.service<comp_app::AppLoader>()
                           ->config<comp_app::GraphicsBindingConfig>();
        void* (*loader)(const char*) = [](const char* name) {
            return reinterpret_cast<void*>(eglGetProcAddress(name));
        };
        config.loader = loader;
#if defined(EGL_COMP_DOUBLE_GETPROC)
        config.loader = [](const char* name) -> void* {
            if(!detail::gles2_dl)
                detail::gles2_dl = dlopen("libGLESv2.so", RTLD_NOW);
            if(!detail::gles2_dl)
                Throw(implementation_error("failed to open libGLESv2.so.2"));
            if(auto core_sym = dlsym(detail::gles2_dl, name))
                return core_sym;
            if(auto egl_sym = reinterpret_cast<void*>(eglGetProcAddress(name)))
                return egl_sym;
            fprintf(stderr, "Symbol %s not found\n", name);
            return nullptr;
        };
#endif
    }
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

    auto&          config = comp_app::AppLoader::config<comp_app::GLConfig>(e);
    DisplayHandle& handle = *e.service<DisplayHandle>();
    auto color   = properties::get<properties::layout>(config.framebufferFmt);
    auto depth   = properties::get<properties::layout>(config.depthFmt);
    auto display = handle.context().display;

    std::set<std::string_view> extensions;
    if(auto display_extensions = eglQueryString(display, EGL_EXTENSIONS))
    {
        using spliterator = stl_types::str::split::spliterator<char>;

        spliterator extension_list(display_extensions, ' ');
        while(extension_list != spliterator())
        {
            auto ext_name = *extension_list;
            if(!ext_name.empty())
                extensions.insert(ext_name);
            ++extension_list;
        }
#if defined(EGL_VERSION_1_5)
        if(handle.m_major > 1 || (handle.m_major == 1 && handle.m_minor >= 5))
        {
            auto client_extensions
                = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
            extension_list = spliterator(client_extensions, ' ');
            while(extension_list != spliterator())
            {
                auto ext_name = *extension_list;
                if(!ext_name.empty())
                    extensions.insert(ext_name);
                ++extension_list;
            }
        }
#endif
    }

#if defined(EGL_VERSION_1_5) && defined(EGL_EXT_pixel_format_float)
    if(properties::get<properties::is_floating_point>(config.framebufferFmt))
    {
        if(extensions.contains("EGL_EXT_pixel_format_float"))
        {
            // Leave the color struct as is
        } else
        {
            /* Unsupported */
            config.framebufferFmt = PixFmt::RGB565;
            color = properties::get<properties::layout>(config.framebufferFmt);
        }
    }
#endif

    // clang-format off
    std::vector<std::pair<EGLint, EGLint>> surfaceConfig = {
        {EGL_SURFACE_TYPE, EGL_WINDOW_BIT},

        {EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER},

        {EGL_RED_SIZE, color.r},
        {EGL_GREEN_SIZE, color.g},
        {EGL_BLUE_SIZE, color.b},
        {EGL_ALPHA_SIZE, color.a},

        {EGL_STENCIL_SIZE, depth.stencil},
        {EGL_DEPTH_SIZE, depth.depth},
    };
    // clang-format on

#if defined(EGL_ANDROID_recordable)
    if(extensions.contains("EGL_ANDROID_recordable"))
        surfaceConfig.push_back({EGL_RECORDABLE_ANDROID, EGL_TRUE});
#endif

#if defined(EGL_VERSION_1_5) && defined(EGL_EXT_pixel_format_float)
    if(properties::get<properties::is_floating_point>(config.framebufferFmt))
    {
        surfaceConfig.push_back(
            {EGL_COLOR_COMPONENT_TYPE_EXT, EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT});
    }
#endif

    auto render_type =
#if defined(EGL_VERSION_1_4)
        (config.profile & Profile::Core) ? EGL_OPENGL_BIT :
#endif
                                         EGL_OPENGL_ES2_BIT;

    surfaceConfig.push_back({EGL_RENDERABLE_TYPE, render_type});

    surfaceConfig.push_back({EGL_CONFORMANT, render_type});

    surfaceConfig.push_back({EGL_NONE, EGL_NONE});

    auto* configPtr = reinterpret_cast<EGLint const*>(surfaceConfig.data());

    EGLint numConfig = 0;
    eglChooseConfig(display, configPtr, &m_config, 1, &numConfig);

    if(numConfig == 0)
    {
        ec = "eglChooseConfig:" + egl_to_error();
        ec = comp_app::AppError::FramebufferMismatch;
        return;
    }

    std::vector<std::pair<EGLint, EGLint>> attribs = {};

#if defined(EGL_VERSION_1_5)
    if(handle.m_major > 1 || (handle.m_major == 1 && handle.m_minor >= 5))
    {
        attribs.push_back({EGL_CONTEXT_MAJOR_VERSION, config.version.major});
        attribs.push_back({EGL_CONTEXT_MINOR_VERSION, config.version.minor});

        attribs.push_back(
            {EGL_CONTEXT_OPENGL_DEBUG,
             (config.profile & comp_app::GLConfig::Debug) ? EGL_TRUE
                                                          : EGL_FALSE});
    } else
#endif
    {
        /* Set client version to 3 on iOS to indicate we want GL ES 3.0
         * On Android we still set it to 2 if we want to use GL ES 3.x+
         */
        attribs.push_back(
            {EGL_CONTEXT_CLIENT_VERSION,
             compile_info::platform::is_ios ? 3 : 2});
    }
#if defined(EGL_IMG_context_priority)
    if(extensions.contains("EGL_IMG_context_priority"))
    {
        //        attribs.push_back(
        //            {EGL_CONTEXT_PRIORITY_LEVEL_IMG,
        //            EGL_CONTEXT_PRIORITY_HIGH_IMG});
    }
#endif

    attribs.push_back({EGL_NONE, EGL_NONE});

    configPtr = reinterpret_cast<EGLint const*>(attribs.data());

    m_context = eglCreateContext(display, m_config, EGL_NO_CONTEXT, configPtr);

    if(!m_context)
    {
        ec = "eglCreateContext:" + egl_to_error();
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
    if constexpr(compile_info::debug_mode)
        Coffee::Profiler::PushContext("egl::GraphicsFramebuffer::swapBuffers");

    auto display = m_container->service<DisplayHandle>()->context().display;

    if(!eglSwapBuffers(display, m_surface))
    {
        ec = "eglSwapBuffers:" + egl_to_error();
        ec = comp_app::AppError::SwapBuffersFailed;
    }

    if constexpr(compile_info::debug_mode)
        Coffee::Profiler::PopContext();
}

void GraphicsFramebuffer::end_frame(ContainerProxy&, const time_point&)
{
    defaultSwap();
}

void GraphicsFramebuffer::load(entity_container& e, comp_app::app_error& ec)
{
    m_container = &e;

    auto display = e.service<DisplayHandle>()->context().display;

    auto ptr_info = e.service<comp_app::PtrNativeWindowInfoService>();

    /* TODO: Create attrib list supporting sRGB */
    m_surface = eglCreateWindowSurface(
        display,
        e.service<egl::GraphicsContext>()->m_config,
        C_RCAST<EGLNativeWindowType>(ptr_info->window),
        nullptr);

    if(!m_surface)
    {
        ec = "eglCreateWindowSurface:" + egl_to_error();
        ec = comp_app::AppError::FramebufferMismatch;
        return;
    }

    auto context = e.service<GraphicsContext>()->m_context;

    if(!eglMakeCurrent(display, m_surface, m_surface, context))
    {
        ec = "eglMakeCurrent:" + egl_to_error();
        ec = comp_app::AppError::ContextNotAvailable;
        return;
    }

#if defined(EGL_ANDROID_get_frame_timestamps)
    auto& feature_flags = e.service<GraphicsContext>()->feature_flags;

    android_egl::GetCompositorTimingSupported
        = reinterpret_cast<PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC>(
            eglGetProcAddress("eglGetCompositorTimingSupportedANDROID"));
    android_egl::GetCompositorTiming
        = reinterpret_cast<PFNEGLGETCOMPOSITORTIMINGANDROIDPROC>(
            eglGetProcAddress("eglGetCompositorTimingANDROID"));

    if(android_egl::GetCompositorTimingSupported)
    {
        feature_flags.android_composite_deadline
            = android_egl::GetCompositorTimingSupported(
                display, m_surface, EGL_COMPOSITE_DEADLINE_ANDROID);
        feature_flags.android_present_latency
            = android_egl::GetCompositorTimingSupported(
                display, m_surface, EGL_COMPOSITE_TO_PRESENT_LATENCY_ANDROID);
    }
#endif
}

void GraphicsFramebuffer::unload(entity_container& e, comp_app::app_error& ec)
{
    auto display = e.service<DisplayHandle>()->context().display;

    if(!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
    {
        ec = "eglMakeCurrent:" + egl_to_error();
        ec = comp_app::AppError::FramebufferTeardownFail;
        return;
    }

    if(!eglDestroySurface(display, m_surface))
    {
        ec = "eglDestroySurface:" + egl_to_error();
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

void GraphicsSwapControl::load(entity_container& p, comp_app::app_error&)
{
    m_container = &p;
}

libc_types::i32 GraphicsSwapControl::swapInterval() const
{
    return 0;
}

void GraphicsSwapControl::setSwapInterval(libc_types::i32 interval)
{
    auto display = m_container->service<DisplayHandle>()->context().display;

    if(!eglSwapInterval(display, interval))
    {
        comp_app::app_error ec;
        ec = "eglSwapInterval:" + egl_to_error();
        ec = comp_app::AppError::InvalidSwapInterval;
    }
}

void Windowing::load(entity_container& e, comp_app::app_error&)
{
    m_container = &e;
}

comp_app::size_2d_t Windowing::size() const
{
    return m_container->service<GraphicsFramebuffer>()->size();
}

comp_app::detail::WindowState Windowing::state() const
{
    using W = comp_app::detail::WindowState;
    return W::Maximized | W::FullScreen | W::Focused | W::Undecorated;
}

void Windowing::setState(comp_app::detail::WindowState)
{
}

} // namespace egl
