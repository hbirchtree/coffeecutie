#include <coffee/asio/net_resource.h>

#include <coffee/core/base/textprocessing/cregex.h>

#define NETRSC_TAG "NetRsc::"

namespace Coffee{
namespace Net{

bool ExtractUrlComponents(CString const& url,
                          CString* protocol,
                          CString* host,
                          CString* resource)
{
    auto patt = Regex::Compile("([A-Za-z]+)://([A-Za-z0-9-\\.]+)/(.*)");

    auto matches = Regex::Match(patt, url, true);

    if(matches.size() == 4)
    {
        *protocol = matches[1].s_match[0];
        *host = matches[2].s_match[0];
        *resource = matches[3].s_match[0];

        return true;
    }
    return false;
}

Resource::Resource(ASIO::AsioContext ctxt, const Url &url):
    m_resource(*url),
    m_access(url.netflags)
{
    DProfContext a(NETRSC_TAG "Initializing NetResource");

    CString urlS = *url;

    CString protocol, resource;

    if(!ExtractUrlComponents(urlS, &protocol, &m_host, &resource))
    {
        cVerbose(10, "Failed to decode URL");
        Profiler::DeepProfile(NETRSC_TAG "Failed to decode URL");
        return;
    }

#if defined(ASIO_USE_SSL)
    bool secure = (url.netflags & HTTPAccess::Secure) != HTTPAccess::None;

    if(secure)
    {
        ssl = MkUq<TCP::SSLSocket>(ctxt);

        ssl->connect(m_host, protocol);
    }else
#endif
    {
        normal = MkUq<TCP::Socket>();

        normal->connect(m_host, protocol);

        if(normal->bad())
        {
            Profiler::DeepProfile(NETRSC_TAG "Connection failed");
            return;
        }
    }

    HTTP::InitializeRequest(m_request);

    m_request.resource = "/" + resource;
    m_request.reqtype = "GET";
    m_request.mimeType.clear();
    m_request.header.insert({"Accept", "*/*"});
}

Resource::~Resource()
{
#if defined(ASIO_USE_SSL)
    if((m_access & HTTPAccess::Secure) != HTTPAccess::None)
        ssl.release();
    else
#endif
        normal.release();
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

bool Resource::fetch()
{
    DProfContext a(NETRSC_TAG "Fetching data");
    if(!isRequestReady())
    {
        Profiler::DeepProfile(NETRSC_TAG "Resource not ready");
        return false;
    }

#if defined(ASIO_USE_SSL)
    bool secure = (m_access & HTTPAccess::Secure) != HTTPAccess::None;

    if(secure)
    {
        HTTP::GenerateRequest(*ssl, m_host, m_request);
        m_host.clear();

        ssl->flush();
        ssl->pull();

        Profiler::DeepProfile(NETRSC_TAG "HTTPS file received");

        return HTTP::ExtractResponse(*ssl, &m_response);
    }else
#endif
    {
        HTTP::GenerateRequest(*normal, m_host, m_request);
        m_host.clear();

        normal->flush();

        Profiler::DeepProfile(NETRSC_TAG "HTTP file received");

        return HTTP::ExtractResponse(*normal, &m_response);
    }
}

bool Resource::push(CString const& method, const Bytes &data)
{
    m_request.reqtype = method;

    m_request.payload.resize(data.size);
    MemCpy(m_request.payload.data(), data.data, data.size);

    bool secure = (m_access & HTTPAccess::Secure) != HTTPAccess::None;

    if(secure)
    {

    }else
    {

    }
}

cstring Resource::mimeType() const
{
    return m_response.mimeType.c_str();
}

Bytes Resource::data() const
{
    Profiler::DeepProfile(NETRSC_TAG "Retrieving data");

    return Bytes(C_FCAST<byte_t*>(m_response.payload.data()),
                 m_response.payload.size(),
                 0);
}

}
}
