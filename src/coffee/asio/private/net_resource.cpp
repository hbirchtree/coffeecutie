#include <coffee/asio/net_resource.h>

#include <coffee/core/CProfiling>
#include <coffee/core/CRegex>
#include <coffee/core/types/chunk.h>
#include <peripherals/enum/helpers.h>
#include <peripherals/stl/string_casting.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

#define NETRSC_TAG "NetRsc::"

using namespace ::semantic;

namespace Coffee {
namespace Net {

using namespace ::enum_helpers;

void Resource::initRsc(const Url& url)
{
    DProfContext a(DTEXT(NETRSC_TAG "Initializing NetResource"));

    CString urlS = *url;

    Profiler::DeepPushContext(DTEXT(NETRSC_TAG "Extracting URL components"));
    UrlParse urlComponents = UrlParse::From(url);

    {
        if(!urlComponents.valid())
        {
            cVerbose(10, "Failed to decode URL");
            Profiler::DeepProfile(DTEXT(NETRSC_TAG "Failed to decode URL"));
            return;
        }
    }
    Profiler::DeepPopContext();

    u16 port = C_FCAST<u16>(urlComponents.port());
    if(port == 0)
    {
        if(urlComponents.protocol() == "https")
        {
            port = 443;
            m_access |= HTTPAccess::Secure;
        } else
            port = 80;
    }

    if(m_request.host == urlComponents.host() && m_request.port == port &&
       ((secure() && urlComponents.protocol() == "https") ||
        (!secure() && urlComponents.protocol() == "http")))
    {
        m_request.header.resource = "/" + urlComponents.resource();
        return;
    } else
        close();

    m_request.host            = urlComponents.host();
    m_request.header.resource = "/" + urlComponents.resource();
    m_request.header.version  = http::version_t::v11;

    //    const auto verify_https = !feval(m_access, HTTPAccess::NoVerify);

    m_request.port = port;

    m_error = asio::error_code();

#if defined(ASIO_USE_SSL)
    if(urlComponents.protocol() == "http" && secure())
    {
        cVerbose(10, "Switching off SSL, protocol mismatch");
        m_access = m_access & (m_access ^ HTTPAccess::Secure);
    }
#endif

    {
        DProfContext b(DTEXT(NETRSC_TAG "Connecting to host"));
#if defined(ASIO_USE_SSL)
        if(secure())
        {
            ssl = MkUq<net::tcp::ssl_socket>(std::ref(*m_ctxt));

            m_error = ssl->connect(m_request.host, m_request.port);

        } else
#endif
        {
            normal = MkUq<net::tcp::raw_socket>(std::ref(*m_ctxt));

            m_error = normal->connect(m_request.host, m_request.port);
        }
        C_ERROR_CHECK_TYPED(m_error, net_error)
    }

    if(!connected())
    {
        Profiler::DeepProfile(NETRSC_TAG "Failed to connect");
        return;
    }

    http::header::transform::create_request(m_request);

    m_response = {};
}

void Resource::close()
{
    asio::error_code ec;
#if defined(ASIO_USE_SSL)
    if(secure())
    {
        if(!ssl)
            return;

        ec = ssl->disconnect();
        ssl.release();
    } else
#endif
    {
        if(!normal)
            return;

        ec = normal->disconnect();
        normal.release();
    }

    // C_ERROR_CHECK(ec)
}

Resource::Resource(ShPtr<ASIO::Service> ctxt, const Url& url) :
    m_resource(url), m_ctxt(ctxt), m_access(url.netflags)
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

ErrCode Resource::errorCode() const
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
    return m_response.code != 0;
}

void Resource::setHeaderField(http::header_field field, const CString& value)
{
    m_request.header.standard_fields[field] = value;
}

void Resource::setHeaderField(const CString& field, const CString& value)
{
    m_request.header.fields[field] = value;
}

http::request_t& Resource::request()
{
    return m_request;
}

http::response_t const& Resource::response() const
{
    return m_response;
}

bool Resource::fetch()
{
    return push(http::method_t::get, BytesConst());
}

bool Resource::push(const BytesConst& data)
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

void Resource::readResponseHeader(net_buffer& buffer, szptr& consumed)
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
            m_response.header, Bytes::ofContainer(buffer).view);
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
}

void Resource::readResponsePayload(net_buffer& buffer)
{
    using namespace http;

    asio::error_code ec;
    auto&            response_fields = m_response.header.standard_fields;
    auto content_len_it = response_fields.find(header_field::content_length);

    if(content_len_it == response_fields.end())
        return;

    {
        DProfContext __(NETRSC_TAG "Reading payload");
        auto         content_len = cast_string<u64>(content_len_it->second);

        cVerbose(12, NETRSC_TAG "Reading {0} bytes...", content_len);

        m_response.payload.insert(
            m_response.payload.begin(), buffer.begin(), buffer.end());
        m_response.payload.resize(content_len);

        /* If we're lucky, we already got all the data */
        if(content_len == buffer.size())
        {
            cVerbose(12, NETRSC_TAG "All data ready");
            return;
        }

        if(auto view = Bytes::ofContainer(m_response.payload).at(buffer.size()))
        {
            szptr read;
#if defined(ASIO_USE_SSL)
            if(secure())
            {
                read = ssl->read(*view, ec);
            } else
#endif
                read = normal->read(*view, ec);
            C_ERROR_CHECK(ec)

            cVerbose(12, NETRSC_TAG "Read complete");
        } else
        {
            cVerbose(12, NETRSC_TAG "Failed to read response");
        }
    }

    cVerbose(10, NETRSC_TAG "Payload size: {0}", m_response.payload.size());
}

bool Resource::push(http::method_t method, BytesConst const& data)
{
    using namespace http;

    DProfContext a(NETRSC_TAG "Sending HTTP request");

    auto& st_fields = m_request.header.standard_fields;

    if(!isRequestReady())
    {
        Profiler::DeepProfile(NETRSC_TAG "Resource not ready");
        return false;
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

    szptr written = 0;
#if defined(ASIO_USE_SSL)
    if(secure())
        written = ssl->write(BytesConst::ofContainer(header), ec);
    else
#endif
        written = normal->write(BytesConst::ofContainer(header), ec);
    C_ERROR_CHECK(ec)

    szptr      consumed = 0;
    Vector<u8> recv_buf;
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
            return false;
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
            ssl->write(data, ec);
            ssl->flush();
        } else
#endif
        {
            normal->write(data, ec);
            normal->flush();
        }
        C_ERROR_CHECK(ec)
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
    return response_code == response_class::success;
}

CString Resource::mimeType() const
{
    using field = http::header_field;

    auto const it = m_response.header.standard_fields.find(field::content_type);

    if(it != m_response.header.standard_fields.end())
        return {};

    return it->second;
}

u32 Resource::responseCode() const
{
    return m_response.header.code;
}

BytesConst Resource::data() const
{
    Profiler::DeepProfile(NETRSC_TAG "Retrieving data");

    return BytesConst::ofContainer(m_response.payload);
}

Bytes Resource::move()
{
    auto out = Bytes::move(std::move(m_response.payload));
    return out;
}

} // namespace Net
} // namespace Coffee
