#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include "coffee.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

namespace Coffee{
namespace CGraphicsData{

struct CGCamera
{
    enum CameraFlags{
        OrthographicFlag        = 0b1,
        PerspectiveFlag         = 0b10,
    };

    byte        flags            = 0;

    scalar      aspect           = 1.f;
    CSizeF      zVals;
    scalar      fieldOfView      = 90.f;

    CVec3       position;
    CQuat       rotation;
};

} //CGraphicsData
} //Coffee

#endif // CGRAPHICSDATA_H
