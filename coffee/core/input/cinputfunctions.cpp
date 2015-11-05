#include "cinputfunctions.h"

namespace Coffee{
namespace CInput{

void coffee_input_mouse_rotate(CMath::quat *quat, const CIMouseMoveEvent *evsrc)
{
    *quat = CMath::normalize(
                CMath::quat(glm::vec3(0.01*evsrc->rel.y,0,0))
                *CMath::quat(glm::vec3(0,0.01*evsrc->rel.x,0))
                *(*quat));
}

void coffee_input_controller_rotate(CMath::quat *quat, const CIControllerAtomicEvent *jev, CIAxisFilter& filter)
{
    if(CMath::fabs(jev->value)<0.1)
        return;
    scalar vx=0,vy=0;
    switch(jev->index){
    case CK_AXIS_RIGHT_X:{
        vx = filter.filterDelta(
                    jev->value/CMath::pow(2.0,9.0));
        break;
    }
    case CK_AXIS_RIGHT_Y:{
        vy = filter.filterDelta(
                    jev->value/CMath::pow(2.0,9.0));
        break;
    }
    default:
        break;
    }

    *quat = CMath::normalize(
                CMath::quat(glm::vec3(0.01*vy,0,0))
                *CMath::quat(glm::vec3(0,0.01*vx,0))
                *(*quat));
}

CIAxisFilter::CIAxisFilter():
    last(0)
{
}

const scalar &CIAxisFilter::filterDelta(const scalar &v)
{
    if(v-last < 0 && v > 0.f)
        this->delta = 0;
    else if(v-last > 0 && v < 0.f)
        this->delta = 0;
    else if(CMath::abs(v-last) < 0.02f)
        C_NOOP();
    else
        this->delta = v-last;

    this->last = v;
    return this->delta;
}

}
}
