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
    static constexpr auto event_type = AppEvent::NavigationEvent;

    enum Type
    {
        None,
        Back = 1,
    };

    Type navigation_type;
};

struct LifecycleEvent
{
    static constexpr auto event_type = AppEvent::LifecycleEvent;

    enum Type
    {
        None,

        /* Transition events */
        WillEnterBackground = 1,
        WillEnterForeground,

        /* Core events */
        Startup,
        Background,
        FrameRequested,
        Foreground,
        Terminate,

        /* Resource hints */
        ResourcesLost, /* on Android, we might lose the window in some cases*/
        LowMemory,
    };

    Type lifecycle_type;
};

}
