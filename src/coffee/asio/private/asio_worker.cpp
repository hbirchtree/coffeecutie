#include <coffee/asio/asio_worker.h>

#include <coffee/core/CProfiling>

namespace Coffee::ASIO {

#if !defined(USE_EMSCRIPTEN_HTTP)
std::shared_ptr<ASIO::Service> global_service;

void Worker::stop()
{
    DProfContext _("ASIO_Worker::stop");

    return;

    auto self = State::SwapState(context_name, {});

    if(context)
    {
        DProfContext _("ASIO_Worker::Awaiting task");
        context->resolver.cancel();
        context->resolver_udp.cancel();

        context->service.stop();
        context->service.restart();

        context.reset();
    }
}

Coffee::ASIO::Worker::~Worker()
{
    stop();
}
#endif

} // namespace Coffee::ASIO
