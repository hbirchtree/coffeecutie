#pragma once

#include <coffee/comp_app/services.h>

namespace x11 {

using comp_app::position_t;
using comp_app::size_2d_t;
using comp_app::detail::WindowState;

struct X11Config : comp_app::Config<X11Config>
{
    bool hildon = false;
};

struct Windowing
    : comp_app::Windowing
    , comp_app::AppLoadableService
{
    using type = Windowing;

    virtual void load(entity_container& e, comp_app::app_error& ec) final;
    virtual void unload(entity_container&, comp_app::app_error&) final;

    virtual void start_restricted(Proxy&, time_point const&) final;

    virtual void show() final;
    virtual void close() final;

    virtual size_2d_t size() const final;
    virtual void      resize(size_2d_t const& newSize) final;

    virtual position_t position() const final;
    virtual void       move(position_t const& newPos) final;

    virtual WindowState state() const final;
    virtual void        setState(WindowState state) final;

    void* m_display = nullptr;
    void* m_window  = nullptr;

    bool m_shouldClose = false;

    virtual bool notifiedClose() const final;
};

struct WindowInfo
    : comp_app::WindowInfo
    , comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error& ec) final;

    virtual comp_app::text_type_t name() const final;
    virtual void                  setName(comp_app::text_type newName) final;

    entity_container* m_container;
};

using Services = comp_app::detail::
    TypeList<comp_app::PtrNativeWindowInfo, Windowing, WindowInfo>;

} // namespace x11
