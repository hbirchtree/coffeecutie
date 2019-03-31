#include <coffee/asio/asio_worker.h>

#include <coffee/core/CProfiling>

namespace Coffee {
namespace ASIO {

void ASIO_Worker::stop()
{
    DProfContext _("ASIO_Worker::stop");

    State::SwapState(context_name, {});

    runtime_queue_error ec;

    if(context)
    {
        DProfContext _("ASIO_Worker::Awaiting task");
        context->service.stop();

        RuntimeQueue::AwaitTask(worker_queue->threadId(), runner_task, ec);
        C_ERROR_CHECK(ec);

        context.reset();
    }

    if(worker_queue)
    {
        DProfContext _("ASIO_Worker::Terminating thread");
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
