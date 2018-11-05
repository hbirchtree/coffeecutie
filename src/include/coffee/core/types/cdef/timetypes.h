#pragma once

#include <ctime>
#include <peripherals/libc/types.h>

namespace Coffee{

using Timestamp = u64;

using DateTime = tm;

template<typename T>
class _cbasic_timer
{
public:
    virtual ~_cbasic_timer(){}
    virtual void start() = 0;
    virtual T elapsed() = 0;
};

}
