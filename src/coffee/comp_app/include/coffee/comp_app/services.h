#pragma once

#include <coffee/components/components.h>
#include <coffee/core/types/input/event_types.h>

#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/thread_types.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/geometry/point.h>
#include <peripherals/typing/geometry/size.h>
#include <peripherals/typing/pixels/rgba.h>

#include "app_error.h"

#include <fmt/format.h>

namespace comp_app {
namespace detail {
using namespace Coffee::Components;

template<typename T>
using tag_t = detail::TagType<T>;
} // namespace detail

using text_type   = std::string const&;
using text_type_t = std::string;
using position_t  = typing::geometry::point_2d<libc_types::i32>;
using size_2d_t   = typing::geometry::size_2d<libc_types::i32>;
using PixFmt      = typing::pixels::PixFmt;

using Coffee::Components::component_list;
using Coffee::Components::subsystem_list;

enum class window_flags_t : libc_types::u32
{
    none = 0x0,

    /* State flags */
    minimized = 0x1,
    normal    = 0x2,
    maximized = 0x4,
    focused   = 0x8,
    visible   = 0x10,

    /* Window creation flags */
    undecorated       = 0x10001,
    fullscreen        = 0x10002,
    windowed          = 0x10004,
    fullscreen_window = 0x10008 | undecorated | windowed,
    resizable         = 0x10010,
    high_dpi          = 0x10020,
    floating          = 0x10040,
};

C_FLAGS(window_flags_t, libc_types::u32);

namespace interfaces {

struct AppInfo
{
    virtual void        add(text_type key, text_type value) = 0;
    virtual text_type_t get(text_type key)                  = 0;
};

struct Windowing
{
    virtual void show()  = 0;
    virtual void close() = 0;

    virtual size_2d_t  size() const                     = 0;
    virtual void       resize(size_2d_t const& newSize) = 0;
    virtual position_t position() const                 = 0;
    virtual void       move(position_t const& newPos)   = 0;

    virtual void fullscreenBorderless()
    {
        setState(window_flags_t::fullscreen_window);
    }
    virtual void fullscreenExclusive()
    {
        setState(window_flags_t::fullscreen);
    }

    virtual window_flags_t state() const                  = 0;
    virtual void           setState(window_flags_t state) = 0;

    virtual bool notifiedClose() const
    {
        return false;
    }
};

struct StaticWindowing : Windowing
{
    virtual void show() final
    {
    }
    virtual void close()
    {
        m_notifiedClose = true;
    }
    virtual void resize(size_2d_t const&) final
    {
    }
    virtual position_t position() const final
    {
        return {};
    }
    virtual void move(position_t const&) final
    {
    }
    virtual bool notifiedClose() const
    {
        return m_notifiedClose;
    }

  private:
    bool m_notifiedClose{false};
};

struct Dialogs
{
    using prompt_callback  = std::function<void(text_type)>;
    using confirm_callback = std::function<void(bool)>;

    virtual void info(text_type title, text_type message)  = 0;
    virtual void warn(text_type title, text_type message)  = 0;
    virtual void error(text_type title, text_type message) = 0;

    virtual void confirm(
        text_type title, text_type message, confirm_callback&& callback)
        = 0;
    virtual void prompt(text_type title, prompt_callback&& callback) = 0;
};

struct DisplayInfo
{
    virtual size_2d_t       virtualSize() const                     = 0;
    virtual libc_types::u32 count() const                           = 0;
    virtual libc_types::u32 currentDisplay() const                  = 0;
    virtual size_2d_t       size(libc_types::u32 idx) const         = 0;
    virtual size_2d_t       physicalSize(libc_types::u32 idx) const = 0;
    virtual libc_types::f32 dpi(libc_types::u32 idx) const          = 0;
    virtual libc_types::f32 diagonal(libc_types::u32 idx) const     = 0;
};

struct SingleDisplayInfo : DisplayInfo
{
    virtual libc_types::u32 count() const final
    {
        return 1;
    }
    virtual libc_types::u32 currentDisplay() const final
    {
        return 0;
    }
    virtual size_2d_t size(libc_types::u32) const final
    {
        return virtualSize();
    }
};

struct ScreensaverInfo
{
    virtual bool isEnabled() const        = 0;
    virtual void setEnabled(bool enabled) = 0;
};

struct WindowInfo
{
    virtual text_type_t name() const               = 0;
    virtual void        setName(text_type newName) = 0;
};

struct NativeWindowInfo
{
};

struct PtrNativeWindowInfo : NativeWindowInfo
{
    using NDisplay = void*;
    using NWindow  = void*;

    NDisplay display{nullptr};
    NWindow  window{nullptr};
};

template<typename EventType>
struct EventBus
{
    virtual ~EventBus()
    {
    }

    virtual void inject(EventType& ev, libc_types::c_ptr data)
    {
        process(ev, data);
    }
    virtual void process(EventType& ev, libc_types::c_ptr data) = 0;
};

template<typename EventType>
struct BasicEventBus : EventBus<EventType>
{
    virtual ~BasicEventBus()
    {
    }

    struct EvData
    {
        libc_types::u32                                    prio;
        std::function<void(EventType&, libc_types::c_ptr)> handler;
    };

    template<typename HandlerType>
    void addEventHandler(libc_types::u32 prio, HandlerType&& hnd)
    {
        auto handler = [hnd](EventType& ev, libc_types::c_ptr data) mutable {
            if(ev.type == HandlerType::event_type::event_type)
                hnd(ev, C_RCAST<typename HandlerType::event_type*>(data));
        };

        m_handlers.push_back({prio, std::move(handler)});
        sort_handlers();
    }

    template<typename SubEventType>
    void addEventFunction(
        libc_types::u32                                  prio,
        std::function<void(EventType&, SubEventType*)>&& hnd)
    {
        auto handler = [hnd](EventType& ev, libc_types::c_ptr data) mutable {
            if(ev.type == SubEventType::event_type)
                hnd(ev, C_RCAST<SubEventType*>(data));
        };

        m_handlers.push_back({prio, std::move(handler)});
        sort_handlers();
    }

    void addEventData(EvData&& data)
    {
        m_handlers.push_back(std::move(data));
        sort_handlers();
    }

    virtual void process(EventType& ev, libc_types::c_ptr data) final
    {
        for(auto& e : m_handlers)
            e.handler(ev, data);
    }

    std::vector<EvData> m_handlers;

    static bool handler_sorter(EvData const& e1, EvData const& e2)
    {
        return e1.prio < e2.prio;
    }
    void sort_handlers()
    {
        std::sort(m_handlers.begin(), m_handlers.end(), handler_sorter);
    }
};

struct MouseInput
{
    using MouseButton = Coffee::Input::CIMouseButtonEvent::MouseButton;

    virtual bool mouseGrabbed() const       = 0;
    virtual void setMouseGrab(bool enabled) = 0;

    virtual position_t  position() const               = 0;
    virtual void        warp(position_t const& newPos) = 0;
    virtual MouseButton buttons() const                = 0;
};

struct KeyboardInput
{
    using KeyModifiers = Coffee::Input::CIKeyEvent::KeyModifiers;

    virtual void openVirtual() const
    {
    }

    virtual void closeVirtual() const
    {
    }

    virtual KeyModifiers key(libc_types::u32) const
    {
        return KeyModifiers::NoneModifier;
    }
};

struct BasicKeyboardInput : KeyboardInput
{
    using KeyModifiers  = Coffee::Input::CIKeyEvent::KeyModifiers;
    using register_type = std::map<libc_types::u32, KeyModifiers>;

    struct EventHandler
    {
        EventHandler(register_type& registr) : m_register(registr)
        {
        }
        using event_type = Coffee::Input::CIKeyEvent;

        void operator()(Coffee::Input::CIEvent const&, event_type const* ev)
        {
            m_register[ev->key] = ev->mod;
        }
        register_type& m_register;
    };

    virtual KeyModifiers key(libc_types::u32 key) const final
    {
        auto it = m_register.find(key);
        return it == m_register.end() ? KeyModifiers::NoneModifier : it->second;
    }

  protected:
    std::map<libc_types::u32, KeyModifiers> m_register;
};

struct ControllerInput
{
    using controller_map = Coffee::Input::CIControllerState;

    virtual libc_types::u32 count() const                    = 0;
    virtual controller_map  state(libc_types::u32 idx) const = 0;
    virtual text_type_t     name(libc_types::u32 idx) const  = 0;
};

struct TouchInput
{
    virtual libc_types::u32 points() const = 0;

    virtual position_t position(libc_types::u32 idx) const      = 0;
    virtual position_t startPosition(libc_types::u32 idx) const = 0;
};

struct GestureInput
{
    enum class gesture_type_t
    {
        double_tap,
        long_press,
        pan,
        pinch,
        swipe,
        tap,
    };
};

struct GraphicsBinding
{
};

struct GraphicsContext
{
};

struct GraphicsFramebuffer
{
    void defaultSwap()
    {
        app_error ec;
        swapBuffers(ec);
        C_ERROR_CHECK(ec);
    }

    virtual libc_types::u32 buffers() const
    {
        return 1;
    }
    virtual size_2d_t size() const = 0;
    virtual PixFmt    format() const
    {
        return PixFmt::None;
    }
    virtual PixFmt depthFormat() const
    {
        return PixFmt::None;
    }
    virtual PixFmt stencilFormat() const
    {
        return PixFmt::None;
    }
    virtual void swapBuffers(app_error& ec) = 0;
};

struct GraphicsThreadInfo
{
    virtual stl_types::ThreadId::Hash thread() const             = 0;
    virtual void                      makeCurrent(app_error& ec) = 0;
};

struct GraphicsSwapControl
{
    virtual libc_types::i32 swapInterval() const                      = 0;
    virtual void            setSwapInterval(libc_types::i32 interval) = 0;
};

template<typename T>
struct ClockProvider
{
    enum class Governor
    {
        Powersaving,
        Ondemand,
        Performance,
    };

    virtual libc_types::u32 threads() = 0;

    virtual Governor        governor(libc_types::u32)  = 0;
    virtual libc_types::f64 frequency(libc_types::u32) = 0;
};

struct CPUClockProvider : ClockProvider<CPUClockProvider>
{
};

template<typename T>
struct TempProvider
{
    enum class Presence
    {
        Present,
        Absent,
    };

    enum class DeviceClass
    {
        Default,
        CPU,
        GPU,
        SOC,
        Battery,
        Ambient,
        Multi,
    };

    virtual DeviceClass device() = 0;

    virtual Presence sensorPresence(DeviceClass = DeviceClass::Default)
    {
        return Presence::Absent;
    }
    virtual libc_types::f32 value(DeviceClass = DeviceClass::Default)
    {
        return 0.f;
    }
};

struct CPUTempProvider : TempProvider<CPUTempProvider>
{
    virtual DeviceClass device()
    {
        return DeviceClass::CPU;
    }
};

struct GPUTempProvider : TempProvider<GPUTempProvider>
{
    virtual DeviceClass device()
    {
        return DeviceClass::GPU;
    }
};

struct MultiTempProvider : TempProvider<MultiTempProvider>
{
    virtual DeviceClass device()
    {
        return DeviceClass::Multi;
    }
};

struct NetworkStatProvider
{
    virtual libc_types::u32 received() const    = 0;
    virtual libc_types::u32 transmitted() const = 0;
    virtual libc_types::u32 connections() const = 0;

    virtual void reset_counters() = 0;
};

struct MemoryStatProvider
{
    virtual libc_types::u32 resident() = 0;
};

struct BatteryProvider
{
    enum class PowerSource
    {
        AC,
        Battery,
    };

    virtual PowerSource     source() const     = 0;
    virtual libc_types::u16 percentage() const = 0;
};

struct ScreenshotProvider
{
    virtual size_2d_t                                   size() const   = 0;
    virtual semantic::mem_chunk<typing::pixels::rgba_t> pixels() const = 0;
};

} // namespace interfaces

template<class ExposedType, class... OtherServices>
struct AppServiceTraits
{
    using type     = ExposedType;
    using services = detail::subsystem_list<ExposedType, OtherServices...>;
};

template<class ConfigType>
struct ConfigTraits
{
    using type = ConfigType;
};

template<class ExposedConfig>
struct Config : detail::SubsystemBase
{
    using type = ExposedConfig;

    virtual ExposedConfig const& get() const final
    {
        return *C_CAST<ExposedConfig const*>(this);
    }
    virtual ExposedConfig& get() final
    {
        return *C_CAST<ExposedConfig*>(this);
    }
};

struct WindowConfig : Config<WindowConfig>
{
    text_type_t    title;
    size_2d_t      size;
    position_t     position;
    window_flags_t flags = window_flags_t::resizable | window_flags_t::windowed
                           | window_flags_t::visible;
};

struct ControllerConfig : Config<ControllerConfig>
{
    enum Options
    {
        None            = 0x0,
        BackgroundInput = 0x1,
    };

    text_type_t     mapping;
    libc_types::i16 deadzone = 6000;
    Options         options  = None;
};

struct TouchConfig : Config<TouchConfig>
{
    enum Options
    {
        None         = 0x0,
        TouchToMouse = 0x1,
        Gestures     = 0x2,
    };

    Options options = Gestures;
};
C_FLAGS(TouchConfig::Options, libc_types::u32);

struct GraphicsBindingConfig : Config<GraphicsBindingConfig>
{
    using loader_func = void* (*)(const char*);

    loader_func loader = nullptr;
};

template<detail::is_subsystem ExposedType, typename Iface = void>
struct AppService : detail::SubsystemBase
{
    using type         = ExposedType;
    using exposed_type = ExposedType;
    using components   = type_safety::empty_list_t;
    using subsystems   = type_safety::empty_list_t;
    using services     = typename std::conditional_t<
        std::is_same_v<Iface, void>,
        detail::subsystem_list<ExposedType>,
        detail::subsystem_list<ExposedType, Iface>>;
    using time_point = detail::time_point;

    virtual ~AppService()
    {
    }

    virtual void start_frame(
        detail::ContainerProxy& c, detail::time_point const& t)
    {
        detail::restricted::start_frame(
            C_OCAST<exposed_type&>(*this), c.underlying(), t);
    }
    virtual void end_frame(
        detail::ContainerProxy& c, detail::time_point const& t)
    {
        detail::restricted::end_frame(
            C_OCAST<exposed_type&>(*this), c.underlying(), t);
    }

    virtual std::string_view subsystem_name() const final
    {
        return typeid(ExposedType).name();
    }

    template<typename InternalType, typename... Args>
    requires std::is_convertible_v<InternalType*, ExposedType*>
    static InternalType& register_service(
        detail::EntityContainer& container, Args... args)
    {
//        using tag_type = AppServiceTraits<ExposedType>;

        auto& subsys
            = container.register_subsystem_inplace<ExposedType, InternalType>(
                std::forward<Args>(args)...);

        container.register_subsystem_services<ExposedType>(&subsys);

        if(subsys.priority == AppService::default_prio)
            subsys.priority = AppService::system_prio;

        return subsys;
    }
};

struct AppLoadableService
{
    enum State
    {
        Uninitialized,
        Initialized,
        Error,
    };

    using entity_container = detail::EntityContainer;

    bool do_load(entity_container& e, app_error& ec)
    {
        load(e, ec);
        m_state = ec ? Error : Initialized;
        return static_cast<bool>(ec);
    }
    bool do_unload(entity_container& e, app_error& ec)
    {
        unload(e, ec);
        m_state = ec ? Error : Uninitialized;
        return static_cast<bool>(ec);
    }

    bool is_loaded() const
    {
        return m_state != Uninitialized;
    }

    State current_state() const
    {
        return m_state;
    }

  protected:
    virtual void load(entity_container& e, app_error& ec)
    {
        (void)e, (void)ec;
    }
    virtual void unload(entity_container& e, app_error& ec)
    {
        (void)e, (void)ec;
    }

  private:
    State m_state = Uninitialized;
};

using AppInfo             = detail::tag_t<interfaces::AppInfo>;
using BatteryProvider     = detail::tag_t<interfaces::BatteryProvider>;
using ControllerInput     = detail::tag_t<interfaces::ControllerInput>;
using CPUClockProvider    = detail::tag_t<interfaces::CPUClockProvider>;
using CPUTempProvider     = detail::tag_t<interfaces::CPUTempProvider>;
using DisplayInfo         = detail::tag_t<interfaces::DisplayInfo>;
using GPUTempProvider     = detail::tag_t<interfaces::GPUTempProvider>;
using GraphicsBinding     = detail::tag_t<interfaces::GraphicsBinding>;
using GraphicsContext     = detail::tag_t<interfaces::GraphicsContext>;
using GraphicsFramebuffer = detail::tag_t<interfaces::GraphicsFramebuffer>;
using GraphicsSwapControl = detail::tag_t<interfaces::GraphicsSwapControl>;
using GraphicsThreadInfo  = detail::tag_t<interfaces::GraphicsThreadInfo>;
using KeyboardInput       = detail::tag_t<interfaces::KeyboardInput>;
using MemoryStatProvider  = detail::tag_t<interfaces::MemoryStatProvider>;
using MouseInput          = detail::tag_t<interfaces::MouseInput>;
using NetworkStatProvider = detail::tag_t<interfaces::NetworkStatProvider>;
using PtrNativeWindowInfo = detail::tag_t<interfaces::PtrNativeWindowInfo>;
using ScreenshotProvider  = detail::tag_t<interfaces::ScreenshotProvider>;
using TouchInput          = detail::tag_t<interfaces::TouchInput>;
using WindowInfo          = detail::tag_t<interfaces::WindowInfo>;
using Windowing           = detail::tag_t<interfaces::Windowing>;

template<typename EType>
struct EventBus
    : interfaces::EventBus<EType>,
      AppService<EventBus<EType>, detail::tag_t<interfaces::EventBus<EType>>>
{
};

template<typename EType>
struct BasicEventBus : interfaces::BasicEventBus<EType>,
                       AppService<
                           BasicEventBus<EType>,
                           detail::tag_t<interfaces::BasicEventBus<EType>>>
{
    //    using services = detail::subsystem_list<
    //        BasicEventBus<EType>,
    //        interfaces::EventBus<EType>,
    //        interfaces::BasicEventBus<EType>>;

    using readable_services = type_safety::empty_list_t;
    using proxy_type        = detail::restricted::proxy_t<BasicEventBus<EType>>;

    void start_restricted(proxy_type&, detail::time_point const&)
    {
    }
};

struct PtrNativeWindowInfoService : interfaces::PtrNativeWindowInfo,
                                    AppService<PtrNativeWindowInfoService>
{
    PtrNativeWindowInfoService()
    {
        priority = 500;
    }
};

} // namespace comp_app

template<>
struct fmt::formatter<Coffee::Input::CIControllerState>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(Coffee::Input::CIControllerState const& s, FormatCtx& ctx)
    {
        return fmt::format_to(
            ctx.out(),
            "controller("
            "a={}, b={}, x={}, y={}, "
            "bumper_l={}, bumper_r{}, "
            "stick_l={}, stick_r={}, "
            "start={}, back={}, "
            "left={}|{}, right={}|{}, "
            "triggers={}|{}, "
            "dpad={}|{}|{}|{})",
            s.buttons.e.a,
            s.buttons.e.b,
            s.buttons.e.x,
            s.buttons.e.y,
            s.buttons.e.b_l,
            s.buttons.e.b_r,
            s.buttons.e.s_l,
            s.buttons.e.s_r,
            s.buttons.e.start,
            s.buttons.e.back,
            s.axes.e.l_x,
            s.axes.e.l_y,
            s.axes.e.r_x,
            s.axes.e.r_y,
            s.axes.e.t_l,
            s.axes.e.t_r,
            s.buttons.e.p_left,
            s.buttons.e.p_right,
            s.buttons.e.p_up,
            s.buttons.e.p_down);
    }
};

#include "services.inl"
