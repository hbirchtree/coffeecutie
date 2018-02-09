#pragma once

#include <coffee/CAsio>
#include <coffee/asio/http_parsing.h>
#include <coffee/core/base/files/url.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{
namespace Net{

FORCEDINLINE bool Supported()
{
#if !defined(COFFEE_EMSCRIPTEN)
    return true;
#else
    return false;
#endif
}

struct Resource
{
private:
    CString m_resource;
    Vector<byte_t> m_store;

#if defined(ASIO_USE_SSL)
    UqPtr<TCP::SSLSocket> ssl;
    ASIO::AsioContext m_ctxt;
#endif
    UqPtr<TCP::Socket> normal;

    CString m_host;
    REST::Request m_request;
    HTTP::Response m_response;

    HTTPAccess m_access;

    asio::error_code m_error;

    void initRsc(Url const& url);
    void close();

public:
    Resource(ASIO::AsioContext ctxt, Url const& url);
    ~Resource();

    bool secure() const;
    bool connected() const;

    ErrCode errorCode() const;

    cstring resource() const;

    bool isRequestReady() const;
    bool isResponseReady() const;

    void setHeaderField(CString const& field, CString const& value);

    bool fetch();
    bool push(Bytes const& data);
    bool push(const CString &method, Bytes const& data);

    cstring mimeType() const;
    u32 responseCode() const;
    Bytes data() const;
    Map<CString, CString> const& headers() const;
};

FORCEDINLINE Url MkUrl(cstring url,
                       HTTPAccess access = HTTPAccess::DefaultAccess)
{
    return {url, Url::Networked,
                ResourceAccess::None,
                access, {}};
}

}

FORCEDINLINE Url operator "" _web(const char* url, size_t)
{
    return Net::MkUrl(url);
}

FORCEDINLINE Url operator "" _http(const char* url, size_t)
{
    return Net::MkUrl(url, HTTPAccess::GET);
}

}
