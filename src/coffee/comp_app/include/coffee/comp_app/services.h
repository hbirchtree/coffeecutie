#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <coffee/components/types.h>
#include <coffee/core/types/display/properties.h>
#include <coffee/core/types/input/event_types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/thread_types.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/geometry/point.h>
#include <peripherals/typing/geometry/size.h>

#include "app_error.h"

namespace comp_app {
namespace detail {
using namespace Coffee::Components;

using WindowState = Coffee::Display::Properties::State;
} // namespace detail

using text_type   = Coffee::CString const&;
using text_type_t = Coffee::CString;
using position_t  = typing::geometry::point_2d<libc_types::i32>;
using size_2d_t   = typing::geometry::size_2d<libc_types::i32>;
using PixFmt      = typing::pixels::PixFmt;

template<class ExposedType, class... OtherServices>
struct AppServiceTraits
{
    using type     = ExposedType;
    using services = typename detail::TypeList<ExposedType, OtherServices...>;
};

struct WindowConfig
{
    text_type_t         title;
    size_2d_t           size;
    position_t          position;
    detail::WindowState flags;
};

template<
    class ExposedType,
    typename ServiceList = detail::TypeList<ExposedType>>
struct AppService : detail::RestrictedSubsystem<
                        AppServiceTraits<ExposedType>,
                        detail::TypeList<void>,
                        detail::TypeList<void>,
                        ServiceList>
{
    using services = ServiceList;

    virtual ~AppService()
    {
    }

    template<typename InternalType, typename... Args>
    static InternalType& register_service(
        detail::EntityContainer& container, Args... args)
    {
        using tag_type = AppServiceTraits<ExposedType>;

        auto& subsys =
            container.register_subsystem_inplace<tag_type, InternalType>(
                std::forward<Args>(args)...);

        container.register_subsystem_services<tag_type>(&subsys);

        return subsys;
    }

    virtual ExposedType& get() final
    {
        return *C_DCAST<ExposedType>(this);
    }

    virtual ExposedType const& get() const final
    {
        return *C_DCAST<ExposedType const>(this);
    }
};

struct AppLoadableService
{
    using entity_container = detail::EntityContainer;

    virtual void load(entity_container&, app_error&)
    {
    }
    virtual void unload(entity_container&, app_error&)
    {
    }
};

struct AppInfo : AppService<AppInfo>
{
    virtual void add(text_type key, text_type value) = 0;
};

struct Windowing : AppService<Windowing>
{
    virtual ~Windowing();

    virtual void show()  = 0;
    virtual void close() = 0;

    virtual size_2d_t  size() const                     = 0;
    virtual void       resize(size_2d_t const& newSize) = 0;
    virtual position_t position() const                 = 0;
    virtual void       move(position_t const& newPos)   = 0;

    virtual void fullscreenBorderless()
    {
        setState(detail::WindowState::WindowedFullScreen);
    }
    virtual void fullscreenExclusive()
    {
        setState(detail::WindowState::FullScreen);
    }

    virtual detail::WindowState state() const                       = 0;
    virtual void                setState(detail::WindowState state) = 0;
};

struct Dialogs : AppService<Dialogs>
{
    using prompt_callback  = Coffee::Function<void(text_type)>;
    using confirm_callback = Coffee::Function<void(bool)>;

    virtual void info(text_type title, text_type message)  = 0;
    virtual void warn(text_type title, text_type message)  = 0;
    virtual void error(text_type title, text_type message) = 0;

    virtual void confirm(
        text_type title, text_type message, confirm_callback&& callback) = 0;
    virtual void prompt(text_type title, prompt_callback&& callback)     = 0;
};

struct DisplayInfo : AppService<DisplayInfo>
{
    virtual size_2d_t       virtualSize() const                     = 0;
    virtual libc_types::u32 count() const                           = 0;
    virtual libc_types::u32 currentDisplay() const                  = 0;
    virtual size_2d_t       size(libc_types::u32 idx) const         = 0;
    virtual size_2d_t       physicalSize(libc_types::u32 idx) const = 0;
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

struct ScreensaverInfo : AppService<ScreensaverInfo>
{
    virtual bool isEnabled() const        = 0;
    virtual void setEnabled(bool enabled) = 0;
};

struct WindowInfo : AppService<WindowInfo>
{
    virtual text_type name() const               = 0;
    virtual void      setName(text_type newName) = 0;
};

struct NativeWindowInfo : AppService<NativeWindowInfo>
{
};

template<typename EventType>
struct EventBus : AppService<EventBus<EventType>>
{
    virtual ~EventBus()
    {
    }

    virtual void inject(EventType& ev, libc_types::c_ptr data)  = 0;
    virtual void process(EventType& ev, libc_types::c_ptr data) = 0;
};

struct MouseInput : AppService<MouseInput>
{
    using MouseButton = Coffee::Input::CIMouseButtonEvent::MouseButton;

    virtual bool mouseGrabbed() const       = 0;
    virtual void setMouseGrab(bool enabled) = 0;

    virtual position_t  position() const               = 0;
    virtual void        warp(position_t const& newPos) = 0;
    virtual MouseButton buttons() const                = 0;
};

struct KeyboardInput : AppService<KeyboardInput>
{
    using KeyModifiers = Coffee::Input::CIKeyEvent::KeyModifiers;

    virtual void openVirtual() const
    {
    }

    virtual KeyModifiers key(libc_types::u32) const
    {
        return KeyModifiers::NoneModifier;
    }
};

struct BasicKeyboardInput : KeyboardInput
{
    using register_type = stl_types::Map<libc_types::u32, KeyModifiers>;

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

    virtual KeyModifiers key(libc_types::u32 key) const
    {
        auto it = m_register.find(key);
        return it == m_register.end() ? KeyModifiers::NoneModifier : it->second;
    }

  private:
    stl_types::Map<libc_types::u32, KeyModifiers> m_register;
};

struct ControllerInput : AppService<ControllerInput>
{
    using controller_map = Coffee::Input::CIControllerState;

    virtual libc_types::u32 count() const              = 0;
    virtual controller_map  state(libc_types::u32 idx) = 0;
};

struct TouchInput : AppService<TouchInput>
{
    virtual libc_types::u32 points() const = 0;

    virtual position_t position(libc_types::u32 idx) const      = 0;
    virtual position_t startPosition(libc_types::u32 idx) const = 0;
};

struct GraphicsBinding : AppService<GraphicsBinding>
{
};

struct GraphicsContext : AppService<GraphicsContext>
{
    virtual void swapBuffers(app_error& ec) = 0;
};

struct GraphicsFramebuffer : AppService<GraphicsFramebuffer>
{
    virtual libc_types::u32 buffers() const       = 0;
    virtual size_2d_t       size() const          = 0;
    virtual PixFmt          format() const        = 0;
    virtual PixFmt          depthFormat() const   = 0;
    virtual PixFmt          stencilFormat() const = 0;
};

struct GraphicsThreadInfo : AppService<GraphicsThreadInfo>
{
    virtual stl_types::ThreadId::Hash thread() const             = 0;
    virtual void                      makeCurrent(app_error& ec) = 0;
};

struct GraphicsSwap : AppService<GraphicsSwap>
{
    virtual libc_types::i32 swapInterval() const                      = 0;
    virtual void            setSwapInterval(libc_types::i32 interval) = 0;
};

} // namespace comp_app

#include "services.inl"
