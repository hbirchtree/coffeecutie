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
    virtual void load(entity_container& c, comp_app::app_error&) override;
    virtual void unload(entity_container& c, comp_app::app_error&) override;
    virtual void end_restricted(Proxy& p, time_point const&) override;

    bool m_shouldClose = false;
};

struct Windowing : comp_app::Windowing, comp_app::AppLoadableService
{
    virtual ~Windowing();

    virtual void load(entity_container&, comp_app::app_error& ec) override;
    virtual void unload(entity_container& c, comp_app::app_error& ec) override;

    virtual void show() override;
    virtual void close() override;

    virtual size_2d_t  size() const override;
    virtual void       resize(const size_2d_t& newSize) override;
    virtual position_t position() const override;
    virtual void       move(const position_t& newPos) override;

    virtual comp_app::detail::WindowState state() const override;
    virtual void setState(comp_app::detail::WindowState state) override;

    SDL_Window*  m_window;
    SDL_Surface* m_surface;
};

} // namespace sdl2
