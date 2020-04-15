#pragma once

namespace comp_app {

struct AppEvent
{
    enum Type
    {
        None,
        LifecycleEvent,
        NavigationEvent,
    };

    Type type;
};

struct NavigationEvent
{
    enum Type
    {
        None,
        Back = 1,
    };

    Type navigation_type;
};

struct LifecycleEvent
{
    enum Type
    {
        None,

        /* Transition events */
        WillEnterBackground = 1,
        WillEnterForeground,

        /* Core events */
        Background,
        Foreground,
        Terminate,

        /* Special events */
        LowMemory,
    };

    Type lifecycle_type;
};

}
