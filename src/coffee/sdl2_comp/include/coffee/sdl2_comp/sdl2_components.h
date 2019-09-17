#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/core/types/display/event.h>

struct SDL_Window;
struct SDL_Surface;
using SDL_GLContext = void*;

namespace sdl2 {

using comp_app::position_t;
using comp_app::size_2d_t;

struct Context : comp_app::AppService<
                     Context,
                     comp_app::detail::TypeList<
                         comp_app::EventBus<Coffee::Input::CIEvent>,
                         comp_app::EventBus<Coffee::Display::Event>>>,
                 comp_app::AppLoadableService
{
    using type = Context;

    virtual void load(entity_container& c, comp_app::app_error&) final;
    virtual void unload(entity_container& c, comp_app::app_error&) final;
    virtual void end_restricted(Proxy& p, time_point const&) final;

    bool m_shouldClose = false;
};

struct Windowing : comp_app::Windowing, comp_app::AppLoadableService
{
    using type = Windowing;

    virtual ~Windowing();

    virtual void load(entity_container& c, comp_app::app_error& ec) final;
    virtual void unload(entity_container& c, comp_app::app_error& ec) final;

    virtual void start_restricted(Proxy& p, time_point const&) final;

    virtual void show() final;
    virtual void close() final;

    virtual size_2d_t  size() const final;
    virtual void       resize(const size_2d_t& newSize) final;
    virtual position_t position() const final;
    virtual void       move(const position_t& newPos) final;

    virtual comp_app::detail::WindowState state() const final;
    virtual void setState(comp_app::detail::WindowState state) final;

    virtual bool notifiedClose() const final;

    SDL_Window* m_window = nullptr;
    entity_container* m_container = nullptr;
};

struct WindowInfo : comp_app::WindowInfo, comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error&) final;

    virtual comp_app::text_type_t name() const final;
    virtual void setName(comp_app::text_type newName) final;

    entity_container* m_container = nullptr;
};

struct DisplayInfo : comp_app::DisplayInfo
{
    virtual comp_app::size_2d_t virtualSize() const final;
    virtual libc_types::u32     count() const final;
    virtual libc_types::u32     currentDisplay() const final;
    virtual comp_app::size_2d_t size(libc_types::u32 idx) const final;
    virtual comp_app::size_2d_t physicalSize(libc_types::u32 idx) const final;
};

struct GLContext : comp_app::GraphicsContext, comp_app::AppLoadableService
{
    using type = GLContext;

    void setupAttributes(entity_container& c);

    virtual void load(entity_container& c, comp_app::app_error& ec) final;
    virtual void unload(entity_container& c, comp_app::app_error& ec) final;

    SDL_GLContext     m_context   = nullptr;
    entity_container* m_container = nullptr;
};

struct GLSwapControl : comp_app::GraphicsSwapControl
{
    virtual libc_types::i32 swapInterval() const final;
    virtual void            setSwapInterval(libc_types::i32 interval) final;
};

struct GLFramebuffer : comp_app::GraphicsFramebuffer,
                       comp_app::AppLoadableService
{
    using type = GLFramebuffer;

    virtual void load(entity_container& c, comp_app::app_error&) final;

    virtual void                swapBuffers(comp_app::app_error& ec) final;
    virtual comp_app::size_2d_t size() const final;

    entity_container* m_container;
};

struct ControllerInput : comp_app::ControllerInput, comp_app::AppLoadableService
{
    virtual void load(entity_container&, comp_app::app_error& ec) final;
    virtual void unload(entity_container&, comp_app::app_error& ec) final;
    virtual void start_restricted(Proxy& p, time_point const&) final;

    virtual libc_types::u32       count() const final;
    virtual controller_map        state(libc_types::u32 idx) const final;
    virtual comp_app::text_type_t name(libc_types::u32 idx) const final;

    libc_types::i16 rescale(libc_types::i16 value) const;
    int             controllerDisconnect(int device);

    libc_types::scalar m_axisScale;
    libc_types::i16    m_axisDeadzone;

    stl_types::Map<int, void*> m_controllers;
    stl_types::Map<int, void*> m_playerIndex;
};

struct KeyboardInput : comp_app::BasicKeyboardInput
{
    virtual void start_restricted(Proxy& p, time_point const&) final;

    virtual void openVirtual() const final;
};

struct MouseInput : comp_app::MouseInput, comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error&) override;
    virtual void start_restricted(Proxy&, time_point const&) final;

    virtual bool                 mouseGrabbed() const final;
    virtual void                 setMouseGrab(bool enabled) final;
    virtual comp_app::position_t position() const final;
    virtual void                 warp(const comp_app::position_t& newPos) final;
    virtual MouseButton          buttons() const final;

    MouseButton       m_buttons = MouseButton::NoneBtn;
    entity_container* m_container;
};

using Services = comp_app::detail::TypeList<
    Context,
    Windowing,
    WindowInfo,
    DisplayInfo,
    comp_app::PtrNativeWindowInfo,
    ControllerInput,
    KeyboardInput,
    MouseInput>;

using GLServices =
    comp_app::detail::TypeList<GLSwapControl, GLContext, GLFramebuffer>;

} // namespace sdl2
