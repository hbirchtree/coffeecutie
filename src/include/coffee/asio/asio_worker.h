#pragma once

#include <coffee/asio/asio_data.h>
#include <coffee/core/internal_state.h>
#include <coffee/core/task_queue/task.h>

namespace Coffee {
namespace ASIO {

static const constexpr cstring context_name = "asioContext";

struct ASIO_Worker : State::GlobalState
{
    virtual ~ASIO_Worker();

    RuntimeQueue* worker_queue;
    asio_context  context;
    u64           runner_task;

    void stop();
};

STATICINLINE void RunWorker()
{
    auto ptr = State::PeekState(context_name);

    if(!ptr.get())
        Throw(implementation_error("asio state not set"));

    ASIO_Worker* worker = C_DCAST<ASIO_Worker>(ptr.get());

    auto work = asio::make_work_guard(worker->context->service);
    worker->context->service.run();
}

STATICINLINE ShPtr<ASIO_Worker> GenWorker()
{
    if(State::PeekState(context_name).get())
        return {};

    runtime_queue_error ec;
    auto queue = RuntimeQueue::CreateNewThreadQueue("::asio net", ec);

    C_ERROR_CHECK(ec);

    auto worker          = MkShared<ASIO_Worker>();
    worker->worker_queue = queue;
    worker->context      = service::InitService();

    State::SwapState(context_name, worker);

    auto taskId = RuntimeQueue::QueueImmediate(
        queue, Chrono::milliseconds(0), RunWorker, ec);

    worker->runner_task = taskId;

    return worker;
}

STATICINLINE asio_context GetContext()
{
    auto ptr = State::PeekState(context_name);
    if(!ptr)
        return {};

    ASIO_Worker* worker = C_DCAST<ASIO_Worker>(ptr.get());

    return worker->context;
}

} // namespace ASIO
} // namespace Coffee
