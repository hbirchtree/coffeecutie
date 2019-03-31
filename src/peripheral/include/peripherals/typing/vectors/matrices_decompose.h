#pragma once

#include "matrices.h"

namespace typing {
namespace vectors {
namespace decompose {

enum DecompositionType
{
    translation,
    scale,

    rotation,      /*!< rotation, multiplied before scaling */
    rotation_post, /*!< rotation, multiplied after scaling */

    /* TODO: Add euler version */
};

template<
    u32 Variant,
    typename T,
    typename std::enable_if<Variant == translation>::type* = nullptr>
FORCEDINLINE tvector<T, 3> get(tmatrix<T, 4> const& from)
{
    return tvector<T, 3>(from[3]);
}

template<
    u32 Variant,
    typename T,
    typename std::enable_if<Variant == scale>::type* = nullptr>
FORCEDINLINE tvector<T, 3> get(tmatrix<T, 4> const& from)
{
    using vec_t = tvector<T, 3>;

    return vec_t(
        length(vec_t(from[0])), length(vec_t(from[1])), length(vec_t(from[2])));
}

template<
    u32 Variant,
    typename T,
    typename std::enable_if<Variant == rotation>::type* = nullptr>
FORCEDINLINE tmatrix<T, 4> get(tmatrix<T, 4> const& from)
{
    using vec_t = tvector<T, 3>;

    tmatrix<T, 4> out;

    auto scale = tvector<T, 4>(
        length(vec_t(from[0])),
        length(vec_t(from[1])),
        length(vec_t(from[2])),
        1.f);

    out[0] = from[0] / scale.x();
    out[1] = from[1] / scale.y();
    out[2] = from[2] / scale.z();

    return out;
}

template<
    u32 Variant,
    typename T,
    typename std::enable_if<Variant == rotation_post>::type* = nullptr>
FORCEDINLINE tmatrix<T, 4> get(tmatrix<T, 4> const& from)
{
    using vec_t = tvector<T, 3>;

    tmatrix<T, 4> out;

    auto scale = tvector<T, 4>(
        length(vec_t(from[0])),
        length(vec_t(from[1])),
        length(vec_t(from[2])),
        1.f);

    out[0] = scale.x() / from[0];
    out[1] = scale.y() / from[1];
    out[2] = scale.z() / from[2];

    return out;
}

} // namespace decompose
} // namespace vectors
} // namespace typing
