#ifndef COFFEE_CORE_DEBUG_PRINT_H
#define COFFEE_CORE_DEBUG_PRINT_H

#include "extensionresolvers.h"
#include "information_extensions.h"
#include "vector_print_extensions.h"

namespace Coffee{
namespace Strings{

/* Core string resolution */

template<typename T>
FORCEDINLINE CString cStringReplace(CString& fmt, size_t index,
                                    T const& arg)
{
    return extArgReplace(fmt, index, to_string(arg));
}

template<typename T>
FORCEDINLINE CString cStringResolve(CString& fmt, size_t index,
                                    const T& arg)
{
    return cStringReplace(fmt,index,arg);
}

template<typename... Args, typename T>
FORCEDINLINE CString cStringResolve(CString& fmt, size_t index,
                                    const T& arg, Args... args)
{
    CString str = cStringReplace(fmt,index,arg);
    return cStringResolve(str,++index,args...);
}

template<typename... Arg>
FORCEDINLINE CString cStringFormat(cstring fmt, Arg... args)
{
    CString fmt_ = fmt;

    fmt_ = cStringResolve(fmt_, 0, args...);

    return fmt_;
}

FORCEDINLINE CString cStringFormat(cstring fmt)
{
    return fmt;
}

template<typename... Arg>
FORCEDINLINE CString fmt(cstring fmt, Arg... arg)
{
    return cStringFormat(fmt, arg...);
}

}
}

#endif
