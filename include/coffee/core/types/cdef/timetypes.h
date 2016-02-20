#pragma once

#include <ctime>
#include "../tdef/integertypes.h"

namespace Coffee{

using Timestamp = uint64;

using DateTime = tm;

template<typename T> class _cbasic_timer
{
public:
    virtual ~_cbasic_timer(){}
    virtual void start() = 0;
    virtual T elapsed() = 0;
};

}