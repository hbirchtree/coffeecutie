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

struct curl_data : std::enable_shared_from_this<curl_data>
{
    curl_data();
    ~curl_data();

    void* context{nullptr};

    struct stats_t
    {
        libc_types::u64 transmitted{0};
        libc_types::u64 received{0};
        libc_types::u64 connections{0};
        libc_types::u64 sockets_opened{0};
    } stats;

    using download_task_t = std::unique_ptr<
        rq::dependent_task<void, semantic::mem_chunk<const u8>>>;
    using dependent_download_task_t =
        std::unique_ptr<rq::dependent_task<Url, semantic::mem_chunk<const u8>>>;

    download_task_t           create_download(Url const& source);
    dependent_download_task_t create_download(std::future<Url>&& source);
};

struct curl_request_data
{
    curl_request_data();
    ~curl_request_data();

    void*                    handle{nullptr};
    struct curl_slist*       headers{nullptr};
    std::vector<std::string> header_strings{};
    std::vector<char>        payload{};
    bool                     active{false};

    struct stats_t
    {
        std::chrono::milliseconds queue{};
        std::chrono::milliseconds name_lookup{};
        std::chrono::milliseconds connect{};
        std::chrono::milliseconds transfer{};
        std::chrono::milliseconds total{};
    } stats{};
};

using curl_context = std::shared_ptr<curl_data>;
using curl_request = std::shared_ptr<curl_request_data>;

curl_context create_curl_context();

} // namespace net
