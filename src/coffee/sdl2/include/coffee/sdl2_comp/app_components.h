#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/core/types/display/event.h>

struct SDL_Window;
struct SDL_Surface;

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
    virtual ~Windowing();

    virtual void load(entity_container&, comp_app::app_error& ec) final;
    virtual void unload(entity_container& c, comp_app::app_error& ec) final;

    virtual void show() final;
    virtual void close() final;

    virtual size_2d_t  size() const final;
    virtual void       resize(const size_2d_t& newSize) final;
    virtual position_t position() const final;
    virtual void       move(const position_t& newPos) final;

    virtual comp_app::detail::WindowState state() const final;
    virtual void setState(comp_app::detail::WindowState state) final;

    SDL_Window*  m_window;
    SDL_Surface* m_surface;
};

struct DisplayInfo : comp_app::DisplayInfo
{
    virtual comp_app::size_2d_t virtualSize() const final;
    virtual libc_types::u32 count() const final;
    virtual libc_types::u32 currentDisplay() const final;
    virtual comp_app::size_2d_t size(libc_types::u32 idx) const final;
    virtual comp_app::size_2d_t physicalSize(libc_types::u32 idx) const final;
};

} // namespace sdl2
