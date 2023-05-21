#pragma once

#include <coffee/comp_app/app_events.h>
#include <coffee/comp_app/services.h>

struct android_app;
struct AInputEvent;

namespace ndk_helper {

class TapDetector;
class DoubletapDetector;
class PinchDetector;
class DragDetector;

} // namespace ndk_helper

namespace anative {

struct Windowing : comp_app::interfaces::StaticWindowing,
                   comp_app::AppService<Windowing, comp_app::Windowing>,
                   comp_app::AppLoadableService
{
    Windowing()
    {
        priority = 511;
    }

    virtual void load(entity_container& e, comp_app::app_error& ec) override;

    virtual comp_app::size_2d_t           size() const override;
    virtual comp_app::window_flags_t state() const override;
    virtual void setState(comp_app::window_flags_t /*state*/) override;

    void close() final;
};

struct ControllerInput
    : comp_app::interfaces::ControllerInput,
      comp_app::AppService<ControllerInput, comp_app::ControllerInput>
{
    using type = ControllerInput;

    ControllerInput() : m_cache(1)
    {
    }

    std::vector<controller_map>                m_cache;
    std::map<libc_types::i32, libc_types::u32> m_mapping;

    virtual libc_types::u32       count() const override;
    virtual controller_map        state(libc_types::u32 idx) const override;
    virtual comp_app::text_type_t name(libc_types::u32 idx) const override;
};

struct KeyboardInput
    : comp_app::interfaces::BasicKeyboardInput,
      comp_app::AppService<KeyboardInput, comp_app::KeyboardInput>
{
    void openVirtual() const;
    void closeVirtual() const;
};

struct MouseInput : comp_app::interfaces::MouseInput,
                    comp_app::AppService<MouseInput, comp_app::MouseInput>
{
    bool mouseGrabbed() const;
    void setMouseGrab(bool enabled);
    comp_app::position_t position() const;
    void warp(const comp_app::position_t &newPos);
    MouseButton buttons() const;

    comp_app::position_t m_position{};
    MouseButton m_buttons{MouseButton::NoneBtn};
};

struct AndroidEventBus : comp_app::AppService<AndroidEventBus>,
                         comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error&) override;

    void handleKeyEvent(AInputEvent* event);
    void handleMotionEvent(AInputEvent* event);

    void handleMouseEvent(AInputEvent* event);
    bool handleGamepadEvent(AInputEvent* event);
    void handleInputEvent(AInputEvent* event);
    bool filterTouchEvent(AInputEvent* event);
    void handleWindowEvent(android_app* app, libc_types::i32 event);

    void emitLifecycleEvent(comp_app::LifecycleEvent event);

    entity_container* m_container;

    ndk_helper::TapDetector*       m_tapDetector;
    ndk_helper::DoubletapDetector* m_doubleDetector;
    ndk_helper::PinchDetector*     m_pinchDetector;
    ndk_helper::DragDetector*      m_dragDetector;

    comp_app::TouchConfig* m_touchConfig;
    comp_app::BasicEventBus<Coffee::Input::CIEvent>* m_inputBus;
    comp_app::BasicEventBus<comp_app::AppEvent>* m_appBus;

    std::optional<Coffee::Input::CIMouseButtonEvent> m_tapButtonEvent;
};

using Services = comp_app::detail::TypeList<
    comp_app::PtrNativeWindowInfoService,
    Windowing,
    ControllerInput,
    KeyboardInput,
    MouseInput,
    AndroidEventBus>;

} // namespace anative
