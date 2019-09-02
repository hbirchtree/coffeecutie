#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <coffee/components/types.h>
#include <coffee/core/types/display/properties.h>
#include <coffee/core/types/input/event_types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/thread_types.h>
#include <peripherals/typing/geometry/point.h>
#include <peripherals/typing/geometry/size.h>

#include "app_error.h"

namespace comp_app {
namespace detail {
using namespace Coffee::Components;

using WindowState = Coffee::Display::Properties::State;
} // namespace detail

template<class ExposedType, class... OtherServices>
struct AppServiceTraits
{
    using type     = ExposedType;
    using services = typename detail::TypeList<ExposedType, OtherServices...>;
};

template<class ExposedType>
struct AppService : detail::RestrictedSubsystem<AppServiceTraits<ExposedType>>
{
    virtual ~AppService()
    {
    }

    template<typename InternalType, typename... Args>
    static void register_service(
        detail::EntityContainer& container, Args... args)
    {
        using tag_type = AppServiceTraits<ExposedType, InternalType>;

        auto& subsys =
            container.register_subsystem_inplace<tag_type, InternalType>(
                std::forward<Args>(args)...);

        container.register_subsystem_services<tag_type>(&subsys);
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

struct Windowing : AppService<Windowing>
{
    using position_t = typing::geometry::point_2d<libc_types::i32>;
    using size_2d_t  = typing::geometry::size_2d<libc_types::i32>;

    virtual ~Windowing();

    virtual void show()  = 0;
    virtual void close() = 0;

    virtual size_2d_t  size() const                     = 0;
    virtual void       resize(size_2d_t const& newSize) = 0;
    virtual position_t position() const                 = 0;
    virtual void       move(position_t const& newPos)   = 0;

    virtual detail::WindowState setState()                          = 0;
    virtual void                setState(detail::WindowState state) = 0;
};

struct Dialogs : AppService<Dialogs>
{
    using text_type = Coffee::CString const&;

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
    using position_t = typing::geometry::point_2d<libc_types::i32>;
    using size_2d_t  = typing::geometry::size_2d<libc_types::i32>;

    virtual libc_types::u32 monitorCount() const = 0;

    virtual size_2d_t size(libc_types::u32 idx) = 0;
};

struct ScreensaverInfo : AppService<ScreensaverInfo>
{
    virtual bool isEnabled() const = 0;
    virtual void disable()         = 0;
    virtual void enable()          = 0;
};

struct WindowInfo : AppService<WindowInfo>
{
    virtual Coffee::CString name() = 0;
};

template<typename EventType>
struct EventBus : AppService<EventBus<EventType>>
{
    virtual void inject(EventType ev, libc_types::c_ptr data)  = 0;
    virtual void process(EventType ev, libc_types::c_ptr data) = 0;
};

struct MouseInput : AppService<MouseInput>
{
    using position_t = typing::geometry::point_2d<libc_types::i32>;

    virtual bool mouseGrabbed() const       = 0;
    virtual void setMouseGrab(bool enabled) = 0;

    virtual position_t position();
    virtual void       warp(position_t const& newPos);
};

struct KeyboardInput : AppService<KeyboardInput>
{
    virtual void openVirtual() const
    {
    }
};

struct ControllerInput : AppService<ControllerInput>
{
    using controller_map = Coffee::Input::CIControllerState;

    virtual libc_types::u32 controllerCount() const              = 0;
    virtual controller_map  controllerState(libc_types::u32 idx) = 0;
};

struct InputCache : AppService<InputCache>
{
};

struct GraphicsBinding : AppService<GraphicsBinding>
{
    virtual void load(app_error& ec) = 0;
};

struct GraphicsContext : AppService<GraphicsContext>
{
    virtual void swapBuffers() = 0;
};

struct GraphicsThreadInfo : AppService<GraphicsThreadInfo>
{
    virtual stl_types::ThreadId::Hash thread() = 0;
};

struct GraphicsSwap : AppService<GraphicsSwap>
{
    virtual libc_types::i32 swapInterval() const                      = 0;
    virtual void            setSwapInterval(libc_types::i32 interval) = 0;
};

} // namespace comp_app

#include "services.inl"
