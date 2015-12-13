#ifndef COFFEE_CORE_DEBUG_PRINT_H
#define COFFEE_CORE_DEBUG_PRINT_H

#include "coffee/core/types/basetypes.h"

namespace Coffee{
namespace CFunctional{

template<typename... Arg>
inline CString cStringFormat(cstring fmt, Arg... args)
{
    CString str = fmt;
//    str.resize(snprintf(NULL,0,fmt,args...)+1);
//    sprintf(&str[0],fmt,args...);
    str[str.size()-1] = 0;
    return str;
}

}
}

#endif
