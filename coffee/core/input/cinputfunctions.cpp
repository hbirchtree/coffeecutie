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

}
}
