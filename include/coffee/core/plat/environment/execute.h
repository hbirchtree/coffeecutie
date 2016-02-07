#ifndef COFFEE_CORE_PLAT_ENV_EXEC_H
#define COFFEE_CORE_PLAT_ENV_EXEC_H

#include <coffee/core/CTypes>

namespace Coffee{

/*!
 * \brief Unsafe way of running another process
 * \param cmd
 * \return
 */
inline int Command(cstring cmd)
{
    return system(cmd);
}

}

#endif