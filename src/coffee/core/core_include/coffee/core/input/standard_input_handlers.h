#pragma once

#include <coffee/core/types/input/event_types.h>
#include <glm/geometric.hpp>
#include <peripherals/stl/time_types.h>
#include <peripherals/typing/enum/graphics/direction.h>

#include <functional>
#include <map>
#include <memory>

namespace Coffee::StandardInput {

using namespace Input;
using typing::vector_types::Quatf;

template<typename InputRegister, const CIEvent::EventType EventType>
/*!
 * \brief By default, event handlers only receive state changes. This function
 * captures the state of keyboard and mouse clicks into a register which allows
 * random access to keys. \param keyRegister A std::map-like structure with
 * operator[] \param e \param data \return true when an event was found
 */
bool StandardKeyRegister(
    InputRegister& keyRegister, const CIEvent& e, c_cptr data)
{
    static_assert(
        EventType == CIEvent::Keyboard || EventType == CIEvent::MouseButton,
        "Invalid template argument for function");

    if(e.type != EventType)
        return false;

    switch(e.type)
    {
    case CIEvent::Keyboard: {
        auto const& ev = *C_RCAST<CIKeyEvent const*>(data);

        if(ev.mod & CIKeyEvent::RepeatedModifier)
            break;

        bool pressed = ev.mod & CIKeyEvent::PressedModifier;

        auto& v = keyRegister[ev.key];
        if(pressed)
            v |= 0x1;
        else
            v &= ~0x1;
        break;
    }
    case CIEvent::MouseButton: {
        auto const& ev = *C_RCAST<CIMouseButtonEvent const*>(data);

        bool pressed = ev.mod & CIMouseButtonEvent::Pressed;

        auto& v = keyRegister[ev.btn];
        if(pressed)
            v |= 0x1;
        else
            v &= ~0x1;

        break;
    }
    default:
        return false;
    }

    return true;
}

struct StandardCameraOpts
{
    struct
    {
        f32 base{5.f};
        f32 alt{10.f};
        f32 fast{100.f};
    } accel;

    Vecf3     up{};
    glm::mat3 world_basis{1.f}; /* transforms camera-space column vectors to
                                   world/position space */
};

/*!
 * \brief Camera movement dependent on a fixed update cycle
 * \param c
 * \param reg
 * \return true when Camera reference is updated
 */
struct StandardCamera
{
    using Reg = std::map<u16, u16>;

    struct KeyboardInput
    {
        using event_type = CIKeyEvent;

        KeyboardInput(std::function<Reg*()>&& reg)
            : m_reg(std::move(reg))
        {
        }

        void operator()(CIEvent const& e, CIKeyEvent const* ev)
        {
            auto* reg = m_reg();
            if(!reg)
                return;
            StandardKeyRegister<Reg, CIEvent::Keyboard>(*reg, e, ev);
        }

        std::function<Reg*()> m_reg;
    };

    struct MouseInput
    {
        using event_type = CIMouseMoveEvent;

        MouseInput(
            std::function<Vecf2*()>&& look,
            u32                       button = CIMouseButtonEvent::LeftButton)
            : m_button(button)
            , m_look(std::move(look))
        {
        }

        void operator()(CIEvent const&, CIMouseMoveEvent const* ev)
        {
            if(ev->btn != m_button)
                return;
            auto* look = m_look();
            if(!look)
                return;
            look->x += -0.01f * ev->delta.x;
            look->y += -0.01f * ev->delta.y;
        }

        u32                      m_button;
        std::function<Vecf2*()>  m_look;
    };

    static inline bool has_key(Reg const& reg, u16 key)
    {
        auto it = reg.find(key);

        return it != reg.end() && (it->second & 0x1);
    }

    template<typename Camera>
    inline void move(Camera& camera, f32 forward, f32 right, f32 up, f32 accel = 5.f)
    {
        camera.position += forward * cached.forward * movement_speed * accel;
        camera.position += right * cached.right * movement_speed * accel;
        camera.position += up * cached.up * movement_speed * accel;
    }

    template<typename Camera>
    inline void rotate(Camera& camera, f32 pitch, f32 yaw)
    {
        auto& q = camera.rotation;
        /* Pitch (look up/down): pre-multiply around view-space +X = {1,0,0}.
         * cached.right is a BSP-space vector and is NOT the same 3D vector as
         * view +X, so using it only accidentally produces correct pitch at the
         * spawn facing angle. Pre-multiplying by angleAxis({1,0,0}) always
         * rotates around screen-right. */
        q = glm::normalize(glm::angleAxis(yaw, Vecf3{-1.f, 0.f, 0.f}) * q);
        /* Yaw (turn left/right): post-multiply around the fixed GL Y axis.
         * Using cached.up (BSP Z) here would roll the camera instead of turning
         * it, because R_vertex lives in GL-intermediate space. The GL Y axis
         * maps to BSP Z (world up) via bsp_basis, so this produces correct
         * horizontal turning without S-curves. */
        q = glm::normalize(q * glm::angleAxis(-pitch, Vecf3{0.f, 1.f, 0.f}));
    }

    /*!
     * \brief Recompute the movement basis from the camera's orientation.
     */
    template<typename Camera>
    void refresh_basis(Camera const& camera, StandardCameraOpts const& opts)
    {
        auto& rotation = cached.rotation = glm::mat3_cast(camera.rotation);

        glm::mat3 Rt   = glm::transpose(rotation);
        cached.right   = opts.world_basis * Vecf3(Rt[0]);
        cached.up      = opts.world_basis * Vecf3(Rt[1]);
        cached.forward = opts.world_basis * -Vecf3(Rt[2]);
    }

    /*!
     * \brief Held keys as camera-relative axis scalars, plus the speed
     * modifier they select.
     */
    static void sample_keys(
        Reg const&                                 reg,
        StandardCameraOpts const&                  opts,
        Vecf3&                                     movement,
        f32&                                       accel,
        std::chrono::system_clock::duration const& t)
    {
        using namespace stl_types::chrono;
        f32 acceleration = opts.accel.base;

        if(has_key(reg, CK_LShift))
            acceleration = opts.accel.alt;
        if(has_key(reg, CK_LCtrl))
            acceleration = opts.accel.fast;

        accel = std::max(accel, acceleration * stl_types::chrono::to_f32(t));

        for(auto const& [key, mod] : reg)
        {
            if((mod & 0x1) == 0)
                continue;
            switch(key)
            {
            case CK_w: movement.x += to_f32(t) * 10.f; break;
            case CK_s: movement.x -= to_f32(t) * 10.f; break;
            case CK_a: movement.y -= to_f32(t) * 10.f; break;
            case CK_d: movement.y += to_f32(t) * 10.f; break;
            case CK_q: movement.z += to_f32(t) * 10.f; break;
            case CK_e: movement.z -= to_f32(t) * 10.f; break;
            }
        }
    }

    f32 movement_speed{0.4f};

    struct
    {
        glm::mat3 rotation;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
    } cached;
};

struct ControllerOpts
{
    ControllerOpts()
    {
        deadzone    = 6000;
        curve       = 600.f;
        sens.move.x = sens.move.y = .1f;
        sens.look.x = sens.look.y = 2.f;

        invertYLook();
    }

    struct
    {
        struct
        {
            f32 x, y;
        } move;

        struct
        {
            f32 x, y;
        } look;
    } sens;

    i16 deadzone;
    f32 curve;

    FORCEDINLINE bool isYLookInverted() const
    {
        return sens.look.y < 0;
    }

    FORCEDINLINE void invertYLook()
    {
        sens.look.y *= -1.f;
    }
};

/*!
 * \brief Sample controller sticks into per-frame input state.
 */
inline void controller_sample_input(
    Vecf2&                                     look,
    Vecf3&                                     movement,
    f32&                                       accel,
    ControllerOpts const&                      opt,
    CIControllerState const&                   state,
    std::chrono::system_clock::duration const& t)
{
    using stl_types::chrono::to_f32;

    const auto filter = [](i16 raw, f32 sens) {
        return convert_i16_f(raw) * sens;
    };

    const auto acceleration =
        1.f + convert_i16_f(state.axes.e.t_l) * opt.curve * to_f32(t);

    movement.x += filter(state.axes.e.l_y, opt.sens.move.y) * -1.f;
    movement.y += filter(state.axes.e.l_x, opt.sens.move.x);
    accel = std::max(accel, acceleration);

    look.x += filter(state.axes.e.r_x, opt.sens.look.x) * to_f32(t) * -1.f;
    look.y += filter(state.axes.e.r_y, opt.sens.look.y) * to_f32(t) * -1.f;
}

template<typename Camera>
void controller_camera_update(
    StandardCamera&                            camera_,
    Camera&                                    camera,
    ControllerOpts const&                      opt,
    CIControllerState const&                   state,
    std::chrono::system_clock::duration const& t)
{
    using stl_types::chrono::to_f32;

    const auto filter = [](i16 raw, f32 sens) {
        return convert_i16_f(raw) * sens;
    };

    const auto acceleration =
        1.f + convert_i16_f(state.axes.e.t_l) * opt.curve * to_f32(t);
    camera_.move(
        camera,
        filter(state.axes.e.l_y, opt.sens.move.y) * -1.f,
        filter(state.axes.e.l_x, opt.sens.move.x) * 1.f,
        0.f,
        acceleration);
    camera_.rotate(
        camera,
        filter(state.axes.e.r_x, opt.sens.look.x) * to_f32(t) * -1.f,
        filter(state.axes.e.r_y, opt.sens.look.y) * to_f32(t) * -1.f);
}

template<typename CameraPtr, typename OptsPtr>
struct ControllerCamera
{
    ControllerCamera(CameraPtr cam, OptsPtr options)
        : m_camera(cam)
        , m_opts(options)
    {
    }

    void operator()(
        CIControllerState const&                   state,
        std::chrono::system_clock::duration const& t)
    {
        return controller_camera_update(m_camera, opts(), state, t);
    }

    ControllerOpts& opts()
    {
        return *m_opts;
    }

  private:
    FORCEDINLINE f32 filter(i16 raw, f32 sens)
    {
        return convert_i16_f(raw) * sens;
    }

    CameraPtr m_camera;
    OptsPtr   m_opts;
};

} // namespace Coffee::StandardInput
