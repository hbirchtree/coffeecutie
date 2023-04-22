#pragma once

#include <peripherals/stl/any_of.h>
#include <peripherals/typing/enum/pixels/format.h>

#include "blam_base_types.h"

namespace blam::vert {

struct compressed
{
};
struct uncompressed
{
};

template<typename T>
concept is_vert_compression = stl_types::is_any_of<T, compressed, uncompressed>;

using vec3_i16 = typing::vector_types::tvec3<i16>;

template<typename V>
requires is_vert_compression<V>
using normal_type = std::
    conditional_t<std::is_same_v<V, uncompressed>, Vecf3, typing::pixels::f11>;

template<typename V>
requires is_vert_compression<V>
struct alignas(4) vertex
{
    Vecf3          position;
    normal_type<V> normal; /* TODO: Fix Xbox normals, they're *not* f11 */
    normal_type<V> binorm;
    normal_type<V> tangent;
    Vecf2          texcoord;
};

struct compressed_weights
{
    u8  node0;
    u8  node1;
    u16 weight0;
};

struct uncompressed_weights
{
    u16 node0;
    u16 node1;
    f32 weight0;
    f32 weight1;
};

template<typename V>
requires is_vert_compression<V>
struct alignas(4) mod2_vertex
{
    static constexpr bool is_compressed = std::is_same_v<V, compressed>;
    using texcoord_value_type = std::conditional_t<is_compressed, i16, f32>;
    using weight_value_type   = std::
        conditional_t<is_compressed, compressed_weights, uncompressed_weights>;

    static constexpr size_t padding_size
        = std::is_same_v<V, uncompressed> ? 12 : 2;

    Vecf3          position;
    normal_type<V> normal;
    normal_type<V> binorm;
    normal_type<V> tangent;

    typing::vectors::tvector<texcoord_value_type, 2> texcoord;

    weight_value_type weights;
};

static_assert(
    sizeof(vertex<compressed>) == 32, "vertex<compessed> must be 32 bytes");
static_assert(
    sizeof(vertex<uncompressed>) == 56,
    "vertex<uncompressed> must be 56 bytes");
static_assert(
    sizeof(mod2_vertex<compressed>) == 32,
    "mod2_vertex<compressed> must be 32 bytes");
static_assert(
    sizeof(mod2_vertex<uncompressed>) == 68,
    "mod2_vertex<uncompressed> must be 68 bytes");

using idx_t = u16;
using face  = std::array<idx_t, 3>;

template<
    typename RType = uncompressed,
    bool           = std::is_same<RType, uncompressed>::value>
struct light_vertex
{
    Vecf3 normal;
    Vecf2 texcoord;
};

template<typename RType>
struct light_vertex<RType, false>
{
    // Compressed Xbox variant
    typing::pixels::f11              normal;   /*!< PixFmt::R11G11B10F */
    typing::vectors::tvector<i16, 2> texcoord; /*!< PixFmt::R16, Normalized */
};

} // namespace blam::vert
