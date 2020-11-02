#pragma once

#include <peripherals/libc/types.h>

#if defined(COFFEE_GEKKO)
using MainWithArgs = int (*)(int, char**);
using MainNoArgs   = int (*)();
#else
using MainWithArgs = libc_types::i32 (*)(libc_types::i32, char**);
using MainNoArgs   = libc_types::i32 (*)();
#endif

namespace Coffee {

struct MainStorage
{
    union
    {
        MainWithArgs with_args;
        MainNoArgs   no_args;
    };
    bool           is_no_args;
    libc_types::u8 _padding;
};

extern MainStorage main_functions;

extern void CaptureMainFunction(MainWithArgs mainfun);
extern void CaptureMainFunction(MainNoArgs mainfun);

struct MainCapture
{
    MainCapture(MainWithArgs mainfun)
    {
        CaptureMainFunction(mainfun);
    }
    MainCapture(MainNoArgs mainfun)
    {
        CaptureMainFunction(mainfun);
    }
};

} // namespace Coffee
