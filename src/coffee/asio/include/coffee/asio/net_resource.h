#pragma once

#if defined(USE_EMSCRIPTEN_HTTP)
namespace asio {
namespace error
{
enum basic_errors
{
    none,
    invalid_argument,
};
}
struct error_code
{
    enum code
    {
        none,
        request_failed,
    };

    error_code(code = none) {}

    operator code() const
    {
        return none;
    }
    const char* message() const
    {
        return "";
    }
};

}

#include <emscripten/fetch.h>
#else
#include <coffee/asio/tcp_socket.h>
#endif
#include <coffee/asio/http.h>

#include <coffee/core/url.h>
#include <coffee/interfaces/byte_provider.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/enum/http_access.h>

namespace net {

using platform::url::Url;

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

using chunk_u8       = semantic::mem_chunk<libc_types::u8>;
using const_chunk_u8 = semantic::mem_chunk<const libc_types::u8>;

struct Resource
{
    using net_buffer = std::vector<libc_types::u8>;

  private:
    Url                                    m_resource;
#if defined(USE_EMSCRIPTEN_HTTP)
    emscripten_fetch_t* m_fetch{nullptr};
#else
    std::shared_ptr<Coffee::ASIO::Service> m_ctxt;
#if defined(ASIO_USE_SSL)
    std::unique_ptr<net::tcp::ssl_socket> ssl;
#endif
    std::unique_ptr<net::tcp::raw_socket> normal;
#endif

    http::request_t  m_request;
    http::response_t m_response;

    semantic::HTTPAccess m_access;

    asio::error_code m_error;

    void                            initRsc(Url const& url);
    std::optional<asio::error_code> close();

#if !defined(USE_EMSCRIPTEN_HTTP)
    std::optional<asio::error_code> readResponseHeader(
        net_buffer& buffer, libc_types::szptr& consumed);
    std::optional<asio::error_code> readResponsePayload(net_buffer& buffer);
#endif

  public:
    Resource(
#if defined(USE_EMSCRIPTEN_HTTP)
        int,
#else
        std::shared_ptr<Coffee::ASIO::Service> ctxt,
#endif
        Url const& url);
    ~Resource();

    C_MOVE_CONSTRUCTOR(Resource);

    bool             secure() const;
    bool             connected() const;
    asio::error_code connectError() const;

    Url resource() const;

    bool              isRequestReady() const;
    bool              isResponseReady() const;
    FORCEDINLINE bool valid() const
    {
        return isResponseReady();
    }

    void setHeaderField(http::header_field field, std::string const& value);
    void setHeaderField(std::string const& field, std::string const& value);

    http::request_t&                request();
    std::optional<http::response_t> response() const;

    std::optional<asio::error_code> fetch();
    std::optional<asio::error_code> push(const_chunk_u8 const& data);
    std::optional<asio::error_code> push(
        http::method_t method, const_chunk_u8 const& data);

    std::optional<std::string>    mimeType() const;
    libc_types::u32               responseCode() const;
    std::optional<const_chunk_u8> data() const;

    std::optional<chunk_u8>       move();
    std::optional<const_chunk_u8> move_const() const;

    operator platform::url::Path() const
    {
        return platform::url::Path(m_request.header.resource);
    }
};

FORCEDINLINE Url MkUrl(
    std::string const&   url,
    semantic::HTTPAccess access = semantic::HTTPAccess::DefaultAccess)
{
    return {url, Url::Networked, semantic::RSCA::None, access};
}

#endif

namespace url_literals {
FORCEDINLINE platform::url::Url operator"" _http(const char* url, size_t)
{
    return net::MkUrl(url, semantic::HTTPAccess::GET);
}

FORCEDINLINE platform::url::Url operator"" _https(const char* url, size_t)
{
    return net::MkUrl(
        url, semantic::HTTPAccess::GET | semantic::HTTPAccess::Secure);
}

FORCEDINLINE platform::url::Url operator"" _web(const char* url, size_t)
{
    return net::MkUrl(url, semantic::HTTPAccess::DefaultAccess);
}

} // namespace url_literals
} // namespace net
