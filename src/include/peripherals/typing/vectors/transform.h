#pragma once

#include "quaternion.h"

namespace typing {
namespace vectors {
namespace scene {

template<typename T>
struct transform
{
    FORCEDINLINE transform() :
        rotation(), position(0), scale(1.0)
    {
    }

    tquaternion<T> rotation;
    tvector<T, 3>  position;
    tvector<T, 3>  scale;
};

}
}
}
