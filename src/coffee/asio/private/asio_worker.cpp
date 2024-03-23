#include <coffee/asio/asio_worker.h>

#include <coffee/core/CProfiling>

namespace Coffee::ASIO {

std::shared_ptr<ASIO::Service> global_service;

void Worker::stop()
{
    DProfContext _("ASIO_Worker::stop");

    return;

    auto self = State::SwapState(context_name, {});

#if !defined(USE_EMSCRIPTEN_HTTP)
    if(context)
    {
        DProfContext _("ASIO_Worker::Awaiting task");
        context->resolver.cancel();
        context->resolver_udp.cancel();

        context->service.stop();
        context->service.restart();

        context.reset();
    }
#endif
}

Coffee::ASIO::Worker::~Worker()
{
    stop();
}

} // namespace Coffee::ASIO
