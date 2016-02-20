#include <coffee/core/input/cinputfunctions.h>

#include <coffee/core/base/cmath.h>

namespace Coffee{
namespace CInput{

using namespace CMath;

void coffee_input_controller_rotate(
        CQuat *cqt, const CIControllerAtomicEvent *jev, CIAxisFilter& filter)
{
    CQuat* qt = cqt;

    if(CMath::fabs(jev->value)<0.1)
        return;
    CVec2 v(0);
    scalar tmp;
    switch(jev->index){
    case CK_AXIS_RIGHT_X:{
        tmp = jev->value/CMath::pow(2.0,16.0);
        v = filter.filterDelta(CVec2(tmp,0));
        break;
    }
    case CK_AXIS_RIGHT_Y:{
        tmp = jev->value/CMath::pow(2.0,16.0);
        v = filter.filterDelta(CVec2(0,tmp));
        break;
    }
    default:
        return;
    }

    *qt = normalize_quat(
                CQuat(CVec3(v.y()*0.1,0,0))
                *CQuat(CVec3(0,v.x()*0.1,0))
                *(*qt));
}

}
}
