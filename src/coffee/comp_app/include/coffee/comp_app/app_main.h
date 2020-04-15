#pragma once

#include <coffee/components/components.h>
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
        Coffee::runtime_queue_error ec;

        if(!Coffee::RuntimeQueue::GetCurrentQueue(ec))
            Coffee::RuntimeQueue::CreateNewQueue(
                        platform::state->GetAppData()->application_name);

        ec.clear();

#if defined(COFFEE_EMSCRIPTEN)
        emscripten_set_main_loop(BundleData::EmscriptenLoop, -1, 1);
#elif !defined(COFFEE_CUSTOM_EXIT_HANDLING)
        app_error                   appec;

        auto queue = Coffee::RuntimeQueue::GetCurrentQueue(ec);
        C_ERROR_CHECK(ec);

        container.service<AppMain>()->load(container, appec);

        while(!container.service<Windowing>()->notifiedClose())
        {
            container.exec();

            if(queue)
                queue->executeTasks();
        }

        auto services = container.services_with<AppLoadableService>(
            Coffee::Components::reverse_query);
        for(auto& service : services)
            service.unload(container, appec);
            /* TODO: Unload all services */
#endif
        return 0;
    }
};

}
