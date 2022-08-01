#pragma once

#include <coffee/asio/asio_worker.h>
#include <coffee/asio/net_resource.h>
#include <coffee/components/components.h>
#include <coffee/core/task_queue/task.h>

namespace Coffee {
namespace ASIO {

struct Subsystem : Components::SubsystemBase
{
    using type = Subsystem;

    Subsystem()
    {
        m_worker = GenWorker();
    }

    ShPtr<ASIO::Service> context()
    {
        return m_worker->context;
    }

    void stop()
    {
        m_worker->stop();
    }

    auto create_download(std::future<Url>&& source)
    {
        return rq::dependent_task<Url, mem_chunk<u8>>::CreateTask(
            std::move(source),
            [this](Url* source) {
                auto rsc = Net::Resource(this->context(), *source);
                if (!rsc.fetch())
                    return mem_chunk<u8>();
                return rsc.move();
            });
    }

    auto create_download(Url const& source)
    {
        std::promise<Url> source_promise;
        source_promise.set_value(source);
        return create_download(source_promise.get_future());
    }

    ShPtr<Worker> m_worker;
};

} // namespace ASIO
} // namespace Coffee
