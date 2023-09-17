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

    std::shared_ptr<ASIO::Service> context()
    {
        return m_worker->context;
    }

    void stop()
    {
        m_worker->stop();
    }

    auto create_download(std::future<Url>&& source)
    {
        return rq::dependent_task<Url, semantic::mem_chunk<const u8>>::
            CreateProcessor(std::move(source), [this](Url* source) {
                auto rsc = net::Resource(this->context(), *source);
                if(auto error = rsc.fetch())
                    return semantic::mem_chunk<const u8>();
                return rsc.move_const().value_or(
                    semantic::mem_chunk<const u8>());
            });
    }

    auto create_download(Url const& source)
    {
        return rq::dependent_task<void, semantic::mem_chunk<const u8>>::
            CreateSource([this, source]() {
                auto rsc = net::Resource(this->context(), source);
                if(auto error = rsc.fetch())
                    return semantic::mem_chunk<const u8>();
                return rsc.move_const().value_or(
                    semantic::mem_chunk<const u8>());
            });
    }

    std::shared_ptr<Worker> m_worker;
};

} // namespace ASIO
} // namespace Coffee
