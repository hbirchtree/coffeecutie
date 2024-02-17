#pragma once

#include <coffee/asio/asio_data.h>
#include <coffee/core/internal_state.h>
#include <coffee/core/task_queue/task.h>

namespace Coffee::ASIO {

#if !defined(USE_EMSCRIPTEN_HTTP)
static const constexpr cstring context_name = "asioContext";

struct Worker : State::GlobalState
{
    virtual ~Worker();

    std::shared_ptr<ASIO::Service> context;

    void stop();
};

STATICINLINE std::shared_ptr<Worker> GenWorker()
{
    auto worker =
        std::dynamic_pointer_cast<Worker>(State::PeekState(context_name));
    if(worker)
        return worker;

    worker          = std::make_shared<Worker>();
    worker->context = ASIO::InitService();

    State::SwapState(context_name, worker);

    return worker;
}

STATICINLINE std::shared_ptr<ASIO::Service> GetContext()
{
    auto ptr = State::PeekState(context_name);
    if(!ptr)
        return {};

    Worker* worker = C_DCAST<Worker>(ptr.get());

    return worker->context;
}
#else
struct Worker
{
    int context{0};

    void stop()
    {
    }
};

STATICINLINE Worker* GenWorker()
{
    static Worker worker{};
    return &worker;
}

STATICINLINE int GetContext()
{
    return 0;
}
#endif

} // namespace Coffee::ASIO
