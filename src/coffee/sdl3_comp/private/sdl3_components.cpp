#include <SDL3/SDL_gamepad.h>
#include <coffee/sdl3_comp/sdl3_components.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/entity_container.inl>
#include <coffee/core/CProfiling>
#include <peripherals/stl/magic_enum.hpp>
#include <peripherals/stl/string_casting.h>
#include <peripherals/typing/enum/pixels/format_transform.h>
#include <platforms/sysinfo.h>

#include "sdl3events.h"

#include <SDL3/SDL.h>

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten.h>
#include <emscripten/html5_webgl.h>
#endif

#include <coffee/core/debug/formatting.h>

/* SDL3 returns true on success where SDL2 returned 0 */
#define SDL_FAILED(v) (!(v))

#define EMIT_IEVENT(from)                  \
    {                                      \
        auto data    = from;               \
        inputEv.type = data.event_type;    \
        inputBus->process(inputEv, &data); \
    }

#if defined(COFFEE_EMSCRIPTEN)
#define SUPPORTS_WINDOW_ACTIONS 0
#else
#define SUPPORTS_WINDOW_ACTIONS 1
#endif

namespace sdl3 {

namespace {

#if defined(COFFEE_EMSCRIPTEN)
int canvas_width = 0, canvas_height = 0;
#endif

/* SDL3 scopes text input to a window, and KeyboardInput has no container */
SDL_Window* active_window = nullptr;

} // namespace

using Coffee::Logging::cDebug;

struct current_config_t
{
    int major_version, minor_version;
    int r, g, b, a, depth, stencil;
    int srgb;
    int profile;
};

using ws_t = comp_app::interfaces::PtrNativeWindowInfo::window_system_t;

static ws_t get_wm_selection(SDL_Window* window);

inline void print_current_config()
{
    auto const ver = SDL_GetVersion();
    current_config_t config;
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &config.r);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &config.g);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &config.b);
    SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &config.a);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &config.depth);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &config.stencil);
    SDL_GL_GetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &config.srgb);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &config.profile);
    cDebug(
        "SDL config: red={0} green={1} blue={2}, alpha={3} "
        "depth={4} stencil={5} srgb={6} profile={7} version={8}.{9}",
        config.r,
        config.g,
        config.b,
        config.a,
        config.depth,
        config.stencil,
        config.srgb == true,
        !!(config.profile & SDL_GL_CONTEXT_PROFILE_CORE),
        SDL_VERSIONNUM_MAJOR(ver),
        SDL_VERSIONNUM_MINOR(ver));
}

using F      = comp_app::window_flags_t;
using WState = comp_app::window_flags_t;

/* SDL3 dropped SDL_WINDOW_SHOWN (windows are shown unless SDL_WINDOW_HIDDEN)
 * and SDL_WINDOW_FULLSCREEN_DESKTOP (SDL_WINDOW_FULLSCREEN without a display
 * mode set is the borderless-desktop mode). Both are handled outside the
 * table. */
static constexpr std::array<std::pair<SDL_WindowFlags, F>, 6>
    window_flag_mapping = {{
        {SDL_WINDOW_FULLSCREEN, F::fullscreen},

#if SUPPORTS_WINDOW_ACTIONS == 1
        {SDL_WINDOW_MINIMIZED, F::minimized},
        {SDL_WINDOW_MAXIMIZED, F::maximized},
#endif
        {SDL_WINDOW_RESIZABLE, F::resizable},
        {SDL_WINDOW_BORDERLESS, F::undecorated},
        {SDL_WINDOW_HIGH_PIXEL_DENSITY, F::high_dpi},
    }};

static SDL_WindowFlags window_to_sdl3(comp_app::window_flags_t state)
{
    SDL_WindowFlags out = 0;

    for(auto const& v : window_flag_mapping)
        if(enum_helpers::feval(state, v.second))
            out |= v.first;

    if(enum_helpers::feval(state, F::fullscreen_window))
        out |= SDL_WINDOW_FULLSCREEN;

    /* F::visible is deliberately not mapped to SDL_WINDOW_HIDDEN: callers that
     * set flags wholesale leave it out, and SDL2 showed those windows anyway */

    return out;
}

static comp_app::window_flags_t window_from_sdl3(SDL_WindowFlags state)
{
    comp_app::window_flags_t out = comp_app::window_flags_t::none;

    for(auto const& v : window_flag_mapping)
        if(state & v.first)
            out |= v.second;

    if(!(state & SDL_WINDOW_HIDDEN))
        out |= F::visible;

    return out;
}

static void getWindow(
    SDL_Window* window, comp_app::interfaces::PtrNativeWindowInfo& info);

using namespace stl_types;

void Context::load(entity_container& c, comp_app::app_error& ec)
{
    if(SDL_FAILED(SDL_Init(0)))
    {
        ec = comp_app::AppError::SystemError;
        ec = SDL_GetError();
        return;
    }

    c.register_subsystem_services<comp_app::AppServiceTraits<Context>>(this);

    if(auto info = c.service<comp_app::AppInfo>(); info)
    {
        auto const ver = SDL_GetVersion();

        auto verString = cast_pod(SDL_VERSIONNUM_MAJOR(ver)) + "." +
                         cast_pod(SDL_VERSIONNUM_MINOR(ver)) + "." +
                         cast_pod(SDL_VERSIONNUM_MICRO(ver));

        info->add("sdl3:version", verString);
    }
}

void Context::unload(entity_container&, comp_app::app_error&)
{
    SDL_Quit();
}

void Context::end_restricted(proxy_type& p, time_point const&)
{
    using namespace Coffee::Display;
    using namespace Coffee::Input;

    auto inputBus = p.service<comp_app::BasicEventBus<CIEvent>>();

    C_PTR_CHECK_MSG(inputBus, "EventBus<CIEvent> not specified")

    CIEvent inputEv;

    SDL_Event event;
    while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_QUIT, SDL_EVENT_QUIT))
    {
        auto data    = translate::event<CIQuit>(event);
        inputEv.type = data.event_type;
        inputBus->process(inputEv, &data);

        m_shouldClose = data.shouldClose;
    }
}

Windowing::~Windowing()
{
}

void Windowing::load(entity_container& c, comp_app::app_error& ec)
{
#if defined(COFFEE_LINUX) || defined(COFFEE_EMSCRIPTEN)
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
#endif

    if(SDL_FAILED(SDL_InitSubSystem(SDL_INIT_VIDEO)))
    {
        ec = comp_app::AppError::NoDisplay;
        ec = SDL_GetError();
        return;
    }

    auto& config = comp_app::AppLoader::config<comp_app::WindowConfig>(c);

    SDL_WindowFlags extraFlags = 0 /*SDL_WINDOW_HIGH_PIXEL_DENSITY*/;

    extraFlags |= SDL_WINDOW_OPENGL;
    // if([[maybe_unused]] auto glContext = c.service<GLContext>())
    // else
    //     extraFlags |= SDL_WINDOW_VULKAN;

    /* SDL3 takes no position here, the window manager places the window */
    m_window = SDL_CreateWindow(
        config.title.c_str(),
        config.size.w,
        config.size.h,
        sdl3::window_to_sdl3(config.flags) | extraFlags);

    if(!m_window)
    {
        ec = comp_app::AppError::NoWindow;
        ec = SDL_GetError();
        return;
    }

    active_window = m_window;

    if(auto glContext = c.service<GLContext>())
        glContext->setupAttributes(c);

    SDL_SetWindowTitle(m_window, config.title.c_str());

    if(auto nativeWindowInfo = c.service<comp_app::PtrNativeWindowInfo>())
        getWindow(m_window, *nativeWindowInfo);

    if(auto info = c.service<comp_app::AppInfo>())
    {
        std::string wm_selection;
        switch(get_wm_selection(m_window))
        {
        case ws_t::wayland:
            wm_selection = "Wayland";
            break;
        case ws_t::x11:
            wm_selection = "X11";
            break;
        default:
            wm_selection = "Default";
            break;
        }
        info->add("sdl3:windowManager", wm_selection);
    }

    m_container = &c;
}

void Windowing::unload(entity_container&, comp_app::app_error&)
{
    if(m_window)
        SDL_DestroyWindow(m_window);
    active_window = nullptr;
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Windowing::start_restricted(proxy_type& p, time_point const&)
{
    using namespace Coffee::Display;

    auto displayBus =
        p.service<comp_app::BasicEventBus<Event>>();
    if(!displayBus)
        Throw(implementation_error("display bus not available!"));
    Event displayEv;

#define EMIT_DEVENT(from)                      \
    {                                          \
        auto data      = from;                 \
        displayEv.type = data.event_type;      \
        displayBus->process(displayEv, &data); \
    }

    SDL_Event event;
    SDL_PumpEvents();

    /* SDL3 promoted the window events to top-level event types */
    while(SDL_PeepEvents(
        &event,
        1,
        SDL_GETEVENT,
        SDL_EVENT_WINDOW_FIRST,
        SDL_EVENT_WINDOW_LAST))
    {
        switch(event.type)
        {
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        case SDL_EVENT_WINDOW_EXPOSED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            EMIT_DEVENT(translate::event<FocusEvent>(event))
            break;
#if !defined(COFFEE_EMSCRIPTEN)
        case SDL_EVENT_WINDOW_RESIZED:
            EMIT_DEVENT(translate::event<ResizeEvent>(event))
            break;
#endif
        case SDL_EVENT_WINDOW_MOVED:
            EMIT_DEVENT(translate::event<MoveEvent>(event))
            break;
        default:
            break;
        }
    }

#if defined(COFFEE_EMSCRIPTEN)
    int current_width  = EM_ASM_INT({ return canvas.width; });
    int current_height = EM_ASM_INT({ return canvas.height; });
    if(canvas_width != current_width || canvas_height != current_height)
    {
        EMIT_DEVENT(ResizeEvent(current_width, current_height));
        canvas_width  = current_width;
        canvas_height = current_height;
        // SDL's internal window->w/h can drift from canvas dimensions (e.g.
        // when JS sets canvas.height from body.offsetHeight after the 1s
        // template timeout). This drifts SDL's mouse yscale = window->h / css_h
        // off by a large factor until a browser resize triggers
        // Emscripten_HandleResize.
        SDL_SetWindowSize(m_window, current_width, current_height);
    }
#endif

#undef EMIT_DEVENT
}

void Windowing::show()
{
    SDL_ShowWindow(m_window);
}

void Windowing::close()
{
    SDL_HideWindow(m_window);
    if(!m_container)
        return;
    if(auto ctxt = m_container->service<Context>())
        ctxt->m_shouldClose = true;
}

comp_app::size_2d_t Windowing::size() const
{
    size_2d_t out;
#if defined(COFFEE_EMSCRIPTEN)
    out = {canvas_width, canvas_height};
#else
    SDL_GetWindowSize(m_window, &out.w, &out.h);
#endif
    return out;
}

void Windowing::resize([[maybe_unused]] const comp_app::size_2d_t& newSize)
{
    SDL_SetWindowSize(m_window, newSize.w, newSize.h);
}

comp_app::position_t Windowing::position() const
{
    position_t out;
    SDL_GetWindowPosition(m_window, &out.x, &out.y);
    return out;
}

void Windowing::move(const comp_app::position_t& newPos)
{
    SDL_SetWindowPosition(m_window, newPos.x, newPos.y);
}

comp_app::window_flags_t Windowing::state() const
{
    return static_cast<WState>(window_from_sdl3(SDL_GetWindowFlags(m_window)));
}

void Windowing::setState(comp_app::window_flags_t state)
{
    if(enum_helpers::feval(state & F::visible))
        SDL_ShowWindow(m_window);

#if SUPPORTS_WINDOW_ACTIONS == 1
    if(enum_helpers::feval(state & F::minimized))
        SDL_MinimizeWindow(m_window);
    if(enum_helpers::feval(state & F::maximized))
        SDL_MaximizeWindow(m_window);
#endif

    /* Without a display mode set, SDL3 fullscreen is the borderless-desktop
     * mode, which is what F::fullscreen_window asked for */
    if(enum_helpers::feval(state & F::fullscreen) ||
       enum_helpers::feval(state & F::fullscreen_window))
        SDL_SetWindowFullscreen(m_window, true);
    if(enum_helpers::feval(state & F::normal))
        SDL_SetWindowFullscreen(m_window, false);

    if(enum_helpers::feval(state & F::undecorated))
        SDL_SetWindowBordered(m_window, false);
    else
        SDL_SetWindowBordered(m_window, true);

    if(enum_helpers::feval(state & F::focused))
        SDL_RaiseWindow(m_window);
}

bool Windowing::notifiedClose() const
{
    return m_container->service<Context>()->m_shouldClose;
}

namespace {

/* SDL3 display IDs are opaque and need not be contiguous, so a comp_app
 * display index is a position in the current display list */
SDL_DisplayID display_at(libc_types::u32 index)
{
    int  count    = 0;
    auto displays = SDL_GetDisplays(&count);

    if(!displays)
        return 0;

    SDL_DisplayID out =
        index < C_FCAST<libc_types::u32>(count) ? displays[index] : 0;
    SDL_free(displays);
    return out;
}

/* SDL3 replaced display DPI with a content scale where 1.0 is 96 DPI */
constexpr libc_types::f32 base_dpi = 96.f;

libc_types::f32 display_dpi(SDL_DisplayID display)
{
    auto scale = SDL_GetDisplayContentScale(display);
    return scale > 0.f ? scale * base_dpi : base_dpi;
}

} // namespace

comp_app::size_2d_t DisplayInfo::virtualSize() const
{
    size_2d_t out;
    SDL_Rect  displayRect = {};

    int  count    = 0;
    auto displays = SDL_GetDisplays(&count);

    if(!displays)
        return out;

    for(auto i : Range<int>(count))
    {
        SDL_GetDisplayBounds(displays[i], &displayRect);
        out.w = std::max(out.w, displayRect.x + displayRect.w);
        out.h = std::max(out.h, displayRect.y + displayRect.h);
    }
    SDL_free(displays);
    return out;
}

libc_types::u32 DisplayInfo::count() const
{
    int count = 0;
    if(auto displays = SDL_GetDisplays(&count); displays)
        SDL_free(displays);
    return C_FCAST<libc_types::u32>(count);
}

libc_types::u32 DisplayInfo::currentDisplay() const
{
    auto current = active_window ? SDL_GetDisplayForWindow(active_window)
                                 : SDL_GetPrimaryDisplay();

    int  count    = 0;
    auto displays = SDL_GetDisplays(&count);

    if(!displays)
        return 0;

    libc_types::u32 out = 0;
    for(auto i : Range<int>(count))
        if(displays[i] == current)
        {
            out = C_FCAST<libc_types::u32>(i);
            break;
        }
    SDL_free(displays);
    return out;
}

comp_app::size_2d_t DisplayInfo::size(libc_types::u32 idx) const
{
    SDL_Rect rect{};
    SDL_GetDisplayBounds(display_at(idx), &rect);
    return {rect.w, rect.h};
}

comp_app::size_2d_t DisplayInfo::physicalSize(libc_types::u32 i) const
{
    auto display = display_at(i);
    auto density = display_dpi(display);
    auto mode    = SDL_GetCurrentDisplayMode(display);

    if(!mode || density <= 0.f)
        return {};

    return typing::geometry::size_2d<libc_types::f32>{
        mode->w / density, mode->h / density}
        .convert<libc_types::i32>();
}

libc_types::f32 DisplayInfo::dpi([[maybe_unused]] libc_types::u32 i) const
{
#if defined(COFFEE_EMSCRIPTEN)
    return platform::info::device::emscripten::dpi();
#else
    /* Callers use this as a scale factor, not as dots-per-inch: ImGui divides
     * its display size by it. That is the window's pixel-to-logical ratio,
     * which stays 1 unless the window took SDL_WINDOW_HIGH_PIXEL_DENSITY. */
    if(!active_window)
        return 1.f;

    int logical = 0, pixels = 0;
    SDL_GetWindowSize(active_window, &logical, nullptr);
    SDL_GetWindowSizeInPixels(active_window, &pixels, nullptr);

    return logical > 0 ? static_cast<libc_types::f32>(pixels) /
                             static_cast<libc_types::f32>(logical)
                       : 1.f;
#endif
}

libc_types::f32 DisplayInfo::diagonal(libc_types::u32 i) const
{
    auto display = display_at(i);
    auto density = display_dpi(display);
    auto mode    = SDL_GetCurrentDisplayMode(display);

    if(!mode || density <= 0.f)
        return 0.f;

    libc_types::f32 h = mode->w / density, w = mode->h / density;

    return std::sqrt(h * h + w * w);
}

void GLContext::setupAttributes(entity_container& c)
{
    using GLConfig = comp_app::GLConfig;
    auto& glConfig = comp_app::AppLoader::config<GLConfig>(c);

    using namespace typing::pixels;

    SDL_GL_SetAttribute(
        SDL_GL_FRAMEBUFFER_SRGB_CAPABLE,
        properties::get<properties::supports_srgb>(glConfig.framebufferFmt)
            ? true
            : false);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, true);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glConfig.version.major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glConfig.version.minor);

    {
        Sint32 contextProfile = SDL_GL_CONTEXT_PROFILE_CORE;

        if(glConfig.profile & GLConfig::Embedded)
            contextProfile = SDL_GL_CONTEXT_PROFILE_ES;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, contextProfile);
    }

    {
        Sint32 contextFlags = 0;

        if constexpr(compile_info::platform::is_macos)
            contextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

        if(glConfig.profile & GLConfig::Robust)
            contextFlags |= SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;
        if((glConfig.profile & GLConfig::Debug) &&
           !compile_info::platform::is_emscripten)
            contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
    }

    using namespace typing::pixels;

    auto color = properties::get<properties::layout>(glConfig.framebufferFmt);
    auto depth_stencil = properties::get<properties::layout>(glConfig.depthFmt);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, color.r);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, color.g);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, color.b);

    if(glConfig.multisampling.enabled)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, true);
        SDL_GL_SetAttribute(
            SDL_GL_MULTISAMPLESAMPLES, glConfig.multisampling.samples);
    }

    //    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, color.a);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth_stencil.depth);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, depth_stencil.stencil);
}

struct try_create_context
{
    try_create_context(
        SDL_Window* window, SDL_GLContext* context, comp_app::GLConfig& config)
        : window(window)
        , context(context)
        , config(config)
    {
    }

    template<typename version>
    void operator()()
    {
        if((config.profile & version::profile) == 0 || *context)
            return;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version::major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, version::minor);

        *context = SDL_GL_CreateContext(window);

        if(*context)
        {
            config.version.major = version::major;
            config.version.minor = version::minor;
            SDL_GetError();
        }
    }

    SDL_Window*         window;
    SDL_GLContext*      context;
    comp_app::GLConfig& config;
};

void GLContext::load(entity_container& c, comp_app::app_error& ec)
{
    using comp_app::GLConfig;

    auto  window   = c.service<Windowing>()->m_window;
    auto& glConfig = comp_app::AppLoader::config<GLConfig>(c);

    m_context = nullptr;

#if defined(COFFEE_EMSCRIPTEN)
    auto try_create = try_create_context(window, &m_context, glConfig);
    try_create.template operator()<GLConfig::gles3_version>();
    try_create.template operator()<GLConfig::gles2_version>();
#else
    type_list::for_each_rev<GLConfig::valid_versions>(
        try_create_context(window, &m_context, glConfig));
#endif

    if(!m_context)
    {
        ec = SDL_GetError();
        ec = comp_app::AppError::NoContext;
        return;
    }

    if(SDL_FAILED(SDL_GL_MakeCurrent(window, m_context)))
    {
        ec = SDL_GetError();
        ec = comp_app::AppError::SystemError;
        return;
    }

    /* For Emscripten, setting 0 here seems to crash Firefox
     * While Chrome really couldn't care less
     */
    c.service<GLSwapControl>()->setSwapInterval(
        compile_info::platform::is_emscripten ? 1 : glConfig.swapInterval);

    {
        auto& bindConf =
            comp_app::AppLoader::config<comp_app::GraphicsBindingConfig>(c);
        /* SDL3 returns SDL_FunctionPointer instead of void* */
        bindConf.loader =
            reinterpret_cast<comp_app::GraphicsBindingConfig::loader_func>(
                SDL_GL_GetProcAddress);
    }

#if defined(COFFEE_EMSCRIPTEN)
    if(glConfig.profile & GLConfig::Debug)
    {
        auto context_handle = emscripten_webgl_get_current_context();
        emscripten_webgl_enable_extension(
            context_handle, "WEBGL_debug_renderer_info");
        emscripten_webgl_enable_extension(
            context_handle, "WEBGL_debug_shaders");
    }
#endif

    m_container = &c;
}

void GLContext::unload(entity_container&, comp_app::app_error&)
{
    if(m_context)
        SDL_GL_DestroyContext(m_context);
}

libc_types::i32 GLSwapControl::swapInterval() const
{
    int interval = 0;
    SDL_GL_GetSwapInterval(&interval);
    return interval;
}

void GLSwapControl::setSwapInterval(libc_types::i32 interval)
{
    SDL_GL_SetSwapInterval(interval);
}

void GLFramebuffer::load(entity_container& c, comp_app::app_error&)
{
    m_container = &c;
}

void GLFramebuffer::swapBuffers(comp_app::app_error&)
{
    if constexpr(compile_info::debug_mode)
        Coffee::Profiler::PushContext("sdl3::GLFramebuffer::swapBuffers");

    SDL_GL_SwapWindow(m_container->service<sdl3::Windowing>()->m_window);

    if constexpr(compile_info::debug_mode)
        Coffee::Profiler::PopContext();
}

comp_app::size_2d_t GLFramebuffer::size() const
{
    size_2d_t out;
#if defined(COFFEE_EMSCRIPTEN)
    out = {canvas_width, canvas_height};
#else
    SDL_GetWindowSizeInPixels(
        m_container->service<Windowing>()->m_window, &out.w, &out.h);
#endif
    return out;
}

void GLFramebuffer::start_frame(
    comp_app::detail::ContainerProxy&, time_point const&)
{
    defaultSwap();
}

void ControllerInput::load(entity_container& c, comp_app::app_error& ec)
{
    using namespace platform::url::constructors;

    auto& config = comp_app::AppLoader::config<comp_app::ControllerConfig>(c);

    if(config.mapping.empty())
    {
        auto controllerdb = *"controllerdb.txt"_config;
        cDebug("Looking for controllerdb in: {}", controllerdb);
        SDL_AddGamepadMappingsFromFile(controllerdb.c_str());
        SDL_SetHint(SDL_HINT_GAMECONTROLLERCONFIG_FILE, controllerdb.c_str());
    } else
        SDL_SetHint(SDL_HINT_GAMECONTROLLERCONFIG_FILE, config.mapping.c_str());
    if(config.options & comp_app::ControllerConfig::BackgroundInput)
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if(SDL_FAILED(SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMEPAD)))
    {
        ec = SDL_GetError();
        ec = comp_app::AppError::SystemError;
        return;
    }

    {
        constexpr auto max_val  = std::numeric_limits<libc_types::i16>::max();
        auto           dead_val = max_val - config.deadzone;

        m_axisScale    = max_val / libc_types::f32(dead_val);
        m_axisDeadzone = config.deadzone;
    }
}

void ControllerInput::unload(entity_container&, comp_app::app_error&)
{
    for(auto const& controller : m_controllers)
        SDL_CloseGamepad(C_RCAST<SDL_Gamepad*>(controller.second));

    m_controllers.clear();
    m_playerIndex.clear();
    m_deviceToPlayer.clear();

    SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMEPAD);
    SDL_GetError();
}

void ControllerInput::start_restricted(proxy_type& p, time_point const&)
{
    using namespace Coffee::Input;

    auto inputBus =
        p.service<comp_app::BasicEventBus<CIEvent>>();
    CIEvent inputEv;

    SDL_Event event;
    SDL_PumpEvents();

    while(SDL_PeepEvents(
        &event,
        1,
        SDL_GETEVENT,
        SDL_EVENT_GAMEPAD_ADDED,
        SDL_EVENT_GAMEPAD_REMAPPED))
    {
        if(event.type == SDL_EVENT_GAMEPAD_ADDED)
        {
            /* SDL3 addresses gamepads by instance ID, not device index, so
             * event.gdevice.which is already the key used below */
            auto instanceId = event.gdevice.which;
            auto controller = SDL_OpenGamepad(instanceId);

            if(!controller)
            {
                /* Browsers can announce a gamepad before it is readable, so
                 * this is not fatal */
                Coffee::Logging::cDebug(
                    "Failed to open controller {}: {}",
                    instanceId,
                    SDL_GetError());
                continue;
            }

            auto playerIdx = SDL_GetGamepadPlayerIndex(controller);

            playerIdx = playerIdx >= 0
                            ? playerIdx
                            : C_FCAST<libc_types::i32>(m_playerIndex.size());

            m_playerIndex.insert({playerIdx, controller});
            m_controllers.insert({C_FCAST<int>(instanceId), controller});
            m_deviceToPlayer.insert({C_FCAST<int>(instanceId), playerIdx});

            CIEvent                  ev{.type = CIEvent::ControllerConnect};
            CIControllerConnectEvent connect = {
                .index = static_cast<libc_types::u16>(event.gdevice.which),
                .player_index = static_cast<libc_types::i16>(playerIdx),
                .connected    = true,
            };
            inputBus->process(ev, &connect);

#if 0
            SDL_RumbleGamepad(controller, 7000, 9000, 200);
#endif
            Coffee::Logging::cDebug(
                "Player {} connected (instance={})", playerIdx, instanceId);
        } else if(event.type == SDL_EVENT_GAMEPAD_REMOVED)
        {
            auto    playerIdx = m_deviceToPlayer[event.gdevice.which];
            CIEvent ev{.type = CIEvent::ControllerConnect};
            CIControllerConnectEvent connect = {
                .index = static_cast<libc_types::u16>(event.gdevice.which),
                .player_index = static_cast<libc_types::i16>(playerIdx),
                .connected    = false,
            };
            inputBus->process(ev, &connect);

            controllerDisconnect(event.gdevice.which);
            Coffee::Logging::cDebug("Player {} disconnected", playerIdx);
        } else if(event.type == SDL_EVENT_GAMEPAD_REMAPPED)
        {
            Coffee::Logging::cDebug("Player {} remapped", event.gdevice.which);
        }

        EMIT_IEVENT(translate::event<CIControllerAtomicUpdateEvent>(event))
    }

    while(SDL_PeepEvents(
        &event,
        1,
        SDL_GETEVENT,
        SDL_EVENT_GAMEPAD_AXIS_MOTION,
        SDL_EVENT_GAMEPAD_BUTTON_UP))
        ;

    while(SDL_PeepEvents(
        &event, 1, SDL_GETEVENT, SDL_EVENT_JOYSTICK_AXIS_MOTION, SDL_EVENT_JOYSTICK_REMOVED))
        ;
}

libc_types::u32 ControllerInput::count() const
{
    return C_FCAST<libc_types::u32>(m_controllers.size());
}

ControllerInput::controller_map ControllerInput::state(
    libc_types::u32 idx) const
{
    auto it = m_playerIndex.find(C_FCAST<libc_types::i32>(idx));

    if(it == m_playerIndex.end())
        return {};

    auto controller = C_RCAST<SDL_Gamepad*>(it->second);

    controller_map out;

    /* SDL3 has a real gamepad backend on Emscripten, unlike SDL2 */
    {
#define BTN SDL_GetGamepadButton
#define AXIS SDL_GetGamepadAxis

        out.buttons.e.a   = BTN(controller, SDL_GAMEPAD_BUTTON_SOUTH);
        out.buttons.e.b   = BTN(controller, SDL_GAMEPAD_BUTTON_EAST);
        out.buttons.e.x   = BTN(controller, SDL_GAMEPAD_BUTTON_WEST);
        out.buttons.e.y   = BTN(controller, SDL_GAMEPAD_BUTTON_NORTH);
        out.buttons.e.b_l = BTN(controller, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
        out.buttons.e.b_r =
            BTN(controller, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
        out.buttons.e.s_l  = BTN(controller, SDL_GAMEPAD_BUTTON_LEFT_STICK);
        out.buttons.e.s_r  = BTN(controller, SDL_GAMEPAD_BUTTON_RIGHT_STICK);
        out.buttons.e.p_up = BTN(controller, SDL_GAMEPAD_BUTTON_DPAD_UP);
        out.buttons.e.p_down = BTN(controller, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        out.buttons.e.p_left = BTN(controller, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        out.buttons.e.p_right =
            BTN(controller, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
        out.buttons.e.back  = BTN(controller, SDL_GAMEPAD_BUTTON_BACK);
        out.buttons.e.start = BTN(controller, SDL_GAMEPAD_BUTTON_START);
        out.buttons.e.guide = BTN(controller, SDL_GAMEPAD_BUTTON_GUIDE);

        out.axes.e.l_x = rescale(AXIS(controller, SDL_GAMEPAD_AXIS_LEFTX));
        out.axes.e.l_y = rescale(AXIS(controller, SDL_GAMEPAD_AXIS_LEFTY));
        out.axes.e.r_x = rescale(AXIS(controller, SDL_GAMEPAD_AXIS_RIGHTX));
        out.axes.e.r_y = rescale(AXIS(controller, SDL_GAMEPAD_AXIS_RIGHTY));
        out.axes.e.t_l = AXIS(controller, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
        out.axes.e.t_r = AXIS(controller, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

#undef BTN
#undef AXIS
    }

    return out;
}

comp_app::text_type_t ControllerInput::name(libc_types::u32 idx) const
{
    auto it = m_playerIndex.find(C_FCAST<libc_types::i32>(idx));

    if(it == m_playerIndex.end())
        return {};

    auto name = SDL_GetGamepadName(C_RCAST<SDL_Gamepad*>(it->second));

    return name ? name : std::string();
}

libc_types::i16 ControllerInput::rescale(libc_types::i16 value) const
{
    if(value < 0)
    {
        if(value > -m_axisDeadzone)
            value = 0;
        else
            value += m_axisDeadzone;
    } else if(value > 0)
    {
        if(value < m_axisDeadzone)
            value = 0;
        else
            value -= m_axisDeadzone;
    }

    return libc_types::i16(m_axisScale * value);
}

int ControllerInput::controllerDisconnect(int device)
{
    auto it = m_controllers.find(device);

    if(it == m_controllers.end())
    {
        Throw(undefined_behavior("failed to close device"));
    }

    auto controller = C_RCAST<SDL_Gamepad*>(it->second);

    SDL_CloseGamepad(controller);

    decltype(m_playerIndex)::key_type delete_key = -1;

    for(auto const& v : m_playerIndex)
        if(v.second == it->second)
            delete_key = v.first;

    m_playerIndex.erase(delete_key);
    m_controllers.erase(device);
    m_deviceToPlayer.erase(device);

    return delete_key;
}

void KeyboardInput::start_restricted(proxy_type& p, time_point const&)
{
    using namespace Coffee::Input;

    CIEvent   inputEv;
    SDL_Event event;

    auto inputBus =
        p.service<comp_app::BasicEventBus<CIEvent>>();

    while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_KEY_DOWN, SDL_EVENT_KEY_UP))
    {
        auto ev            = translate::event<CIKeyEvent>(event);
        m_register[ev.key] = ev.mod;
        EMIT_IEVENT(ev)
    }

    while(
        SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_TEXT_EDITING, SDL_EVENT_TEXT_INPUT))
    {
        switch(event.type)
        {
        case SDL_EVENT_TEXT_EDITING: {
            CIWEditEvent edit;
            edit.cursor = event.edit.start;
            edit.len    = event.edit.length;
            edit.text   = event.edit.text;
            EMIT_IEVENT(edit)
            break;
        }
        case SDL_EVENT_TEXT_INPUT: {
            CIWriteEvent write;
            write.text = event.text.text;
            EMIT_IEVENT(write)
            break;
        }
        default:
            break;
        }
    }
}

void KeyboardInput::startWriting() const
{
    if(active_window)
        SDL_StartTextInput(active_window);
}

void KeyboardInput::stopWriting() const
{
    if(active_window)
        SDL_StopTextInput(active_window);
}

void MouseInput::load(entity_container& e, comp_app::app_error&)
{
    m_container = &e;
    priority    = 256;
}

void MouseInput::start_restricted(proxy_type&, time_point const&)
{
    using namespace Coffee::Input;

    CIEvent inputEv;
    auto    inputBus = m_container->service<comp_app::BasicEventBus<CIEvent>>();

    SDL_Event event;
    while(SDL_PeepEvents(
        &event, 1, SDL_GETEVENT, SDL_EVENT_MOUSE_MOTION, SDL_EVENT_MOUSE_WHEEL))
    {
        switch(event.type)
        {
        case SDL_EVENT_MOUSE_MOTION:
            EMIT_IEVENT(translate::event<CIMouseMoveEvent>(event))
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            auto ev = translate::event<CIMouseButtonEvent>(event);
            EMIT_IEVENT(ev)

            if(event.button.down)
                m_buttons |= ev.btn;
            else
                m_buttons &= (m_buttons ^ ev.btn);

            break;
        }
        case SDL_EVENT_MOUSE_WHEEL:
            EMIT_IEVENT(translate::event<CIScrollEvent>(event))
            break;
        }
    }

    while(
        SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_DROP_FILE, SDL_EVENT_DROP_COMPLETE))
    {
        cDebug("Drop event: {}", static_cast<int>(event.drop.type));
        switch(event.type)
        {
        case SDL_EVENT_DROP_FILE: {
            /* SDL3 owns drop.data, and it is null outside file/text drops */
            if(!event.drop.data)
                break;
            cDebug("Received file: {}", event.drop.data);
            if(!std::isprint(event.drop.data[0]))
                break;
            inputEv.type = Coffee::Input::CIEvent::Drop;
            CIDropEvent drop;
            drop.file = platform::url::constructors::MkUrl(event.drop.data);
            inputBus->process(inputEv, &drop);
            break;
        }
        case SDL_EVENT_DROP_TEXT:
            break;
        case SDL_EVENT_DROP_BEGIN:
            cDebug("Something's coming!");
            break;
        case SDL_EVENT_DROP_COMPLETE:
            cDebug("Aaaand it's done");
            break;
        }
    }
}

bool MouseInput::mouseGrabbed() const
{
    auto window = m_container->service<Windowing>()->m_window;
    return window ? SDL_GetWindowRelativeMouseMode(window) : false;
}

void MouseInput::setMouseGrab(bool enabled)
{
    if(auto window = m_container->service<Windowing>()->m_window; window)
        SDL_SetWindowRelativeMouseMode(window, enabled);
}

comp_app::position_t MouseInput::position() const
{
    /* SDL3 reports mouse position in floats */
    libc_types::f32 x{0.f}, y{0.f};

    if(mouseGrabbed())
        SDL_GetRelativeMouseState(&x, &y);
    else
        SDL_GetMouseState(&x, &y);

    position_t out;
    out.x = static_cast<libc_types::i32>(x);
    out.y = static_cast<libc_types::i32>(y);
    return out;
}

void MouseInput::warp(const comp_app::position_t& newPos)
{
    SDL_WarpMouseInWindow(
        m_container->service<Windowing>()->m_window,
        static_cast<libc_types::f32>(newPos.x),
        static_cast<libc_types::f32>(newPos.y));
}

MouseInput::MouseButton MouseInput::buttons() const
{
    return m_buttons;
}

void WindowInfo::load(entity_container& e, comp_app::app_error&)
{
    m_container = &e;
}

comp_app::text_type_t WindowInfo::name() const
{
    auto name = SDL_GetWindowTitle(m_container->service<Windowing>()->m_window);

    if(name)
        return name;
    return {};
}

void WindowInfo::setName(comp_app::text_type newName)
{
    SDL_SetWindowTitle(
        m_container->service<Windowing>()->m_window, newName.c_str());
}

} // namespace sdl3

#include <string_view>

namespace sdl3 {

namespace {

/* SDL3 names the active backend at runtime, so the handles below no longer
 * depend on which SDL_VIDEO_DRIVER_* defines SDL happened to be built with */
bool driver_is(std::string_view name)
{
    auto const driver = SDL_GetCurrentVideoDriver();
    return driver && driver == name;
}

} // namespace

void getWindow(
    SDL_Window*                                                 window,
    [[maybe_unused]] comp_app::interfaces::PtrNativeWindowInfo& info)
{
    using namespace comp_app;

    auto const props = SDL_GetWindowProperties(window);

    if(driver_is("x11"))
    {
        info.display = SDL_GetPointerProperty(
            props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
        info.window        = C_RCAST<void*>(C_FCAST<intptr_t>(
            SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0)));
        info.window_system = ws_t::x11;
    } else if(driver_is("wayland"))
    {
        info.display = SDL_GetPointerProperty(
            props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
        info.window = SDL_GetPointerProperty(
            props, SDL_PROP_WINDOW_WAYLAND_EGL_WINDOW_POINTER, nullptr);
        info.surface = SDL_GetPointerProperty(
            props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        info.window_system = ws_t::wayland;
    } else if(driver_is("windows"))
    {
        info.display = SDL_GetPointerProperty(
            props, SDL_PROP_WINDOW_WIN32_HDC_POINTER, nullptr);
        info.window = SDL_GetPointerProperty(
            props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    } else if(driver_is("cocoa"))
    {
        info.window = SDL_GetPointerProperty(
            props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
    }
    /* offscreen, emscripten, KMSDRM and dummy expose no handles to hand out */
}

static ws_t get_wm_selection(SDL_Window*)
{
    if(driver_is("wayland"))
        return ws_t::wayland;
    if(driver_is("x11"))
        return ws_t::x11;
    if(driver_is("offscreen"))
        return ws_t::surfaceless;
    return ws_t::nullws;
}

} // namespace sdl3
