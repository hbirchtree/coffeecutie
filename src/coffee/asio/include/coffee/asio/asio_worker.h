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

    RuntimeQueue*        worker_queue;
    ShPtr<ASIO::Service> context;
    u64                  runner_task;

    void stop();
};

STATICINLINE void RunWorker()
{
    auto ptr = State::PeekState(context_name);

    if(!ptr.get())
        Throw(implementation_error("asio state not set"));

    ASIO_Worker* worker = C_DCAST<ASIO_Worker>(ptr.get());

    auto work = asio::make_work_guard(worker->context->service);

    runtime_queue_error ec;

    while(RuntimeQueue::IsRunning(worker->worker_queue, ec))
    {
        C_ERROR_CHECK(ec)

        asio::error_code asio_ec;
        worker->context->service.run_for(Chrono::milliseconds(5));

        C_ERROR_CHECK(asio_ec)
    }
}

STATICINLINE ShPtr<ASIO_Worker> GenWorker()
{
    if(State::PeekState(context_name).get())
        return {};

    auto worker     = MkShared<ASIO_Worker>();
    worker->context = ASIO::InitService();

    runtime_queue_error ec;
    auto queue = RuntimeQueue::CreateNewThreadQueue("::asio net", ec);
    C_ERROR_CHECK(ec);

    worker->worker_queue = queue;

    State::SwapState(context_name, worker);

    auto taskId = RuntimeQueue::QueueImmediate(
        queue, Chrono::milliseconds(0), RunWorker, ec);

    C_ERROR_CHECK(ec);

    worker->runner_task = taskId;

    return worker;
}

STATICINLINE ShPtr<ASIO::Service> GetContext()
{
    auto ptr = State::PeekState(context_name);
    if(!ptr)
        return {};

    ASIO_Worker* worker = C_DCAST<ASIO_Worker>(ptr.get());

    return worker->context;
}

} // namespace ASIO
} // namespace Coffee
