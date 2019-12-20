#pragma once

#include "cblam_structures.h"

namespace blam {

namespace detail {
/*!
 * \brief Used to decode magic index pointers
 * \param base Pointer to the file header
 * \param magic Magic number from tag index
 * \param offset Data offset which will be transformed
 * \return A pointer to the described data
 */
extern const void* magic_ptr(c_cptr base, i32 magic, i32 offset);

} // namespace detail

template<typename T>
STATICINLINE T magic_ptr(c_cptr base, i32 magic, u32 offset)
{
    return C_RCAST<T>(detail::magic_ptr(base, magic, C_FCAST<i32>(offset)));
}

} // namespace blam
