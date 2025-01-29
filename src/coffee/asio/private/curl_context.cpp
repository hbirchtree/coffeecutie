#include <coffee/net/curl_context.h>

#include <coffee/core/debug/formatting.h>
#include <coffee/net/net_resource.h>
#include <future>

#if defined(USE_CURL)
#include <curl/curl.h>
#endif

namespace net {

curl_data::curl_data()
#if defined(USE_CURL)
    : context(curl_multi_init())
#endif
{
}

curl_data::~curl_data()
{
#if defined(USE_CURL)
    curl_multi_cleanup(context);
#endif
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

curl_context create_curl_context()
{
    static curl_context context;
    if(!context)
        context = std::make_shared<curl_data>();
    return context;
}

} // namespace net
