#pragma once

#include <coffee/comp_app/services.h>

using EGLContext = void*;
using EGLSurface = void*;
using EGLConfig  = void*;

namespace egl {
namespace detail {

struct EGLData;

struct EGLDataDeleter
{
    void operator()(EGLData* data);
};

} // namespace detail

struct DisplayHandle : comp_app::AppService<DisplayHandle>,
                       comp_app::AppLoadableService
{
    virtual void load(entity_container&, comp_app::app_error& ec) final;
    virtual void unload(entity_container&, comp_app::app_error& ec) final;

    detail::EGLData& context();

    stl_types::UqPtr<detail::EGLData, detail::EGLDataDeleter> m_data;
};

struct GraphicsContext : comp_app::GraphicsContext, comp_app::AppLoadableService
{
    using type = GraphicsContext;

    virtual void load(entity_container& e, comp_app::app_error& ec) final;
    virtual void unload(entity_container& c, comp_app::app_error&) final;

    EGLContext        m_context;
    EGLConfig         m_config;
    entity_container* m_container;

    struct feature_flags_t
    {
        bool android_composite_deadline = false;
        bool android_present_latency    = false;
    } feature_flags;
};

struct GraphicsFramebuffer : comp_app::GraphicsFramebuffer,
                             comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error& ec) final;
    virtual void unload(entity_container& e, comp_app::app_error& ec) final;

    virtual comp_app::size_2d_t size() const final;
    virtual void                swapBuffers(comp_app::app_error& ec) final;

    EGLSurface        m_surface;
    entity_container* m_container;
};

struct GraphicsSwapControls : comp_app::GraphicsSwapControl,
                              comp_app::AppLoadableService
{
    virtual void load(entity_container& p, comp_app::app_error&) final;

    virtual libc_types::i32 swapInterval() const final;
    virtual void            setSwapInterval(libc_types::i32 interval) final;

    entity_container* m_container;
};

struct Windowing : comp_app::StaticWindowing, comp_app::AppLoadableService
{
    void load(entity_container &e, comp_app::app_error &ec);

    comp_app::size_2d_t size() const final;
    comp_app::detail::WindowState state() const final;
    void setState(comp_app::detail::WindowState) final;

  private:
    entity_container* m_container;
};

using Services = comp_app::detail::TypeList<
    DisplayHandle,
    GraphicsContext,
    GraphicsFramebuffer,
    GraphicsSwapControls,
    comp_app::PtrNativeWindowInfo>;

} // namespace egl
