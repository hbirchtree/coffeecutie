#include <coffee/asio/net_resource.h>

#include <coffee/core/base/textprocessing/cregex.h>
#include <coffee/core/plat/timing/profiling.h>
#include <peripherals/stl/string_casting.h>
#include <coffee/core/types/cdef/memsafe.h>

#include <coffee/core/CDebug>

#define NETRSC_TAG "NetRsc::"

namespace Coffee {
namespace Net {

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
            port = 443;
        else
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

    const auto verify_https = !feval(m_access, HTTPAccess::NoVerify);

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
            ssl = MkUq<TCP::SSLSocket>(m_ctxt);

            ssl->connect(
                verify_https, m_request.host, cast_pod(m_request.port));

            m_error = ssl->error();
        } else
#endif
        {
            normal = MkUq<TCP::Socket>(m_ctxt);

            normal->connect(m_request.host, cast_pod(m_request.port));

            m_error = normal->error();
        }
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
#if defined(ASIO_USE_SSL)
    if(secure())
    {
        if(!ssl)
            return;

        if(ssl->error() == asio::error_code())
            ssl->sync_close();
        ssl.release();
    } else
#endif
    {
        if(!normal)
            return;

        if(normal->error() == asio::error_code())
            normal->sync_close();
        normal.release();
    }
}

Resource::Resource(ASIO::asio_context ctxt, const Url& url) :
    m_resource(url), m_ctxt(ctxt), m_access(url.netflags)
{
    using namespace http::header::to_string;
    using namespace http::header;

    using field   = http::header_field;
    using content = http::content_type;

    transform::create_request(m_request);

    auto& fields = m_request.header.standard_fields;

    fields[field::accept] = content_type(content::any);

    //    HTTP::InitializeRequest(m_request);
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

http::request_t& Resource::getRequest()
{
    return m_request;
}

bool Resource::fetch()
{
    return push(http::method_t::get, Bytes());
}

bool Resource::push(const Bytes& data)
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

void Resource::readResponseHeader(std::istream& http_istream)
{
#if defined(ASIO_USE_SSL)
    if(secure())
    {
        DProfContext __(NETRSC_TAG "Read response");
        auto         socketError = ssl->flush();

        if(socketError)
            cWarning(NETRSC_TAG "asio error: {0}", socketError.message());

        ssl->read_until(http::header_terminator);

        if(socketError)
            cWarning(NETRSC_TAG "asio error: {0}", socketError.message());
    } else
#endif
    {
        DProfContext __(NETRSC_TAG "Read response");
        normal->flush();
        normal->read_until(http::header_terminator);
    }

    {
        DProfContext __(NETRSC_TAG "Parsing response");
        m_response.header = http::stream::read_response(http_istream);
    }

#if MODE_DEBUG
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
#endif
}

void Resource::readResponsePayload(std::istream& http_istream)
{
    using namespace http;

    auto& response_fields = m_response.header.standard_fields;

    auto content_len_it = response_fields.find(header_field::content_length);

    if(content_len_it == response_fields.end())
        return;

    {
        DProfContext __(NETRSC_TAG "Reading payload");
        auto         content_len = cast_string<u64>(content_len_it->second);

        cVerbose(12, NETRSC_TAG "Reading {0} bytes...", content_len);

#if defined(ASIO_USE_SSL)
        if(secure())
        {
            ssl->read_some(content_len);
        } else
#endif
            normal->read_some(content_len);

        cVerbose(12, NETRSC_TAG "Read complete");

        m_response.payload =
            http::stream::read_payload(http_istream, content_len);
    }

    cVerbose(10, NETRSC_TAG "Payload size: {0}", m_response.payload.size());
}

bool Resource::push(http::method_t method, const Bytes& data)
{
    using namespace http;

    DProfContext a(NETRSC_TAG "Sending HTTP request");

    auto& st_fields = m_request.header.standard_fields;

    if(!isRequestReady())
    {
        Profiler::DeepProfile(NETRSC_TAG "Resource not ready");
        return false;
    }

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
    {
        st_fields[header_field::content_length] = cast_pod(data.size);

        m_request.payload.reserve(data.size);
        MemCpy(data, m_request.payload);
    }

    std::istream* http_istream = nullptr;
    std::ostream* http_ostream = nullptr;

#if defined(ASIO_USE_SSL)
    if(secure())
    {
        http_istream = ssl.get();
        http_ostream = ssl.get();
    } else
#endif
    {
        http_istream = normal.get();
        http_ostream = normal.get();
    }

    auto header = header::serialize::request(m_request.header);

    cVerbose(8, NETRSC_TAG "Sending request:\n{0}", header);

#if defined(ASIO_USE_SSL)
    if(secure())
        ssl->write(header);
    else
#endif
        normal->write(header);

    if(should_expect)
    {
        readResponseHeader(*http_istream);
        if(m_response.header.code != status::continue_)
        {
            cWarning(NETRSC_TAG "No continue received");
            return false;
        }
    }

    if(has_payload)
    {
        cVerbose(
            12,
            NETRSC_TAG "Pushed payload size: {0}",
            m_request.payload.size());

#if defined(ASIO_USE_SSL)
        if(secure())
        {
            ssl->write(m_request.payload);
            ssl->flush();
        } else
#endif
        {
            normal->write(m_request.payload);
            normal->flush();
        }
    }

    readResponseHeader(*http_istream);

    readResponsePayload(*http_istream);

    auto& response_fields = m_response.header.standard_fields;

    auto response_code = header::classify_status(m_response.header.code);

    if(response_code == response_class::redirect)
    {
        const auto redirect_location =
            response_fields.find(header_field::location);

        if(response_fields.find(header_field::connection) !=
               response_fields.end() &&
           util::strings::iequals(
               response_fields[header_field::connection], "close"))
            close();
        initRsc(MkUrl(redirect_location->second, m_access));

        return push(method, data);
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

Bytes Resource::data() const
{
    Profiler::DeepProfile(NETRSC_TAG "Retrieving data");

    return Bytes(
        C_FCAST<byte_t*>(m_response.payload.data()),
        m_response.payload.size(),
        m_response.payload.size());
}

} // namespace Net
} // namespace Coffee
