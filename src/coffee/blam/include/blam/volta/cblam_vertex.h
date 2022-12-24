#pragma once

#include <peripherals/stl/any_of.h>

#include "cblam_base_types.h"

namespace blam::vert {

struct compressed
{
};
struct uncompressed
{
};

template<typename T>
concept is_vert_compression = stl_types::is_any_of<T, compressed, uncompressed>;

template<typename V>
requires is_vert_compression<V>
using normal_type = std::
    conditional_t<std::is_same_v<V, uncompressed>, Vecf3, typing::pixels::f11>;

template<typename V>
requires is_vert_compression<V>
struct alignas(4) vertex
{
    Vecf3          position;
    normal_type<V> normal;
    normal_type<V> binorm;
    normal_type<V> tangent;
    Vecf2          texcoord;
};

template<typename V>
requires is_vert_compression<V>
struct alignas(4) mod2_vertex
{
    using texcoord_value_type
        = std::conditional_t<std::is_same_v<V, uncompressed>, f32, i16>;
    static constexpr size_t padding_size
        = std::is_same_v<V, uncompressed> ? 12 : 2;

    Vecf3          position;
    normal_type<V> normal;
    normal_type<V> binorm;
    normal_type<V> tangent;

    typing::vectors::tvector<texcoord_value_type, 2> texcoord;

    union
    {
        u16  weight;
        char padding[padding_size];
    };
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
using face  = Array<idx_t, 3>;

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
