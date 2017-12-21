#include <coffee/asio/rest-client.h>

#include <coffee/core/CProfiling>
#include <coffee/asio/http_parsing.h>

namespace Coffee{
namespace ASIO{


RestClientImpl::Response RestClientImpl::RestRequest(AsioContext_internal c, Host h, Request const& r)
{
    /* For HTTP/S we use socket streams, because they are simple
         *  and neat to work with. Their complexity behind
         *  the scenes is no matter. */
    if(r.transp==HTTP)
        return RestRequestHTTP(c, h,r);
    else if(r.transp==HTTPS)
        return RestRequestHTTPS(c, h,r);
    else
        return Response();
}

RestClientImpl::Response RestClientImpl::RestRequestHTTP(AsioContext_internal, Host h, Request const& r)
{
    ProfContext _m("ASIO request");

    CString port;
    switch(r.port)
    {
    case 0:
        port = "http";
        break;
    default:
        port = Convert::uintstostring(r.port);
        break;
    }

    asio::ip::tcp::iostream s;
    s.connect(h,port);

    Profiler::DeepProfile("Connect");

    HTTP::GenerateRequest(s,h,r);

    Profiler::DeepProfile("Request generation");

    Response resp;

    if(!HTTP::ExtractResponse(s,&resp))
        return Response();
    else
    {
        Profiler::DeepProfile("Response gathering");
        return resp;
    }
}

RestClientImpl::Response RestClientImpl::RestRequestHTTPS(AsioContext_internal c, Host h, Request const& r)
{
#if defined(ASIO_USE_SSL)
    asio::ssl::stream<asio::ip::tcp::socket> socket(c->service,
                                                    c->sslctxt);

    CString port;
    switch(r.port)
    {
    case 0:
        port = "https";
        break;
    default:
        port = Convert::uintstostring(r.port);
        break;
    }

    asio::ip::tcp::resolver::query q(h,port);
    auto it = c->resolver.resolve(q);

    asio::connect(socket.next_layer(),it);

    socket.handshake(asio::ssl::stream_base::client);

    asio::streambuf trans;
    std::ostream os(&trans);
    HTTP::GenerateRequest(os,h,r);
    os.flush();
    asio::write(socket,trans);

    asio::streambuf recp;
    std::istream is(&recp);
#ifndef NDEBUG
    try{
#endif
        asio::read(socket,recp);
#ifndef NDEBUG
    }
    catch(std::system_error)
    {}
#endif

    Response resp;

    if(!HTTP::ExtractResponse(is,&resp))
        return {};
    else
        return resp;
#else
    return {};
#endif
}

CString RestClientImpl::GetContentType(const Response &resp)
{
    constexpr const cstring query = "content-type";

    CString data;

    CString lowered;
    for(Pair<CString,CString> const& v : resp.header)
    {
        lowered = StrUtil::lower(v.first);
        if(lowered == query)
        {
            data = v.second;
            break;
        }
    }

    data = StrUtil::lower(data);

    return data;
}

}
}
