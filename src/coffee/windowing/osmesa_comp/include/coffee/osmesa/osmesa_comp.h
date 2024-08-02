#pragma once

#include <EGL/egl.h>
#include <GL/osmesa.h>
#include <coffee/comp_app/services.h>

namespace osmesa {

struct GraphicsContext
    : comp_app::interfaces::GraphicsContext
    , comp_app::AppService<GraphicsContext>
{
    using type = GraphicsContext;
};

struct GraphicsFramebuffer
    : comp_app::interfaces::GraphicsFramebuffer
    , comp_app::AppService<GraphicsFramebuffer, comp_app::GraphicsFramebuffer>
{
    using type = GraphicsFramebuffer;
    
    void start_frame(compo::ContainerProxy&, compo::time_point const&);

    virtual comp_app::size_2d_t size() const final { return comp_app::size_2d_t{1280, 720}; }
    virtual void                swapBuffers(comp_app::app_error&) final;
};

struct GraphicsSwapControl
    : comp_app::interfaces::GraphicsSwapControl
    , comp_app::AppService<GraphicsSwapControl>
{
    virtual libc_types::i32 swapInterval() const final { return 0; }
    virtual void            setSwapInterval(libc_types::i32) final {}
};

struct Windowing
    : comp_app::interfaces::StaticWindowing
    , comp_app::AppService<Windowing, comp_app::Windowing>
    , comp_app::AppLoadableService
{
  public:
    void load(entity_container& e, comp_app::app_error& ec);

    void end_frame(compo::ContainerProxy&, compo::time_point const&);

    comp_app::size_2d_t      size() const final;
    comp_app::window_flags_t state() const final;
    void                     setState(comp_app::window_flags_t state) final;

  private:
    void dump_framebuffer();

    OSMesaContext m_context{};
    std::vector<typing::pixels::rgba_t> m_framebuffer{};
    libc_types::u64 m_targetFrames{0};
};

using Services = comp_app::detail::TypeList<
    comp_app::PtrNativeWindowInfoService,
    GraphicsContext,
    GraphicsFramebuffer,
    GraphicsSwapControl>;

} // namespace osmesa
