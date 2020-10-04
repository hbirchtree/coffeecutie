#pragma once

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/http.h>
#include <coffee/asio/tcp_socket.h>
#endif

#include <coffee/core/types/chunk.h>
#include <coffee/core/url.h>
#include <coffee/interfaces/byte_provider.h>
#include <peripherals/semantic/enum/http_access.h>

namespace Coffee {
namespace Net {

struct net_error : resource_error
{
    using resource_error::resource_error;
};

FORCEDINLINE bool Supported()
{
#if !defined(COFFEE_EMSCRIPTEN) && !defined(COFFEE_WINDOWS_UWP)
    return true;
#else
    return false;
#endif
}

#if defined(FEATURE_ENABLE_ASIO)

struct Resource : semantic::ByteProvider
{
    using net_buffer = Vector<u8>;

  private:
    Url                  m_resource;
    ShPtr<ASIO::Service> m_ctxt;
#if defined(ASIO_USE_SSL)
    UqPtr<net::tcp::ssl_socket> ssl;
#endif
    UqPtr<net::tcp::raw_socket> normal;

    http::request_t  m_request;
    http::response_t m_response;

    semantic::HTTPAccess m_access;

    asio::error_code m_error;

    void initRsc(Url const& url);
    void close();

    void readResponseHeader(net_buffer& buffer, szptr& consumed);
    void readResponsePayload(net_buffer& buffer);

  public:
    Resource(ShPtr<ASIO::Service> ctxt, Url const& url);
    ~Resource();

    C_MOVE_CONSTRUCTOR(Resource);

    bool secure() const;
    bool connected() const;

    ErrCode errorCode() const;

    Url resource() const;

    bool              isRequestReady() const;
    bool              isResponseReady() const;
    FORCEDINLINE bool valid() const
    {
        return isResponseReady();
    }

    void setHeaderField(http::header_field field, CString const& value);
    void setHeaderField(CString const& field, CString const& value);

    http::request_t&        request();
    http::response_t const& response() const;

    bool fetch();
    bool push(Bytes const& data);
    bool push(http::method_t method, Bytes const& data);

    CString mimeType() const;
    u32     responseCode() const;
    Bytes   data() const;

    operator Bytes()
    {
        if(!isResponseReady() && !fetch())
            return Bytes();

        return data();
    }

    operator Path() const
    {
        return Path(m_request.header.resource);
    }
};

FORCEDINLINE Url MkUrl(
    CString const&       url,
    semantic::HTTPAccess access = semantic::HTTPAccess::DefaultAccess)
{
    return {url.c_str(), Url::Networked, semantic::RSCA::None, access, {}};
}

FORCEDINLINE Url MkUrl(
    cstring              url,
    semantic::HTTPAccess access = semantic::HTTPAccess::DefaultAccess)
{
    return {url, Url::Networked, semantic::RSCA::None, access, {}};
}

} // namespace Net

#endif

FORCEDINLINE platform::url::Url operator"" _http(const char* url, size_t)
{
    return Net::MkUrl(url, semantic::HTTPAccess::GET);
}

FORCEDINLINE platform::url::Url operator"" _https(const char* url, size_t)
{
    return Net::MkUrl(
        url, semantic::HTTPAccess::GET | semantic::HTTPAccess::Secure);
}

FORCEDINLINE platform::url::Url operator"" _web(const char* url, size_t)
{
    return Net::MkUrl(url, semantic::HTTPAccess::DefaultAccess);
}

} // namespace Coffee
