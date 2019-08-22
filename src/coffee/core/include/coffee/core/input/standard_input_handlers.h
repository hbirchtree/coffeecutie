#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/types/direction.h>
#include <coffee/core/types/input/event_types.h>

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

template<
    const CIEvent::EventType EventType,
    typename KeyType     = u16,
    typename StorageType = u16>
/*!
 * \brief StandardKeyRegisterKB is a key register for input devices, suitable to
 * be hooked up with installEventHandler(). \param reg \param e \param data
 */
void StandardKeyRegisterImpl(void* reg, const CIEvent& e, c_cptr data)
{
    using RegisterType = Map<KeyType, StorageType>;

    auto& regRef = *C_FCAST<RegisterType*>(reg);

    StandardKeyRegister<RegisterType, EventType>(regRef, e, data);
}

template<typename CameraPtr>
/*!
 * \brief Camera movement dependent on a fixed update cycle
 * \param c
 * \param reg
 * \return true when Camera reference is updated
 */
struct StandardCameraMoveInput
{
    using event_type = CIKeyEvent;

    using Reg = Map<u16, u16>;

    StandardCameraMoveInput(CameraPtr cam) : m_camera(cam)
    {
    }

    inline bool has_key(u16 key) const
    {
        auto it = m_reg.find(key);

        return it != m_reg.end() && (it->second & 0x1);
    }

    void operator()(CIEvent const& e, CIKeyEvent const* ev) const
    {
        using namespace typing::vectors;
        static const Set<u16> keys = {CK_w, CK_s, CK_a, CK_d, CK_q, CK_e};

        StandardKeyRegister<Reg, CIEvent::Keyboard>(
            C_CCAST<Reg&>(m_reg), e, ev);

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

    CameraPtr m_camera;
    Reg       m_reg;
};

template<typename Camera>
/*!
 * \brief Perform standard camera operations
 * Allows control with WASD and mouse
 * Is used as an event handler
 * User pointer should be a camera structure with
 *  .position and .rotation
 * \param r Pointer to a CGCamera structure or similar
 * \param e An event
 * \param data
 */
void StandardCamera(void* r, const CIEvent& e, c_cptr data)
{
    Camera& camera = *C_CAST<Camera*>(r);

    switch(e.type)
    {
    case CIEvent::MouseMove:
    {
        auto ev = C_CAST<const CIMouseMoveEvent*>(data);

        auto& cqt = camera.rotation;

        auto yaw   = 0.01f * ev->delta.y;
        auto pitch = 0.01f * ev->delta.x;

        cqt = normalize_quat(Quatf(1, 0, pitch, 0) * cqt);

        cqt.x() = CMath::max(-0.5f, CMath::min(0.5f, cqt.x()));

        cqt = normalize_quat(Quatf(1, yaw, 0, 0) * cqt);

        break;
    }
    default:
        break;
    }
}

FORCEDINLINE scalar FilterJoystickInput(i16 raw, i16 deadzone, scalar sens)
{
    if(raw > deadzone || (-raw) > deadzone)
        return convert_i16_f(raw) * sens;
    return 0.f;
}

template<typename Camera>
/*!
 * \brief Allow control with generic game controller
 * Requires state-dump from joystick on every frame
 * \param cam Camera for a given player
 * \param state Controller state dump for a given controller
 * \param deadzone
 * \param moveSensitivity
 * \param lookSensitivityX
 * \param lookSensitivityY
 */
void ControllerCamera(
    Camera&                  cam,
    CIControllerState const& state,
    i16                      deadzone         = 6000,
    scalar                   moveSensitivity  = 0.3f,
    scalar                   lookSensitivityX = 0.02f,
    scalar                   lookSensitivityY = -0.02f)
{
    using namespace typing::vectors;

    Vecf3& position = cam.position;
    Quatf& rotation = cam.rotation;

    const auto forward =
        quaternion_to_direction<CameraDirection::Forward>(rotation);
    const auto right =
        quaternion_to_direction<CameraDirection::Right>(rotation);

    const auto acceleration = 1.f + convert_i16_f(state.axes.e.t_l) * 19.f;

    position +=
        forward * acceleration *
        FilterJoystickInput(state.axes.e.l_y, deadzone, moveSensitivity);
    position +=
        right * acceleration *
        FilterJoystickInput(state.axes.e.l_x, deadzone, moveSensitivity);

    auto pitch =
        FilterJoystickInput(state.axes.e.r_x, deadzone, lookSensitivityX);
    auto yaw =
        FilterJoystickInput(state.axes.e.r_y, deadzone, lookSensitivityY);

    rotation = normalize_quat(Quatf(1, 0, pitch, 0) * rotation);
    rotation = normalize_quat(Quatf(1, yaw, 0, 0) * rotation);
}

} // namespace StandardInput
} // namespace Coffee
