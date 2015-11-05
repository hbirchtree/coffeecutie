#ifndef CINPUTFUNCTIONS_H
#define CINPUTFUNCTIONS_H

#include "coffee/core/base/cinput.h"
#include "coffee/core/base/cmath.h"

namespace Coffee{
namespace CInput{

/*!
 * \brief Class made to filter out movement towards the center of the control stick
 */
class CIAxisFilter
{
public:
    CIAxisFilter();
    const scalar &filterDelta(const scalar &v);
private:
    scalar last;
    scalar delta;
};

/*!
 * \brief Stock function for rotating a quaternion by mouse movement.
 * \param quat
 * \param evsrc
 */
extern void coffee_input_mouse_rotate(CMath::quat* quat, const CIMouseMoveEvent* evsrc);

/*!
 * \brief Stock function for rotating a quaternion by controller stick movement.
 * \param quat
 * \param jev Controller event
 */
extern void coffee_input_controller_rotate(
        CMath::quat *quat,
        const CIControllerAtomicEvent *jev,
        CIAxisFilter &filter);

}
}

#endif
