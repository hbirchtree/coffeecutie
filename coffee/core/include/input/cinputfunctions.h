#ifndef CINPUTFUNCTIONS_H
#define CINPUTFUNCTIONS_H

#include <base/cinput.h>

namespace Coffee{
namespace CInput{

/*!
 * \brief Class made to filter out movement towards the center of the control stick
 */
class CIAxisFilter
{
public:
    CIAxisFilter();
    const CVec2 &filterDelta(const CVec2 &v);
private:
    CVec2 last;
    scalar m_deadzone;
};

/*!
 * \brief Stock function for rotating a quaternion by mouse movement.
 * \param cqt
 * \param evsrc
 */
extern void coffee_input_mouse_rotate(
        CQuat *cqt, const CIMouseMoveEvent* evsrc);

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
