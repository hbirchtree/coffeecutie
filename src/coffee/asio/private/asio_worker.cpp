#include <coffee/asio/asio_worker.h>

namespace Coffee {
namespace ASIO {

void ASIO_Worker::stop()
{
    State::SwapState(context_name, {});

    runtime_queue_error ec;

    if(context)
    {
        context->service.stop();

        RuntimeQueue::AwaitTask(worker_queue->threadId(), runner_task, ec);

        context.reset();
    }

    if(worker_queue)
    {
        RuntimeQueue::TerminateThread(worker_queue, ec);
        worker_queue = nullptr;
    }
}

Coffee::ASIO::ASIO_Worker::~ASIO_Worker()
{
    stop();
}

} // namespace ASIO
} // namespace Coffee
