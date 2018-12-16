#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/types/application_main.h>
#include <peripherals/base.h>

#if defined(COFFEE_LOWFAT)
#include <coffee/core/task_queue/task.h>
#endif

#if(defined(COFFEE_ANDROID) && defined(COFFEE_USE_SDL2))
#include <jni.h>
#endif

#if defined(COFFEE_SDL_MAIN)
#define main SDL_main
#elif defined(COFFEE_CUSTOM_MAIN)
#define main Coffee_main

#if defined(COFFEE_APPLE_MOBILE)
extern CoffeeMainWithArgs apple_entry_point;
#elif defined(COFFEE_ANDROID)
extern CoffeeMainWithArgs android_entry_point;
#endif

#endif

extern int deref_main(
    CoffeeMainWithArgs mainfun, int argc, char** argv, Coffee::u32 flags = 0);

// This is a cheeky little macro that allows us to wrap the main function.
#if defined(COFFEE_LOWFAT)

#define COFFEE_APPLICATION_MAIN(mainfun)                              \
    int main(int argv, char** argc)                                   \
    {                                                                 \
        using namespace Coffee;                                       \
        RuntimeQueue::SetQueueContext(RuntimeQueue::CreateContext()); \
        State::SetInternalState(State::CreateNewState());             \
        return mainfun(argv, argc);                                   \
    }

#define COFFEE_APPLICATION_MAIN_CUSTOM_ARG(mainfun) \
    COFFEE_APPLICATION_MAIN(mainfun)

#define COFFEE_APPLICATION_MAIN_CUSTOM(mainfun, flags) \
    COFFEE_APPLICATION_MAIN(mainfun)

#elif defined(COFFEE_SDL_MAIN)

// Wraps it with a custom name defined above
#define COFFEE_APPLICATION_MAIN(mainfun)        \
    extern "C" int main(int argv, char** argc)  \
    {                                           \
        return deref_main(mainfun, argv, argc); \
    }

#define COFFEE_APPLICATION_MAIN_CUSTOM_ARG(mainfun) \
    COFFEE_APPLICATION_MAIN(mainfun)

#define COFFEE_APPLICATION_MAIN_CUSTOM(mainfun, flags) \
    COFFEE_APPLICATION_MAIN(mainfun)

#elif defined(COFFEE_CUSTOM_MAIN)

#if defined(COFFEE_APPLE_MOBILE)
// This is loaded from AppDelegate.m in CoffeeWindow_GLKit
#define COFFEE_APPLICATION_MAIN(mainfun) \
    CoffeeMainWithArgs apple_entry_point = mainfun;

#elif defined(COFFEE_ANDROID)
#define COFFEE_APPLICATION_MAIN(mainfun) \
    CoffeeMainWithArgs android_entry_point = mainfun;

#endif

#define COFFEE_APPLICATION_MAIN_CUSTOM_ARG(mainfun) \
    COFFEE_APPLICATION_MAIN(mainfun)

#define COFFEE_APPLICATION_MAIN_CUSTOM(mainfun, flags) \
    COFFEE_APPLICATION_MAIN(mainfun)

#else

// Plain old main() function
#define COFFEE_APPLICATION_MAIN(mainfun)        \
    int main(int argv, char** argc)             \
    {                                           \
        return deref_main(mainfun, argv, argc); \
    }

#define COFFEE_APPLICATION_MAIN_CUSTOM_ARG(mainfun)  \
    int main(int argv, char** argc)                  \
    {                                                \
        return deref_main(mainfun, argv, argc, 0x1); \
    }

#define COFFEE_APPLICATION_MAIN_CUSTOM(mainfun, flags) \
    int main(int argv, char** argc)                    \
    {                                                  \
        return deref_main(mainfun, argv, argc, flags); \
    }

#endif
