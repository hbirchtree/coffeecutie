#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/core/types/display/event.h>

struct SDL_Window;
struct SDL_Surface;
using SDL_GLContext = void*;

namespace sdl2 {

using comp_app::position_t;
using comp_app::size_2d_t;

struct Context
    : comp_app::AppService<Context>
    , comp_app::AppLoadableService
{
    using readable_services = comp_app::detail::subsystem_list<
        comp_app::BasicEventBus<Coffee::Input::CIEvent>,
        comp_app::BasicEventBus<Coffee::Display::Event>>;
    using proxy_type = comp_app::detail::restricted::proxy_t<Context>;

    Context()
    {
        priority = 512;
    }

    virtual void load(entity_container& c, comp_app::app_error&) final;
    virtual void unload(entity_container& c, comp_app::app_error&) final;
    void         end_restricted(proxy_type& p, time_point const&);

    bool m_shouldClose = false;
};

struct Windowing
    : comp_app::interfaces::Windowing
    , comp_app::AppService<Windowing, comp_app::Windowing>
    , comp_app::AppLoadableService
{
    using readable_services = comp_app::detail::subsystem_list<
        comp_app::EventBus<Coffee::Display::Event>>;
    using proxy_type = comp_app::detail::restricted::proxy_t<Windowing>;

    Windowing()
    {
        priority = 513;
    }

    virtual ~Windowing();

    virtual void load(entity_container& c, comp_app::app_error& ec) final;
    virtual void unload(entity_container& c, comp_app::app_error& ec) final;

    void start_restricted(proxy_type& p, time_point const&);

    virtual void show() final;
    virtual void close() final;

    virtual size_2d_t  size() const final;
    virtual void       resize(const size_2d_t& newSize) final;
    virtual position_t position() const final;
    virtual void       move(const position_t& newPos) final;

    virtual comp_app::window_flags_t state() const final;
    virtual void setState(comp_app::window_flags_t state) final;

    virtual bool notifiedClose() const final;

    SDL_Window*       m_window    = nullptr;
    entity_container* m_container = nullptr;
};

struct NativeWindowInfo
    : comp_app::interfaces::PtrNativeWindowInfo
    , comp_app::AppService<NativeWindowInfo, comp_app::PtrNativeWindowInfo>
{
    NativeWindowInfo()
    {
        priority = 511;
    }
};

struct WindowInfo
    : comp_app::interfaces::WindowInfo
    , comp_app::AppService<WindowInfo, comp_app::WindowInfo>
    , comp_app::AppLoadableService
{
    WindowInfo()
    {
        priority = 514;
    }

    virtual void load(entity_container& e, comp_app::app_error&) final;

    virtual comp_app::text_type_t name() const final;
    virtual void                  setName(comp_app::text_type newName) final;

    entity_container* m_container = nullptr;
};

struct DisplayInfo
    : comp_app::interfaces::DisplayInfo
    , comp_app::AppService<DisplayInfo, comp_app::DisplayInfo>
{
    DisplayInfo()
    {
        priority = 514;
    }

    virtual comp_app::size_2d_t virtualSize() const final;
    virtual libc_types::u32     count() const final;
    virtual libc_types::u32     currentDisplay() const final;
    virtual comp_app::size_2d_t size(libc_types::u32 idx) const final;
    virtual comp_app::size_2d_t physicalSize(libc_types::u32 i) const final;
    virtual libc_types::f32     dpi(libc_types::u32 i) const final;
    virtual libc_types::f32     diagonal(libc_types::u32 i) const final;
};

struct GLContext
    : comp_app::interfaces::GraphicsContext
    , comp_app::AppService<GLContext, comp_app::GraphicsContext>
    , comp_app::AppLoadableService
{
    GLContext()
    {
        priority = 514;
    }

    void setupAttributes(entity_container& c);

    virtual void load(entity_container& c, comp_app::app_error& ec) final;
    virtual void unload(entity_container& c, comp_app::app_error& ec) final;

    SDL_GLContext     m_context   = nullptr;
    entity_container* m_container = nullptr;
};

struct GLSwapControl
    : comp_app::interfaces::GraphicsSwapControl
    , comp_app::AppService<GLSwapControl, comp_app::GraphicsSwapControl>
{
    GLSwapControl()
    {
        priority = 514;
    }

    virtual libc_types::i32 swapInterval() const final;
    virtual void            setSwapInterval(libc_types::i32 interval) final;
};

struct GLFramebuffer
    : comp_app::interfaces::GraphicsFramebuffer
    , comp_app::AppService<GLFramebuffer, comp_app::GraphicsFramebuffer>
    , comp_app::AppLoadableService
{
    GLFramebuffer()
    {
        priority = 128;
    }

    virtual void load(entity_container& c, comp_app::app_error&) final;

    virtual void                swapBuffers(comp_app::app_error& ec) final;
    virtual comp_app::size_2d_t size() const final;

    virtual void start_frame(
        comp_app::detail::ContainerProxy&, time_point const&) final;

    entity_container* m_container;
};

struct ControllerInput
    : comp_app::interfaces::ControllerInput
    , comp_app::AppService<ControllerInput, comp_app::ControllerInput>
    , comp_app::AppLoadableService
{
    using readable_services =
        comp_app::subsystem_list<comp_app::EventBus<Coffee::Input::CIEvent>>;
    using proxy_type = comp_app::detail::restricted::proxy_t<ControllerInput>;

    ControllerInput()
    {
        priority = 514;
    }

    virtual void load(entity_container&, comp_app::app_error& ec) final;
    virtual void unload(entity_container&, comp_app::app_error& ec) final;
    void         start_restricted(proxy_type& p, time_point const&);

    virtual libc_types::u32       count() const final;
    virtual controller_map        state(libc_types::u32 idx) const final;
    virtual comp_app::text_type_t name(libc_types::u32 idx) const final;

    libc_types::i16 rescale(libc_types::i16 value) const;
    int             controllerDisconnect(int device);

    libc_types::f32 m_axisScale;
    libc_types::i16 m_axisDeadzone;

    std::map<int, void*> m_controllers;
    std::map<int, void*> m_playerIndex;
};

struct KeyboardInput
    : comp_app::interfaces::BasicKeyboardInput
    , comp_app::AppService<KeyboardInput, comp_app::KeyboardInput>
{
    using readable_services =
        comp_app::subsystem_list<comp_app::EventBus<Coffee::Input::CIEvent>>;
    using proxy_type = comp_app::detail::restricted::proxy_t<KeyboardInput>;

    KeyboardInput()
    {
        priority = 514;
    }

    void start_restricted(proxy_type& p, time_point const&);

    virtual void openVirtual() const final;
    virtual void closeVirtual() const final;
};

struct MouseInput
    : comp_app::interfaces::MouseInput
    , comp_app::AppService<MouseInput, comp_app::MouseInput>
    , comp_app::AppLoadableService
{
    using readable_services =
        comp_app::subsystem_list<comp_app::EventBus<Coffee::Input::CIEvent>>;
    using proxy_type = comp_app::detail::restricted::proxy_t<MouseInput>;

    MouseInput()
    {
        priority = 514;
    }

    virtual void load(entity_container& e, comp_app::app_error&) override;
    void         start_restricted(proxy_type&, time_point const&);

    virtual bool                 mouseGrabbed() const final;
    virtual void                 setMouseGrab(bool enabled) final;
    virtual comp_app::position_t position() const final;
    virtual void                 warp(const comp_app::position_t& newPos) final;
    virtual MouseButton          buttons() const final;

    MouseButton       m_buttons = MouseButton::NoneBtn;
    entity_container* m_container;
};

using Services = comp_app::subsystem_list<
    Context,
    Windowing,
    WindowInfo,
    DisplayInfo,
    NativeWindowInfo,
    KeyboardInput,
    MouseInput>;

using GLServices =
    comp_app::subsystem_list<GLSwapControl, GLContext, GLFramebuffer>;

} // namespace sdl2
