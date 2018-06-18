#pragma once

#include <coffee/core/base/types/cdisplay.h>
#include "cinputfunctions.h"
#include <coffee/core/types/edef/logicenum.h>
#include <coffee/core/base/renderer/eventapplication.h>

namespace Coffee{
namespace StandardInput{

using namespace CInput;

template<typename InputRegister, const CIEvent::EventType EventType>
/*!
 * \brief By default, event handlers only receive state changes. This function captures the state of keyboard and mouse clicks into a register which allows random access to keys.
 * \param keyRegister A std::map-like structure with operator[]
 * \param e
 * \param data
 * \return true when an event was found
 */
bool StandardKeyRegister(InputRegister& keyRegister,
                         const CIEvent& e, c_cptr data)
{
    static_assert(EventType == CIEvent::Keyboard ||
                  EventType == CIEvent::MouseButton,
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

        auto v = keyRegister[ev.key];
        keyRegister[ev.key] = pressed | (v ^ (v & 0x1));
        break;
    }
    case CIEvent::MouseButton:
    {
        auto const& ev = *C_FCAST<CIMouseButtonEvent const*>(data);

        bool pressed = ev.mod & CIMouseButtonEvent::Pressed;

        auto v = keyRegister[ev.btn];
        keyRegister[ev.btn] = pressed | (v ^ (v & 0x1));

        break;
    }
    default:
        return false;
    }

    return true;
}

template<const CIEvent::EventType EventType,
         typename  KeyType = u16, typename StorageType = u16>
/*!
 * \brief StandardKeyRegisterKB is a key register for input devices, suitable to be hooked up with installEventHandler().
 * \param reg
 * \param e
 * \param data
 */
void StandardKeyRegisterImpl(void* reg, const CIEvent& e, c_cptr data)
{
    using RegisterType = Map<KeyType, StorageType>;

    auto& regRef = *C_FCAST<RegisterType*>(reg);

    StandardKeyRegister<RegisterType, EventType>(regRef, e,  data);
}

template<typename Camera>
/*!
 * \brief Camera movement dependent on a fixed update cycle
 * \param c
 * \param reg
 * \return true when Camera reference is updated
 */
bool StandardCameraMoveInput(Camera& c, Map<u16, u16>& reg)
{
    using Reg = Map<u16, u16>;

    static Set<u16> keys = {
        CK_w, CK_s, CK_a, CK_d, CK_q, CK_e
    };

    Reg::const_iterator it = std::find_if(reg.begin(), reg.end(),
                 [](Pair<const u16,u16>& p) {
        return keys.find(p.first) != keys.end();
    });

    if(it == reg.end())
        return false;

    auto camDirection = quaternion_to_direction<CameraDirection::Forward>(c.rotation);
    const auto camUp = Vecf3(0, 1.f, 0);
    auto camRight = cross(camUp, camDirection);
    scalar acceleration = 5.f;

    if(reg[CK_LShift] & 0x1)
        acceleration = 10.f;

    while(it != reg.end())
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

    return true;
}

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
        MouseRotate(camera.rotation, ev);
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
void ControllerCamera(Camera& cam, CIControllerState const& state,
                      i16 deadzone = 6000, scalar moveSensitivity = 0.3f,
                      scalar lookSensitivityX = 0.02f,
                      scalar lookSensitivityY = -0.02f)
{
    Vecf3& position = cam.position;
    Quatf& rotation = cam.rotation;

    const auto forward =
            quaternion_to_direction<CameraDirection::Forward>(rotation);
    const auto right =
            quaternion_to_direction<CameraDirection::Right>(rotation);

    position += forward * FilterJoystickInput(state.axes.e.l_y,
                                              deadzone,
                                              moveSensitivity);
    position += right * FilterJoystickInput(state.axes.e.l_x,
                                              deadzone,
                                              moveSensitivity);

    auto pitch = FilterJoystickInput(state.axes.e.r_x,
                                     deadzone, lookSensitivityX);
    auto yaw = FilterJoystickInput(state.axes.e.r_y,
                                   deadzone, lookSensitivityY);

    rotation = normalize_quat(Quatf(1, 0, pitch, 0) * rotation);
    rotation = normalize_quat(Quatf(1, yaw, 0, 0) * rotation);
}

}
}
