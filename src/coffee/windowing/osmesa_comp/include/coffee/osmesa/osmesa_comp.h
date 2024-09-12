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
    , comp_app::AppLoadableService
{
    using type = GraphicsFramebuffer;

    GraphicsFramebuffer()
    {
        this->priority = 514;
    }

    void load(entity_container& e, comp_app::app_error&);

    void end_frame(compo::ContainerProxy&, compo::time_point const&);

    virtual comp_app::size_2d_t size() const final
    {
        return m_size;
    }

    virtual void swapBuffers(comp_app::app_error&) final;

  private:
    comp_app::size_2d_t m_size{};
};

struct GraphicsSwapControl
    : comp_app::interfaces::GraphicsSwapControl
    , comp_app::AppService<GraphicsSwapControl>
{
    virtual libc_types::i32 swapInterval() const final
    {
        return 0;
    }

    virtual void setSwapInterval(libc_types::i32) final
    {
    }
};

struct Windowing
    : comp_app::interfaces::StaticWindowing
    , comp_app::AppService<Windowing, comp_app::Windowing>
    , comp_app::AppLoadableService
{
  public:
    Windowing()
    {
        this->priority = 515;
    }

    void load(entity_container& e, comp_app::app_error& ec);

    comp_app::size_2d_t      size() const final;
    comp_app::window_flags_t state() const final;
    void                     setState(comp_app::window_flags_t state) final;

  private:
    void dump_framebuffer();

    OSMesaContext                       m_context{};
    std::vector<typing::pixels::rgba_t> m_framebuffer{};
    comp_app::size_2d_t                 m_size{};
    libc_types::u64                     m_targetFrames{0};
};

using Services = comp_app::detail::TypeList<
    comp_app::PtrNativeWindowInfoService,
    GraphicsContext,
    GraphicsFramebuffer,
    GraphicsSwapControl>;

} // namespace osmesa
