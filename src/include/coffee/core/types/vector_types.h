#pragma once

#include <peripherals/typing/vectors/matrices.h>
#include <peripherals/typing/vectors/quaternion.h>
#include <peripherals/typing/vectors/vectors.h>

#include <peripherals/typing/vectors/matrices_decompose.h>
#include <peripherals/typing/vectors/quaternion_matrix_ops.h>

namespace Coffee{

template<typename T, size_t Num>
using _cbasic_tvector = typing::vectors::tvector<T,Num>;

template<typename T, size_t Num>
using _cbasic_tmatrix = typing::vectors::tmatrix<T, Num>;

template<typename T, size_t NumM, size_t NumN>
using _cbasic_tmnmatrix = typing::vectors::tmnmatrix<T, NumM, NumN>;

template<typename T>
using _cbasic_vec2 = typing::vectors::tvector<T,2>;

template<typename T>
using _cbasic_vec3 = typing::vectors::tvector<T,3>;

template<typename T>
using _cbasic_vec4 = typing::vectors::tvector<T,4>;

template<typename T>
using _cbasic_quat = typing::vectors::tquaternion<T>;

using Vecf2 = _cbasic_vec2<scalar>;
using Vecf3 = _cbasic_vec3<scalar>;
using Vecf4 = _cbasic_vec4<scalar>;

using Vecd2 = _cbasic_vec2<bigscalar>;
using Vecd3 = _cbasic_vec3<bigscalar>;
using Vecd4 = _cbasic_vec4<bigscalar>;

using Veci2 = _cbasic_vec2<i32>;
using Veci3 = _cbasic_vec3<i32>;
using Veci4 = _cbasic_vec4<i32>;

using Vecui2 = _cbasic_vec2<u32>;
using Vecui3 = _cbasic_vec3<u32>;
using Vecui4 = _cbasic_vec4<u32>;

using Quatf = _cbasic_quat<scalar>;
using Quatd = _cbasic_quat<bigscalar>;

using Matf2 = _cbasic_tmatrix<scalar,2>;
using Matf3 = _cbasic_tmatrix<scalar,3>;
using Matf4 = _cbasic_tmatrix<scalar,4>;

using Matf2_3 = _cbasic_tmnmatrix<scalar,2,3>;
using Matf2_4 = _cbasic_tmnmatrix<scalar,2,4>;
using Matf3_4 = _cbasic_tmnmatrix<scalar,3,4>;

using Matf3_2 = _cbasic_tmnmatrix<scalar,3,2>;
using Matf4_2 = _cbasic_tmnmatrix<scalar,4,2>;
using Matf4_3 = _cbasic_tmnmatrix<scalar,4,3>;

using Matd2 = _cbasic_tmatrix<bigscalar, 2>;
using Matd3 = _cbasic_tmatrix<bigscalar, 3>;
using Matd4 = _cbasic_tmatrix<bigscalar, 4>;

using Matd2_3 = _cbasic_tmnmatrix<bigscalar,2,3>;
using Matd2_4 = _cbasic_tmnmatrix<bigscalar,2,4>;
using Matd3_4 = _cbasic_tmnmatrix<bigscalar,3,4>;

using Matd3_2 = _cbasic_tmnmatrix<bigscalar,3,2>;
using Matd4_2 = _cbasic_tmnmatrix<bigscalar,4,2>;
using Matd4_3 = _cbasic_tmnmatrix<bigscalar,4,3>;

}
