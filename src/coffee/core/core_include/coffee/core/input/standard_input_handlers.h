#pragma once

#include <coffee/core/types/input/event_types.h>
#include <peripherals/stl/time_types.h>
#include <peripherals/typing/enum/graphics/direction.h>

#include <algorithm>
#include <map>
#include <memory>
#include <set>

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

        auto v              = keyRegister[ev.key];
        keyRegister[ev.key] = pressed | (v ^ (v & 0x1));
        break;
    }
    case CIEvent::MouseButton: {
        auto const& ev = *C_RCAST<CIMouseButtonEvent const*>(data);

        bool pressed = ev.mod & CIMouseButtonEvent::Pressed;

        auto v              = keyRegister[ev.btn];
        keyRegister[ev.btn] = pressed | (v ^ (v & 0x1));

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

    Vecf3 up{};
};

template<typename CameraPtr, typename CameraOptsPtr>
/*!
 * \brief Camera movement dependent on a fixed update cycle
 * \param c
 * \param reg
 * \return true when Camera reference is updated
 */
struct StandardCamera
    : std::enable_shared_from_this<StandardCamera<CameraPtr, CameraOptsPtr>>
{
    struct KeyboardInput
    {
        using event_type = CIKeyEvent;

        KeyboardInput(std::weak_ptr<StandardCamera> cam) : m_container(cam)
        {
        }

        void operator()(CIEvent const& e, CIKeyEvent const* ev)
        {
            StandardKeyRegister<Reg, CIEvent::Keyboard>(
                m_container.lock().get()->m_reg, e, ev);
        }

        std::weak_ptr<StandardCamera> m_container;
    };

    struct MouseInput
    {
        using event_type = CIMouseMoveEvent;

        MouseInput(
            std::shared_ptr<StandardCamera> cam,
            u32 button = CIMouseButtonEvent::LeftButton) :
            m_button(button),
            m_container(cam)
        {
        }

        void operator()(CIEvent const&, CIMouseMoveEvent const* ev)
        {
            if(ev->btn != m_button)
                return;
            auto yaw   = 0.01f * ev->delta.y;
            auto pitch = 0.01f * ev->delta.x;
            m_container->rotate(-pitch, -yaw);
        }

        u32                             m_button;
        std::shared_ptr<StandardCamera> m_container;
    };

    using Reg = std::map<u16, u16>;

    StandardCamera(CameraPtr cam, CameraOptsPtr opts) :
        m_opts(opts), m_camera(cam)
    {
    }

    inline bool has_key(u16 key) const
    {
        auto it = m_reg.find(key);

        return it != m_reg.end() && (it->second & 0x1);
    }

    inline void move(f32 forward, f32 right, f32 up, f32 accel = 5.f)
    {
        m_camera->position += forward * cached.forward * movement_speed * accel;
        m_camera->position += right * cached.right * movement_speed * accel;
        m_camera->position += up * cached.up * movement_speed * accel;
    }
    inline void rotate(f32 pitch, f32 yaw)
    {
        auto& rotation = m_camera->rotation;
        rotation       = glm::normalize(
            rotation * glm::quat(cached.right * yaw * -1.f)
            * glm::quat(cached.up * pitch * -1.f));
    }

    void tick(std::chrono::system_clock::duration const& t)
    {
        using namespace typing::vectors;

        auto& rotation = cached.rotation = glm::mat3_cast(m_camera->rotation);

        cached.forward = Vecf3{rotation[0][2], rotation[1][2], rotation[2][2]};
        cached.right   = Vecf3{rotation[0][0], rotation[1][0], rotation[2][0]};
        cached.up      = cross(cached.forward, cached.right);
        f32 acceleration = m_opts->accel.base;

        if(has_key(CK_LShift))
            acceleration = m_opts->accel.alt;
        if(has_key(CK_LCtrl))
            acceleration = m_opts->accel.fast;

        acceleration *= stl_types::Chrono::to_float(t);

        for(auto const& [key, mod] : m_reg)
        {
            if((mod & 0x1) == 0)
                continue;
            switch(key)
            {
            case CK_w:
                move(1, 0, 0, acceleration);
                break;
            case CK_s:
                move(-1, 0, 0, acceleration);
                break;
            case CK_a:
                move(0, 1, 0, acceleration);
                break;
            case CK_d:
                move(0, -1, 0, acceleration);
                break;
            case CK_q:
                move(0, 0, 1, acceleration);
                break;
            case CK_e:
                move(0, 0, -1, acceleration);
                break;
            }
        }
    }

    CameraOptsPtr m_opts;
    CameraPtr     m_camera;
    Reg           m_reg;
    f32           movement_speed{0.4f};

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
        sens.move.x = sens.move.y = 1.f;
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

template<typename CameraPtr, typename OptsPtr>
struct ControllerCamera
{
    ControllerCamera(CameraPtr cam, OptsPtr options) :
        m_camera(cam), m_opts(options)
    {
    }

    void operator()(
        CIControllerState const&                   state,
        std::chrono::system_clock::duration const& t)
    {
        using stl_types::Chrono::to_float;
        auto const& opt = opts();
        const auto  acceleration
            = 1.f + convert_i16_f(state.axes.e.t_l) * opt.curve * to_float(t);
        m_camera->move(
            filter(state.axes.e.l_y, opt.sens.move.y) * -1.f,
            filter(state.axes.e.l_x, opt.sens.move.x) * -1.f,
            0.f,
            acceleration);
        m_camera->rotate(
            filter(state.axes.e.r_x, opt.sens.look.x) * to_float(t) * -1.f,
            filter(state.axes.e.r_y, opt.sens.look.y) * to_float(t) * -1.f);
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
