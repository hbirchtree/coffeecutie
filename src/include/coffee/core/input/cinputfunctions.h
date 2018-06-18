#ifndef CINPUTFUNCTIONS_H
#define CINPUTFUNCTIONS_H

#include "../base/input/cinput.h"

namespace Coffee{
namespace CInput{

/*!
 * \brief Stock function for rotating a quaternion by mouse movement.
 * \param cqt
 * \param evsrc
 */
FORCEDINLINE void MouseRotate(CQuat& cqt,
                              const CIMouseMoveEvent* evsrc,
                              scalar const& sens = 0.01)
{
    auto yaw = sens * evsrc->delta.y;
    auto pitch = sens * evsrc->delta.x;

    cqt = normalize_quat(Quatf(1, 0, pitch, 0) * cqt);

    cqt.x() = CMath::max(-0.5f, CMath::min(0.5f, cqt.x()));

    cqt = normalize_quat(Quatf(1, yaw, 0, 0) * cqt);
}

FORCEDINLINE void ControllerRotate(CQuat& cqt,
                                   const CIControllerAtomicEvent* evsrc,
                                   scalar const& sens = 0.01)
{
    bool v = evsrc->index+CK_AXIS_LEFT_X == CK_AXIS_RIGHT_X;
    cqt = normalize_quat(
                CQuat(1,
                      sens*(evsrc->value/(scalar)Int16_Max)*(!v),
                      sens*(evsrc->value/(scalar)Int16_Max)*(v),
                      0)
                * cqt);
}

}
}

#endif
