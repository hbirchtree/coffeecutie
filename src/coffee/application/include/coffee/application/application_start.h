#pragma once

#include <coffee/core/argument_handling.h>
#include <coffee/core/internal_state.h>
#include <coffee/core/libc_types.h>
#include <coffee/core/resource_prefix.h>
#include <coffee/core/types/application_main.h>
#include <peripherals/base.h>
#include <platforms/argument_parse.h>

#if defined(COFFEE_LOWFAT)
#include <coffee/core/task_queue/task.h>
#endif

#if(defined(COFFEE_ANDROID) && defined(COFFEE_USE_SDL2))
#include <jni.h>
#endif

#if defined(COFFEE_SDL_MAIN)
#define main SDL_main
#elif defined(COFFEE_CUSTOM_MAIN)
#include <coffee/foreign/foreign.h>

#define main Coffee_main

/* bootstrapping class, for handing main function pointer over to loader */
struct app_main_init
{
    app_main_init(CoffeeMainWithArgs main_)
    {
        coffee_main_function_ptr = main_;
    }
};

#if defined(COFFEE_ANDROID)
extern CoffeeMainWithArgs android_entry_point;
#endif

#endif

namespace Coffee {

COFFEE_APP_FUNC void SetPlatformState();

}

extern int deref_main(
    CoffeeMainWithArgs mainfun, int argc, char** argv, Coffee::u32 flags = 0);

// This is a cheeky little macro that allows us to wrap the main function.
#if defined(COFFEE_LOWFAT)

#define COFFEE_APPLICATION_MAIN(mainfun)                              \
    int main(int argc, char** argv)                                   \
    {                                                                 \
        using namespace Coffee;                                       \
        RuntimeQueue::SetQueueContext(RuntimeQueue::CreateContext()); \
        State::SetInternalState(State::CreateNewState());             \
        SetPlatformState();                                           \
        GetInitArgs() = ::platform::args::AppArg::Clone(argc, argv);  \
        if(argc >= 2)                                                 \
        {                                                             \
            platform::file::ResourcePrefix(argv[1]);                  \
            GetInitArgs().m_ptrStorage.erase(                         \
                GetInitArgs().m_ptrStorage.begin());                  \
        }                                                             \
                                                                      \
        return mainfun(argc, argv);                                   \
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
    static app_main_init _coffee_application_main_init(mainfun);

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

#define COFFEE_SIMPLE_MAIN(mainfun)                       \
    namespace {                                           \
    int simplified_main(int, char**)                      \
    {                                                     \
        mainfun(0, nullptr);                              \
    }                                                     \
    }                                                     \
    int main(int argv, char** argc)                       \
    {                                                     \
        return deref_main(::simplified_main, argv, argc); \
    }

#endif
