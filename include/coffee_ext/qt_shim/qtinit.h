#pragma once

#include <coffee/core/types/composite_types.h>

namespace CoffeeExt{

struct QtSystem : Coffee::Subsystem
{
    STATICINLINE void Init()
    {
        int v = 0;
        Init(v,nullptr);
    }
    static bool Init(int& argc, char** argv);
    static bool Deinit();

    static bool Process(int timeout = 20);
};

}