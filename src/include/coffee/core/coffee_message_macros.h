#pragma once

#include "plat/memory/cmemory.h"
#include "types/tdef/integertypes.h"

namespace Coffee{

/*!
 * \brief This is used for marking functionality as stubbed, used for smaller, less important features.
 * \param name
 */
FORCEDINLINE void C_STUBBED(cstring name)
{
#ifndef NDEBUG
    fprintf(DefaultDebugOutputPipe,"STUBD:%s@%i: %s\n",
            __FILE__,
            __LINE__,
            name);
#endif
}

FORCEDINLINE void C_FIXME(cstring identifier)
{
#ifndef NDEBUG
    fprintf(DefaultDebugOutputPipe,"FIXME:%s@%i: %s\n",
            __FILE__,
            __LINE__,
            identifier);
#endif
}

FORCEDINLINE void C_PERFWARN(cstring file, int line, cstring msg)
{
#ifndef NDEBUG
    fprintf(DefaultDebugOutputPipe,"PERFW:%s@%i: %s\n",file,line,msg);
#endif
}

/*!
 * \brief For reporting fatal lack of functionality in platform layer, used for features such as file mapping if it is unsupported on a platform.
 * \param name
 */
FORCEDINLINE void C_BADERROR(cstring name)
{
#ifndef NDEBUG
    fprintf(DefaultDebugOutputPipe,"FATAL:%s@%i: %s\n",
            __FILE__,
            __LINE__,
            name);
#endif
}

//FORCEDINLINE void C_DEPRECATED()
//{
//#ifndef NDEBUG
//    fprintf(DefaultDebugOutputPipe,"DEPRECATED");
//#endif
//}

}