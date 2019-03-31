#pragma once

#include <coffee/asio/asio_worker.h>
#include <coffee/components/types.h>

namespace Coffee {
namespace ASIO {
namespace detail {
struct WorkerData
{
    ShPtr<ASIO_Worker> worker;
};
} // namespace detail

using Tag = Components::TagType<detail::WorkerData>;

struct Subsystem : Components::Globals::ValueSubsystem<Tag>
{
    Subsystem()
    {
        get().worker = GenWorker();
    }

    asio_context& context()
    {
        return get().worker->context;
    }

    void stop()
    {
        get().worker->context->service.stop();

        runtime_queue_error ec;
        RuntimeQueue::TerminateThread(get().worker->worker_queue, ec);

        C_ERROR_CHECK(ec);
    }
};

} // namespace ASIO
} // namespace Coffee
