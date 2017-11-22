#pragma once

#include <coffee/CAsio>
#include <coffee/asio/http_parsing.h>
#include <coffee/core/base/files/url.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{
namespace Net{

struct Resource
{
private:
    CString m_resource;
    Vector<byte_t> m_store;

#if defined(ASIO_USE_SSL)
    UqPtr<TCP::SSLSocket> ssl;
#endif
    UqPtr<TCP::Socket> normal;

    CString m_host;
    REST::Request m_request;
    HTTP::Response m_response;

    HTTPAccess m_access;

public:
    Resource(ASIO::AsioContext ctxt, Url const& url);
    ~Resource();

    cstring resource() const;

    bool isRequestReady() const;
    bool isResponseReady() const;

    bool fetch();

    cstring mimeType() const;
    Bytes data() const;
};

}

FORCEDINLINE Url operator "" _web(const char* url, size_t)
{
    return {url, Url::Networked,
                ResourceAccess::None,
                HTTPAccess::DefaultAccess};
}

FORCEDINLINE Url operator "" _http(const char* url, size_t)
{
    return {url, Url::Networked,
                ResourceAccess::None,
                HTTPAccess::GET};
}

}
