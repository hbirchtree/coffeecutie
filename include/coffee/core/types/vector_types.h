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

using CVec2 = _cbasic_vec2<scalar>;
using CVec3 = _cbasic_vec3<scalar>;
using CVec4 = _cbasic_vec4<scalar>;

using Vecf2 = CVec2;
using Vecf3 = CVec3;
using Vecf4 = CVec4;

using Vecd2 = _cbasic_vec2<bigscalar>;
using Vecd3 = _cbasic_vec3<bigscalar>;
using Vecd4 = _cbasic_vec4<bigscalar>;

using Veci2 = _cbasic_vec2<int32>;
using Veci3 = _cbasic_vec3<int32>;
using Veci4 = _cbasic_vec4<int32>;

using Vecui2 = _cbasic_vec2<uint32>;
using Vecui3 = _cbasic_vec3<uint32>;
using Vecui4 = _cbasic_vec4<uint32>;

using CQuat = _cbasic_quat<scalar>;

using Quatf = CQuat;
using Quatd = _cbasic_quat<bigscalar>;

using CMat2 = _cbasic_tmatrix<scalar,2>;
using CMat3 = _cbasic_tmatrix<scalar,3>;
using CMat4 = _cbasic_tmatrix<scalar,4>;

using Matf2 = CMat2;
using Matf3 = CMat3;
using Matf4 = CMat4;

using Matf2_3 = _cbasic_tmnmatrix<scalar,2,3>;
using Matf2_4 = _cbasic_tmnmatrix<scalar,2,4>;
using Matf3_4 = _cbasic_tmnmatrix<scalar,3,4>;

using Matf3_2 = _cbasic_tmnmatrix<scalar,3,2>;
using Matf4_2 = _cbasic_tmnmatrix<scalar,4,2>;
using Matf4_3 = _cbasic_tmnmatrix<scalar,4,3>;

}

#endif
