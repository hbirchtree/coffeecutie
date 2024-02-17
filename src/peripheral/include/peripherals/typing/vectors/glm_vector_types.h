#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <glm/gtx/quaternion.hpp>

#include <peripherals/libc/types.h>

namespace typing::vectors {

template<typename T>
using tquaternion = glm::qua<T>;

template<typename T, size_t N>
using tvector = glm::vec<N, T>;

template<typename T, size_t N>
using tmatrix = glm::mat<N, N, T>;

} // namespace typing::vectors

namespace typing::vector_types {

using libc_types::i16;
using libc_types::i32;
using libc_types::i8;

using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

using libc_types::f32;
using libc_types::f64;

template<typename T>
using tquat = glm::qua<T>;

template<typename T, size_t N>
using tvector = glm::vec<N, T>;

template<typename T>
using tvec2 = tvector<T, 2>;

template<typename T>
using tvec3 = tvector<T, 3>;

template<typename T>
using tvec4 = tvector<T, 4>;

using Vecf2 = tvec2<f32>;
using Vecf3 = tvec3<f32>;
using Vecf4 = tvec4<f32>;

using Vecd2 = tvec2<f64>;

using Vecui2 = tvec2<u32>;
using Vecui3 = tvec3<u32>;
using Vecui4 = tvec4<u32>;

using Veci2 = tvec2<i32>;
using Veci3 = tvec3<i32>;
using Veci4 = tvec4<i32>;

using Matf2 = glm::mat2;
using Matf3 = glm::mat3;
using Matf4 = glm::mat4;

using Quatf = glm::tquat<f32>;

static_assert(sizeof(Vecf2) == 8);
static_assert(sizeof(Vecf3) == 12);
static_assert(sizeof(Vecf4) == 16);

static_assert(sizeof(Matf2) == 16);
static_assert(sizeof(Matf3) == 36);
static_assert(sizeof(Matf4) == 64);
} // namespace typing::vector_types
