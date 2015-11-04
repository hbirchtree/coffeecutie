#ifndef CINPUTFUNCTIONS_H
#define CINPUTFUNCTIONS_H

#include "coffee/core/base/cinput.h"
#include "coffee/core/base/cmath_glm.h"

namespace Coffee{
namespace CInput{

extern void coffee_input_mouse_rotate(CMath::quat* quat, const CIMouseMoveEvent* evsrc);

}
}

#endif
