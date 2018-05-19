#ifndef COFFEE_TYPES_VECTORS_COFFEE_H
#define COFFEE_TYPES_VECTORS_COFFEE_H

#include "coffee/core/types/matrices.h"
#include "coffee/core/types/quaternion.h"

#include "coffee/core/types/vector_types.h"

namespace Coffee{

typedef _cbasic_vec2<scalar> CVec2;
typedef _cbasic_vec3<scalar> CVec3;
typedef _cbasic_vec4<scalar> CVec4;

typedef _cbasic_quat<scalar> CQuat;

typedef _cbasic_matrix_n<scalar,3> CMat3;
typedef _cbasic_matrix_n<scalar,4> CMat4;

}

#endif
