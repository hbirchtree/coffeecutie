#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/types/direction.h>
#include <coffee/core/types/input/event_types.h>

#include <algorithm>

namespace Coffee {
namespace StandardInput {

using namespace Input;

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
    case CIEvent::Keyboard:
    {
        auto const& ev = *C_FCAST<CIKeyEvent const*>(data);

        if(ev.mod & CIKeyEvent::RepeatedModifier)
            break;

        bool pressed = ev.mod & CIKeyEvent::PressedModifier;

        auto v              = keyRegister[ev.key];
        keyRegister[ev.key] = pressed | (v ^ (v & 0x1));
        break;
    }
    case CIEvent::MouseButton:
    {
        auto const& ev = *C_FCAST<CIMouseButtonEvent const*>(data);

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
    StandardCameraOpts()
    {
        accel.base = 5.f;
        accel.alt  = 10.f;

        up = {0, 1, 0};
    }

    struct
    {
        scalar base, alt;
    } accel;

    Vecf3 up;
};

template<typename CameraPtr, typename CameraOptsPtr>
/*!
 * \brief Camera movement dependent on a fixed update cycle
 * \param c
 * \param reg
 * \return true when Camera reference is updated
 */
struct StandardCamera
{
    struct KeyboardInput
    {
        using event_type = CIKeyEvent;

        KeyboardInput(WkPtr<StandardCamera> cam) : m_container(cam)
        {
        }

        void operator()(CIEvent const& e, CIKeyEvent const* ev) const
        {
            StandardKeyRegister<Reg, CIEvent::Keyboard>(
                m_container.lock().get()->m_reg, e, ev);
        }

        WkPtr<StandardCamera> m_container;
    };

    struct MouseInput
    {
        using event_type = CIMouseMoveEvent;

        MouseInput(
            CameraPtr cam, u32 button = CIMouseButtonEvent::LeftButton) :
            m_button(button),
            m_container(cam)
        {
        }

        void operator()(CIEvent const& e, CIMouseMoveEvent const* ev) const
        {
            if(ev->btn != m_button)
                return;

            auto& cqt = (*m_container).rotation;

            auto yaw   = 0.01f * ev->delta.y;
            auto pitch = 0.01f * ev->delta.x;

            cqt = normalize_quat(Quatf(1, 0, pitch, 0) * cqt);

            cqt.x() = CMath::max(-0.5f, CMath::min(0.5f, cqt.x()));

            cqt = normalize_quat(Quatf(1, yaw, 0, 0) * cqt);
        }

        u32       m_button;
        CameraPtr m_container;
    };

    using Reg = Map<u16, u16>;

    StandardCamera(CameraPtr cam, CameraOptsPtr opts) :
        m_opts(opts), m_camera(cam)
    {
    }

    inline bool has_key(u16 key) const
    {
        auto it = m_reg.find(key);

        return it != m_reg.end() && (it->second & 0x1);
    }

    void tick()
    {
        using namespace typing::vectors;
        static const Set<u16> keys = {CK_w, CK_s, CK_a, CK_d, CK_q, CK_e};

        Reg::const_iterator it = std::find_if(
            m_reg.cbegin(), m_reg.cend(), [](Pair<const u16, u16> const& p) {
                return keys.find(p.first) != keys.cend();
            });

        auto& c = *m_camera;

        if(it == m_reg.end())
            return;

        auto camDirection =
            quaternion_to_direction<CameraDirection::Forward>(c.rotation);
        const auto camUp        = Vecf3(0, 1.f, 0);
        auto       camRight     = cross(camUp, camDirection);
        scalar     acceleration = 5.f;

        if(has_key(CK_LShift))
            acceleration = 10.f;

        while(it != m_reg.end())
        {
            if((*it).second & 0x1)
                switch((*it).first)
                {
                case CK_w:
                    c.position -= camDirection * 0.05f * acceleration;
                    break;
                case CK_s:
                    c.position += camDirection * 0.05f * acceleration;
                    break;
                case CK_a:
                    c.position -= camRight * 0.05f * acceleration;
                    break;
                case CK_d:
                    c.position += camRight * 0.05f * acceleration;
                    break;
                case CK_q:
                    c.position.y() += 0.05f * acceleration;
                    break;
                case CK_e:
                    c.position.y() -= 0.05f * acceleration;
                    break;
                }
            it++;
        }
    }

    CameraOptsPtr m_opts;
    CameraPtr     m_camera;
    Reg           m_reg;
};

struct ControllerOpts
{
    ControllerOpts()
    {
        deadzone    = 6000;
        curve       = 19.f;
        sens.move.x = sens.move.y = 0.15f;
        sens.look.x = sens.look.y = 0.005f;

        invertYLook();
    }

    struct
    {
        struct
        {
            scalar x, y;
        } move;
        struct
        {
            scalar x, y;
        } look;
    } sens;

    i16    deadzone;
    scalar curve;

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

    void operator()(CIControllerState const& state)
    {
        using namespace typing::vectors;

        Vecf3& position = (*m_camera).position;
        Quatf& rotation = (*m_camera).rotation;

        const auto forward =
            quaternion_to_direction<CameraDirection::Forward>(rotation);
        const auto right =
            quaternion_to_direction<CameraDirection::Right>(rotation);

        auto const& opt = opts();

        const auto acceleration =
            1.f + convert_i16_f(state.axes.e.t_l) * opt.curve;

        position += right * acceleration *
                    FilterJoystickInput(state.axes.e.l_x, opt.sens.move.x);
        position += forward * acceleration *
                    FilterJoystickInput(state.axes.e.l_y, opt.sens.move.y);

        auto pitch = FilterJoystickInput(state.axes.e.r_x, opt.sens.look.x);
        auto yaw   = FilterJoystickInput(state.axes.e.r_y, opt.sens.look.y);

        rotation = normalize_quat(Quatf(1, 0, pitch, 0) * rotation);
        rotation = normalize_quat(Quatf(1, yaw, 0, 0) * rotation);
    }

    ControllerOpts& opts()
    {
        return *m_opts;
    }

  private:
    FORCEDINLINE scalar FilterJoystickInput(i16 raw, scalar sens)
    {
        if(raw > opts().deadzone || (-raw) > opts().deadzone)
        {
            return convert_i16_f(raw) * sens;
        }
        return 0.f;
    }

    CameraPtr m_camera;
    OptsPtr   m_opts;
};

} // namespace StandardInput
} // namespace Coffee
