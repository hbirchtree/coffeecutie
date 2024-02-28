#pragma once

[[noreturn]] inline void unreachable()
{
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else
    __builtin_unreachable();
#endif
}
