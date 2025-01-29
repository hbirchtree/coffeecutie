#include <coffee/net/net_resource.h>

#include <coffee/core/CProfiling>
#include <peripherals/enum/helpers.h>
#include <peripherals/stl/string_casting.h>

#include <coffee/core/CDebug>

#define NETRSC_TAG "NetRsc::"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace ::semantic;
using namespace Coffee::Logging;
using Coffee::DProfContext;
using Coffee::Profiler;
using platform::url::UrlParse;

namespace net {

using namespace ::enum_helpers;

void Resource::initRsc(const Url& url)
{
    DProfContext a(DTEXT(NETRSC_TAG "Initializing NetResource"));

    Profiler::DeepPushContext(DTEXT(NETRSC_TAG "Extracting URL components"));
    auto [protocol, host, resource, port] = UrlParse::from(url);
    if(host.empty())
    {
        cVerbose(10, "Failed to decode URL");
        Profiler::DeepProfile(DTEXT(NETRSC_TAG "Failed to decode URL"));
        return;
    }
    Profiler::DeepPopContext();

    if(port == 0)
    {
        if(protocol == "https")
        {
            port = 443;
            m_access |= HTTPAccess::Secure;
        } else if(protocol == "http")
            port = 80;
    }

    //    if((secure() && protocol == "https") || (!secure() && protocol ==
    //    "http"))
    //    {
    //        m_request.header.resource = "/" + resource;
    //        return;
    //    } else
    //        close();

    m_request.host            = host;
    m_request.header.resource = "/" + resource;
    m_request.header.version  = http::version_t::v11;
    m_request.port            = port;

    //    const auto verify_https = !feval(m_access, HTTPAccess::NoVerify);

    m_error = error_code();

#if defined(ASIO_USE_SSL)
    if(protocol == "http" && secure())
    {
        cVerbose(10, "Switching off SSL, protocol mismatch");
        m_access = m_access & (m_access ^ HTTPAccess::Secure);
    }
#endif

#if !defined(USE_EMSCRIPTEN_HTTP)
    m_handle = std::make_shared<curl_request_data>();
    {
        auto full_url = *url;
        if(auto err = curl_easy_setopt(
               m_handle->handle, CURLOPT_URL, full_url.c_str());
           err != CURLE_OK)
        {
            cWarning("Failed setting URL");
        }
    }
//     {
//         DProfContext b(DTEXT(NETRSC_TAG "Connecting to host"));
// #if defined(ASIO_USE_SSL)
//         if(secure())
//         {
//             ssl = std::make_unique<net::tcp::ssl_socket>(std::ref(*m_ctxt));

//             m_error = ssl->connect(m_request.host, m_request.port);

//         } else
// #endif
//         {
//             normal =
//             std::make_unique<net::tcp::raw_socket>(std::ref(*m_ctxt));

//             m_error = normal->connect(m_request.host, m_request.port);
//         }
//         C_ERROR_CHECK_TYPED(m_error, net_error)
//     }
#endif

    if(!connected())
    {
        Profiler::DeepProfile(NETRSC_TAG "Failed to connect");
        return;
    }

    http::header::transform::create_request(m_request);

    m_response = {};
}

std::optional<error_code> Resource::close()
{
    error_code ec;
// #if defined(ASIO_USE_SSL)
//     if(secure())
//     {
//         if(!ssl)
//             return std::nullopt;

//         ec = ssl->disconnect();
//         ssl.release();
//     } else
// #endif
#if defined(USE_EMSCRIPTEN_HTTP)
    emscripten_fetch_close(m_fetch);
#else
    if(m_handle->active)
    {
        curl_multi_remove_handle(m_ctxt->context, m_handle->handle);
    }
    // {
    //     if(!normal)
    //         return std::nullopt;

    //     ec = normal->disconnect();
    //     normal.release();
    // }
#endif

    return ec ? std::make_optional(ec) : std::nullopt;
}

Resource::Resource(resource_context ctxt, const Url& url)
    : m_resource(url)
#if !defined(USE_EMSCRIPTEN_HTTP)
    , m_ctxt(ctxt)
#endif
    , m_access(url.netflags)
{
    using namespace http::header::to_string;
    using namespace http::header;

    using field   = http::header_field;
    using content = http::content_type;

    transform::create_request(m_request);

    auto& fields = m_request.header.standard_fields;

    fields[field::accept] = content_type(content::any);

    initRsc(url);
}

Resource::~Resource()
{
    close();
}

bool Resource::secure() const
{
    return (m_access & HTTPAccess::Secure) != HTTPAccess::None;
}

bool Resource::connected() const
{
    return !m_error;
}

error_code Resource::connectError() const
{
    return m_error;
}

Url Resource::resource() const
{
    return m_resource;
}

bool Resource::isRequestReady() const
{
    return http::validate::request(m_request) == http::validate::result::valid;
}

bool Resource::isResponseReady() const
{
    return m_response.header.code != 0;
}

void Resource::setHeaderField(
    http::header_field field, const std::string& value)
{
    m_request.header.standard_fields[field] = value;
}

void Resource::setHeaderField(
    const std::string& field, const std::string& value)
{
    m_request.header.fields[field] = value;
}

http::request_t& Resource::request()
{
    return m_request;
}

std::optional<error_code> Resource::fetch()
{
    return push(http::method_t::get, const_chunk_u8());
}

std::optional<error_code> Resource::push(const const_chunk_u8& data)
{
    using method_t = http::method_t;

    method_t meth = method_t::get;
    switch(m_access & HTTPAccess::RequestMask)
    {
    case HTTPAccess::POST:
        meth = method_t::post;
        break;
    case HTTPAccess::PUT:
        meth = method_t::put;
        break;
    case HTTPAccess::UPDATE:
        meth = method_t::update;
        break;
    case HTTPAccess::DELETE:
        meth = method_t::delet;
        break;
    case HTTPAccess::PATCH:
        meth = method_t::patch;
        break;
    case HTTPAccess::HEAD:
        meth = method_t::head;
        break;
    default:
        break;
    }
    return push(meth, data);
}

#if defined(USE_EMSCRIPTEN_HTTP)
void emscripten_push_success(emscripten_fetch_t* fetch)
{
    cDebug(
        "Request finished: {}: code={}: {}",
        fetch->url,
        fetch->status,
        fetch->statusText);
}

void emscripten_push_error(emscripten_fetch_t* fetch)
{
    cWarning(
        "Error in emscripten_fetch: code={}: {}",
        fetch->status,
        fetch->statusText);
}

void emscripten_push_statechange(emscripten_fetch_t* fetch)
{
    const char* status = nullptr;
    switch(fetch->readyState)
    {
    case 0:
        status = "unsent";
        break;
    case 1:
        status = "opened";
        break;
    case 2:
        status = "headers_received";
        break;
    case 3:
        status = "loading";
        break;
    case 4:
        status = "done";
        break;
    default:
        return;
    }
    cDebug("emscripten_fetch: url={}, state={}", fetch->url, status);
}

std::optional<error_code> Resource::push(
    http::method_t method, const_chunk_u8 const& data)
{
    std::string resource, param;
    /* We have to copy the body if it exists, otherwwise it might go out of
     * scope outside */
    m_request.payload.insert(
        m_request.payload.begin(), data.begin(), data.end());
    if(auto idx = m_request.header.resource.find('?'); idx == std::string::npos)
    {
        resource = m_request.header.resource.substr(0, idx);
        param    = m_request.header.resource.substr(idx + 1);
    } else
        resource = m_request.header.resource;
    auto url = fmt::format(
        "https://{}:{}/{}", m_request.host, m_request.port, resource);
    auto method_s = http::header::to_string::method(method);

    std::vector<const char*> headers;
    for(auto const& field : m_request.header.fields)
    {
        headers.push_back(field.first.c_str());
        headers.push_back(field.second.c_str());
    }
    for(auto const& field : m_request.header.standard_fields)
    {
        headers.push_back(http::header::to_string::field(field.first));
        headers.push_back(field.second.c_str());
    }
    headers.push_back(nullptr);

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    strcpy(attr.requestMethod, method_s);
    attr.requestHeaders = headers.data();
    attr.userData       = this;
    if(!m_request.payload.empty())
    {
        attr.requestData =
            reinterpret_cast<const char*>(m_request.payload.data());
        attr.requestDataSize = m_request.payload.size();
    }
    attr.onsuccess          = emscripten_push_success;
    attr.onerror            = emscripten_push_error;
    attr.onreadystatechange = emscripten_push_statechange;

    m_fetch = emscripten_fetch(&attr, url.c_str());

    if(!m_fetch)
        return error_code::request_failed;
    return std::nullopt;
}
#elif defined(USE_CURL)

std::future<error_code> pushAsync(
    http::method_t method, const_chunk_u8 const& data)
{
    return {};
}

namespace {

size_t write_payload(void* data, size_t size, size_t nmemb, void* ptr)
{
    size_t num_bytes = size * nmemb;
    auto*  request   = reinterpret_cast<curl_request_data*>(ptr);
    auto input_view = gsl::span<char>(reinterpret_cast<char*>(data), num_bytes);
    auto& payload   = request->payload;
    payload.insert(payload.end(), input_view.begin(), input_view.end());
    return num_bytes;
}

} // namespace

std::optional<error_code> Resource::push(
    http::method_t method, const_chunk_u8 const& data)
{
    using HA = HTTPAccess;

    DProfContext a(NETRSC_TAG "Sending HTTP request");

    curl_easy_setopt(m_handle->handle, CURLOPT_FAILONERROR, 1);
    if((m_access & HA::NoRedirect) != HA::NoRedirect)
        curl_easy_setopt(m_handle->handle, CURLOPT_FOLLOWLOCATION, 1);

    switch(method)
    {
    case http::method_t::get:
        curl_easy_setopt(m_handle->handle, CURLOPT_HTTPGET, 1);
        break;
    case http::method_t::put:
    case http::method_t::post:
        curl_easy_setopt(m_handle->handle, CURLOPT_POST, 1);
        if(data.data)
        {
            curl_easy_setopt(m_handle->handle, CURLOPT_POSTFIELDS, data.data);
            curl_easy_setopt(
                m_handle->handle, CURLOPT_POSTFIELDSIZE_LARGE, data.size);
        }
        break;
    case http::method_t::head:
        curl_easy_setopt(m_handle->handle, CURLOPT_CUSTOMREQUEST, "HEAD");
        curl_easy_setopt(m_handle->handle, CURLOPT_NOBODY, 1);
        break;
    default: {
        curl_easy_setopt(
            m_handle->handle,
            CURLOPT_CUSTOMREQUEST,
            http::header::to_string::method(method));
        break;
    }
    }

    auto add_header = [this](std::string_view field, std::string_view value) {
        m_handle->header_strings.push_back(fmt::format("{}: {}", field, value));
    };
    for(auto const& field : m_request.header.standard_fields)
        add_header(http::header::to_string::field(field.first), field.second);
    for(auto const& field : m_request.header.fields)
        add_header(field.first, field.second);
    for(auto const& field : m_handle->header_strings)
        m_handle->headers = curl_slist_append(m_handle->headers, field.c_str());
    if(m_handle->header_strings.size() > 0)
        curl_easy_setopt(
            m_handle->handle, CURLOPT_HTTPHEADER, m_handle->headers);

    curl_easy_setopt(m_handle->handle, CURLOPT_WRITEFUNCTION, write_payload);
    curl_easy_setopt(m_handle->handle, CURLOPT_WRITEDATA, m_handle.get());

    if(auto ret = curl_easy_perform(m_handle->handle); ret != CURLE_OK)
    {
        Profiler::DeepProfile(NETRSC_TAG "Failed HTTP request");
        return error_code{
            .err_code = ret,
            .err_msg  = curl_easy_strerror(ret),
        };
    }

    long response_code{};
    curl_easy_getinfo(m_handle->handle, CURLINFO_RESPONSE_CODE, &response_code);
    m_response.header.code = static_cast<libc_types::u16>(response_code);

    struct curl_header* header{nullptr};
    do
    {
        header =
            curl_easy_nextheader(m_handle->handle, CURLH_HEADER, -1, header);
        if(!header)
            break;
        if(auto field = http::header::from_string::field(header->name);
           field != http::header_field::none)
            m_response.header.standard_fields[field] = header->value;
        else
            m_response.header.fields[header->name] = header->value;
    } while(true);

    m_response.payload = std::move(m_handle->payload);

    cVerbose(12, NETRSC_TAG "HTTP headers received({}):", m_request.host);
    for(auto const& header : m_response.header.standard_fields)
        cVerbose(
            12,
            NETRSC_TAG "  {}: {}",
            http::header::to_string::field(header.first),
            header.second);
    for(auto const& header : m_response.header.fields)
        cVerbose(12, NETRSC_TAG "  {}: {}", header.first, header.second);
    {
        auto content_type_ =
            m_response.header.standard_fields[http::header_field::content_type];
        auto content_type =
            http::header::from_string::content_type(content_type_);
        switch(content_type)
        {
        case http::content_type::json:
            cVerbose(
                12,
                NETRSC_TAG "HTTP payload(application/json): {}",
                nlohmann::json::parse(
                    std::string_view(
                        m_response.payload.data(), m_response.payload.size()),
                    nullptr,
                    false)
                    .dump(2));
            break;
        case http::content_type::html:
        case http::content_type::text:
        case http::content_type::xml:
            cVerbose(
                12,
                NETRSC_TAG "HTTP payload ({}):\n-------\n{}\n-------",
                content_type_,
                std::string_view(
                    &m_response.payload[0], m_response.payload.size()));
            break;
        default:
            break;
        }
    }

    auto&      stats = m_handle->stats;
    curl_off_t prev_time{}, time{};
    /* Collect timing info */
    curl_easy_getinfo(m_handle->handle, CURLINFO_QUEUE_TIME_T, &time);
    stats.queue = std::chrono::milliseconds(time / 1000);
    prev_time   = time;
    curl_easy_getinfo(m_handle->handle, CURLINFO_NAMELOOKUP_TIME_T, &time);
    stats.name_lookup = std::chrono::milliseconds((time - prev_time) / 1000);
    prev_time         = time;
    curl_easy_getinfo(m_handle->handle, CURLINFO_APPCONNECT_TIME_T, &time);
    stats.connect = std::chrono::milliseconds((time - prev_time) / 1000);
    prev_time     = time;
    curl_easy_getinfo(m_handle->handle, CURLINFO_STARTTRANSFER_TIME_T, &time);
    stats.transfer = std::chrono::milliseconds((time - prev_time) / 1000);
    prev_time      = time;
    curl_easy_getinfo(m_handle->handle, CURLINFO_TOTAL_TIME_T, &time);
    stats.total = std::chrono::milliseconds(time / 1000);

    long request_size{};
    long response_size{};
    long num_connects{};
    curl_easy_getinfo(m_handle->handle, CURLINFO_SIZE_UPLOAD_T, &request_size);
    curl_easy_getinfo(
        m_handle->handle, CURLINFO_SIZE_DOWNLOAD_T, &response_size);
    curl_easy_getinfo(m_handle->handle, CURLINFO_NUM_CONNECTS, &num_connects);
    m_ctxt->stats.connections += num_connects;
    m_ctxt->stats.received += response_size;
    m_ctxt->stats.transmitted += request_size;

    cVerbose(
        10,
        NETRSC_TAG
        "HTTP request timing: resolve/connect/transfer/total {}/{}/{}/{}",
        stats.name_lookup,
        stats.connect,
        stats.transfer,
        stats.total);

    return std::nullopt;
}

#endif

std::optional<http::response_t> Resource::response() const
{
    if(isResponseReady())
        return m_response;
    return std::nullopt;
}

std::optional<std::string> Resource::mimeType() const
{
    using field = http::header_field;

    auto const it = m_response.header.standard_fields.find(field::content_type);

    if(it == m_response.header.standard_fields.end())
        return std::nullopt;
    return it->second;
}

libc_types::u32 Resource::responseCode() const
{
    return m_response.header.code;
}

std::optional<Url> Resource::responseLocation() const
{
    using field = http::header_field;

    auto const it = m_response.header.standard_fields.find(field::location);

    if(it == m_response.header.standard_fields.end())
        return std::nullopt;
    if(it->second.find("://") == std::string::npos)
    {
        auto [protocol, host, resource, port] = UrlParse::from(m_resource);
        if(port != 0)
            host = fmt::format("{}:{}", host, port);
        return MkUrl(fmt::format("{}://{}{}", protocol, host, it->second));
    } else
    {
        return MkUrl(it->second);
    }
}

std::optional<const_chunk_u8> Resource::data() const
{
    if(!isResponseReady())
        return std::nullopt;
    return const_chunk_u8::ofContainer(m_response.payload);
}

std::optional<chunk_u8> Resource::move()
{
    if(!isResponseReady())
        return std::nullopt;
    auto out = chunk_u8::move(std::move(m_response.payload));
    return out;
}

std::optional<const_chunk_u8> Resource::move_const() const
{
    if(!isResponseReady())
        return std::nullopt;
    auto out = const_chunk_u8::move(std::move(m_response.payload));
    return out;
}

} // namespace net
