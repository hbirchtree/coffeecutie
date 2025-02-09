#pragma once

#if defined(USE_EMSCRIPTEN_HTTP)
namespace asio {
namespace error {
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

    error_code(code = none)
    {
    }

    operator code() const
    {
        return none;
    }

    const char* message() const
    {
        return "";
    }
};

} // namespace asio

#include <coffee/asio/asio_data.h>
#include <emscripten/fetch.h>
#elif defined(USE_CURL)
#include <future>
#endif

#include <coffee/core/url.h>
#include <coffee/interfaces/byte_provider.h>
#include <coffee/net/curl_context.h>
#include <coffee/net/http.h>
#include <fmt/format.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/enum/http_access.h>

namespace net {

using platform::url::Url;

using resource_context = curl_context;

struct error_code
{
    int         err_code{};
    std::string err_msg{};

    inline operator bool() const
    {
        return err_code != 0;
    }

    inline operator std::string() const
    {
        return err_msg;
    }

#if !defined(USE_CURL)
    enum code_t
    {
        no_error,
        request_failed,
    };

    error_code()
    {
    }

    error_code(code_t err_code)
        : err_code(err_code)
    {
    }
#endif
};

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
    Url m_resource;
#if defined(USE_EMSCRIPTEN_HTTP)
    emscripten_fetch_t* m_fetch{nullptr};
#elif defined(USE_CURL)
    resource_context m_ctxt{};
    curl_request     m_handle{};
#else
    resource_context m_ctxt{};
#if defined(ASIO_USE_SSL)
    std::unique_ptr<net::tcp::ssl_socket> ssl;
#endif
    std::unique_ptr<net::tcp::raw_socket> normal;
#endif

    http::request_t  m_request;
    http::response_t m_response;

    semantic::HTTPAccess m_access;

    error_code m_error;

    void                      initRsc(Url const& url);
    std::optional<error_code> close();

#if !defined(USE_EMSCRIPTEN_HTTP)
    std::optional<error_code> readResponseHeader(
        net_buffer& buffer, libc_types::szptr& consumed);
    std::optional<error_code> readResponsePayload(net_buffer& buffer);
#endif

#if defined(USE_CURL)
    void preRequest(http::method_t method, const_chunk_u8 const& data);
    void postRequest();
#endif

  public:
    Resource(resource_context ctxt, Url const& url);
    ~Resource();

    C_MOVE_CONSTRUCTOR(Resource);

    bool       secure() const;
    bool       connected() const;
    error_code connectError() const;

    Url resource() const;

    bool isRequestReady() const;
    bool isResponseReady() const;

    FORCEDINLINE bool valid() const
    {
        return isResponseReady();
    }

    void setHeaderField(http::header_field field, std::string const& value);
    void setHeaderField(std::string const& field, std::string const& value);

    http::request_t&           request();
    std::optional<std::string> mimeType() const;

    std::optional<error_code> fetch();
#if defined(USE_CURL)
    std::future<void> pushAsync(
        http::method_t          method,
        const_chunk_u8 const&   data,
        std::function<void()>&& success,
        std::function<void()>&& error);
#endif
    std::optional<error_code> push(const_chunk_u8 const& data);
    std::optional<error_code> push(
        http::method_t method, const_chunk_u8 const& data);

    std::optional<http::response_t> response() const;
    libc_types::u32                 responseCode() const;
    std::optional<Url>              responseLocation() const;
    std::optional<const_chunk_u8>   data() const;

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
FORCEDINLINE platform::url::Url operator""_http(const char* url, size_t)
{
    return net::MkUrl(url, semantic::HTTPAccess::GET);
}

FORCEDINLINE platform::url::Url operator""_https(const char* url, size_t)
{
    return net::MkUrl(
        url, semantic::HTTPAccess::GET | semantic::HTTPAccess::Secure);
}

FORCEDINLINE platform::url::Url operator""_web(const char* url, size_t)
{
    return net::MkUrl(url, semantic::HTTPAccess::DefaultAccess);
}

} // namespace url_literals
} // namespace net

template<>
struct fmt::formatter<net::error_code>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatCtx>
    auto format(net::error_code const& p, FormatCtx& ctx) const
    {
        return fmt::format_to(ctx.out(), "{} ({})", p.err_msg, p.err_code);
    }
};
