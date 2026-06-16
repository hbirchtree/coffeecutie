#pragma once

#include <coffee/core/task_queue/task.h>
#include <memory>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <string>
#include <url/url.h>
#include <vector>

struct curl_slist;

namespace net {

using libc_types::u8;
using platform::url::Url;

struct curl_data;
struct curl_request_data;

using curl_context = std::shared_ptr<curl_data>;
using curl_request = std::shared_ptr<curl_request_data>;

enum class queue_behavior
{
    none,
    queue_self,
};

struct curl_data : std::enable_shared_from_this<curl_data>
{
    using poll_interval_t = std::chrono::steady_clock::duration;

    curl_data(queue_behavior behavior, poll_interval_t queue_interval);
    ~curl_data();

    void* context{nullptr};

    struct stats_t
    {
        libc_types::u64 transmitted{0};
        libc_types::u64 received{0};
        libc_types::u64 connections{0};
        libc_types::u64 sockets_opened{0};
    } stats;

#if defined(USE_CURL)
    std::vector<curl_request> queued_requests{};
    std::vector<curl_request> finished_requests{};
#endif

    using download_task_t = std::unique_ptr<
        rq::dependent_task<void, semantic::mem_chunk<const u8>>>;
    using dependent_download_task_t =
        std::unique_ptr<rq::dependent_task<Url, semantic::mem_chunk<const u8>>>;

    download_task_t           create_download(Url const& source);
    dependent_download_task_t create_download(std::future<Url>&& source);

#if defined(USE_CURL)
    bool process(curl_request awaitable = {});
    void run(
        poll_interval_t interval  = std::chrono::milliseconds(10),
        curl_request    awaitable = {});

    std::future<void> add_request(
        curl_request                     request,
        std::function<void()>&&          completion_handler = {},
        std::function<void(int error)>&& error_handler      = {});
    void remove_request(curl_request request);

    void await_request(curl_request request);
#endif
};

struct curl_request_data : std::enable_shared_from_this<curl_request_data>
{
    curl_request_data();
    ~curl_request_data();

    void*                          handle{nullptr};
    struct curl_slist*             headers{nullptr};
    std::vector<std::string>       header_strings{};
    std::vector<char>              payload{};
    std::promise<void>             completion{};
    std::function<void()>          completion_handler{};
    std::function<void(int error)> error_handler{};
    bool                           active{false};

    struct stats_t
    {
        std::chrono::milliseconds queue{};
        std::chrono::milliseconds name_lookup{};
        std::chrono::milliseconds connect{};
        std::chrono::milliseconds transfer{};
        std::chrono::milliseconds total{};
    } stats{};
};

curl_context create_curl_context(
    queue_behavior             behavior       = queue_behavior::none,
    curl_data::poll_interval_t queue_interval = std::chrono::milliseconds(10));

} // namespace net
