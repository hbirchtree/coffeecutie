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
    static void Init(int& argc, char** argv);
    static void Deinit();

    static void Process(int timeout = 20);
};

}