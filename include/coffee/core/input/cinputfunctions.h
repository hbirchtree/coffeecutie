#ifndef CINPUTFUNCTIONS_H
#define CINPUTFUNCTIONS_H

#include "../base/input/cinput.h"

namespace Coffee{
namespace CInput{

/*!
 * \brief Class made to filter out movement towards the center of the control stick
 */
class CIAxisFilter
{
public:
    FORCEDINLINE CIAxisFilter():
        last(0,0)
    {
    }
    FORCEDINLINE const CVec2 &filterDelta(const CVec2 &v)
    {
        CVec2 out = v;

        if(sqrt(pow(out.x(),2.f)+pow(out.y(),2.f))
                < sqrt(pow(m_deadzone,2.f)+pow(m_deadzone,2.f)))
        {
            out = CVec2(0,0);
        }

        //TODO: Filter out movement toward center

        //TODO: Remap stick from 0.1-0.85 to 0.0,1.0

        this->last = out;
        return this->last;
    }
private:
    CVec2 last;
    scalar m_deadzone;
};

/*!
 * \brief Stock function for rotating a quaternion by mouse movement.
 * \param cqt
 * \param evsrc
 */
FORCEDINLINE void MouseRotate(CQuat& cqt,
                              const CIMouseMoveEvent* evsrc,
                              scalar const& sens = 0.01)
{
    cqt = normalize_quat(
                  CQuat(1,sens*evsrc->delta.y,0,0)
                * CQuat(1,0,sens*evsrc->delta.x,0)
                * cqt);
}

FORCEDINLINE void ControllerRotate(CQuat& cqt,
                                   const CIControllerAtomicEvent* evsrc,
                                   scalar const& sens = 0.01)
{
    bool v = evsrc->index+CK_AXIS_LEFT_X == CK_AXIS_RIGHT_X;
    cqt = normalize_quat(
                CQuat(1,
                      sens*(evsrc->value/(scalar)Int16_Max)*v,
                      sens*(evsrc->value/(scalar)Int16_Max)*(!v),
                      0)
                * cqt);
}

/*!
 * \brief Stock function for rotating a quaternion by controller stick movement.
 * \param cqt
 * \param jev Controller event
 * \param filter
 */
extern void coffee_input_controller_rotate(
        CQuat *cqt,
        const CIControllerAtomicEvent *jev,
        CIAxisFilter &filter);

}
}

#endif
