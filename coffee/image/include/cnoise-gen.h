#pragma once

#include <coffee/core/CTypes>

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

namespace Coffee{
namespace NoiseGen{

/*!
 * \brief Returns perlin noise values. Should operate in a thread-safe manner.
 * \param pos Position of sample
 * \param wrap Wrapping size of perlin noise, 256 is the maximum effective value
 * \return The noise value
 */
inline C_FORCE_INLINE scalar Perlin(
        const CVec3& pos,
        const _cbasic_vec3<int32>& wrap = _cbasic_vec3<int32>())
{
    return stb_perlin_noise3(pos.x(),pos.y(),pos.z(),wrap.x(),wrap.y(),wrap.z());
}

/*!
 * \brief Makes transfers the floating-point range [0.0,1.0] to integer [0,255]
 * \param v Value to transform, scaled to [0,1] range
 * \return A linear value in uint8 format
 */
inline C_FORCE_INLINE uint8 Linearize(scalar v)
{
    return (uint8)v*255;
}

}
}