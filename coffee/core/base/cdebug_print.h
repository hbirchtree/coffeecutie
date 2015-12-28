#ifndef COFFEE_CORE_DEBUG_PRINT_H
#define COFFEE_CORE_DEBUG_PRINT_H

#include <iomanip>
#include "coffee/core/coffee_macros.h"
#include "coffee/core/plat/cmemory.h"
#include "coffee/core/types/basetypes.h"

namespace Coffee{
namespace CDebugPrint{

inline CString cStringResolve(CString fmt, size_t)
{
    return fmt;
}

inline CString cStrReplace(CString fmt, size_t index, CString replace)
{
    CString subfmt = "{" + std::to_string(index) + "}";
    return c_str_replace(fmt,subfmt,replace);
}

template<typename T>
inline CString cStringReplace(
        CString fmt, size_t index,
        const T* const& ptr)
{
    CString rep;
    std::stringstream ss;
    ss << std::hex << (uintptr_t)ptr;
    ss >> rep;
    return cStrReplace(fmt,index,"0x"+rep);
}

inline CString cStringReplace(
        CString fmt, size_t index,
        cstring arg)
{
    return cStrReplace(fmt,index,(arg) ? arg : "0x0");
}

inline CString cStringReplace(
        CString fmt, size_t index,
        char* const arg)
{
    return cStrReplace(fmt,index,(arg) ? arg : "0x0");
}

template<typename T>
inline CString cStringReplace(
        CString fmt, size_t index,
        const T& arg)
{
    return cStrReplace(fmt,index,std::to_string(arg));
}

template<typename T>
inline CString cStringResolve(CString fmt, size_t index, const T& arg)
{
    CString str = cStringReplace(fmt,index,arg);
    return cStringResolve(str,++index);
}

template<typename... Args, typename T>
inline CString cStringResolve(CString fmt, size_t index, const T& arg, Args... args)
{
    CString str = cStringReplace(fmt,index,arg);
    return cStringResolve(str,++index,args...);
}

template<typename... Arg>
inline CString cStringFormat(cstring fmt, Arg... args)
{
    CString str = cStringResolve(fmt,0,args...);
//    str.resize(snprintf(NULL,0,fmt,args...)+1);
//    sprintf(&str[0],fmt,args...);
    return str;
}

}
}

#endif
