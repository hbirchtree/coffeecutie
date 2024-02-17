#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>

namespace emscripten {

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

struct GLContext
    : comp_app::interfaces::GraphicsContext
    , comp_app::AppService<GLContext, comp_app::GraphicsContext>
    , comp_app::AppLoadableService
{
    GLContext()
    {
        priority = 514;
    }

    virtual void load(entity_container& c, comp_app::app_error& ec) final;
    virtual void unload(entity_container& c, comp_app::app_error& ec) final;

    entity_container* m_container = nullptr;
};

struct ControllerInput
    : comp_app::interfaces::ControllerInput
    , comp_app::AppService<ControllerInput, comp_app::ControllerInput>
{
    virtual ~ControllerInput() = default;

    virtual void start_frame(
        comp_app::detail::ContainerProxy& c,
        const comp_app::detail::time_point&) final;

    virtual libc_types::u32       count() const final;
    virtual controller_map        state(libc_types::u32 idx) const final;
    virtual comp_app::text_type_t name(libc_types::u32 idx) const final;

  private:
    struct state_t
    {
        std::string    name;
        controller_map state;
    };

    std::map<libc_types::u32, state_t> m_controllers;

    libc_types::i16 m_deadzone;
    libc_types::f32 m_scale;
};

struct BatteryProvider
    : comp_app::interfaces::BatteryProvider
    , comp_app::AppService<BatteryProvider, comp_app::BatteryProvider>
{
    virtual ~BatteryProvider() = default;

    virtual PowerSource     source() const final;
    virtual libc_types::u16 percentage() const final;
};

using Services = comp_app::subsystem_list<ControllerInput, BatteryProvider>;
using GLServices =
    comp_app::subsystem_list<GLSwapControl, GLFramebuffer, GLContext>;

} // namespace emscripten
