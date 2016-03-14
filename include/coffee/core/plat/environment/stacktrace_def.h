#ifndef COFFEE_PLAT_ENVIRONMENT_STACKTRACE_DEF_H
#define COFFEE_PLAT_ENVIRONMENT_STACKTRACE_DEF_H

#include "../../types/basetypes.h"

namespace Coffee{

struct StacktracerDef
{
    using Stacktrace = std::vector<CString>;

    /*!
     * \brief Will return stackframe name for calling function at depth 0
     * \param depth
     * \return A string with the name of the stackframe
     */
    static CString GetStackframeName(uint32 depth = 0);
    /*!
     * \brief Dump the full stack
     * \param start
     * \param length
     * \return A vector containing stackframe names
     */
    static Stacktrace GetFullStack(uint32 start = 0, int32 length = -1);

};

}

#endif

#include "linux/stacktrace.h"
#include "android/stacktrace.h"