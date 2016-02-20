#pragma once

#include "integertypes.h"
#include <typeinfo>

namespace Coffee{

template<typename T>
cstring ctypename(const T& t)
{
    return typeid(t).name();
}
template<typename T>
int64 ctypehash(const T& t)
{
    return typeid(t).hash_code();
}

}