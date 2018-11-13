#pragma once

#if defined(FEATURE_ENABLE_CoffeeASIO)
#include <coffee/asio/http.h>
#include <coffee/asio/tcp_socket.h>
#endif

#include <coffee/core/base/files/url.h>
#include <coffee/core/types/chunk.h>
#include <coffee/interfaces/byte_provider.h>

namespace Coffee {
namespace Net {

FORCEDINLINE bool Supported()
{
#if !defined(COFFEE_EMSCRIPTEN) && !defined(COFFEE_WINDOWS_UWP)
    return true;
#else
    return false;
#endif
}

#if defined(FEATURE_ENABLE_CoffeeASIO)

struct Resource : ByteProvider
{
  private:
    Url                m_resource;
    ASIO::asio_context m_ctxt;
#if defined(ASIO_USE_SSL)
    UqPtr<TCP::SSLSocket> ssl;
#endif
    UqPtr<TCP::Socket> normal;

    http::request_t  m_request;
    http::response_t m_response;

    HTTPAccess m_access;

    asio::error_code m_error;

    void initRsc(Url const& url);
    void close();

    void readResponseHeader(std::istream& http_istream);
    void readResponsePayload(std::istream& http_istream);

  public:
    Resource(ASIO::asio_context ctxt, Url const& url);
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

    http::request_t& getRequest();

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

FORCEDINLINE Url
             MkUrl(CString const& url, HTTPAccess access = HTTPAccess::DefaultAccess)
{
    return {url.c_str(), Url::Networked, RSCA::None, access, {}};
}

FORCEDINLINE Url
             MkUrl(cstring url, HTTPAccess access = HTTPAccess::DefaultAccess)
{
    return {url, Url::Networked, RSCA::None, access, {}};
}

} // namespace Net

#endif

FORCEDINLINE Url operator"" _http(const char* url, size_t)
{
    return Net::MkUrl(url, HTTPAccess::GET);
}

} // namespace Coffee
