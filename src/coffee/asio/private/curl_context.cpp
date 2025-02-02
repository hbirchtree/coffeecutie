#include <coffee/net/curl_context.h>

#include <coffee/core/debug/formatting.h>
#include <coffee/net/net_resource.h>
#include <future>

#if defined(USE_CURL)
#include <curl/curl.h>
#endif

namespace net {

curl_data::curl_data(queue_behavior behavior, poll_interval_t queue_interval)
#if defined(USE_CURL)
    : context(curl_multi_init())
#endif
{
    if(behavior != queue_behavior::queue_self)
        return;
    if(auto q = rq::runtime_queue::GetCurrentQueue(); q.has_value())
    {
        std::weak_ptr<curl_data> weak = shared_from_this();
        rq::runtime_queue::QueuePeriodic(q.value(), queue_interval, [weak] {
            if(auto ptr = weak.lock())
                ptr->process();
        }).assume_value();
    }
}

curl_data::~curl_data()
{
#if defined(USE_CURL)
    curl_multi_cleanup(context);
#endif
}

bool curl_data::process(curl_request awaitable)
{
    int       num_requests{};
    CURLMcode ecode{};
    if(queued_requests.empty())
        return true;
    ecode = curl_multi_perform(context, &num_requests);
    if(ecode != CURLM_OK)
        return true;
    int num_msgs{};
    while(auto msg = curl_multi_info_read(context, &num_msgs))
    {
        auto handle = msg->easy_handle;
        for(auto& req : queued_requests)
        {
            if(!req || req->handle != handle)
                continue;
            curl_multi_remove_handle(context, req->handle);
            req->completion.set_value();
            finished_requests.emplace_back(req);
            if(msg->data.result == CURLE_OK && req->completion_handler)
                req->completion_handler();
            if(msg->data.result != CURLE_OK && req->error_handler)
                req->error_handler(msg->data.result);
            if(req == awaitable)
            {
                req = {};
                return true;
            } else
                req = {};
        }
    }
    std::erase_if(queued_requests, [](curl_request& req) {
        return !static_cast<bool>(req);
    });
    return num_requests == 0;
}

void curl_data::run(
    std::chrono::steady_clock::duration interval, curl_request awaitable)
{
    do
    {
        if(process(awaitable))
            return;
        std::this_thread::sleep_for(interval);
    } while(true);
}

std::future<void> curl_data::add_request(
    curl_request               request,
    std::function<void()>&&    completion_handler,
    std::function<void(int)>&& error_handler)
{
    request->completion_handler = completion_handler;
    request->error_handler      = error_handler;
    queued_requests.push_back(request);
    request->completion = std::promise<void>();
    curl_multi_add_handle(context, request->handle);
    return request->completion.get_future();
}

void curl_data::remove_request(curl_request request)
{
    auto queued_count =
        std::erase_if(queued_requests, [&request](curl_request const& req) {
            return req == request;
        });
    if(queued_count > 0)
        curl_multi_remove_handle(context, request->handle);
    std::erase_if(finished_requests, [&request](curl_request const& req) {
        return req == request;
    });
}

curl_data::download_task_t curl_data::create_download(const Url& source)
{
    return rq::dependent_task<void, semantic::mem_chunk<const u8>>::
        CreateSource([this, source]() {
            auto rsc = net::Resource(create_curl_context(), source);
            if(auto error = rsc.fetch())
                return semantic::mem_chunk<const u8>();
            return rsc.move_const().value_or(semantic::mem_chunk<const u8>());
        });
}

curl_data::dependent_download_task_t curl_data::create_download(
    std::future<Url>&& source)
{
    return rq::dependent_task<Url, semantic::mem_chunk<const u8>>::
        CreateProcessor(std::move(source), [this](Url* source) {
            auto rsc = net::Resource(create_curl_context(), *source);
            if(auto error = rsc.fetch())
                return semantic::mem_chunk<const u8>();
            return rsc.move_const().value_or(semantic::mem_chunk<const u8>());
        });
}

curl_request_data::curl_request_data()
#if defined(USE_CURL)
    : handle(curl_easy_init())
#endif
{
}

curl_request_data::~curl_request_data()
{
#if defined(USE_CURL)
    curl_easy_cleanup(handle);
#endif
}

curl_context create_curl_context(queue_behavior behavior, curl_data::poll_interval_t queue_interval)
{
    static curl_context context;
    if(!context)
        context = std::make_shared<curl_data>(behavior, queue_interval);
    return context;
}

} // namespace net
