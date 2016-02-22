#ifndef COFFEE_CORE_DEBUG_PRINT_H
#define COFFEE_CORE_DEBUG_PRINT_H

#include "extensionresolvers.h"

namespace Coffee{
namespace DebugFun{

/* Core string resolution */

template<typename T>
FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const T& arg)
{
    return extArgReplace(fmt,index,conversion::to_string(arg));
}

template<typename T>
FORCEDINLINE CString cStringResolve(CString fmt, size_t index, const T& arg)
{
    CString str = cStringReplace(fmt,index,arg);
    return cStringResolve(str,++index);
}

template<typename... Args, typename T>
FORCEDINLINE CString cStringResolve(CString fmt, size_t index, const T& arg, Args... args)
{
    CString str = cStringReplace(fmt,index,arg);
    return cStringResolve(str,++index,args...);
}

template<typename... Arg>
FORCEDINLINE CString cStringFormat(cstring fmt, Arg... args)
{
    return cStringResolve(fmt,0,args...);
}

}
}

#endif
