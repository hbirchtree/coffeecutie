#pragma once

#if defined(USE_HOMEGROWN_VECTORS)

#include <peripherals/typing/vectors/matrices.h>
#include <peripherals/typing/vectors/quaternion.h>
#include <peripherals/typing/vectors/vectors.h>

#include <peripherals/libc/types.h>
#include <peripherals/typing/vectors/matrices_decompose.h>
#include <peripherals/typing/vectors/quaternion_matrix_ops.h>

#else

#include "glm_vector_types.h"

#endif

namespace typing::vector_types {

#if defined(USE_HOMEGROWN_VECTORS)
using libc_types::f32;
using libc_types::f64;
using libc_types::i32;
using libc_types::u32;

namespace decompose {
using namespace typing::vectors::decompose;
}

using typing::vectors::tmatrix;

using typing::vectors::tmnmatrix;

template<typename T>
using tvec2 = typing::vectors::tvector<T, 2>;

template<typename T>
using tvec3 = typing::vectors::tvector<T, 3>;

template<typename T>
using tvec4 = typing::vectors::tvector<T, 4>;

template<typename T>
using tquat = typing::vectors::tquaternion<T>;

using Vecf2 = tvec2<f32>;
using Vecf3 = tvec3<f32>;
using Vecf4 = tvec4<f32>;

using Vecd2 = tvec2<f64>;
using Vecd3 = tvec3<f64>;
using Vecd4 = tvec4<f64>;

using Veci2 = tvec2<i32>;
using Veci3 = tvec3<i32>;
using Veci4 = tvec4<i32>;

using Vecui2 = tvec2<u32>;
using Vecui3 = tvec3<u32>;
using Vecui4 = tvec4<u32>;

using Quatf = tquat<f32>;
using Quatd = tquat<f64>;

using Matf2 = tmatrix<f32, 2>;
using Matf3 = tmatrix<f32, 3>;
using Matf4 = tmatrix<f32, 4>;

using Matf2_3 = tmnmatrix<f32, 2, 3>;
using Matf2_4 = tmnmatrix<f32, 2, 4>;
using Matf3_4 = tmnmatrix<f32, 3, 4>;

using Matf3_2 = tmnmatrix<f32, 3, 2>;
using Matf4_2 = tmnmatrix<f32, 4, 2>;
using Matf4_3 = tmnmatrix<f32, 4, 3>;

using Matd2 = tmatrix<f64, 2>;
using Matd3 = tmatrix<f64, 3>;
using Matd4 = tmatrix<f64, 4>;

using Matd2_3 = tmnmatrix<f64, 2, 3>;
using Matd2_4 = tmnmatrix<f64, 2, 4>;
using Matd3_4 = tmnmatrix<f64, 3, 4>;

using Matd3_2 = tmnmatrix<f64, 3, 2>;
using Matd4_2 = tmnmatrix<f64, 4, 2>;
using Matd4_3 = tmnmatrix<f64, 4, 3>;
#endif

} // namespace typing::vector_types
