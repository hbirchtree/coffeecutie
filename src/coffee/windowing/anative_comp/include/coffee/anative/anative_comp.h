#pragma once

#include <coffee/comp_app/services.h>

struct android_app;
struct AInputEvent;

namespace anative {

struct Windowing : comp_app::StaticWindowing, comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error& ec) override;

    virtual comp_app::size_2d_t           size() const override;
    virtual comp_app::detail::WindowState state() const override;
    virtual void setState(comp_app::detail::WindowState state) override;
};

struct ControllerInput : comp_app::ControllerInput
{
    using type = ControllerInput;

    ControllerInput() : m_cache(1)
    {
    }

    stl_types::Vector<controller_map> m_cache;
    stl_types::Map<libc_types::i32, libc_types::u32> m_mapping;

    virtual libc_types::u32       count() const override;
    virtual controller_map        state(libc_types::u32 idx) const override;
    virtual comp_app::text_type_t name(libc_types::u32 idx) const override;
};

struct AndroidEventBus : comp_app::AppService<AndroidEventBus>,
                         comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error&) override;

    void handleKeyEvent(AInputEvent* event);
    void handleMotionEvent(AInputEvent* event);

    void handleGamepadEvent(AInputEvent* event);
    void handleInputEvent(AInputEvent* event);
    void handleWindowEvent(android_app* app, libc_types::i32 event);

    entity_container* m_container;
};

using Services = comp_app::detail::TypeList<
    Windowing,
    comp_app::PtrNativeWindowInfo,
    ControllerInput,
    AndroidEventBus>;

} // namespace anative
