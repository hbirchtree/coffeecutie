#pragma once

namespace comp_app {

struct AppEvent
{
    enum Type
    {
        Back,
    };

    Type type;
};

}
