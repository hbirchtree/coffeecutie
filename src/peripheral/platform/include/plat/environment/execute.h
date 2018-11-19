#pragma once

#include <peripherals/libc/types.h>

namespace Coffee {

/*!
 * \brief Unsafe way of running another process
 * \param cmd
 * \return
 */
inline int Command(cstring cmd)
{
    return system(cmd);
}

} // namespace Coffee
