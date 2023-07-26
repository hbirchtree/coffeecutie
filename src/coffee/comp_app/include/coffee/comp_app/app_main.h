#pragma once

#include <coffee/components/components.h>
#include <coffee/comp_app/bundle.h>
#include <coffee/core/task_queue/task.h>

#include "services.h"

namespace comp_app {

struct AppMain : AppLoadableService
{
    using type = AppMain;
};

template<typename BundleData>
struct ExecLoop
{
    static int exec(detail::EntityContainer& container)
    {
//        rq::runtime_queue* queue = nullptr;
        if(auto r = rq::runtime_queue::CreateNewQueue(
                        platform::state->GetAppData()->application_name);
           r.has_value())
           ;
//            queue = r.value();
        else
            Throw(std::move(r.error()));

#if defined(COFFEE_EMSCRIPTEN)
        emscripten_set_main_loop(BundleData::EmscriptenLoop, -1, 1);
#elif !defined(COFFEE_CUSTOM_EXIT_HANDLING)
        app_error                   appec;

        comp_app::setup_container(container);

        while(comp_app::loop_container(container));

        comp_app::cleanup_container(container);
#endif
        return 0;
    }
};

}
