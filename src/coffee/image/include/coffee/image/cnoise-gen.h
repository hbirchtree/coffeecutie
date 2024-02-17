#pragma once

#include <coffee/core/types/vector_types.h>

namespace Coffee {
namespace NoiseGen {

/*!
 * \brief Returns perlin noise values. Should operate in a thread-safe manner.
 * \param pos Position of sample
 * \param wrap Wrapping size of perlin noise, 256 is the maximum effective value
 * \return The noise value
 */
extern scalar Perlin(const Vecf3& pos, const Veci3& wrap = Veci3(255));

/*!
 * \brief Makes transfers the floating-point range [0.0,1.0] to integer [0,255]
 * \param v Value to transform, scaled to [0,1] range
 * \return A linear value in uint8 format
 */
inline C_FORCE_INLINE u8 Linearize(scalar v)
{
    return (u8)(v * 255);
}

} // namespace NoiseGen
} // namespace Coffee
