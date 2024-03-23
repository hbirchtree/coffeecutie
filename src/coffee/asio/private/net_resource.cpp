#include <coffee/asio/net_resource.h>

#include <coffee/core/CProfiling>
#include <peripherals/enum/helpers.h>
#include <peripherals/stl/string_casting.h>

#include <coffee/core/CDebug>

#define NETRSC_TAG "NetRsc::"

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

    m_error = asio::error_code();

#if defined(ASIO_USE_SSL)
    if(protocol == "http" && secure())
    {
        cVerbose(10, "Switching off SSL, protocol mismatch");
        m_access = m_access & (m_access ^ HTTPAccess::Secure);
    }
#endif

#if !defined(USE_EMSCRIPTEN_HTTP)
    {
        DProfContext b(DTEXT(NETRSC_TAG "Connecting to host"));
#if defined(ASIO_USE_SSL)
        if(secure())
        {
            ssl = std::make_unique<net::tcp::ssl_socket>(std::ref(*m_ctxt));

            m_error = ssl->connect(m_request.host, m_request.port);

        } else
#endif
        {
            normal = std::make_unique<net::tcp::raw_socket>(std::ref(*m_ctxt));

            m_error = normal->connect(m_request.host, m_request.port);
        }
        C_ERROR_CHECK_TYPED(m_error, net_error)
    }
#endif

    if(!connected())
    {
        Profiler::DeepProfile(NETRSC_TAG "Failed to connect");
        return;
    }

    http::header::transform::create_request(m_request);

    m_response = {};
}

std::optional<asio::error_code> Resource::close()
{
    asio::error_code ec;
#if defined(ASIO_USE_SSL)
    if(secure())
    {
        if(!ssl)
            return std::nullopt;

        ec = ssl->disconnect();
        ssl.release();
    } else
#endif
#if defined(USE_EMSCRIPTEN_HTTP)
        emscripten_fetch_close(m_fetch);
#else
    {
        if(!normal)
            return std::nullopt;

        ec = normal->disconnect();
        normal.release();
    }
#endif

    return ec ? std::make_optional(ec) : std::nullopt;
}

Resource::Resource(
    std::shared_ptr<Coffee::ASIO::Service> ctxt,
    const Url& url)
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

asio::error_code Resource::connectError() const
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

std::optional<http::response_t> Resource::response() const
{
    if(isResponseReady())
        return m_response;
    return std::nullopt;
}

std::optional<asio::error_code> Resource::fetch()
{
    return push(http::method_t::get, const_chunk_u8());
}

std::optional<asio::error_code> Resource::push(const const_chunk_u8& data)
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
    case 0: status = "unsent"; break;
    case 1: status = "opened"; break;
    case 2: status = "headers_received"; break;
    case 3: status = "loading"; break;
    case 4: status = "done"; break;
    default:
        return;
    }
    cDebug("emscripten_fetch: url={}, state={}", fetch->url, status);
}

std::optional<asio::error_code> Resource::push(
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
        return asio::error_code::request_failed;
    return std::nullopt;
}
#else
std::optional<asio::error_code> Resource::readResponseHeader(
    net_buffer& buffer, libc_types::szptr& consumed)
{
    asio::error_code ec;

#if defined(ASIO_USE_SSL)
    if(secure())
    {
        DProfContext __(NETRSC_TAG "Read response");
        auto         socketError = ssl->flush();

        if(socketError)
            cWarning(NETRSC_TAG "asio error: {0}", socketError.message());

        ssl->read_until(buffer, http::header_terminator, ec);

        if(socketError)
            cWarning(NETRSC_TAG "asio error: {0}", socketError.message());
    } else
#endif
    {
        DProfContext __(NETRSC_TAG "Read response");
        normal->flush();
        normal->read_until(buffer, http::header_terminator, ec);
    }
    if(ec.value() != asio::error::eof)
        C_ERROR_CHECK(ec)

        {
            DProfContext __(NETRSC_TAG "Parsing response");
            consumed = http::buffer::read_response(
                m_response.header, chunk_u8::ofContainer(buffer).view);
        }

    if constexpr(compile_info::debug_mode)
    {
        cVerbose(
            12,
            NETRSC_TAG "Response: {0} {1} {2}",
            http::header::to_string::version(m_response.header.version),
            m_response.header.code,
            m_response.header.message);
        for(auto const& header : m_response.header.standard_fields)
            cVerbose(
                12,
                NETRSC_TAG "-- {0}: {1}",
                http::header::to_string::field(header.first),
                header.second);
        for(auto const& header : m_response.header.fields)
            cVerbose(12, NETRSC_TAG "-- {0}: {1}", header.first, header.second);
    }
    return std::nullopt;
}

std::optional<asio::error_code> Resource::readResponsePayload(
    net_buffer& buffer)
{
    using namespace http;

    asio::error_code ec;
    auto&            response_fields = m_response.header.standard_fields;
    auto content_len_it = response_fields.find(header_field::content_length);

    if(content_len_it == response_fields.end())
        return asio::error_code();

    {
        DProfContext __(NETRSC_TAG "Reading payload");
        auto         content_len = cast_string<u64>(content_len_it->second);

        cVerbose(12, NETRSC_TAG "Reading {0} chunk_u8...", content_len);

        m_response.payload.insert(
            m_response.payload.begin(), buffer.begin(), buffer.end());
        m_response.payload.resize(content_len);

        /* If we're lucky, we already got all the data */
        if(content_len == buffer.size())
        {
            cVerbose(12, NETRSC_TAG "All data ready");
            return asio::error_code();
        }

        if(auto view =
               chunk_u8::ofContainer(m_response.payload).at(buffer.size()))
        {
#if defined(ASIO_USE_SSL)
            if(secure())
            {
                ssl->read(view->view, ec);
            } else
#endif
                normal->read(view->view, ec);
            if(ec)
                return ec;

            cVerbose(12, NETRSC_TAG "Read complete");
        } else
        {
            cVerbose(12, NETRSC_TAG "Failed to read response");
        }
    }

    cVerbose(10, NETRSC_TAG "Payload size: {0}", m_response.payload.size());
    return std::nullopt;
}

std::optional<asio::error_code> Resource::push(
    http::method_t method, const_chunk_u8 const& data)
{
    using namespace http;

    DProfContext a(NETRSC_TAG "Sending HTTP request");

    auto& st_fields = m_request.header.standard_fields;

    if(!isRequestReady())
    {
        Profiler::DeepProfile(NETRSC_TAG "Resource not ready");
        return asio::error_code(asio::error::basic_errors::invalid_argument);
    }

    asio::error_code ec;

    bool has_payload = data.size > 0;
    bool should_expect =
        m_request.header.version != version_t::v10 && has_payload;

    /* We do this to allow GET/POST/PUT/UPDATE/whatever */
    m_request.header.method = method;

    /* Most services require a MIME-type with the request */
    if(has_payload &&
       st_fields.find(header_field::content_type) == st_fields.end())
        st_fields[header_field::content_type] =
            header::to_string::content_type(content_type::octet_stream);

    if(st_fields.find(header_field::expect) == st_fields.end() && should_expect)
        st_fields[header_field::expect] = "100-continue";

    if(has_payload)
        st_fields[header_field::content_length] = cast_pod(data.size);

    auto header = header::serialize::request(m_request.header);

    cVerbose(8, NETRSC_TAG "Sending request:\n{0}", header);

    Profiler::DeepPushContext(NETRSC_TAG "Writing header");

#if defined(ASIO_USE_SSL)
    if(secure())
        ssl->write(const_chunk_u8::ofContainer(header).view, ec);
    else
#endif
        normal->write(const_chunk_u8::ofContainer(header).view, ec);
    if(ec)
        return ec;

    szptr           consumed = 0;
    std::vector<u8> recv_buf;
    if(should_expect)
    {
        readResponseHeader(recv_buf, consumed);
        if(m_response.header.code != status::continue_)
        {
            auto& head = m_response.header;

            cWarning(NETRSC_TAG "No continue received, got {0}", head.code);

            cVerbose(12, NETRSC_TAG "HTTP header dump:");
            cVerbose(
                12,
                NETRSC_TAG "{0} {1} {2}",
                http::header::to_string::method(head.method),
                head.resource,
                cast_pod(head.code),
                head.message);

            for(auto const& header : head.fields)
                cVerbose(
                    12, NETRSC_TAG "{0}: {1}", header.first, header.second);

            recv_buf.erase(
                recv_buf.begin(), recv_buf.begin() + C_FCAST<ptroff>(consumed));

            readResponsePayload(recv_buf);

            auto type = http::header::from_string::content_type(
                head.standard_fields[header_field::content_type]);
            if(type == content_type::json || type == content_type::text)
            {
                cWarning(NETRSC_TAG "Payload:\n{0}", m_response.payload.data());
            }

            close();
            return ec;
        }
    }
    recv_buf.clear();

    Profiler::DeepPopContext();

    if(has_payload)
    {
        DProfContext _(NETRSC_TAG "Writing payload");

        cVerbose(12, NETRSC_TAG "Pushed payload size: {0}", data.size);

#if defined(ASIO_USE_SSL)
        if(secure())
        {
            ssl->write(data.view, ec);
            ssl->flush();
        } else
#endif
        {
            normal->write(data.view, ec);
            normal->flush();
        }
        if(ec)
            return ec;
    }

    consumed = 0;
    readResponseHeader(recv_buf, consumed);
    consumed = std::min(consumed, recv_buf.size());
    recv_buf.erase(recv_buf.begin(), recv_buf.begin() + consumed);
    readResponsePayload(recv_buf);

    auto& response_fields = m_response.header.standard_fields;

    auto response_code = header::classify_status(m_response.header.code);

    if(response_code == response_class::redirect)
    {
        if(auto loc = response_fields.find(header_field::location);
           loc != response_fields.end())
        {
            if(auto conn = response_fields.find(header_field::connection);
               conn != response_fields.end() &&
               util::strings::iequals(conn->second, "close"))
                close();
            initRsc(MkUrl(loc->second, m_access));
            return push(method, data);
        }
    }

    close();
    if(response_code == response_class::success)
        return std::nullopt;
    return ec;
}
#endif

std::optional<std::string> Resource::mimeType() const
{
    using field = http::header_field;

    auto const it = m_response.header.standard_fields.find(field::content_type);

    if(it != m_response.header.standard_fields.end())
        return std::nullopt;

    return it->second;
}

libc_types::u32 Resource::responseCode() const
{
    return m_response.header.code;
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
