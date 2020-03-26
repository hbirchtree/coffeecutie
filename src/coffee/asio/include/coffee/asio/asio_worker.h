#pragma once

#include <coffee/asio/asio_data.h>
#include <coffee/core/internal_state.h>
#include <coffee/core/task_queue/task.h>

namespace Coffee {
namespace ASIO {

static const constexpr cstring context_name = "asioContext";

struct Worker : State::GlobalState
{
    virtual ~Worker();

    ShPtr<ASIO::Service> context;

    void stop();
};

STATICINLINE ShPtr<Worker> GenWorker()
{
    if(State::PeekState(context_name).get())
        return {};

    auto worker     = MkShared<Worker>();
    worker->context = ASIO::InitService();

    State::SwapState(context_name, worker);

    return worker;
}

STATICINLINE ShPtr<ASIO::Service> GetContext()
{
    auto ptr = State::PeekState(context_name);
    if(!ptr)
        return {};

    Worker* worker = C_DCAST<Worker>(ptr.get());

    return worker->context;
}

} // namespace ASIO
} // namespace Coffee
