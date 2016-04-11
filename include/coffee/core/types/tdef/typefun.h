#pragma once

#include "../../plat/plat_quirks_toggling.h"

#include "integertypes.h"
#if defined(COFFEE_USE_RTTI)
#include <typeinfo>
#endif

namespace Coffee{

template<typename T>
cstring ctypename(const T& t)
{
#if !defined(COFFEE_USE_RTTI)
    return "(undefined)";
#else
    return typeid(t).name();
#endif
}
template<typename T>
int64 ctypehash(const T& t)
{
#if !defined(COFFEE_USE_RTTI)
    return 0;
#else
    return typeid(t).hash_code();
#endif
}

}
