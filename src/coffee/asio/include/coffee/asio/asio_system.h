#pragma once

#include <coffee/asio/asio_worker.h>
#include <coffee/components/components.h>

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
        get().worker->stop();
    }
};

} // namespace ASIO
} // namespace Coffee
