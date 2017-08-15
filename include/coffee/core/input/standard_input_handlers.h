#pragma once

#include <coffee/core/base/types/cdisplay.h>
#include <coffee/core/base/input/cinput.h>
#include "cinputfunctions.h"
#include <coffee/core/types/edef/logicenum.h>

namespace Coffee{
namespace StandardInput{

using namespace CInput;

template<typename Camera>
void StandardCamera(void* r, const CIEvent& e, c_cptr data)
{
    Camera& camera = *C_CAST<Camera*>(r);

    auto camDirection = quaternion_to_direction<CameraDirection::Forward>(camera.rotation);
    const auto camUp = Vecf3(0, 1.f, 0);
    auto camRight = cross(camUp, camDirection);

    switch(e.type)
    {
    case CIEvent::Keyboard:
    {
        auto kev = C_CAST<CIKeyEvent const*>(data);
        scalar acceleration = 1.0;

        if(kev->mod & CIKeyEvent::LShiftModifier)
            acceleration = 15;

        switch(kev->key)
        {
        case CK_w:
            camera.position -= camDirection * 0.05f * acceleration;
            break;
        case CK_s:
            camera.position += camDirection * 0.05f * acceleration;
            break;
        case CK_a:
            camera.position -= camRight * 0.05f * acceleration;
            break;
        case CK_d:
            camera.position += camRight * 0.05f * acceleration;
            break;
        case CK_q:
            camera.position.y() += 0.05f * acceleration;
            break;
        case CK_e:
            camera.position.y() -= 0.05f * acceleration;
            break;
        default:
            break;
        }
        break;
    }
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
void ControllerCamera(Camera& cam, CIControllerState const& state,
                      i16 deadzone, scalar moveSensitivity,
                      scalar lookSensitivityX,
                      scalar lookSensitivityY)
{
    Vecf3& position = cam.position;
    Quatf& rotation = cam.rotation;

    const auto forward = quaternion_to_direction<CameraDirection::Forward>(rotation);
    const auto right = quaternion_to_direction<CameraDirection::Right>(rotation);

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
