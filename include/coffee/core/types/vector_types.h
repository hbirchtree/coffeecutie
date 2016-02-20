#ifndef COFFEE_VECTOR_TYPES_H
#define COFFEE_VECTOR_TYPES_H

#include <cmath>
#include "vectors.h"
#include "matrices.h"
#include "quaternion.h"

namespace Coffee{

template<typename T>
using _cbasic_vec2 = _cbasic_tvector<T,2>;

template<typename T>
using _cbasic_vec3 = _cbasic_tvector<T,3>;

template<typename T>
using _cbasic_vec4 = _cbasic_tvector<T,4>;

template<typename T>
using _cbasic_quat = _cbasic_tquaternion<T>;

template<typename T,size_t Size>
using _cbasic_matrix_n = _cbasic_tmatrix<T,Size>;

typedef _cbasic_vec2<scalar> CVec2;
typedef _cbasic_vec3<scalar> CVec3;
typedef _cbasic_vec4<scalar> CVec4;

typedef _cbasic_quat<scalar> CQuat;

typedef _cbasic_matrix_n<scalar,3> CMat3;
typedef _cbasic_matrix_n<scalar,4> CMat4;

}

#endif
