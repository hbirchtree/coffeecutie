#include "cinputfunctions.h"

namespace Coffee{
namespace CInput{

using namespace CMath;

void coffee_input_mouse_rotate(CMath::quat *qt, const CIMouseMoveEvent *evsrc)
{
    *qt = CMath::normalize(
                quat(vec3(0.01*evsrc->rel.y,0,0))
                *quat(vec3(0,0.01*evsrc->rel.x,0))
                *(*qt));
}

void coffee_input_controller_rotate(CMath::quat *qt, const CIControllerAtomicEvent *jev, CIAxisFilter& filter)
{
    if(CMath::fabs(jev->value)<0.1)
        return;
    CVec2 v;
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

    *qt = normalize(
                quat(vec3(v.y*0.1,0,0))
                *quat(vec3(0,v.x*0.1,0))
                *(*qt));
}

CIAxisFilter::CIAxisFilter():
    last(0,0)
{
}

const CVec2 &CIAxisFilter::filterDelta(const CVec2 &v)
{
    CVec2 out = v;

    if(sqrt(pow(out.x,2.f)+pow(out.x,2.f)) < sqrt(pow(m_deadzone,2.f)+pow(m_deadzone,2.f)))
    {
        out = CVec2(0,0);
    }

    //TODO: Filter out movement toward center

    //TODO: Remap stick from 0.1-0.85 to 0.0,1.0

    this->last = out;
    return this->last;
}

}
}
