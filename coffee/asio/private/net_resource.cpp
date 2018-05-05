#include <coffee/asio/net_resource.h>

#include <coffee/core/string_casting.h>
#include <coffee/core/base/textprocessing/cregex.h>
#include <coffee/core/types/cdef/memsafe.h>
#include <coffee/core/CDebug>

#define NETRSC_TAG "NetRsc::"

namespace Coffee{
namespace Net{

static bool ExtractUrlComponents(CString const& url,
                                 CString* protocol,
                                 CString* port,
                                 CString* host,
                                 CString* resource)
{
    auto patt = Regex::Compile(
                "([A-Za-z]+)://([A-Za-z0-9-\\.]+):?([0-9]*)/?(.*)");

    auto matches = Regex::Match(patt, url, true);

    if(matches.size() == 4)
    {
        *protocol = matches[1].s_match[0];
        *host = matches[2].s_match[0];
        *resource = matches[3].s_match[0];

        return true;
    }else if(matches.size() == 5)
    {
        *protocol = matches[1].s_match[0];
        *host = matches[2].s_match[0];
        *port = matches[3].s_match[0];
        *resource = matches[4].s_match[0];

        return true;
    }
    return false;
}

void Resource::initRsc(const Url &url)
{
    DProfContext a(DTEXT(NETRSC_TAG "Initializing NetResource"));

    CString urlS = *url;

    CString protocol, port, resource;

    if(!ExtractUrlComponents(urlS, &protocol, &port,
                             &m_host, &resource))
    {
        cVerbose(10, "Failed to decode URL");
        Profiler::DeepProfile(DTEXT(NETRSC_TAG "Failed to decode URL"));
        return;
    }else
        cVerbose(15, "URL components: {0} {1} {2} {3}",
                 protocol, m_host, port, resource);

    if(!port.size())
        port = protocol;

#if defined(ASIO_USE_SSL)

    if(protocol == "http" && secure())
    {
        cVerbose(10, "Switching off SSL, protocol mismatch");
        m_access = m_access & (m_access ^ HTTPAccess::Secure);
    }

    m_error = asio::error_code();

    if(secure())
    {
        ssl = MkUq<TCP::SSLSocket>(m_ctxt);

        ssl->connect(m_host, port,
                     !feval(m_access, HTTPAccess::NoVerify));

        m_error = ssl->error();
    }else
#endif
    {
        normal = MkUq<TCP::Socket>();

        normal->connect(m_host, port);

        m_error = normal->error();
    }

    if(!connected())
    {
        Profiler::DeepProfile(NETRSC_TAG "Failed to connect");
        return;
    }

    m_request.resource = "/" + resource;

    m_response = {};
}

void Resource::close()
{
#if defined(ASIO_USE_SSL)
    if(secure())
    {
        if(ssl->error() == asio::error_code())
            ssl->close();
        ssl.release();
    }else
#endif
    {
        if(normal->error() == asio::error_code())
            normal->close();
        normal.release();
    }
}

Resource::Resource(ASIO::AsioContext ctxt, const Url &url):
    m_resource(*url),
    #if defined(ASIO_USE_SSL)
    m_ctxt(ctxt),
    #endif
    m_access(url.netflags)
{
    HTTP::InitializeRequest(m_request);
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

cstring Resource::resource() const
{
    return m_resource.c_str();
}

bool Resource::isRequestReady() const
{
    return m_request.resource.size() > 0 && m_host.size() > 0;
}

bool Resource::isResponseReady() const
{
    return m_response.code != 0;
}

void Resource::setHeaderField(const CString &field, const CString &value)
{
    if(field == "Content-Type")
    {
        m_request.mimeType = value;
        return;
    }
    m_request.header[field] = value;
}

bool Resource::fetch()
{
    return push("GET", Bytes());
}

bool Resource::push(const Bytes &data)
{
    CString method = "GET";
    switch(m_access & HTTPAccess::RequestMask)
    {
    case HTTPAccess::POST:
        method = "POST";
        break;
    case HTTPAccess::PUT:
        method = "PUT";
        break;
    case HTTPAccess::UPDATE:
        method = "UPDATE";
        break;
    case HTTPAccess::DELETE:
        method = "UPDATE";
        break;
    case HTTPAccess::PATCH:
        method = "PATCH";
        break;
    case HTTPAccess::HEAD:
        method = "HEAD";
        break;
    default:
        break;
    }
    return push(method, data);
}

bool Resource::push(CString const& method, const Bytes &data)
{
    DProfContext a(NETRSC_TAG "Sending HTTP request");

    if(!isRequestReady())
    {
        Profiler::DeepProfile(NETRSC_TAG "Resource not ready");
        return false;
    }

    /* We do this to allow GET/POST/PUT/UPDATE/whatever */
    m_request.reqtype = method;

    /* Most services require a MIME-type with the request */
    if(!m_request.mimeType.size())
        m_request.mimeType = "application/octet-stream";

    if(data.size)
    {
        m_request.payload.reserve(data.size);
        MemCpy(data, m_request.payload);
//        MemCpy(m_request.payload.data(), data.data, data.size);
    }

    bool result = false;

#if defined(ASIO_USE_SSL)
    if(secure())
    {
        HTTP::GenerateRequest(*ssl, m_host, m_request);
        m_host.clear();

        szptr size = 0;
        {
            DProfContext _("Flushing SSL socket");
            auto writeError = ssl->flush(&size);
            if(writeError)
                cWarning("Write error code: {0}, wrote {1} bytes",
                         writeError.message(), size);
        }
        {
            DProfContext _("Pulling SSL socket");
            auto readError = ssl->pull(&size);
            if(readError)
                cWarning("Read error code: {0}, received {1} bytes",
                         readError.message(), size);
        }

        Profiler::DeepProfile(NETRSC_TAG "HTTPS request sent");

        result = HTTP::ExtractResponse(*ssl, &m_response);
    }else
#endif
    {
        HTTP::GenerateRequest(*normal, m_host, m_request);
        m_host.clear();

        normal->flush();

        Profiler::DeepProfile(NETRSC_TAG "HTTP request sent");

        result = HTTP::ExtractResponse(*normal, &m_response);
    }

    if(feval(m_access & HTTPAccess::NoRedirect))
        return result;

    switch(m_response.code)
    {
    case 301:
    case 302:
    case 308:
    {
        auto u = Net::MkUrl(m_response.header["Location"].c_str(),
                HTTPAccess::DefaultAccess);
        close();
        initRsc(u);
        return push(method, data);
    }
    default:
        return result;
    }
}

cstring Resource::mimeType() const
{
    return m_response.mimeType.c_str();
}

u32 Resource::responseCode() const
{
    return m_response.code;
}

Bytes Resource::data() const
{
    Profiler::DeepProfile(NETRSC_TAG "Retrieving data");

    return Bytes(C_FCAST<byte_t*>(m_response.payload.data()),
                 m_response.payload.size(),
                 0);
}

const Map<CString, CString> &Resource::headers() const
{
    return m_response.header;
}

}
}
