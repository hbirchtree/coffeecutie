#pragma once

#include <peripherals/identify/system.h>

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten.h>
#else
#include <csignal>
#include <stdlib.h>
#endif

FORCEDINLINE void C_BREAK()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
    if(getenv("DEBUG_BREAK"))
        std::raise(SIGINT);
#elif defined(COFFEE_EMSCRIPTEN)
    emscripten_debugger();
#endif
}

